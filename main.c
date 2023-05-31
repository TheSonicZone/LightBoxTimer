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

#define LED1 BIT0


// Globals

extern display_data displaydata;



// Functions




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

    displaydata.digit1 = 0x80;
    displaydata.digit2 = 0xA4;
    displaydata.misc = 0xFC;
    displaydata.digit3 = 0xA4;
    displaydata.digit4 = 0x80;

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
//    P1OUT ^= LED1;


}
