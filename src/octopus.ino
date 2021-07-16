#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "background.h"
#include "buff.h"
#include "glcd.h"
#include "octopus_body.h"
#include "octopus_legs.h"
#include "player.h"

bool isBlack;

unsigned int playerLocation = 0;  // location of the player from 0 to 4

void octopus(int i, int j, bool isisBlack) {
    bool temp = isBlack;
    isBlack = isisBlack;
    switch (i) {
        case 0:
            switch (j) {
                case 0:
                    putCell(octfoot11, 40, 8);
                    break;
                case 1:
                    putCell(octfoot14, 35, 13);
                    break;
                case 2:
                    putCell(octfoot15, 31, 19);
                    break;
                case 3:
                    putCell(octfoot16, 28, 25);
                    break;
                default:
                    break;
            }
            break;
        case 1:
            switch (j) {
                case 0:
                    putCell(octfoot21, 55, 17);
                    break;
                case 1:
                    putCell(octfoot22, 55, 23);
                    break;
                case 2:
                    putCell(octfoot23, 51, 28);
                    break;
                case 3:
                    putCell(octfoot24, 49, 36);
                    break;
                case 4:
                    putCell(octfoot35, 43, 43);
                    break;
                default:
                    break;
            }
            break;
        case 2:
            switch (j) {
                case 0:
                    putCell(octfoot31, 75, 25);
                    break;
                case 1:
                    putCell(octfoot32, 76, 32);
                    break;
                case 2:
                    putCell(octfoot33, 78, 39);
                    break;
                case 3:
                    putCell(octfoot34, 79, 46);
                    break;
                default:
                    break;
            }
            break;
        case 3:
            switch (j) {
                case 0:
                    putCell(octfoot41, 105, 35);
                    break;
                case 1:
                    putCell(octfoot43, 108, 43);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    isBlack = temp;

    /*putCell(octfoot42, 108, 42); ERASED*/
}

void initOctopus(bool temp) {
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 5; j++) {
            octopus(i, j, temp);
        }
    }
}

const int footlocateMax[] = {3, 4, 3, 1};
int footlocate[] = {0, 0, 0, 0};

void _random() {
    long OF[4] = {random(4), random(5), random(4), 1};
    for (size_t i = 0; i <= 3; i++) {
        for (size_t j = 0; j <= OF[i]; j++) {
            octopus(i, j, true);
        }
        footlocate[i] = OF[i];
    }
}

bool isExtend[] = {false, false, false, false};

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
        octopus(i, footlocate[i] + 1, true);
        footlocate[i]++;
    } else {
        octopus(i, footlocate[i], false);
        footlocate[i]--;
    }
    if (footlocate[i] == temp || footlocate[i] == -1) {
        isExtend[i] = !isExtend[i];
    }
}

void player(int location, bool isisBlack) {
    bool temp = isBlack;
    isBlack = isisBlack;
    switch (location) {
        case 0:
            putCell(Man1, 2, 3);
            putCell(ManHukuro1, 22, 14);
            break;
        case 1:
            putCell(Man2, 8, 27);
            putCell(ManHukuro2, 21, 44);
            break;
        case 2:
            putCell(Man3, 25, 44);
            putCell(ManHukuro3, 42, 58);
            break;
        case 3:
            putCell(Man4, 60, 43);
            putCell(ManHukuro4, 75, 55);
            break;
        case 4:
            putCell(Man5, 90, 44);
            putCell(ManHukuro5, 82, 51);
            break;
        case 7:
            putCell(ManTe1, 91, 54);
            break;
        case 5:
            putCell(ManTe2, 103, 55);
            break;
        case 6:
            putCell(ManTe3, 106, 54);
            break;
        default:
            break;
    }
    isBlack = temp;
}

void movePlayer(bool isRight) {
    if (playerLocation == 0 && !isRight) {
        return;
    }

    if (playerLocation == 3 && isRight) {
        player(5, true);
    } else if (playerLocation == 4 && !isRight) {
        player(5, false);
        player(6, false);
        player(7, false);
    }

    int i = isRight ? 1 : -1;
    player(playerLocation, false);
    player(playerLocation + i, true);

    playerLocation += i;
    tone(18, 523, 30);
}

// capture: show player captured by octopus
void capture() {
    player(playerLocation, false);
    player(5, false);
    player(6, false);
    player(7, false);

    octopus(1, 0, true);
    octopus(1, 1, true);
    octopus(1, 2, false);
    octopus(1, 3, false);
    octopus(1, 4, false);
    octopus(2, 0, true);
    octopus(2, 1, true);
    octopus(2, 2, false);
    octopus(2, 3, false);
    isBlack = false;
    fillRect(109, 0, 19, 9);
    isBlack = true;

    // putCell(octfoot25,50,50);
    putCell(ManHHead92, 64, 22);
    putCell(ManHMTe93, 60, 26);  //実際は触手
    putCell(ManHHTe91, 72, 17);  //左手

    putCell(ManHMTe99, 70, 34);
    putCell(ManHHFoot97, 85, 25);
    putCell(ManHMFoot95, 85, 33);
}

