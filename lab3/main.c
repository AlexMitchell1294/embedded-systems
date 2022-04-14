/************** ECE2049 DEMO CODE ******************/
/**************  25 August 2021   ******************/
/***************************************************/

#include <displayUtils.h>
#include <msp430.h>

/* Peripherals.c and .h are where the functions that implement
 * the LEDs and keypad, etc are. It is often useful to organize
 * your code by putting like functions together in files.
 * You include the header associated with that file(s)
 * into the main file of your project. */
#include "peripherals.h"

// Function Prototypes
void runtimerA2(void);
void stoptimerA2();
void setMoveOn(int);
void TimerA2_ISR (void);
void ADC12_ISR();
inline long map(long,long,long,long,long);



#define MA_per_bit .244 //1.0A/4096
#define CALADC12_15V_30C  *((unsigned int *)0x1A1A)
#define CALADC12_15V_85C  *((unsigned int *)0x1A1C)

const int month_AmountDay[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
// Declare globals here
int timer_cnt = 0;
unsigned int in_temp;
unsigned int in_wheel;
int moveOn = 0;//timer_cnt + moveOnCount;
long unsigned int utc = 20822400;//day 242
unsigned long times[36];
float tempCReadings[36];
bool tempChange = true;
bool enabled = true;
int lastAction = 0;

// Main
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    initLeds();
    configDisplay();
    configKeypad();
    initButtons();

    REFCTL0 &= ~REFMSTR;    // Reset REFMSTR to hand over control of
    // internal reference voltages to
    // ADC12_A control registers

    P6SEL |= BIT4;

    ADC12CTL0 = ADC12SHT0_9 | ADC12REFON | ADC12ON | ADC12MSC;     // Internal ref = 1.5V
    ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1;                     // Enable sample timer
    // Using ADC12MEM0 to store reading
    ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10;  // ADC i/p ch A10 = temp sense


    __delay_cycles(100);                    // delay to allow Ref to settle
    ADC12CTL0 |= ADC12ENC;
    ADC12IE = BIT1;
    _BIS_SR(GIE);

    ADC12MCTL1 = ADC12SREF_0 + ADC12INCH_5 + ADC12EOS;
    int state = 0;
    int state_editing = 1;

    volatile float tempC = 0.0;
    volatile float tempF = 0.0;
    volatile float tempC_per_bits = 0.0;
    int i=0;
    for(i=0; i<36;i++){
        tempCReadings[i]= (float) ((long) in_temp - CALADC12_15V_30C ) * tempC_per_bits + 30.0;
    }


    tempC_per_bits =((float)(85.0-30.0) / (float)(CALADC12_15V_85C - CALADC12_15V_30C));

    runtimerA2();
    setMoveOn(10);
    ADC12CTL0 &= ~ADC12SC;
    ADC12CTL0 |= ADC12SC;
    while(1){
        switch(state){
            case 0://just run check for edit button
                if (getPressed()==1){
                    state = 1;
                    state_editing = 1;
                    enabled = false;
                    lastAction = timer_cnt+50;
                }
                break;
            case 1://edit mode
                if(getPressed()==2){
                    state = 0;
                    enabled = true;
                    lastAction = timer_cnt+50;
                }
                long unsigned int day = utc / 86400 + 1;
                long unsigned int a = utc;
                long unsigned int hour = (utc-(day-1)*86400)/3600;
                long unsigned int min = ((utc-(day-1)*86400)/60);
                long unsigned int sec = ((utc-(day-1)*86400));
                long unsigned int remain = 0;
                int month = 0;
                while(day>month_AmountDay[month]){
                    month++;
                    day -= month_AmountDay[month];
                }
                switch(state_editing){
                    case 1://editing months
                        if (getPressed()==1 && lastAction < timer_cnt){
                            state_editing = 2;
                            lastAction = timer_cnt+50;
                        }
                        int monthT = map(in_wheel, 0, 4095, 0, 11);
                        int monthZ = monthT;
                        while(monthT != month){
                            day = month_AmountDay[monthT];
                            if (monthT<month){
                                utc -= day*24*60*60;
                                monthT++;
                            }
                            else if(monthT>month){
                                utc += day*24*60*60;
                                monthT--;
                            }
                        }
                        GrLineDrawH(&g_sContext, 30, 50, 35);
                        Graphics_flushBuffer(&g_sContext);
                        month = monthZ;
                        break;
                    case 2://editing days
                            if (getPressed()==1 && lastAction < timer_cnt){
                                state_editing = 3;
                                lastAction = timer_cnt+50;
                            }
                            int dayT = map(in_wheel, 0, 4095, 1, month_AmountDay[month]);
                            int dayZ=dayT;
                            while(dayT != day){
                                if (dayT<day){
                                    utc -= 86400;
                                    dayT++;
                                }
                                else if(dayT>day){
                                    utc += 86400;
                                    dayT--;
                                }
                            }
                            GrLineDrawH(&g_sContext, 50, 65, 35);
                            Graphics_flushBuffer(&g_sContext);
                            day=dayZ;
                            break;
                    case 3://editing hours
                            if (getPressed()==1 && lastAction < timer_cnt){
                                state_editing = 4;
                                lastAction = timer_cnt+50;
                            }
                            int hourT = map(in_wheel, 0, 4095, 0, 23);
                            int hourZ = hourT;
                            while(hourT != hour){
                                if (hourT<hour){
                                    utc -= 3600;
                                    hourT++;
                                }
                                else if(hourT>hour){
                                    utc += 3600;
                                    hourT--;
                                }
                            }
                            GrLineDrawH(&g_sContext, 20, 35, 45);
                            Graphics_flushBuffer(&g_sContext);
                            hour = hourZ;
                            break;

                    case 4://editing min
                            day = utc / 86400 + 1;
                            remain = utc-(day-1)*86400;
                            hour = remain / 3600;
                            remain = remain - hour*3600;
                            min = remain/60;
                            remain = remain - min*60;
                            sec = remain;
                            if (getPressed()==1 && lastAction < timer_cnt){
                                state_editing = 5;
                                lastAction = timer_cnt+50;
                            }
                            int minT = map(in_wheel, 0, 4095, 0, 59);
                            int minZ = minT;
                            while(minT != min){
                                if (minT<min){
                                    utc -= 60;
                                    minT++;
                                }
                                else if(minT>min){
                                    utc += 60;
                                    minT--;
                                }
                            }
                            GrLineDrawH(&g_sContext, 40, 50, 45);
                            Graphics_flushBuffer(&g_sContext);
                            min = minZ;
                            break;
                    case 5://editing sec
                            day = utc / 86400 + 1;
                            remain = utc-(day-1)*86400;
                            hour = remain / 3600;
                            remain = remain - hour*3600;
                            min = remain/60;
                            remain = remain - min*60;
                            sec = remain;
                            if (getPressed()==1 && lastAction < timer_cnt){
                                state_editing = 1;
                                lastAction = timer_cnt+50;
                            }
                            int secT = map(in_wheel, 0, 4095, 0, 59);
                            int secZ = secT;
                            while(secT != sec){
                                if (secT<sec){
                                    utc -= 1;
                                    secT++;
                                }
                                else if(secT>sec){
                                    utc += 1;
                                    secT--;
                                }
                            }
                            GrLineDrawH(&g_sContext, 59, 69, 45);
                            Graphics_flushBuffer(&g_sContext);
                            sec = secZ;
                            break;

                    break;
                }


        }


        if (tempChange) {
                    tempC = (float) ((long) in_temp - CALADC12_15V_30C ) * tempC_per_bits + 30.0;
                    tempF = convertTemp(tempC);
                    tempChange = false;
                }

        //times[utc % 36] = utc;
        tempCReadings[utc%36] = tempC;

        int i;
        float t;
        float tempTotal=0.0;
        for(i=0; i<36; i++){
            t = tempCReadings[i];
            tempTotal+=t;
        }
        float dispTempC = tempTotal/36;
        if (moveOn < timer_cnt){
            displayAll(dispTempC, utc);
            setMoveOn(600);
        }
    }
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    in_temp = ADC12MEM0;
    in_wheel = ADC12MEM1 & 0x0FFF;
    tempChange = true;
}

inline long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    (x > in_max) ? x = in_max : (x < in_min) ? x = in_min : 1;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
};

void runtimerA2(void){
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

void setMoveOn(int count){
    moveOn = timer_cnt + count;
}

#pragma vector=TIMER2_A0_VECTOR
__interrupt void TimerA2_ISR (void) {
        timer_cnt++;
        if (timer_cnt == 60000) timer_cnt = 0;
        if(timer_cnt%200==0 && enabled){
            ADC12CTL0 &= ~ADC12SC;
            ADC12CTL0 |= ADC12SC;
            utc++;
        }
}
