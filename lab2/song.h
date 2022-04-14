/*
 * song.h
 *
 *  Created on: Sep 27, 2021
 *      Author: darth
 */

#ifndef SONG_H_
#define SONG_H_
#include <msp430.h>
#include "grlib.h"
#include "peripherals.h"

typedef struct{
    unsigned char led;
    int pitch;
    int duration;
} note;
#define noteA 440
#define noteBb 466
#define noteB 494
#define noteC 523
#define noteCs 554
#define noteD 587
#define noteEb 622
#define noteE 659
#define noteF 698
#define noteFs 740
#define noteG 784
#define noteAb 831
#define noteA2 880
//a-b led 1
#define led1 0x01
//c-d led 2
#define led2 0x02
//eb-f# led 3
#define led3 0x04
//g-a2 led 4
#define led4 0x08

#define oneSec  200
#define twoSec 400
#define halfSec 100

note notes[14]= {};
char noteChar[14][2] = {"A","Bb", "B", "C", "C#", "D", "Eb", "E", "F", "F#",
                    "G", "Ab", "A"};
int noteFrequency[14] = {440, 466, 494, 523, 554, 587, 622, 659, 698, 740,
                         784, 831, 880};
note lightBringer[] = {
                       {led1,noteAb,oneSec},//skip
                       {led1, noteAb, oneSec },
                       {led4, noteG, halfSec},
                       {led3, noteF, oneSec},
                       {led1, noteAb, oneSec},
                       {led3, noteFs, oneSec},
                       {led3, noteE, oneSec},
                       {led3, noteEb, oneSec},
                       {led1, noteAb, oneSec},
                       {led4, noteG, halfSec},
                       {led3, noteF, oneSec},
                       {led1, noteAb, oneSec},
                       {led4, noteG, halfSec},
                       {led3, noteF, oneSec},
                       {led1, noteAb, oneSec},
                       {led3, noteFs, oneSec},
                       {led3, noteE, oneSec},
                       {led3, noteEb, oneSec},
                       {led1, noteAb, oneSec},
                       {led4, noteG, halfSec},
                       {led3, noteF, oneSec},
                       {led1, noteAb, oneSec},
                       {led1, noteAb, oneSec },
                       {led4, noteG, halfSec},
                       {led3, noteF, oneSec},
                       {led2, noteC, oneSec},
                       {led2, noteCs, twoSec},
                       {led2, noteD, halfSec},
                       {led2, noteC, twoSec},
                       {led1, noteAb, oneSec },
                       {led4, noteG, halfSec},
                       {led3, noteF, oneSec},
};
int lightBringer_len = 30;


#endif /* SONG_H_ */
