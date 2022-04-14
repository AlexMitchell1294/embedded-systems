#ifndef DISPLAYUTILS_H_
#define DISPLAYUTILS_H_

#include <msp430.h>
#include "peripherals.h"
#include <stdio.h>
#include <string.h>

unsigned char intToChar(int val);
void displayTime(long unsigned int time);
void displayTemp(float temp);
float convertTemp(float tempC);
void displayAll(float tempC, long unsigned int utc);

#endif /* DISPLAYUTILS_H_ */
