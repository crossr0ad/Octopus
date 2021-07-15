#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "background.h"
#include "font.h"
#include "octopus_body.h"
#include "octopus_legs.h"
#include "player.h"

byte pins_DB[] = {14, 15, 2, 3, 4, 5, 6, 7};

#define pins_RS 10
#define pins_RW 9
#define pins_E 8
#define pins_CS1 11
#define pins_CS2 12
#define pins_RST 13

bool isBlack;
bool isExtend[] = {false, false, false, false};

int score = 0,      /// スコア
    manLocate = 0;  /// PCの位置（0~4）

void chipSelect(boolean cs) {
    if (cs == 0) {
        digitalWrite(pins_CS1, HIGH);
        digitalWrite(pins_CS2, LOW);
    } else {
        digitalWrite(pins_CS1, LOW);
        digitalWrite(pins_CS2, HIGH);
    }
}
void writeBUS(boolean rs, boolean rw, byte dat) {
    digitalWrite(pins_RS, rs);
    for (int i = 0; i < 8; i++) {  //
        digitalWrite(pins_DB[i], (dat >> i) & 0x01);
    }
    digitalWrite(pins_E, HIGH);
    digitalWrite(pins_E, LOW);
}
void writeCommand(byte dat) { writeBUS(0, 0, dat); }
void writeData(byte dat) { writeBUS(1, 0, dat); }
void setAddress(byte col, byte row) {
    writeBUS(0, 0, 0x40 | (col & 0x3F));
    writeBUS(0, 0, 0xB8 | (row & 0x07));
}
void initGlcd(void) {
    pinMode(pins_RS, OUTPUT);
    pinMode(pins_RW, OUTPUT);
    pinMode(pins_E, OUTPUT);
    pinMode(pins_CS1, OUTPUT);
    pinMode(pins_CS2, OUTPUT);
    pinMode(pins_RST, OUTPUT);
    for (int i = 0; i < 8; i++) pinMode(pins_DB[i], OUTPUT);
    delay(30);
    chipSelect(0);
    writeCommand(0xC0);
    writeCommand(0x3F);
    chipSelect(1);
    writeCommand(0xC0);  // 1100 0000
    writeCommand(0x3F);  // 0011 1111
}
void glcdCLS() {
    byte col, row, i;
    for (i = 0; i < 2; i++) {
        chipSelect(i);
        for (row = 0; row < 8; row++) {
            setAddress(0, row);
            for (col = 0; col < 64; col++) {
                writeData(0);
            }
        }
    }
    setAddress(0, 0);
}

unsigned char buff[8][128];

void InitBuff() {
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 128; c++) buff[r][c] = 0;
}
void dot(int x, int y) {
    if (x > 127 || x < 0 || y > 63 || y < 0) return;
    int c_x = y / 8;
    int c_z = 0;
    c_z = 0x01 << (y % 8);

    if (isBlack)
        buff[c_x][x] |= c_z;
    else
        buff[c_x][x] &= ~c_z;
}
void disp() {
    chipSelect(0);
    for (int i = 0; i < 8; i++) {
        setAddress(0, i);
        for (int j = 0; j < 64; j++) {
            writeData((byte)buff[i][j]);
        }
    }
    chipSelect(1);
    for (int i = 0; i < 8; i++) {
        setAddress(0, i);
        for (int j = 64; j < 128; j++) {
            writeData((byte)buff[i][j]);
        }
    }
}
void bar(int x, int y, int len, bool sv) {  /// sv=1…よこ,0…たて
    if (sv)
        for (int i = 0; i < len; i++) dot(x + i, y);
    else
        for (int i = 0; i < len; i++) dot(x, y + i);
}
void rect(int x, int y, int width, int height) {
    bar(x, y, width, 1);
    bar(x, y, height, 0);
    bar(x, y + height, width, 1);
    bar(x + width, y, height, 0);
    dot(x + width, y + height);
}
void fill(int x, int y, int width, int height) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            dot(x + i, y + j);
        }
    }
}
#define cellput(img, x, y)                           \
    {                                                \
        int height = sizeof(img[0]) * 8;             \
        int width = sizeof(img) / (height / 8);      \
                                                     \
        if (height == 8) {                           \
            uint8_t ln;                              \
            for (int i = 0; i < width; i++) {        \
                ln = pgm_read_byte_near(&(img[i]));  \
                for (int j = 0; j < 8; j++) {        \
                    if (ln & (1 << j)) {             \
                        dot(x + i, y + j);           \
                    }                                \
                }                                    \
            }                                        \
        } else if (height == 16) {                   \
            uint16_t ln;                             \
            for (int i = 0; i < width; i++) {        \
                ln = pgm_read_word_near(&(img[i]));  \
                for (int j = 0; j < 16; j++) {       \
                    if (ln & (((uint16_t)1) << j)) { \
                        dot(x + i, y + j);           \
                    }                                \
                }                                    \
            }                                        \
        } else if (height == 32) {                   \
            uint32_t ln;                             \
            for (int i = 0; i < width; i++) {        \
                ln = pgm_read_dword_near(&(img[i])); \
                for (int j = 0; j < 32; j++) {       \
                    if (ln & (((uint32_t)1) << j)) { \
                        dot(x + i, y + j);           \
                    }                                \
                }                                    \
            }                                        \
        }                                            \
    }

