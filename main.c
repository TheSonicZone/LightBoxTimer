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


// Variables
//-------------------------------
unsigned int timebase = 0;
unsigned int time_seconds = 0;

// Main timer registers
unsigned char seconds = 0;
unsigned char minutes = 0;


// Functions

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

    seconds = 0x55;
    minutes = 0x13;

    // Initial display update
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
        Delay(400);
        displaydata.digit1 = GetSegmentData((minutes >> 4) & 0x0F);
        displaydata.digit2 = GetSegmentData(minutes & 0x0F);
        displaydata.misc = 0xFC;
        displaydata.digit3 = GetSegmentData((seconds >> 4) & 0x0F);
        displaydata.digit4 = GetSegmentData(seconds & 0x0F);


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