void gameOver() {
    const int audiopin = 18;

    tone(audiopin, 740, 200);
    delay(110);
    noTone(audiopin);
    tone(audiopin, 698, 200);
    delay(110);
    noTone(audiopin);
    tone(audiopin, 622, 200);
    delay(110);
    noTone(audiopin);
    tone(audiopin, 554, 200);
    delay(110);
    noTone(audiopin);
    tone(audiopin, 622, 200);
    delay(110);
    noTone(audiopin);
    tone(audiopin, 466, 200);
    delay(110);
    noTone(audiopin);
    tone(audiopin, 523, 200);
    delay(110);
    noTone(audiopin);
    delay(80);
    tone(audiopin, 415, 200);
    delay(110);
    noTone(audiopin);
    delay(80);
    tone(audiopin, 311, 200);
    delay(110);
    noTone(audiopin);
    tone(audiopin, 349, 200);
    delay(110);
    noTone(audiopin);
    tone(audiopin, 370, 200);
    delay(110);
    noTone(audiopin);
    delay(110);
    tone(audiopin, 349, 200);
    delay(110);
    noTone(audiopin);
    delay(110);
    tone(audiopin, 277, 200);
    delay(110);
    noTone(audiopin);
    delay(110);
    tone(audiopin, 311, 280);
    delay(1470);
    noTone(audiopin);
    delay(90);
}

void struggle(bool color) {
    isBlack = color;
    putCell(ManHMTe99, 70, 34);
    putCell(ManHHFoot97, 85, 25);
    putCell(ManHMFoot95, 85, 33);
    // putStr("WAKI", 0, 1);

    isBlack = !color;
    putCell(ManHHTe96, 63, 34);  //実際は右手
    putCell(ManHHFoot94, 85, 30);
    putCell(ManHMFoot98, 83, 36);
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

unsigned int score = 0;
unsigned int count = 0;
int situation = 0;  ///左右のボタンの押下状況
int nishioCount = 0;  ///最左で右を押したときに手を動かすためのカウント
int nishiokaNum = 0;
bool mandilect = true;
bool isCaptured = false;
bool isCatching = false;
bool isGameOver = false;

void setup() {
    initGlcd();
    pinMode(16, INPUT);
    pinMode(17, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    randomSeed(analogRead(4));

    initBuffer();
    isBlack = true;
    // putStr("hello, Nishioka!",10,10);
    putCell(octbodyU, 50, -5);
    putCell(octbodyL, 50, 27);
    putCell(Haikei1, 0, 34);
    putCell(Haikei2, 0, 19);
    putCell(Haikei3, 107, 60);

    isBlack = false;
    fillRect(109, 0, 19, 9);
    isBlack = true;

    // initOctopus(true);
    player(playerLocation, true);
    count = 1;
    display();

    _random();
    tone(18, 784, 30);
    delay(100);
    noTone(18);
    tone(18, 1047, 30);
}

void loop() {
    // an interval (ms) of player hand movement to get treasure
    const int playerHandInterval = 80;

    // an interval (ms) between legs movement of the octopus
    // the higher score, the shorter the interval
    const int octoLegsInterval = 800 - score * 10;

    // number of milliseconds of running
    unsigned long time = millis();

    // string that contains formatted score
    // if score > 999, it will overflow
    char scoreStr[4];
    snprintf(scoreStr, 4, "%03d", score);

    // Serial.println(time / octoLegsInterval);
    isBlack = false;
    fillRect(109, 0, 19, 9);
    isBlack = true;
    putStr(scoreStr, 110, 1);

    Serial.println(scoreStr);

    if (digitalRead(16) || digitalRead(17)) {
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    if (isCaptured) {
        if (!isGameOver) {
            gameOver();
            isGameOver = true;
        }
        if (time / octoLegsInterval != count) {
            struggle(count % 2);
            count = time / octoLegsInterval;
        }
    } else {
        if ((footlocate[playerLocation - 1] ==
             footlocateMax[playerLocation - 1]) ||
            (playerLocation > 4 &&
             footlocate[3] ==
                 1)) {  // footlocateの添字0にあたるのはplayerLocation=1
            // fillRect(4,57,1*count,4);
            capture();
            putStr("GAME OVER", 50, 50);
            isCaptured = true;
        }
        if (nishioCount >= 1 && time / playerHandInterval != nishiokaNum) {
            if (nishioCount == 1) {
                player(6, false);
                player(5, true);
                nishioCount++;
                nishiokaNum = millis() / playerHandInterval;
            } else if (nishioCount == 2) {
                player(5, false);
                player(7, true);
                score++;
                nishioCount++;
                nishiokaNum = millis() / playerHandInterval;
            } else if (nishioCount == 3) {
                player(5, true);
                player(7, false);
                nishioCount = 0;
                tone(18, 784, 30);
                delay(100);
                noTone(18);
                tone(18, 1047, 30);
            }

        } else if (!digitalRead(17) && nishioCount == 0) {
            if (situation == 0) {
                if (playerLocation == 4) {
                    player(5, false);
                    player(6, true);
                    nishioCount = 1;
                    nishiokaNum = millis() / playerHandInterval;
                } else {
                    movePlayer(true);
                }
                situation = 2;
            }
        } else if (!digitalRead(16) && situation == 0) {
            movePlayer(false);
            situation = 1;
        }
        if (digitalRead(16) && digitalRead(17)) {
            situation = 0;
        }

        if (time / octoLegsInterval != count) {
            if (count % 2) {
                moveOct(0);
                moveOct(2);
            } else {
                moveOct(1);
                moveOct(3);
            }
            /*movePlayer(mandilect);
              if(playerLocation==0||playerLocation==7) mandilect=!mandilect;*/
            count = time / octoLegsInterval;
        }
    }

    display();
}
