// buff.c: functions that operate buffer to be sent to the display
#include "buff.h"

#include "font.h"

unsigned char buff[8][128] = {0};

// initBuffer: initilize buffer to zero
void initBuffer() { memset(buff, 0, sizeof(buff)); }

// dot: put a dot on a display
void dot(int x, int y) {
    if (x > 127 || x < 0 || y > 63 || y < 0) {
        return;
    }
    int c_x = y / 8;
    int c_z = 0x01 << (y % 8);

    if (isBlack) {
        buff[c_x][x] |= c_z;
    } else {
        buff[c_x][x] &= ~c_z;
    }
}

// line: draw a straight line
void line(int x, int y, int length, bool vertical) {
    if (vertical) {
        for (size_t i = 0; i < length; i++) {
            dot(x + i, y);
        }
        return;
    }
    // horizontal
    for (size_t i = 0; i < length; i++) {
        dot(x, y + i);
    }
}

// rect: draw a rectangle
void rect(int x, int y, int width, int height) {
    line(x, y, width, 1);
    line(x, y, height, 0);
    line(x, y + height, width, 1);
    line(x + width, y, height, 0);
    dot(x + width, y + height);
}

// fillRect: draw a filled rectangle
void fillRect(int x, int y, int width, int height) {
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {
            dot(x + i, y + j);
        }
    }
}

void putChar(char ch, int x, int y) { putCell(Font[ch - ' '], x, y); }

void putStr(char* str, int x, int y) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        putChar(str[i], x + i * 6, y);
    }
}
