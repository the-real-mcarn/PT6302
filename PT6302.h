/**
 * @file PT6302.h
 * Header file for PT6302 library.
 * 
 * Arduino library for communicating with the Princeton Technology PT6302 VFD Driver/Controller IC with Character RAM
 * This library only includes the ASCII compatible characters, all others will have to be printed using the hex codes in the datasheet. 
 * 
 * Copyright (c) 2021 Arne van Iterson
 */

#ifndef PT6302_H
#define PT6302_H

#include <Arduino.h>

/**
 * @class PT6302 class
 */
class PT6302
{
public:
    /**
     * @typedef Pin
     * Represents a physical pin on the arduino board. 
     */
    using Pin = unsigned int;

    /**
     * @enum Mode
     * Different modes of operation, see datasheet
     */
    enum class Mode
    {
        NORMAL = 0x00,
        ALLON = 0x40,
        ALLOFF = 0x80
    };

    /**
     * Constructor for the PT6302 class
     * @param CLKB Serial clock pin, connect to CLKB pin of IC
     * @param RSTB Reset pin pin, connect to RSTB pin of IC
     * @param CSB Chip select pin, connect to CSB pin of IC
     * @param DIN Data out pin, connect to DIN pin of IC
     */
    PT6302(PT6302::Pin CLKB, PT6302::Pin RSTB, PT6302::Pin CSB, PT6302::Pin DIN);

    /**
     * Initialises default pin states and resets PT6302
     */
    void init();

    /**
     * Will just reset the PT6302 for manually setting settings
     */
    void reset();

    /**
     * Print something in the center of the display
     * @param c String to print
     * @param overwrite Wether or not the print should clear everything else on the display
     */
    void center(const char c[], bool overwrite = true);

    /**
     * Print something on the display at position start.
     * @param start Position to start printing to (first digit is 0)
     * @param c String to print
     * @param overwrite Wether or not the print should clear everything else on the display
     */
    void print(const unsigned int start, const char c[], bool overwrite = true);

    /**
     * Print something on the display left-aligned.
     * @param c String to print
     * @param overwrite Wether or not the print should clear everything else on the display
     */
    void print(const char c[], bool overwrite = true);

    /**
     * Clear all characters
     */
    void clear();

    /**
     * Write a single byte to Data Control Ram (DCRAM). Usually used to print characters made with writeCGRAM().
     * This function will automatically switch MSB and LSB like the IC expects it
     * @param start Position to start writing to
     * @param byte Byte to send
     */
    void writeDCRAM(const unsigned int start, uint8_t byte);

    /**
     * Write an array of bytes to Data Control Ram (DCRAM).
     * This function will automatically switch MSB and LSB like the IC expects it
     * @param start Position to start writing to
     * @param bytes Bytes to send
     */
    void writeDCRAM(const unsigned int start, uint8_t bytes[], size_t length);

    /**
     * Write to the Character Generator Ram (CGRAM)
     * Used to create a character to be put in one of seven ram positions which can then be displayed using writeDCRAM()
     * @param position Ram position (0-7)
     * @param bytes Bytes representing the custom character, see datasheet 
     */
    void writeCGRAM(const unsigned int target, uint8_t bytes[5]);

    /**
     * Write symbol data to Additional Ram (ADRAM) to be used as a cursor.
     * @param start Position to start writing to
     * @param byte Byte to send
     */
    void writeADRAM(const unsigned int start, uint8_t byte);

    /**
     * Set the state of the general purpose output ports P1 and P2
     * @param P1 State of P1
     * @param P2 State of P2
     */
    void setGPOP(bool P1, bool P2);

    /**
     * Sets the duty cycle of the display
     * @param cycles Amount of cycles (8-15 / 16)
     * @exception throws static error if amount of cycles is not between 8 and 15
     */
    void setDuty(const unsigned int cycles);

    /**
     * Sets the amount of digits connected to the PT6302
     * @param amount Amount of digits (9-16)
     * @exception throws static error if amount of digits is not between 9 and 16
     */
    void setDigitNo(const unsigned int amount);

    /**
     * Sets operation mode of the PT6302
     * @param mode Operation mode
     */
    void setMode(PT6302::Mode mode);

private:
    PT6302::Pin CLKB;
    PT6302::Pin RSTB;
    PT6302::Pin CSB;
    PT6302::Pin DIN;

    unsigned int digits = 16;

    /**
     * Internal function, sends a single bit to PT6302
     * @param bit Bit to send
     * @exception throws static error if bit is not 1 or 0
     */
    void sendBit(const unsigned char bit);

    /**
     * Internal function, sends single byte to PT6302
     * @param byte Byte to send
     */
    void sendByte(uint8_t byte);

    /**
     * Internal function, sends series of bytes to PT6302
     * @param bytes Sequential bytes to send
     * @param length Number of bytes in the array (param bytes)
     */
    void sendBytes(uint8_t bytes[], size_t length);

    /**
     * Internal function, swaps MSB LSB because the PT6302 needs it LSB first
     * @param in Byte to be rotated
     * @returns the rotated byte
     */
    uint8_t rotateByte(uint8_t in);

    /**
     * Internal function, rotates string because the display will print it right to left
     * @param in String to be rotated, original will not be affected    
     * @returns the reversed string  
     */
    char *rotateString(const char *in);
};

#endif
