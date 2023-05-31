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
}display_data;


// Function Prototypes
//--------------------
void MuxDisplay(void);


#endif /* DISPLAY_H_ */
