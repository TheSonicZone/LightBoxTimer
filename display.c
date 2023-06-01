//--------------------------------------------------------------------------------------------
//              DISPLAY
//          Driver for a LYD-4232G multiplexed 7-segment display on GPIO
//
//
//--------------------------------------------------------------------------------------------

// Included headers
//-----------------
#include "display.h"
#include <msp430.h>

unsigned char mux_cycle = 0;
unsigned char divider = 0;

display_data displaydata;


//----------------------------------------------------------------------------------------------
// Name: GetSegmentData
// Function: Return segment data (8 bits) from input value
//   Segment Data to suit LYD common anode display
// Parameter: Value from 0 to F
// Returns: 7 segment bit pattern
//------------------------------------------------------------------------------------------------
unsigned char GetSegmentData(unsigned char val){

    switch(val){
    case 0x00:
        return 0xC0;
    case 0x01:
        return 0xF9;
    case 0x02:
        return 0xA4;
    case 0x03:
        return 0xB0;
    case 0x04:
        return 0x99;
    case 0x05:
        return 0x92;
    case 0x06:
        return 0x82;
    case 0x07:
        return 0xF8;
    case 0x08:
        return 0x80;
    case 0x09:
        return 0x90;
    case 0x0A:
        return 0x88;
    case 0x0B:
        return 0x83;
    case 0x0C:
        return 0xC6;
    case 0x0D:
        return 0xA1;
    case 0x0E:
        return 0x86;
    case 0x0F:
        return 0x8E;


    default:
        return 0x00;
    }

}

//---------------------------------------------------------------------------------------------
// Name: MuxDisplay
// Function: Do a display multiplex cycle
// This routine to be called in a timer interrupt so that the display is driven continuously
// Uses: data in struct displaydata
//----------------------------------------------------------------------------------------------
void MuxDisplay(void){
    if(divider < 10){
        divider++;
    } else {
        divider = 0;

        switch (mux_cycle){
        case 0x00:
            P1OUT = displaydata.digit4;     // load pattern for digit 4
            P2OUT &= ~0x01;                 // Turn ON Q1
            P2OUT |= 0x02;                  // Turn OFF Q2
            P2OUT |= 0x04;                  // Turn OFF Q3
            P2OUT |= 0x08;                  // Turn OFF Q4
            P2OUT |= 0x10;                  // Turn OFF Q5
            break;

        case 0x04:
            P1OUT = displaydata.digit3;     // load pattern for digit 3
            P2OUT |= 0x01;                 // Turn OFF Q1
            P2OUT &= ~0x02;                  // Turn ON Q2
            P2OUT |= 0x04;                  // Turn OFF Q3
            P2OUT |= 0x08;                  // Turn OFF Q4
            P2OUT |= 0x10;                  // Turn OFF Q5
            break;

        case 0x08:
            P1OUT = displaydata.misc;     // load pattern for misc segments
            P2OUT |= 0x01;                 // Turn OFF Q1
            P2OUT |= 0x02;                  // Turn OFF Q2
            P2OUT &= ~0x04;                  // Turn ON Q3
            P2OUT |= 0x08;                  // Turn OFF Q4
            P2OUT |= 0x10;                  // Turn OFF Q5
            break;

        case 0x0C:
            P1OUT = displaydata.digit2;     // load pattern for digit 2
            P2OUT |= 0x01;                 // Turn OFF Q1
            P2OUT |= 0x02;                  // Turn OFF Q2
            P2OUT |= 0x04;                  // Turn OFF Q3
            P2OUT &= ~0x08;                  // Turn ON Q4
            P2OUT |= 0x10;                  // Turn OFF Q5

            break;

        case 0x10:
            P1OUT = displaydata.digit1;     // load pattern for digit 1
            P2OUT |= 0x01;                 // Turn OFF Q1
            P2OUT |= 0x02;                  // Turn ON Q2
            P2OUT |= 0x04;                  // Turn OFF Q3
            P2OUT |= 0x08;                  // Turn OFF Q4
            P2OUT &= ~0x10;                  // Turn OFF Q5

            break;



        }

        mux_cycle++;
        if(mux_cycle > 0x13){
            mux_cycle = 0;
        }
    }
}

