#ifndef __KPC_H__
#define __KPC_H__

typedef struct
{
    __IO uint32_t PC        ; /* 0x0000 Keypad Control Register */
    __IO uint32_t RESERVED0;
    __IO uint32_t DK       ; /* 0x0008 Keypad Direct Key Register */
    __IO uint32_t RESERVED1;
    __IO uint32_t REC      ; /* 0x0010 Keypad Rotary Encoder Count Register */
    __IO uint32_t RESERVED2;
    __IO uint32_t MK       ; /* 0x0018 Keypad Matrix Key Register */
    __IO uint32_t RESERVED3;
    __IO uint32_t AS       ; /* 0x0020 Keypad Interface Automatic Scan Register */
    __IO uint32_t RESERVED4;
    __IO uint32_t ASMKP0   ; /* 0x0028 Keypad Interface Automatic Scan Multiple KeypressRegisters 0:3 */
    __IO uint32_t RESERVED5;
    __IO uint32_t ASMKP1   ; /* 0x0030 Keypad Interface Automatic Scan Multiple KeypressRegisters 0:3 */
    __IO uint32_t RESERVED6;
    __IO uint32_t ASMKP2   ; /* 0x0038 Keypad Interface Automatic Scan Multiple KeypressRegisters 0:3 */
    __IO uint32_t RESERVED7;
    __IO uint32_t ASMKP3   ; /* 0x0040 Keypad Interface Automatic Scan Multiple KeypressRegisters 0:3 */
    __IO uint32_t RESERVED8;
    __IO uint32_t KDI      ; /* 0x0048 Keypad Key-Debounce Interval Register */
}KPC_TypeDef;

#define    KPC_BASE                      0xD4012000                              
#define    MRVL_KPC     (( KPC_TypeDef *) KPC_BASE )

/*
 *
 *   THE BIT DEFINES
 *
 */
/*   PC      0x0000   Keypad Control Register */
/*      Bit(s) PC_RSRV_31 reserved */
#define   PC_AS            BIT_30         /* Automatic Scan             */
#define   PC_ASACT         BIT_29         /* Automatic Scan on Activity */
#define   PC_MKRN_MSK      SHIFT26(0x7)   /* Matrix Keypad Row Number   */
#define   PC_MKRN_BASE     26
#define   PC_MKCN_MSK      SHIFT23(0x7)   /* Keypad Column Number       */
#define   PC_MKCN_BASE     23
#define   PC_MI            BIT_22         /* Matrix Keypad Interrupt   */
#define   PC_IMKP          BIT_21         /* Ignore Multiple Keypress  */
#define   PC_MS7           BIT_20         /* Manual Matrix Scan line 7 */
#define   PC_MS6           BIT_19         /* Manual Matrix Scan line 6 */
#define   PC_MS5           BIT_18         /* Manual Matrix Scan line 5 */
#define   PC_MS4           BIT_17         /* Manual Matrix Scan line 4 */
#define   PC_MS3           BIT_16         /* Manual Matrix Scan line 3 */
#define   PC_MS2           BIT_15         /* Manual Matrix Scan line 2 */
#define   PC_MS1           BIT_14         /* Manual Matrix Scan line 1 */
#define   PC_MS0           BIT_13         /* Manual Matrix Scan line 0 */
#define   PC_ME            BIT_12         /* Matrix Keypad Enable      */
#define   PC_MIE           BIT_11         /* Matrix Interrupt Enable   */

/*      Bit(s) PC_RSRV_10 reserved */
#define   PC_DK_DEB_SEL      BIT_9         /* Direct Keypad Debounce Select */
#define   PC_DN_MSK         SHIFT6(0x7)      /* Direct Key Number */
#define   PC_DN_BASE         6
#define   PC_DI            BIT_5         /* Direct Keypad Interrupt */

/* Rotary Encoder Zero Debounce Interval */
#define   PC_RE_ZERO_DEB      BIT_4         
#define   PC_REE1            BIT_3         /* Rotary Encoder 1 Enable */
#define   PC_REE0            BIT_2         /* Rotary Encoder 0 Enable */
#define   PC_DE            BIT_1         /* Direct Keypad Enable */
#define   PC_DIE            BIT_0         /* Direct Keypad Interrupt Enable */

/*   DK      0x0008   Keypad Direct Key Register */
#define   DK_DKP               BIT_31            /* Direct Key Pressed */

/*      Bit(s) DK_RSRV_30_8 reserved */
#define   DK_DK7               BIT_7            /* Direct Key 7 input */
#define   DK_DK6               BIT_6            /* Direct Key 6 input */
#define   DK_DK5               BIT_5            /* Direct Key 5 input */
#define   DK_DK4               BIT_4            /* Direct Key 4 input */
/* Rotary Encoder B / Direct Key 3 input */
#define   DK_RB1_DK3            BIT_3            
/* Rotary Encoder A / Direct Key 2 input */
#define   DK_RA1_DK2            BIT_2            
/* Rotary Encoder B / Direct Key 1 input */
#define   DK_RB0_DK1            BIT_1            
/* Rotary Encoder A / Direct Key 0 input */
#define   DK_RA0_DK0            BIT_0            

