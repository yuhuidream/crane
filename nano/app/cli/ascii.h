/*
 * $Copyright (c) 2008, ARM Ltd. All rights reserved.
 * -------------------------------------------------------
 * File:     ascii.h
 * $ * 
 * ASCII macros
 *  
 */

#ifndef ASCII_H
#define ASCII_H

/* Control Codes */

#define  ASCII_NUL  0x00     /* Null char */
#define  ASCII_SOH  0x01     /* Start of Header */
#define  ASCII_STX  0x02     /* Start of Text */
#define  ASCII_ETX  0x03     /* End of Text */
#define  ASCII_EOT  0x04     /* End of Transmission */
#define  ASCII_ENQ  0x05     /* Enquiry */
#define  ASCII_ACK  0x06     /* Acknowledgment */
#define  ASCII_BEL  0x07     /* Bell */
#define  ASCII_BS   0x08     /* Backspace */
#define  ASCII_HT   0x09     /* Horizontal Tab */
#define  ASCII_LF   0x0A     /* Line Feed */
#define  ASCII_VT   0x0B     /* Vertical Tab */
#define  ASCII_FF   0x0C     /* Form Feed */
#define  ASCII_CR   0x0D     /* Carriage Return */
#define  ASCII_SO   0x0E     /* Shift Out */
#define  ASCII_SI   0x0F     /* Shift In */
#define  ASCII_DLE  0x10     /* Data Link Escape */
#define  ASCII_DC1  0x11     /* Device Control 1 (XON) */
#define  ASCII_DC2  0x12     /* Device Control 2 */
#define  ASCII_DC3  0x13     /* Device Control 3 (XOFF) */
#define  ASCII_DC4  0x14     /* Device Control 4 */
#define  ASCII_NAK  0x15     /* Negative Acknowledgement */
#define  ASCII_SYN  0x16     /* Synchronous Idle */
#define  ASCII_ETB  0x17     /* End of Trans. Block */
#define  ASCII_CAN  0x18     /* Cancel */
#define  ASCII_EM   0x19     /* End of Medium */
#define  ASCII_SUB  0x1A     /* Substitute */
#define  ASCII_ESC  0x1B     /* Escape */
#define  ASCII_FS   0x1C     /* File Separator */
#define  ASCII_GS   0x1D     /* Group Separator */
#define  ASCII_RS   0x1E     /* Request to Send - Record Separator */
#define  ASCII_US   0x1F     /* Unit Separator */
#define  ASCII_SP   0x20     /* Space */
#define  ASCII_AT   0x40     /* @ */
#define  ASCII_CTRL_C  0x3   /* control + C */
#define  ASCII_CTRL_D  0x4   /* control + D */

/*The special control defined for BBU, we use those instead of the various VT keypad maps to avoid confusion 
**For example, the UP key on VT100 type terminal may be "ESC[A", which is complex to handle and 
**also may be different on putty, hypterterminal,etc. no unified mappings beyond the 128 ASCII. 
**In CLI, we only use letters and numbers, so I'd like to use other special single char to do control 
**/

#define  BBU_KEY_INS    0x21     /* '!'switch insert and replace mode*/
#define  BBU_KEY_UP     0x3D     /* '=' stands for UP key to switch to last command*/
#define  BBU_KEY_DOWN   0x27     /*''' stands for DOWN key to switch to next history command*/
#define  BBU_KEY_LEFT   0x5B     /*'[' stands for LEFT key to move cursor left*/
#define  BBU_KEY_RIGHT  0x5D     /*']' stands for RIGHT key to move cursor right*/


#define  ASCII_DEL  0x7F     /* Delete */
//#define  ESC_SEQ    0x5B     /* Escape character for special keys */
//#define  ASCII_UP   0x41     /* Up key */
//#define  ASCII_DOWN 0x42     /* Down key */

#endif /* ASCII_H */
