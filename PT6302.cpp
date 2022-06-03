/**
 * @file PT6302.cpp
 * Main source file for PT6302 library.
 *
 * Arduino library for communicating with the Princeton Technology PT6302 VFD Driver/Controller IC with Character RAM
 * This library only includes the ASCII compatible characters, all others will have to be printed using the hex codes in the datasheet
 *
 * Copyright (c) 2021 Arne van Iterson
 */

#include "PT6302.h"
#include <assert.h>

PT6302::PT6302(PT6302::Pin CLKB, PT6302::Pin RSTB, PT6302::Pin CSB, PT6302::Pin DIN)
{
    this->CLKB = CLKB;
    this->RSTB = RSTB;
    this->CSB = CSB;
    this->DIN = DIN;
}

void PT6302::init()
{
    // Set all pins to output
    pinMode(this->CLKB, OUTPUT);
    pinMode(this->RSTB, OUTPUT);
    pinMode(this->CSB, OUTPUT);
    pinMode(this->DIN, OUTPUT);

    // Init default state
    digitalWrite(this->CSB, HIGH);
    digitalWrite(this->CLKB, HIGH);
    digitalWrite(this->DIN, LOW);
    digitalWrite(this->RSTB, HIGH);

    this->reset();
    return;
}

void PT6302::reset()
{
    digitalWrite(this->RSTB, LOW);
    delay(20);
    digitalWrite(this->RSTB, HIGH);
    return;
}

void PT6302::center(const char c[], bool overwrite)
{
    // Get the length of the input
    uint8_t length = strlen(c);
    // Center the text
    unsigned int start = floor((this->digits - length) / 2.0);
    this->print(start, c, overwrite);
}

void PT6302::print(const char c[], bool overwrite)
{
    // Default printing position to the first digit
    this->print(0, c, overwrite);
    return;
}

void PT6302::print(const unsigned int start, const char c[], bool overwrite)
{
    // Get the length of the input
    uint8_t length = strlen(c);

    // Make a new array to keep the data in, this is the size of the display
    uint8_t *data = new uint8_t[this->digits];
    memset(data, 0x20, this->digits);

    // Reverse the input and put it in the new array
    for (int8_t i = length; i > 0; i--)
    {
        // Serial.print(c[i - 1]);
        // Off by one is because i needs to be an array index
        if (overwrite)
        {
            data[this->digits - i - start] = c[i - 1];
        }
        else
        {
            data[length - i] = c[i - 1];
        }
    }

    if (overwrite)
    {
        // Send it out
        this->writeDCRAM(0, data, this->digits);
    }
    else
    {
        // Send it out
        this->writeDCRAM(start, data, length);
    }

    delete[] data;
    return;
}

void PT6302::clear()
{
    // Create buffer full of spaces (0x20)
    uint8_t *data = new uint8_t[this->digits];
    memset(data, 0x20, this->digits);

    // Send it
    this->writeDCRAM(0, data, this->digits);
    delete[] data;
    return;
}

void PT6302::writeDCRAM(const unsigned int start, uint8_t byte)
{
    uint8_t command[1] = {byte};
    this->writeDCRAM(start, command, 1);
    return;
};

void PT6302::writeDCRAM(const unsigned int startd, uint8_t bytes[], size_t length)
{
    // Reverse start
    int start = this->digits - length - startd;

    // Create an offset if the text goes off screen, the entire array has to be shifted by this because the screen works from right to left
    // Value is -1 by default to account for the start byte added later
    short offset = -1;

    // Check if the text goes off screen
    if (start < 0)
    {
        offset = offset + abs(start);
        length = length + start;
        start = 0;
    }

    // Need to add a start byte to input
    length++;

    // Create command buffer and set it to 0
    uint8_t *command = new uint8_t[length];
    memset(command, 0, length);

    // Add start byte
    command[0] = rotateByte(start) + 0x08;

    // Add input to command buffer
    for (size_t i = 1; i < length; i++)
    {
        // Command buffer is offset by one, the input buffer is not
        // Offset is applied if input goes off screen
        command[i] = rotateByte(bytes[i + offset]);
    }

    // Send out
    this->sendBytes(command, (length));

    // Delete command buffer
    delete[] command;
    return;
}

