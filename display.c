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
//           Also, read the three push buttons attached to multiplex lines
// This routine to be called in a timer interrupt so that the display is driven continuously
// Uses: data in struct displaydata
//----------------------------------------------------------------------------------------------
void MuxDisplay(void){
    if(divider < 20){
        divider++;
    } else {
        divider = 0;
        switch (mux_cycle){

        case 0x00:
            P1OUT = displaydata.digit4;     // load pattern for digit 4
            P2OUT &= ~0x01;                 // Turn ON Q1
            break;

        case 0x01:
            if((P2IN & 0x20) != 0x20){
                // P2.5 = 0, this means SW1 is pressed
                displaydata.button1 = true;
            } else {
                displaydata.button1 = false;
            }
        break;

        case 0x03:
            P2OUT |= 0x1F;
            break;
//-------------------------------------------------------------------------
        case 0x04:
            P1OUT = displaydata.digit3;     // load pattern for digit 3
            P2OUT &= ~0x02;                  // Turn ON Q2
            break;

        case 0x05:
            if((P2IN & 0x20) != 0x20){
                // P2.5 = 0, this means SW2 is pressed
                displaydata.button2 = true;
            } else {
                displaydata.button2 = false;
            }
            break;

        case 0x07:
            P2OUT |= 0x1F;
            break;
//-----------------------------------------------------------------------------
        case 0x08:
            P1OUT = displaydata.misc;     // load pattern for misc segments
            P2OUT &= ~0x04;                  // Turn ON Q3
            break;

        case 0x09:
            if((P2IN & 0x20) != 0x20){
                // P2.5 = 0, this means SW1 is pressed
                displaydata.button3 = true;
            } else {
                displaydata.button3 = false;
            }
            break;

        case 0x0B:
            P2OUT |= 0x1F;
            break;
//----------------------------------------------------------------------
        case 0x0C:
            P1OUT = displaydata.digit2;     // load pattern for digit 2
            P2OUT &= ~0x08;                  // Turn ON Q4
            break;

        case 0x0F:
            P2OUT |= 0x1F;
            break;
//----------------------------------------------------------------------
        case 0x10:
            P1OUT = displaydata.digit1;     // load pattern for digit 1
            P2OUT &= ~0x10;                  // Turn ON Q5
            break;

        case 0x13:
            P2OUT |= 0x1F;
            break;
        }

        mux_cycle++;
        if(mux_cycle >= 0x14){
            mux_cycle = 0;
        }
    }
}

