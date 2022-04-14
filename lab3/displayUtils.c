#include <displayUtils.h>


//days for each month
const int month_AmountDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};



void displayTime(long unsigned int time){
    unsigned char date[4]="    ";
    unsigned char days[2];
    unsigned char hours[3] = "  :";
    unsigned char mins[3] = "  :";
    unsigned char secs[2];
    long unsigned int remain = 0;
    long unsigned int day = 0;
    long unsigned int hour = 0;
    long unsigned int min = 0;
    long unsigned int sec = 0;
    day = time / 86400 + 1;
    remain = time-(day-1)*86400;
    hour = remain / 3600;
    remain = remain - hour*3600;
    min = remain/60;
    remain = remain - min*60;
    sec = remain;
//    day = day + time/60/60/24;
//    hour = day*60*60*24;
//    hour = time - hour + 86400;
//    hour = hour/60/60;



    int month = 0;
    while(day>month_AmountDays[month]){
        day -= month_AmountDays[month];
        month++;
    }
    switch(month){
          case 0:
              date[0] = 'J';
              date[1] = 'a';
              date[2] = 'n';
              break;
          case 1:
              date[0] = 'F';
              date[1] = 'e';
              date[2] = 'b';
              break;
          case 2:
              date[0] = 'M';
              date[1] = 'a';
              date[2] = 'r';
              break;
          case 3:
              date[0] = 'A';
              date[1] = 'p';
              date[2] = 'r';
              break;
          case 4:
              date[0] = 'M';
              date[1] = 'a';
              date[2] = 'y';
              break;
          case 5:
              date[0] = 'J';
              date[1] = 'u';
              date[2] = 'n';
              break;
          case 6:
              date[0] = 'J';
              date[1] = 'u';
              date[2] = 'l';
              break;
          case 7:
              date[0] = 'A';
              date[1] = 'u';
              date[2] = 'g';
              break;
          case 8:
              date[0] = 'S';
              date[1] = 'e';
              date[2] = 'p';
              break;
          case 9:
              date[0] = 'O';
              date[1] = 'c';
              date[2] = 't';
              break;
          case 10:
              date[0] = 'N';
              date[1] = 'o';
              date[2] = 'v';
              break;
          case 11:
              date[0] = 'D';
              date[1] = 'e';
              date[2] = 'c';
              break;
      }

    //Day conversion
    days[0] =  intToChar(day/10);
    days[1] =  intToChar(day%10);

    hours[0] =  intToChar(hour/10);
    hours[1] =  intToChar(hour%10);

    mins[0] = intToChar(min/10);
    mins[1] = intToChar(min%10);

    secs[0] = intToChar(sec/10);
    secs[1] = intToChar(sec%10);

    Graphics_drawStringCentered(&g_sContext, date, 3, 40, 30, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, days, 2, 60, 30, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, hours, 3, 32, 40, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, mins, 3, 49, 40, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, secs, 2, 64, 40, TRANSPARENT_TEXT);
}

unsigned char intToChar(int val){
    unsigned char ret_val = ' ';
    switch(val){
        case 0:
            ret_val = '0';
            break;
        case 1:
            ret_val = '1';
            break;
        case 2:
            ret_val = '2';
            break;
        case 3:
            ret_val = '3';
            break;
        case 4:
            ret_val = '4';
            break;
        case 5:
            ret_val = '5';
            break;
        case 6:
            ret_val = '6';
            break;
        case 7:
            ret_val = '7';
            break;
        case 8:
            ret_val = '8';
            break;
        case 9:
            ret_val = '9';
            break;
    }
    return ret_val;
}

float convertTemp(float tempC){
    return (tempC*9/5)+32;
}

void displayTemp(float tempC){
    float tempF = convertTemp(tempC);


    unsigned char tempFDisp[6] = "ddd.fF";
    unsigned char tempCDisp[6] = "ddd.fC";
    tempFDisp[0] = intToChar((int)tempF/100);
    tempFDisp[1] = intToChar((int)tempF/10);
    tempFDisp[2] = intToChar((int)tempF%10);
    tempFDisp[4] = intToChar((int)(tempF*10)%10);
    tempCDisp[0] = intToChar((int)tempC/100);
    tempCDisp[1] = intToChar((int)tempC/10);
    tempCDisp[2] = intToChar((int)tempC%10);
    tempCDisp[4] = intToChar((int)(tempC*10)%10);

    if(tempCDisp[0] == '0') tempCDisp[0] = ' ';
    if(tempFDisp[0] == '0') tempFDisp[0] = ' ';

    Graphics_drawStringCentered(&g_sContext, tempCDisp, 6, 45, 50, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, tempFDisp, 6, 45, 60, TRANSPARENT_TEXT);

}

void displayAll(float tempC, long unsigned int utc){
    Graphics_clearDisplay(&g_sContext); // Clear the display
    displayTemp(tempC);
    displayTime(utc);
    Graphics_flushBuffer(&g_sContext);
}
