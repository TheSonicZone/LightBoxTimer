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

#define LED1 BIT0






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
    P1DIR = 0x0F;                               // P1.0 has LED on this pin (MSP430 Launchpad)
    P1OUT = 0x02;                               // BIT 1 = HIGH, all others = LOW


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

    P1OUT ^= LED1;


}
