//---------------------------------------------------------------------------------------
//
//                      Prower's Projects UV Light Box
//                      Timer Unit
//                      Author: Sonikku
//                      June 2023
//----------------------------------------------------------------------------------------

#include <msp430.h>
#include <stdio.h>
#include "timer.h"
#include "display.h"

#define RELAY BIT6
#define BUZZER BIT7


// Globals
//------------------------------
extern display_data displaydata;

// Definitions
#define true   1
#define false  0

#define idle 0
#define run  1
#define stop 2

// Variables
//-------------------------------
unsigned int timebase = 0;
unsigned int time_seconds = 0;

// Main timer registers
unsigned char seconds = 0;
unsigned char minutes = 0;

unsigned char timer_run = false;
unsigned char keydown = false;
unsigned int beep_duration = 0;
unsigned char mode = idle;




// Functions

//------------------------------------------------------------------------------
// Name: ShortBeep
// Function: Generate short beeps on the buzzer as used for keypresses
// Parameters: void
// Returns: void
//------------------------------------------------------------------------------
void ShortBeep(void){
    P2OUT |= BUZZER;
    beep_duration = 1000;
}

//------------------------------------------------------------------------------
// Name: IncrementSecond
// Function: Increment the seconds register by 1
// Parameters: void
// Returns: void
//-------------------------------------------------------------------------------
void IncrementSecond(void){

    if (seconds < 0x60){
        seconds++;
        if((seconds & 0x0F) == 0x0A){
            seconds += 6;
        }
    }
}

//------------------------------------------------------------------------------
// Name: IncrementMinute
// Function: Increment the minutes register by 1
// Parameters: void
// Returns: void
//-------------------------------------------------------------------------------
void IncrementMinute(void){

    if (minutes < 0x60){
        minutes++;
        if((minutes & 0x0F) == 0x0A){
            minutes += 6;
        }
    }
}

//-------------------------------------------------------------------------------
// Name: DecrementTime
// Function: Decrement the time loaded into the time registers and stop when zero
// Note: Timer registers are in BCD format
//--------------------------------------------------------------------------------
void DecrementTime(void){

    // Only run the process if there's a valid value in the timer registers
    if(seconds == 0 &&  minutes == 0){
        return;
    }

    if (timer_run == false){
        return;
    }

    seconds--;
    if ((seconds & 0x0F) == 0x0F){
        seconds = seconds - 6;
    }

    if (seconds == 0xF9){
        seconds = 0x59;
        minutes--;
        if ((minutes & 0x0F) == 0x0F){
            minutes = minutes - 6;
        }
    }

}



//------------------------------------------- Main Function ------------------------------
int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // Stop Watchdog timer

    // Configure and calibrate system clock
    //--------------------------------------
    DCOCTL = 0;                                  // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ;                       // Set range
    DCOCTL = CALDCO_16MHZ;                        // Set DCO step + modulation */
    BCSCTL2 = 0x00;



    // Configure ports
    //------------------
    P1DIR = 0xFF;                               // P1.0 drives all common anode segment a - f & dp
    P1OUT = 0xFF;                               // Initialise all as HIGH (LED segments OFF)
    P2DIR = 0xDF;                               // P2.0 to P2.4 anode drivers (logic 0 = drive on) P2.5 = button input, P2.6 = relay drive, P2.7 = buzzer drive
    P2OUT = 0x1F;                               // Initialise
    P2SEL2 = P2SEL2 & 0x3F;
    P2SEL = P2SEL & 0x3F;                       // P2.7 and P2.6 made GPIO

//   P2OUT &= ~0x01;
//    P1OUT &= ~0x04;

    // Reset time registers
    seconds = 0x00;
    minutes = 0x00;

    // Initialise the display memory and the button registers
    displaydata.button1 = false;
    displaydata.button2 = false;
    displaydata.button3 = false;
    displaydata.digit1 = GetSegmentData((minutes >> 4) & 0x0F);
    displaydata.digit2 = GetSegmentData(minutes & 0x0F);
    displaydata.misc = 0xFC;
    displaydata.digit3 = GetSegmentData((seconds >> 4) & 0x0F);
    displaydata.digit4 = GetSegmentData(seconds & 0x0F);

    // Configure WDT as systick timer
    //--------------------------------

    WDTCTL = WDT_MDLY_0_5 ;                     // Enable WDT with period of 500uS
    IE1 |= WDTIE;                               // Enable WDT interrupt



    // Enable interrupts
    //-----------------------------
    __enable_interrupt();                       // Enable System interrupts

    // System Initialisation
    //-----------------------------
    InitTimerSystem();

    // Hardware test code
