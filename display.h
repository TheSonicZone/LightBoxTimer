//--------------------------------------------------------------------------------------------------------------
//
//          Header file for display.h
//         Multiplexed 7 segment display driver
//
//--------------------------------------------------------------------------------------------------------------
#ifndef DISPLAY_H_
#define DISPLAY_H_

// Definitions
//------------
typedef struct{
    unsigned char digit1;
    unsigned char digit2;
    unsigned char misc;
    unsigned char digit3;
    unsigned char digit4;
    unsigned char button1;
    unsigned char button2;
    unsigned char button3;
}display_data;

#define false 0
#define true  1

// Function Prototypes
//--------------------
//------------------------------------------------------------------------------------------------
unsigned char GetSegmentData(unsigned char val);
void MuxDisplay(void);


#endif /* DISPLAY_H_ */
