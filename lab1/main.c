/************** ECE2049 DEMO CODE ******************/
/**************  25 August 2021   ******************/
/***************************************************/

#include <msp430.h>

/* Peripherals.c and .h are where the functions that implement
 * the LEDs and keypad, etc are. It is often useful to organize
 * your code by putting like functions together in files.
 * You include the header associated with that file(s)
 * into the main file of your project. */
#include "peripherals.h"

// Function Prototypes
void swDelay(char numLoops);
void drawTopRow();
void countDown();
bool decend();
void welcomeScreen();
void shoot(int);
bool roundWon();
void organize();
void roundScreen();

int x[4] =  {20, 40, 60, 80};
int y[5] = {20, 35, 50, 65, 80};

int maxAliens = 4;
int minAliens = 2;
int totalCount = 0;
int rounds = 1;
int aliensLen = 50;

long int decendTimer = 40000;
int speed[10] = {10000,9000,8000,6000,4000,2000,1000,500,250,100};
int speedPointer = 0;
int increment = 1;
long int i = 0;
int j = 0;
int k = 0;

unsigned char xCharOne[4] = {'(', '[', '/', 60};
unsigned char xCharTwo[4] = {')', ']', '\\', 62};
unsigned char disp = 2;
unsigned char dispThree[2];


// Declare globals here

// Main
void main(void)

{
    /*
     * faster as game goes on
     * more aliens as time goes on
     *
     *
     * open loading screen
     * when * keypad button is pressed start game (count down 3-2-1)
     * print random alien icons at top (5 rows 5 columns)
     * alien data structure typedef struct x,y
     * if correct button is pressed delete alien closest to the bottom
     * in that row
     * every set time amount move aliens down
     * refresh move down
     *
     *game over go back to start screen
     * play sound when certain things happen
     * simple [] () /\
     * */

    unsigned char currKey=0;

    WDTCTL = WDTPW | WDTHOLD;
    initLeds();
    configDisplay();
    configKeypad();


//    for(int i=0;i<100;i++){
//        aliens[i]=0;
//    }



    // *** Intro Screen ***
    Graphics_clearDisplay(&g_sContext); // Clear the display

    //Graphics_drawStringCentered(&g_sContext, "SPACE INVADERS", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
    welcomeScreen();

    while(1){
        long int l=0;
        int roundKeeper = 0;


        currKey = getKey();
             if (currKey == '*'){
                 Graphics_clearDisplay(&g_sContext); // Clear the display
                 countDown();

                 drawTopRow();
                 roundKeeper = rounds - 1;
                 while(l < decendTimer){

                     currKey = getKey();
                     if(l%speed[speedPointer] == 0 && l!=0){
                         bool gameLost = decend();
                         l=0;
                         if(gameLost==true) {
                             welcomeScreen();
                             break;
                         }
                         if (roundKeeper > 0){
                             drawTopRow();
                             roundKeeper--;
                         }
                     }
                     if(currKey == 49){
                          shoot(0);
                      }
                     if(currKey == 50){
                         shoot(1);
                     }
                     if(currKey == 51){
                         shoot(2);
                     }
                     if(currKey == 52){
                          shoot(3);
                     }

                     if(roundWon(roundKeeper)){
                         rounds++;
                         if(speedPointer < 9){
                             speedPointer++;
                         }
                         roundKeeper = rounds-1;
                         roundScreen();
                         drawTopRow();
                         l=0;
                     }
                     else if (roundKeeper > 0 && totalCount == 0){
                          drawTopRow();
                          roundKeeper--;
                          l=0;
                      }

                     l++;
                     }
                 }
             }
}