void putCh(int ch, int x, int y) { cellput(Font[ch - ' '], x, y); }
void putStr(char ch[], int x, int y) {
    for (int i = 0; ch[i] != '\0'; i++) {
        putCh(ch[i], x + i * 6, y);
    }
}

void oct(int i, int j, bool isisBlack) {
    bool temp = isBlack;
    isBlack = isisBlack;
    switch (i) {
        case 0:
            switch (j) {
                case 0:
                    cellput(octfoot11, 40, 8);
                    break;
                case 1:
                    cellput(octfoot14, 35, 13);
                    break;
                case 2:
                    cellput(octfoot15, 31, 19);
                    break;
                case 3:
                    cellput(octfoot16, 28, 25);
                    break;
                default:
                    break;
            }
            break;
        case 1:
            switch (j) {
                case 0:
                    cellput(octfoot21, 55, 17);
                    break;
                case 1:
                    cellput(octfoot22, 55, 23);
                    break;
                case 2:
                    cellput(octfoot23, 51, 28);
                    break;
                case 3:
                    cellput(octfoot24, 49, 36);
                    break;
                case 4:
                    cellput(octfoot35, 43, 43);
                    break;
                default:
                    break;
            }
            break;
        case 2:
            switch (j) {
                case 0:
                    cellput(octfoot31, 75, 25);
                    break;
                case 1:
                    cellput(octfoot32, 76, 32);
                    break;
                case 2:
                    cellput(octfoot33, 78, 39);
                    break;
                case 3:
                    cellput(octfoot34, 79, 46);
                    break;
                default:
                    break;
            }
            break;
        case 3:
            switch (j) {
                case 0:
                    cellput(octfoot41, 105, 35);
                    break;
                case 1:
                    cellput(octfoot43, 108, 43);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    isBlack = temp;

    /*cellput(octfoot42, 108, 42); ERASED*/
}

void octInit(bool temp) {
    for (int j = 0; j < 4; j++)
        for (int k = 0; k < 5; k++) oct(j, k, temp);
}

const int footlocateMax[] = {3, 4, 3, 1};
int footlocate[] = {0, 0, 0, 0};

void Random() {
    int i, j;
    long OF[4] = {random(4), random(5), random(4), 1};
    for (i = 0; i <= 3; i++) {
        for (j = 0; j <= OF[i]; j++) {
            oct(i, j, true);
        }
        footlocate[i] = OF[i];
    }
}
void moveOct(int i) {
    int temp;
    switch (i) {
        case 0:
            temp = 3;
            break;
        case 1:
            temp = 4;
            break;
        case 2:
            temp = 3;
            break;
        case 3:
            temp = 1;
            break;
    }
    if (isExtend[i]) {
        oct(i, footlocate[i] + 1, true);
        footlocate[i]++;
    } else {
        oct(i, footlocate[i], false);
        footlocate[i]--;
    }
    if (footlocate[i] == temp || footlocate[i] == -1) {
        isExtend[i] = !isExtend[i];
    }
}

void man(int i, bool isisBlack) {
    bool temp = isBlack;
    isBlack = isisBlack;
    switch (i) {
        case 0:
            cellput(Man1, 2, 3);
            cellput(ManHukuro1, 22, 14);
            break;
        case 1:
            cellput(Man2, 8, 27);
            cellput(ManHukuro2, 21, 44);
            break;
        case 2:
            cellput(Man3, 25, 44);
            cellput(ManHukuro3, 42, 58);
            break;
        case 3:
            cellput(Man4, 60, 43);
            cellput(ManHukuro4, 75, 55);
            break;
        case 4:
            cellput(Man5, 90, 44);
            cellput(ManHukuro5, 82, 51);
            break;
        case 7:
            cellput(ManTe1, 91, 54);
            break;
        case 5:
            cellput(ManTe2, 103, 55);
            break;
        case 6:
            cellput(ManTe3, 106, 54);
            break;
        default:
            break;
    }
    isBlack = temp;
}

void moveMan(bool isRight) {
    int i;
    if (manLocate != 0 || isRight) {
        if (isRight)
            i = 1;
        else
            i = -1;

        if (manLocate == 3 && isRight) {
            man(5, true);
        } else if (manLocate == 4 && !isRight) {
            man(5, false);
            man(6, false);
            man(7, false);
        }
        man(manLocate, false);
        man(manLocate + i, true);

        manLocate += i;
        tone(18, 523, 30);
    }
}

void capture() {
    man(manLocate, false);
    man(5, false);
    man(6, false);
    man(7, false);
    oct(1, 0, true);
    oct(1, 1, true);
    oct(1, 2, false);
    oct(1, 3, false);
    oct(1, 4, false);
    oct(2, 0, true);
    oct(2, 1, true);
    oct(2, 2, false);
    oct(2, 3, false);
    isBlack = false;
    fill(109, 0, 19, 9);
    isBlack = true;
    // cellput(octfoot25,50,50);
    cellput(ManHHead92, 64, 22);
    cellput(ManHMTe93, 60, 26);  //実際は触手
    cellput(ManHHTe91, 72, 17);  //左手

    cellput(ManHMTe99, 70, 34);
    cellput(ManHHFoot97, 85, 25);
    cellput(ManHMFoot95, 85, 33);
}

void game_over() {
    tone(18, 740, 200);
    delay(110);
    noTone(18);
    tone(18, 698, 200);
    delay(110);
    noTone(18);
    tone(18, 622, 200);
    delay(110);
    noTone(18);
    tone(18, 554, 200);
    delay(110);
    noTone(18);
    tone(18, 622, 200);
    delay(110);
    noTone(18);
    tone(18, 466, 200);
    delay(110);
    noTone(18);
    tone(18, 523, 200);
    delay(110);
    noTone(18);
    delay(80);
    tone(18, 415, 200);
    delay(110);
    noTone(18);
    delay(80);
    tone(18, 311, 200);
    delay(110);
    noTone(18);
    tone(18, 349, 200);
    delay(110);
    noTone(18);
    tone(18, 370, 200);
    delay(110);
    noTone(18);
    delay(110);
    tone(18, 349, 200);
    delay(110);
    noTone(18);
    delay(110);
    tone(18, 277, 200);
    delay(110);
    noTone(18);
    delay(110);
    tone(18, 311, 280);
    delay(1470);
    noTone(18);
    delay(90);
}

void manstruggle(bool color) {
    isBlack = color;
    cellput(ManHMTe99, 70, 34);
    cellput(ManHHFoot97, 85, 25);
    cellput(ManHMFoot95, 85, 33);
    // putStr("WAKI", 0, 1);

    isBlack = !color;
    cellput(ManHHTe96, 63, 34);  //実際は右手
    cellput(ManHHFoot94, 85, 30);
    cellput(ManHMFoot98, 83, 36);
    putStr("GAME OVER", 50, 50);
    // putStr("WAKI", 0, 1);

    if (color) {
        isBlack = false;
        putStr("GAME OVER", 50, 50);
        // putStr("MIY", 110, 1);
        isBlack = true;
        // putStr("NISHIOKA", 50, 50);
        // putStr("FUC", 110, 1);
    } else {
        isBlack = false;
        // putStr("FUC", 110, 1);
        // putStr("NISHIOKA", 50, 50);
        isBlack = true;
        // putStr("MIY", 110, 1);
        // putStr("GAME OVER", 50, 50);
    }

    // if(color) Serial.println("Captured!!");
    // else Serial.println("Captured!!!");
}

int count = 0,
    situation = 0,  ///左右のボタンの押下状況
    nishioCount = 0,  ///最左で右を押したときに手を動かすためのカウント
    nishiokaNum = 0,    ///
    interval = 800,     ///足の動く間隔(ms)
    man4interval = 80;  /// PCが最左にいるときの
unsigned long time;
bool mandilect = true, isCaptured = false, isCatching = false, isGO = false;
char scoreStr[4];

void setup() {
    initGlcd();
    glcdCLS();
    pinMode(16, INPUT);
    pinMode(17, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    randomSeed(analogRead(4));

    InitBuff();
    isBlack = true;
    // putStr("hello, Nishioka!",10,10);
    cellput(octbodyU, 50, -5);
    cellput(octbodyL, 50, 27);
    cellput(Haikei1, 0, 34);
    cellput(Haikei2, 0, 19);
    cellput(Haikei3, 107, 60);

    isBlack = false;
    fill(109, 0, 19, 9);
    isBlack = true;

    // octInit(true);
    man(manLocate, true);
    count = 1;
    disp();

    Random();
    tone(18, 784, 30);
    delay(100);
    noTone(18);
    tone(18, 1047, 30);
}
void loop() {
    interval = 800 - score * 10;
    time = millis();
    snprintf(scoreStr, 4, "%03d", score);

    // Serial.println(time / interval);
    isBlack = false;
    fill(109, 0, 19, 9);
    isBlack = true;
    putStr(scoreStr, 110, 1);

    Serial.println(scoreStr);

    if (digitalRead(16) || digitalRead(17))
        digitalWrite(LED_BUILTIN, LOW);
    else
        digitalWrite(LED_BUILTIN, HIGH);
    if (isCaptured) {
        if (!isGO) {
            game_over();
            isGO = true;
        }
        if (time / interval != count) {
            manstruggle(count % 2);
            count = time / interval;
        }
    } else {
        if ((footlocate[manLocate - 1] == footlocateMax[manLocate - 1]) ||
            (manLocate > 4 &&
             footlocate[3] == 1)) {  // footlocateの添字0にあたるのはmanLocate=1
            // fill(4,57,1*count,4);
            capture();
            putStr("GAME OVER", 50, 50);
            isCaptured = true;
        }
        if (nishioCount >= 1 && time / man4interval != nishiokaNum) {
            if (nishioCount == 1) {
                man(6, false);
                man(5, true);
                nishioCount++;
                nishiokaNum = millis() / man4interval;
            } else if (nishioCount == 2) {
                man(5, false);
                man(7, true);
                score++;
                nishioCount++;
                nishiokaNum = millis() / man4interval;
            } else if (nishioCount == 3) {
                man(5, true);
                man(7, false);
                nishioCount = 0;
                tone(18, 784, 30);
                delay(100);
                noTone(18);
                tone(18, 1047, 30);
            }

        } else if (!digitalRead(17) && nishioCount == 0) {
            if (situation == 0) {
                if (manLocate == 4) {
                    man(5, false);
                    man(6, true);
                    nishioCount = 1;
                    nishiokaNum = millis() / man4interval;
                } else
                    moveMan(true);
                situation = 2;
            }
        } else if (!digitalRead(16) && situation == 0) {
            moveMan(false);
            situation = 1;
        }
        if (digitalRead(16) && digitalRead(17)) {
            situation = 0;
        }

        if (time / interval != count) {
            if (count % 2) {
                moveOct(0);
                moveOct(2);
            } else {
                moveOct(1);
                moveOct(3);
            }
            /*moveMan(mandilect);
              if(manLocate==0||manLocate==7) mandilect=!mandilect;*/
            count = time / interval;
        }
    }

    disp();
}
