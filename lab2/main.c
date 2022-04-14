/************** ECE2049 DEMO CODE ******************/
/**************  25 August 2021   ******************/
/***************************************************/
#pragma once
#include <msp430.h>

/* Peripherals.c and .h are where the functions that implement
 * the LEDs and keypad, etc are. It is often useful to organize
 * your code by putting like functions together in files.
 * You include the header associated with that file(s)
 * into the main file of your project. */
#include "song.h"
#include "peripherals.h"

// Function Prototypes
void swDelay(char numLoops);
void welcomeScreen();
void countDown();
void setupNotes();
void runtimerA2(void);
void stoptimerA2();
void setMoveOn(int count);
void resetVar();

// Declare globals here

int timer_cnt = 0;
int last_error=0;
int moveOn = 0;//timer_cnt + moveOnCount;
int state = 0;
int number_state = 0;
int current_note = 0;
int duration = 0;
int error = 0;
bool ok = false;
unsigned char btnPressed;

// Main
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    initButtons();
    initLeds();
    configDisplay();
    configKeypad();
    setupNotes();
    //__enable_interrupt();
    _BIS_SR(GIE);
    while(1){
        if(getKey() == '#') {
            resetVar();
        }
        switch(state){
            case 0://intro screen
                Graphics_drawStringCentered(&g_sContext, "MSP430 Hero", AUTO_STRING_LENGTH, 48, 30, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, "Press * to start", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, "Press # to quit", AUTO_STRING_LENGTH, 48, 60, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                if (getKey() == '*'){
                    Graphics_clearDisplay(&g_sContext); // Clear the display
                    state = 1;
                    number_state = 0;
                    runtimerA2();
                    setMoveOn(200);
                }
                break;
            case 1://count down
                switch(number_state){
                    case 0:
                        Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
                        Graphics_flushBuffer(&g_sContext);
                        turnLedsOn(0x08);
                        if(timer_cnt >= moveOn){
                            Graphics_clearDisplay(&g_sContext); // Clear the display
                            number_state = 1;
                            setMoveOn(200);
                        }
                        break;
                    case 1:
                        Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
                        Graphics_flushBuffer(&g_sContext);
                        turnLedsOn(0x0C);
                        if(timer_cnt >= moveOn){
                            Graphics_clearDisplay(&g_sContext); // Clear the display
                            number_state = 2;
                            setMoveOn(200);
                        }
                        break;
                    case 2:
                        Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
                        Graphics_flushBuffer(&g_sContext);
                        turnLedsOn(0x0E);
                        if(timer_cnt >= moveOn){
                            Graphics_clearDisplay(&g_sContext); // Clear the display
                            number_state = 3;
                            setMoveOn(200);
                        }
                        break;
                    case 3:
                        Graphics_drawStringCentered(&g_sContext, "GO", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
                        Graphics_flushBuffer(&g_sContext);
                        turnLedsOn(0x0F);
                        if(timer_cnt >= moveOn){
                            Graphics_clearDisplay(&g_sContext); // Clear the display
                            number_state = 1;
                            turnLedsOn(0x00);
                            runtimerA2();
                            state = 2;
                        }
                        break;
                }
                break;
            case 2://play game
                duration = lightBringer[current_note].duration;
                turnLedsOn(lightBringer[current_note].led);
                if(moveOn <= timer_cnt){
                    if (ok == false){
                        error++;
                    }
                    moveOn = timer_cnt + duration;
                    current_note++;
                    progressOff();
                    ok = false;
                }
                else if(moveOn >= timer_cnt){
                    playNote(lightBringer[current_note].pitch);
                }
                if (lightBringer_len < current_note){
                    setMoveOn(1000);
                    state = 5;
                }
                if(getPressed() == lightBringer[current_note].led){
                    ok = true;
                    noteHit();
                }
                else if(getPressed() == 0x00){

                }
                else{
                    if(timer_cnt-last_error > 50){
                    error++;
                    badHit();
                    }
                    last_error = timer_cnt;
                }
                if(error >= 10){
                    setMoveOn(1000);
                    state = 4;
                }
                break;
            case 4://lose
                BuzzerOff();
                turnLedsOn(0x00);
                Graphics_drawStringCentered(&g_sContext, "The crowd boo'd", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, "you off stage!!!", AUTO_STRING_LENGTH, 48, 60, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                if(moveOn < timer_cnt){
                    resetVar();
                }
                break;
            case 5://win
                BuzzerOff();
                turnLedsOn(0x00);
                Graphics_drawStringCentered(&g_sContext, "They Loved you", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, "woo", AUTO_STRING_LENGTH, 48, 60, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                if(moveOn < timer_cnt){
                   resetVar();
                }
                break;

        }
    }
}

void resetVar(){
    moveOn = 0;//timer_cnt + moveOnCount;
    state = 0;
    number_state = 0;
    current_note = 0;
    duration = 0;
    ok = false;
    error=0;
    last_error=0;
    BuzzerOff();
    turnLedsOn(0x00);
    Graphics_clearDisplay(&g_sContext); // Clear the display
    stoptimerA2();
}

void setMoveOn(int count){
    moveOn = timer_cnt + count;
}

void setupNotes(){
    int i = 0;
    for (i=0; i<14; i++){
        note curr;
        curr.led = 0x02;
        curr.duration = 500;
        curr.pitch = noteFrequency[i];
        notes[i] = curr;

    }
}

void runtimerA2(void) {
    // 0.005 TIMER RESOLUTION
    // 200 counts = 1 second
    // counts = seconds / resolution
    TA2CTL = TASSEL_2 + MC_1 + ID_0;
    TA2CCR0 = 5241;
    TA2CCTL0 = CCIE;     // TA2CCR0 interrupt enabled
}

void stoptimerA2() {
        TA2CTL = MC_0;        // stop timer
        TA2CCTL0 &= ~CCIE;    // TA2CCR0 interrupt disabled
        timer_cnt=0;
}

#pragma vector=TIMER2_A0_VECTOR
__interrupt void TimerA2_ISR (void) {
        timer_cnt++;
        if (timer_cnt == 60000) timer_cnt = 0;
}

void swDelay(char numLoops)
{
	// This function is a software delay. It performs
	// useless loops to waste a bit of time
	//
	// Input: numLoops = number of delay loops to execute
	// Output: none
	//
	// smj, ECE2049, 25 Aug 2021

	volatile unsigned int i,j;	// volatile to prevent removal in optimization
			                    // by compiler. Functionally this is useless code

	for (j=0; j<numLoops; j++)
    {
    	i = 50000 ;					// SW Delay
   	    while (i > 0)				// could also have used while (i)
	       i--;
    }
}