void PT6302::writeCGRAM(const unsigned int target, uint8_t bytes[5])
{
    // Input data + start byte
    uint8_t *command = new uint8_t[5 + 1];

    // Start byte
    command[0] = rotateByte(target) + 0x04;

    // Input data
    for (size_t i = 0; i < 5; i++)
    {
        command[i + 1] = bytes[i];
    }

    // Send out
    this->sendBytes(command, (5 + 1));

    // Delete command buffer
    delete[] command;
    return;
}

void PT6302::writeADRAM(const unsigned int start, uint8_t byte)
{
    uint8_t *command = new uint8_t[2];
    command[0] = rotateByte(start - 1) + 0x0c;
    command[1] = byte;
    this->sendBytes(command, 2);
    delete[] command;
    return;
}

void PT6302::setGPOP(bool p1, bool p2)
{
    uint8_t command = 0x02;
    if (p1)
    {
        command += 0x80;
    }
    if (p2)
    {
        command += 0x40;
    }
    this->sendByte(command);
    return;
}

void PT6302::setDuty(const unsigned int cycles)
{
    assert(cycles >= 8 && cycles <= 15);
    const uint8_t values[8] = {
        0x00, // 8/16 (default)
        0x80, // 9/16
        0x40, // 10/16
        0xc0, // 11/16
        0x20, // 12/16
        0xA0, // 13/16
        0x60, // 14/16
        0xe0  // 15/16
    };

    uint8_t command = 0x0A;
    command += values[cycles - 8];

    this->sendByte(command);
    return;
}

void PT6302::setDigitNo(const unsigned int amount)
{
    assert(amount >= 9 && amount <= 16);
    this->digits = amount;

    const uint8_t values[8] = {
        0x80, // 9 Digits
        0x40, // 10 Digits
        0xc0, // 11 Digits
        0x20, // 12 Digits
        0xA0, // 13 Digits
        0x60, // 14 Digits
        0xe0, // 15 Digits
        0x00  // 16 Digits (default)
    };

    uint8_t command = 0x06;
    command += values[amount - 9];

    this->sendByte(command);
    return;
}

void PT6302::setMode(PT6302::Mode mode)
{
    uint8_t command = 0x0e;
    command += (uint8_t)mode;
    this->sendByte(command);
    return;
}

void PT6302::sendBit(const unsigned char bit)
{
    assert(bit == 0 || bit == 1);
    // Set clock low
    digitalWrite(this->CLKB, LOW);

    // Set data byte
    digitalWrite(this->DIN, bit);
    delayMicroseconds(50);

    // Raise clock
    digitalWrite(this->CLKB, HIGH);
    delayMicroseconds(50);
    return;
}

void PT6302::sendByte(uint8_t byte)
{
    uint8_t command[1] = {byte};
    this->sendBytes(command, 1);
    return;
}

void PT6302::sendBytes(uint8_t bytes[], size_t length)
{
    // Enable data transfer to IC
    digitalWrite(this->CSB, LOW);

    // Wait before starting data transfer
    delayMicroseconds(16);

    // If command consists of more bytes, go through them without making CSB high
    for (int j = 0; j < length; j++)
    {
        // Send bit for bit
        for (int i = 0; i < 8; i++)
        {
            this->sendBit(((bytes[j] << i) & 0b10000000) != 0);
        }

        delayMicroseconds(16);
    }

    // Wait before ending data transfer
    delayMicroseconds(16);

    // Stop data transfer to IC
    digitalWrite(this->CSB, HIGH);
    return;
}

uint8_t PT6302::rotateByte(uint8_t in)
{
    uint8_t result = 0;
    for (int i = 0; i < 8; i++)
        result = (result << 1) | (in >> i & 1);
    return result;
}

char *PT6302::rotateString(const char *in)
{
    char *result = new char[strlen(in) + 1]();
    size_t j = 0;
    for (int i = strlen(in) - 1; i >= 0; i--)
    {
        result[j++] = in[i];
    }
    return result;
    delete[] result;
}
