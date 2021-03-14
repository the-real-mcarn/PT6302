# PT6302 Library
Arduino library for communicating with the Princeton Technology PT6302 VFD Driver/Controller IC with Character RAM. Handles all serial communication and supports all internal functions of the PT6302.

## How to use
- Include the library in your sketch using `#include "PT6302.h"`
- Create an instance of the PT6302 class using `PT6302 vfd(CLKB, RSTB, CSB, DIN);`
    - The parameters correspond to the pins the IC is connected to.
- See the `demo.ino` example, which demonstrates all functions and how to use them or check the library file `PT6302.h`.