void welcomeScreen(){
    Graphics_clearDisplay(&g_sContext); // Clear the display
    Graphics_drawStringCentered(&g_sContext, "SPACE INVADERS", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
}
bool roundWon(int roundKeeper){
    return (totalCount == 0 && roundKeeper == 0);
}

void roundScreen(){
    Graphics_clearDisplay(&g_sContext); // Clear the display
    Graphics_drawStringCentered(&g_sContext, "NEXT ROUND", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
    BuzzerOn(100);
    i=0;
    while(i<40000){
        i++;
    }
    BuzzerOff();
    Graphics_clearDisplay(&g_sContext); // Clear the display
}

void countDown(){
    Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
    BuzzerOn(50);
    while(i<40000){
        if(i==10000) BuzzerOff();
        i++;
    }
    Graphics_clearDisplay(&g_sContext); // Clear the display

    Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
    i=0;
    BuzzerOn(50);
        while(i<40000){
            if(i==10000) BuzzerOff();
            i++;
        }
    Graphics_clearDisplay(&g_sContext); // Clear the display

    Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
    i=0;
    BuzzerOn(50);
        while(i<40000){
            if(i==10000) BuzzerOff();
            i++;
        }
    BuzzerOff();
    Graphics_clearDisplay(&g_sContext); // Clear the display
}

void drawTopRow(){
      int alienCount = rand() % (maxAliens-minAliens+1)+minAliens;
      for (k = totalCount; k < alienCount+totalCount; k++){
          int location = rand() % 4;
          struct alien newAlien;
          newAlien.x=x[location];
          newAlien.y=y[0];
          newAlien.row = 0;
          newAlien.shape[0] = xCharOne[location];
          newAlien.shape[1] = xCharTwo[location];
          aliens[k] = newAlien;
      }

      for (j = totalCount; j < alienCount+totalCount; j++){
        dispThree[0] = aliens[j].shape[0];
        dispThree[1] = aliens[j].shape[1];
        Graphics_drawStringCentered(&g_sContext, dispThree, disp, aliens[j].x, aliens[j].y, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
      }
      totalCount+=alienCount;
}

void resetArray(){
    struct alien newArray[50] = {};
    totalCount = 0;
        for(j = 0; j<aliensLen; j++){
            newArray[j].x = 0;
            aliens[j] = newArray[j];
        }
}


bool decend(){
    bool ret_val= false;
    Graphics_clearDisplay(&g_sContext);
    BuzzerOn(255);
    for(k=0; k<totalCount; k++){
       struct alien curr = aliens[k];
       curr.row++;
       if(curr.row == 5){
           Graphics_clearDisplay(&g_sContext);
           BuzzerOn(150);
           Graphics_drawStringCentered(&g_sContext, "You Lose", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
           Graphics_flushBuffer(&g_sContext);
           i=0;
           while(i<80000){
               i++;
           }
           resetArray();
           ret_val = true;
           break;
       }
       curr.y = y[curr.row];
       aliens[k] = curr;
       dispThree[0] = aliens[k].shape[0];
       dispThree[1] = aliens[k].shape[1];
       Graphics_drawStringCentered(&g_sContext, dispThree, disp, aliens[k].x, aliens[k].y, TRANSPARENT_TEXT);
   }
   Graphics_flushBuffer(&g_sContext);
   BuzzerOff();

   return ret_val;
}

void drawAll(){
    Graphics_clearDisplay(&g_sContext);
    for(k=0; k<totalCount; k++){
               struct alien curr = aliens[k];
               dispThree[0] = curr.shape[0];
               dispThree[1] = curr.shape[1];
               Graphics_drawStringCentered(&g_sContext, dispThree, disp, aliens[k].x, aliens[k].y, TRANSPARENT_TEXT);
           }
        Graphics_flushBuffer(&g_sContext);
}

void shoot(int key){
    BuzzerOn(200);
    int value = x[key];
    int j;
    int i;
    int newCount = 0;
    //delete all aliens with same x as value
    for(j = 0; j<aliensLen-1; j++){
            struct alien curr = aliens[j];
            if(curr.x == value){
                /* Copy next element value to current element */
                for(i=j; i<totalCount; i++)
                {
                    aliens[i] = aliens[i + 1];
                }
                break;
            }
        }
    for(j = 0; j<aliensLen-1; j++){
        struct alien curr = aliens[j];
        if(curr.x != 0){
            newCount++;
        }
    }
    totalCount = newCount;
    drawAll();
    BuzzerOff();

}


//    for(j = 0; j<totalCount; j++){
//        struct alien curr = aliens[j];
//        if(curr.x != value){
//            newArray[j] = curr;
//            newCount++;
//        }
//    }
//    a = totalCount;
//    //redraw aliens
//    //StateMachine
//    stateMachine(5);
//    totalCount = newCount;
//    for(j = 0; j<newCount; j++){
//        aliens[j] = newArray[j];
//    }
//    stateMachine(4);



//    unsigned char currKey=0, dispSz = 3;
//    unsigned char dispThree[3];
//
//    // Define some local variables
//    float a_flt = 190.68;
//    int  test = 0x0600, i=0;     // In C prefix 0x means the number that follows is in hex
//    long unsigned X= 123456;    // No prefix so number is assumed to be in decimal
//    unsigned char myGrade='A';
//    unsigned char initial='S';
//    //unsigned char your_name[14] = "Your Name Here";
//                                    // What happens when you change the array length?
//                                    // What should it be? Do you need null terminator /n ?
//
//
//    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
//                                 // You can then configure it properly, if desired
//
//    // Some utterly useless instructions -- Step through them
//    // What size does the Code Composer MSP430 Compiler use for the
//    // following variable types? A float, an int, a long integer and a char?
//    a_flt = a_flt*test;
//    X = test+X;
//    test = test-myGrade;    // A number minus a letter?? What's actually going on here?
//                            // What value stored in myGrade (i.e. what's the ASCII code for "A")?
//                            // Thus, what is the new value of test? Explain?
//
//    // Useful code starts here
//    initLeds();
//
//    configDisplay();
//    configKeypad();
//
//    // *** Intro Screen ***
//    Graphics_clearDisplay(&g_sContext); // Clear the display
//
//    // Write some text to the display
//    Graphics_drawStringCentered(&g_sContext, "Welcome", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
//    Graphics_drawStringCentered(&g_sContext, "to", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
//    Graphics_drawStringCentered(&g_sContext, "ECE2049-A21!", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
//
//    // Draw a box around everything because it looks nice
//    Graphics_Rectangle box = {.xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
//    Graphics_drawRectangle(&g_sContext, &box);
//
//    // We are now done writing to the display.  However, if we stopped here, we would not
//    // see any changes on the actual LCD.  This is because we need to send our changes
//    // to the LCD, which then refreshes the display.
//    // Since this is a slow operation, it is best to refresh (or "flush") only after
//    // we are done drawing everything we need.
//    Graphics_flushBuffer(&g_sContext);
//
//    dispThree[0] = ' ';
//    dispThree[2] = ' ';
//
//    while (1)    // Forever loop
//    {
//        // Check if any keys have been pressed on the 3x4 keypad
//        currKey = getKey();
//        if (currKey == '*')
//            BuzzerOn();
//        if (currKey == '#')
//            BuzzerOff();
//        if ((currKey >= '0') && (currKey <= '9'))
//            setLeds(currKey - 0x30);
//
//        if (currKey)
//        {
//            dispThree[1] = currKey;
//            // Draw the new character to the display
//            Graphics_drawStringCentered(&g_sContext, dispThree, dispSz, 48, 55, OPAQUE_TEXT);
//
//            // Refresh the display so it shows the new data
//            Graphics_flushBuffer(&g_sContext);
//
//            // wait awhile before clearing LEDs
//            swDelay(1);
//            setLeds(0);
//        }
//
//    }  // end while (1)


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