//    P2OUT |= 0x40;                  // Relay test
//    P2OUT |= 0x80;                  // Buzzer test

    // Main loop
    //------------------------------------------------------------------------------------
    for(;;){

        switch (mode){

        case idle:
            // Idle mode
            // Update the display periodically
            Delay(400);
            displaydata.digit1 = GetSegmentData((minutes >> 4) & 0x0F);
            displaydata.digit2 = GetSegmentData(minutes & 0x0F);
            displaydata.misc = 0xFC;
            displaydata.digit3 = GetSegmentData((seconds >> 4) & 0x0F);
            displaydata.digit4 = GetSegmentData(seconds & 0x0F);

            // Check for user input
            if (displaydata.button1 == true || displaydata.button2 == true || displaydata.button3 == true){
                if(keydown == false){
                    ShortBeep();
                    // Action specific key input here
                    if(displaydata.button1 == true){
                        IncrementMinute();
                    }
                    if(displaydata.button2 == true){
                        IncrementSecond();
                    }
                    if(displaydata.button3 == true){
                        if(minutes > 0 || seconds > 0){
                            timebase = 0;
                            time_seconds = 0;
                            P2OUT |= RELAY;
                            timer_run = true;
                            mode = run;
                        }
                    }

                    keydown = true;
                }
            }

            // Button 1 and 3 held down
            if(displaydata.button1 == true && displaydata.button3 == true){
                minutes = 0;
                seconds = 0;
            }
            // All buttons released
            if(displaydata.button1 == false && displaydata.button2 == false && displaydata.button3 == false){
                keydown = false;
            }
            break;

        //----------------------------------------------------------------------------------------------------
        // run mode - timer will decrement and then stop when time is up
        //----------------------------------------------------------------------------------------------------
        case run:
            // Update the display periodically
            Delay(400);
            displaydata.digit1 = GetSegmentData((minutes >> 4) & 0x0F);
            displaydata.digit2 = GetSegmentData(minutes & 0x0F);
            displaydata.misc = 0xFC;
            displaydata.digit3 = GetSegmentData((seconds >> 4) & 0x0F);
            displaydata.digit4 = GetSegmentData(seconds & 0x0F);

            if (displaydata.button1 == true || displaydata.button2 == true || displaydata.button3 == true){
                if(keydown == false){
                    ShortBeep();
                    if(displaydata.button3 == true){
                        P2OUT &= ~RELAY;
                        timer_run = false;
                        mode = stop;

                    }

                    keydown = true;
                }
            }

            if(minutes == 0 && seconds == 0){
                P2OUT &= ~RELAY;
                timer_run = false;
                unsigned char p, q;
                for(q = 0; q < 4; q++){
                    for(p = 0; p < 4; p++){
                        ShortBeep();
                        Delay(900);
                        ShortBeep();
                        Delay(900);
                    }
                Delay(10200);
                }

                mode = idle;
            }

            // All buttons released
            if(displaydata.button1 == false && displaydata.button2 == false && displaydata.button3 == false){
                keydown = false;
            }

            break;
       //----------------------------------------------------------------------------------------------------
       // stop mode - timer is halted, user can reset or resume
       //----------------------------------------------------------------------------------------------------
        case stop:
            // Update the display periodically
            Delay(400);
            displaydata.digit1 = GetSegmentData((minutes >> 4) & 0x0F);
            displaydata.digit2 = GetSegmentData(minutes & 0x0F);
            displaydata.misc = 0xFC;
            displaydata.digit3 = GetSegmentData((seconds >> 4) & 0x0F);
            displaydata.digit4 = GetSegmentData(seconds & 0x0F);

            if (displaydata.button1 == true || displaydata.button2 == true || displaydata.button3 == true){
                if(keydown == false){
                    ShortBeep();
                    if(displaydata.button3 == true){
                        P2OUT |= RELAY;
                        timer_run = true;
                        mode = run;

                    }

                    keydown = true;
                }
            }
            // Button 1 and 3 held down
            if(displaydata.button1 == true && displaydata.button3 == true){
                minutes = 0;
                seconds = 0;
                mode = idle;
            }

            // All buttons released
            if(displaydata.button1 == false && displaydata.button2 == false && displaydata.button3 == false){
                keydown = false;
            }
            break;
        }

    }


}

// Watchdog Timer interrupt service routine
// Used here as a system timer
// Period is 65.6 uS
//---------------------------------------------------
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void){

    CallInISR();
    MuxDisplay();
    if(beep_duration > 0){
        beep_duration--;
        if (beep_duration == 0){
            P2OUT &= ~BUZZER;
        }
    }

    timebase++;
    if(timebase > 30){
        timebase = 0;
        // We hit here every 1mS
        time_seconds++;
        if(time_seconds > 999){
            time_seconds = 0;
            DecrementTime();
        }
    }



}
