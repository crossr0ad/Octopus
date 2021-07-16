// glcd.c: functions that operate glaphic LCD (GLCD) through Arduino API
#include "glcd.h"

#define pins_RS 10
#define pins_RW 9
#define pins_E 8
#define pins_CS1 11
#define pins_CS2 12
#define pins_RST 13

const uint8_t pins_DB[] = {14, 15, 2, 3, 4, 5, 6, 7};

void selectChip(uint8_t cs) {
    if (cs == 0) {
        digitalWrite(pins_CS1, HIGH);
        digitalWrite(pins_CS2, LOW);
    } else if (cs == 1) {
        digitalWrite(pins_CS1, LOW);
        digitalWrite(pins_CS2, HIGH);
    }
}

void writeBUS(bool rs, /*bool rw,*/ byte dat) {
    digitalWrite(pins_RS, rs);
    for (size_t i = 0; i < 8; i++) {  //
        digitalWrite(pins_DB[i], (dat >> i) & 0x01);
    }
    digitalWrite(pins_E, HIGH);
    digitalWrite(pins_E, LOW);
}

void writeCommand(byte dat) { writeBUS(0, /*0,*/ dat); }

void writeData(byte dat) { writeBUS(1, /*0,*/ dat); }

void setAddress(byte col, byte row) {
    writeBUS(0, /*0,*/ 0x40 | (col & 0x3F));
    writeBUS(0, /*0,*/ 0xB8 | (row & 0x07));
}

void glcdCLS() {
    for (size_t i = 0; i < 2; i++) {
        selectChip(i);
        for (size_t row = 0; row < 8; row++) {
            setAddress(0, row);
            for (size_t col = 0; col < 64; col++) {
                writeData(0);
            }
        }
    }
    setAddress(0, 0);
}

// initGlcd: initialize GLCD
void initGlcd(void) {
    pinMode(pins_RS, OUTPUT);
    pinMode(pins_RW, OUTPUT);
    pinMode(pins_E, OUTPUT);
    pinMode(pins_CS1, OUTPUT);
    pinMode(pins_CS2, OUTPUT);
    pinMode(pins_RST, OUTPUT);

    for (size_t i = 0; i < 8; i++) {
        pinMode(pins_DB[i], OUTPUT)
    };

    delay(30);  // pause 30 ms

    selectChip(0);
    writeCommand(0xC0);  // 1100 0000
    writeCommand(0x3F);  // 0011 1111

    selectChip(1);
    writeCommand(0xC0);  // 1100 0000
    writeCommand(0x3F);  // 0011 1111

    glcdCLS();
}

// display: send buffer to Arduino
void display() {
    selectChip(0);
    for (size_t i = 0; i < 8; i++) {
        setAddress(0, i);
        for (size_t j = 0; j < 64; j++) {
            writeData((byte)buff[i][j]);
        }
    }
    selectChip(1);
    for (size_t i = 0; i < 8; i++) {
        setAddress(0, i);
        for (size_t j = 0; j < 64; j++) {
            writeData((byte)buff[i][64 + j]);
        }
    }
}