/*   REC      0x0010   Keypad Rotary Encoder Count Register */
/* Overflow bit for rotary encoder 1 */
#define   REC_OF1               BIT_31         
/* Underflow bit for rotary encoder 1 */
#define   REC_UF1               BIT_30         
/*      Bit(s) REC_RSRV_29_24 reserved */
/* Count value for rotary encoder 1 */
#define   REC_RE_COUNT1_MSK      SHIFT16(0xff)   
#define   REC_RE_COUNT1_BASE      16
/* Overflow bit for rotary encoder 0 */
#define   REC_OF0               BIT_15         
/* Underflow bit for rotary encoder 0 */
#define   REC_UF0               BIT_14         
/*      Bit(s) REC_RSRV_13_8 reserved */
/* Count value for rotary encoder 0 */
#define   REC_RE_COUNT0_MSK      SHIFT0(0xff)   
#define   REC_RE_COUNT0_BASE      0

/*   MK      0x0018   Keypad Matrix Key Register */
#define   MK_MKP               BIT_31            /* MKP */
/*      Bit(s) MK_RSRV_30_8 reserved */
#define   MK_MR7               BIT_7            /* Matrix Row 7 */
#define   MK_MR6               BIT_6            /* Matrix Row 6 */
#define   MK_MR5               BIT_5            /* Matrix Row 5 */
#define   MK_MR4               BIT_4            /* Matrix Row 4 */
#define   MK_MR3               BIT_3            /* Matrix Row 3 */
#define   MK_MR2               BIT_2            /* Matrix Row 2 */
#define   MK_MR1               BIT_1            /* Matrix Row 1 */
#define   MK_MR0               BIT_0            /* Matrix Row 0 */

/*   AS      0x0020   Keypad Automatic Scan Register */
#define   AS_SO                BIT_31          /* Scan On */
#define   AS_MUKP_MSK          SHIFT26(0x1f)   /* Multiple Keys Pressed */
#define   AS_MUKP_BASE         26
/*      Bit(s) AS_RSRV_25_8 reserved */
#define   AS_DO                BIT_25     
#define   AS_RP_MSK            SHIFT4(0xf)      /* Row Pressed */
#define   AS_RP_BASE            4
#define   AS_CP_MSK            SHIFT0(0xf)      /* Column Pressed */
#define   AS_CP_BASE            0

/*   ASMKP0   0x0028   Keypad Automatic Scan Multiple Keypress Register 0 */
#define   ASMKP0_SO               BIT_31         /* Scan On */
/*      Bit(s) ASMKP0_RSRV_30_24 reserved */
/* Matrix Keypad Column 1 reading */
#define   ASMKP0_MKC1_MSK            SHIFT16(0xff)   
#define   ASMKP0_MKC1_BASE         16
/*      Bit(s) ASMKP0_RSRV_15_8 reserved */
/* Matrix Keypad Column 0 reading */
#define   ASMKP0_MKC0_MSK            SHIFT0(0xff)   
#define   ASMKP0_MKC0_BASE         0

/*   ASMKP1   0x0030   Keypad Automatic Scan Multiple Keypress Register 1 */
#define   ASMKP1_SO               BIT_31         /* Scan On */
/*      Bit(s) ASMKP1_RSRV_30_24 reserved */
/* Matrix Keypad Column 3 Reading */
#define   ASMKP1_MKC3_MSK            SHIFT16(0xff)   
#define   ASMKP1_MKC3_BASE         16
/*      Bit(s) ASMKP1_RSRV_15_8 reserved */
/* Matrix Keypad Column 2 Reading */
#define   ASMKP1_MKC2_MSK            SHIFT0(0xff)   
#define   ASMKP1_MKC2_BASE         0

/*   ASMKP2   0x0038   Keypad Automatic Scan Multiple Keypress Register 2 */
#define   ASMKP2_SO               BIT_31         /* Scan On */
/*      Bit(s) ASMKP2_RSRV_30_24 reserved */
/* Matrix Keypad Column 5 reading */
#define   ASMKP2_MKC5_MSK            SHIFT16(0xff)   
#define   ASMKP2_MKC5_BASE         16
/*      Bit(s) ASMKP2_RSRV_15_8 reserved */
/* Matrix Keypad Column 4 reading */
#define   ASMKP2_MKC4_MSK            SHIFT0(0xff)   
#define   ASMKP2_MKC4_BASE         0

/*   ASMKP3   0x0040   Keypad Automatic Scan Multiple Keypress Register 3 */
#define   ASMKP3_SO               BIT_31         /* Scan On */
/*      Bit(s) ASMKP3_RSRV_30_24 reserved */
/* Matrix Keypad Column 7 reading */
#define   ASMKP3_MKC7_MSK            SHIFT16(0xff)   
#define   ASMKP3_MKC7_BASE         16
/*      Bit(s) ASMKP3_RSRV_15_8 reserved */
/* Matrix Keypad Column 6 reading */
#define   ASMKP3_MKC6_MSK            SHIFT0(0xff)   
#define   ASMKP3_MKC6_BASE         0

/*   KDI      0x0048   Keypad Key Debounce Interval Register */
/*      Bit(s) KDI_RSRV_31_16 reserved */
/* Direct Key Debounce Interval */
#define   KDI_DIRECT_KEY_DEBOUNCE_INTERVAL_MSK      SHIFT8(0xff)   
#define   KDI_DIRECT_KEY_DEBOUNCE_INTERVAL_BASE      8
/* Matrix Key Debounce Interval */
#define   KDI_MATRIX_KEY_DEBOUNCE_INTERVAL_MSK      SHIFT0(0xff)   
#define   KDI_MATRIX_KEY_DEBOUNCE_INTERVAL_BASE      0

#endif
