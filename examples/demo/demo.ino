#include "PT6302.h"

PT6302 vfd(2, 3, 4, 5);

void setup()
{
    Serial.begin(9600);

    // Init the display
    Serial.println("Init");
    vfd.init();

    // Set the general purpose output pins
    Serial.println("GPOP set");
    vfd.setGPOP(true, false);

    // Set to normal operation mode
    Serial.println("Mode set");
    vfd.setMode(PT6302::Mode::NORMAL);

    // Set the amount of digits
    Serial.println("Digit set");
    vfd.setDigitNo(16);

    // Set the amount of duty cycles
    Serial.println("Duty set");
    vfd.setDuty(8);

    // Clear the display after a reset otherwise it will show garbage
    Serial.println("Clear");
    vfd.clear();

    // Write some random characters to the display
    Serial.println("DCRAM write");
    uint8_t DCdata[2] = {0xa0, 0xd7};
    vfd.writeDCRAM(3, DCdata, 2);

    delay(5000);

    // Create a new character in ram position 0 and draw it using writeDCRAM()
    Serial.println("CGRAM write");
    uint8_t CGdata[5] = {0x1c, 0x1c, 0x7c, 0x1c, 0x00};
    vfd.writeCGRAM(0, CGdata);
    vfd.writeDCRAM(6, 0);

    delay(5000);

    // Write to the additional ram if you know how to use it
    // Serial.println("ADRAM write");
    // vfd.writeADRAM(8, 0xC0);

    // delay(5000);

    // Print something without overwriting the entire display
    Serial.println("Print no overwrite");
    vfd.print("No Overwrite", false);

    delay(5000);

    // Print something and clear the entire display
    Serial.println("Print overwrite");
    vfd.print("Overwrite", true);

    delay(5000);

    // Print a string at a specific position
    Serial.println("Print at position");
    vfd.print(3, "43", false);

    delay(5000);

    // Clear display after demo
    Serial.println("Clear");
    vfd.clear();
}

void loop()
{
}