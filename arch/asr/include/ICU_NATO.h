#ifndef __ICU_NATO_H__
#define __ICU_NATO_H__



typedef struct
{



    __IO uint32_t INT_CFGx[96];        // 0x00-0x17C OFFSET
    __IO uint32_t RESERVED1[32];       //0x180-0x1FC
    __IO uint32_t INT_STATUS[3];       //0x204-0x208
    __IO uint32_t RESERVED2;
    __IO uint32_t CP_FIQ_NUM;          //0x210
    __IO uint32_t CP_IRQ_NUM;          //0x214
    __IO uint32_t CP_GBL_INT_MSK;      //0x218
    __IO uint32_t RESERVED0;
    __IO uint32_t APc0_FIQ_NUM;        //0x220
    __IO uint32_t APc0_IRQ_NUM;        //0x224
    __IO uint32_t APc0_GBL_INT_MSK;    //0x228
    __IO uint32_t RESERVED3;
    __IO uint32_t APc1_FIQ_NUM;        //0x230
    __IO uint32_t APc1_IRQ_NUM;        //0x234
    __IO uint32_t APc1_GBL_INT_MSK;    //0x238
    __IO uint32_t RESERVED4;
    __IO uint32_t APc2_FIQ_NUM;        //0x240
    __IO uint32_t APc2_IRQ_NUM;        //0x244
    __IO uint32_t APc2_GBL_INT_MSK;    //0x248
    __IO uint32_t RESERVED5;
    __IO uint32_t APc3_FIQ_NUM;        //0x250
    __IO uint32_t APc3_IRQ_NUM;        //0x254
    __IO uint32_t APc3_GBL_INT_MSK;    //0x258
    __IO uint32_t RESERVED6[(0x300-0x258)/4-1];
    __IO uint32_t ARM_INT_STATUS;      //0x300
    __IO uint32_t ARM_INT_MSK;         //0x304
    __IO uint32_t RESERVED7[2];
    __IO uint32_t DMA_CP_INT_ST;       //0x310
    __IO uint32_t DMA_CP_INT_MSK;      //0x314
    __IO uint32_t RESERVED8[2];
    __IO uint32_t DMA_APc0_INT_ST;     //0x320
    __IO uint32_t DMA_APc0_INT_MSK;    //0x324
    __IO uint32_t DMA_APc0_SEC_INT_ST; //0x328
    __IO uint32_t DMA_APc0_SEC_INT_MSK;//0x32C
    __IO uint32_t DMA_APc1_INT_ST;     //0x330
    __IO uint32_t DMA_APc1_INT_MSK;    //0x334
    __IO uint32_t DMA_APc1_SEC_INT_ST; //0x338
    __IO uint32_t DMA_APc1_SEC_INT_MSK;//0x33C
    __IO uint32_t DMA_APc2_INT_ST;     //0x340
    __IO uint32_t DMA_APc2_INT_MSK;    //0x344
    __IO uint32_t DMA_APc2_SEC_INT_ST; //0x348
    __IO uint32_t DMA_APc2_SEC_INT_MSK;//0x34C
    __IO uint32_t DMA_APc3_INT_ST;     //0x320
    __IO uint32_t DMA_APc3_INT_MSK;    //0x324
    __IO uint32_t DMA_APc3_SEC_INT_ST; //0x328
    __IO uint32_t DMA_APc3_SEC_INT_MSK;//0x32C
}ICU_TypeDef;


#define    ICU_BASE      0xD4282000


#define    ICU  (( ICU_TypeDef *) ICU_BASE )


//IRQ 0 to 63 Configuration Registers
#define IRQ_0_TO_63_CONF_CPU1_INT    BIT_7
#define IRQ_0_TO_63_CONF_CPU0_INT    BIT_6
#define IRQ_0_TO_63_CONF_SEAGULL_INT  BIT_5
#define IRQ_0_TO_63_CONF_PRIORITY_MSK  SHIFT0(0xf)
#define IRQ_0_TO_63_CONF_PRIORITY_BASE   0
#define ICU_INT_CONF_IRQ_FIQ      BIT_4


/*	ICU_SEAGULL_FIQ_SEL_INT_NUM	0x0100	
 *										Interrupt Number Register
 */
/*		Bit(s) SEAGULL_FIQ_SEL_INT_NUM_RSRV_31_7 reserved */
/* Interrupt Pending */
#define	CP_FIQ_SEL_INT_NUM_INT_PENDING				BIT_6				
#define	CP_FIQ_SEL_INT_NUM_SEL_INT_NUM_MSK			SHIFT0(0x3f)		
#define	CP_FIQ_SEL_INT_NUM_SEL_INT_NUM_BASE		0

/*	SEAGULL_IRQ_SEL_INT_NUM	0x0104	
 *										Interrupt Number Register
 */
/*		Bit(s) SEAGULL_IRQ_SEL_INT_NUM_RSRV_31_7 reserved */
/* Interrupt Pending */
#define	SEAGULL_IRQ_SEL_INT_NUM_INT_PENDING				BIT_6				
#define	SEAGULL_IRQ_SEL_INT_NUM_SEL_INT_NUM_MSK			SHIFT0(0x3f)		
#define	SEAGULL_IRQ_SEL_INT_NUM_SEL_INT_NUM_BASE		0

/*	MOHAWK_FIQ_SEL_INT_NUM	0x0108	
 *										Interrupt Number Register
 */
/*		Bit(s) A9C0_FIQ_SEL_INT_NUM_RSRV_31_7 reserved */
/* Interrupt Pending */
#define	A9C0_FIQ_SEL_INT_NUM_INT_PENDING			BIT_6				
#define	A9C0_FIQ_SEL_INT_NUM_SEL_INT_NUM_MSK		SHIFT0(0x3f)		
#define	A9C0_FIQ_SEL_INT_NUM_SEL_INT_NUM_BASE		0

/*	A9C0_IRQ_SEL_INT_NUM	0x010C	
 *										Interrupt Number Register
 */
/*		Bit(s) A9C0_IRQ_SEL_INT_NUM_RSRV_31_7 reserved */
/* Interrupt Pending */
#define	A9C0_IRQ_SEL_INT_NUM_INT_PENDING			BIT_6				
#define	A9C0_IRQ_SEL_INT_NUM_SEL_INT_NUM_MSK		SHIFT0(0x3f)		
#define	A9C0_IRQ_SEL_INT_NUM_SEL_INT_NUM_BASE		0




/*	SEAGULL_GBL_IRQ_MSK		0x0110	
 *										Interrupt Mask Register
 */
/*		Bit(s) CP_GBL_IRQ_MSK_RSRV_31_2 reserved */
#define	CP_GBL_IRQ_MSK_IRQ_MSK				BIT_1				/* Mask IRQ */
#define	CP_GBL_IRQ_MSK_FIQ_MSK				BIT_0				/* Mask FIQ */

/*	A9C0_GBL_IRQ_MSK		0x0114	
 *										Interrupt Mask Register
 */
/*		Bit(s) A9C0_GBL_IRQ_MSK_RSRV_31_2 reserved */
#define	A9C0_GBL_IRQ_MSK_IRQ_MSK				BIT_1				/* Mask IRQ */
#define	A9C0_GBL_IRQ_MSK_FIQ_MSK				BIT_0				/* Mask FIQ */


//ARM Interrupt Mask Register
#define	ARM_INT_MSK_A9_L2C_INT_MSK                                 BIT_23
#define	ARM_INT_MSK_A9_SCU_PA_MSK                                 SHIFT21(0x3)
#define	ARM_INT_MSK_A9_SCU_PA_BASE                                21
#define	ARM_INT_MSK_A9c1_BTAC_PA_MSK                                 BIT_20
#define	ARM_INT_MSK_A9c1_GHB_PA_MSK                                 BIT_19
#define	ARM_INT_MSK_A9c1_ITAG_PA_MSK                                 BIT_18
#define	ARM_INT_MSK_A9c1_IDATA_PA_MSK                                 BIT_17
#define	ARM_INT_MSK_A9c1_TLB_PA_MSK                                 BIT_16
#define	ARM_INT_MSK_A9c1_D_PA_MSK                                 BIT_15
#define	ARM_INT_MSK_A9c1_DTAG_PA_MSK                                 BIT_14
#define	ARM_INT_MSK_A9c1_DDATA_PA_MSK                                 BIT_13
#define	ARM_INT_MSK_A9c1_DEFLAG_MSK                                 BIT_12
#define	ARM_INT_MSK_A9c0_BTAC_PA_MSK                                 BIT_11
#define	ARM_INT_MSK_A9c0_GHB_PA_MSK                                 BIT_10
#define	ARM_INT_MSK_A9c0_ITAG_PA_MSK                                 BIT_9
#define	ARM_INT_MSK_A9c0_IDATA_PA_MSK                                 BIT_8
#define	ARM_INT_MSK_A9c0_TLB_PA_MSK                                 BIT_7
#define	ARM_INT_MSK_A9c0_D_PA_MSK                                 BIT_6
#define	ARM_INT_MSK_A9c0_DTAG_PA_MSK                                 BIT_5
#define	ARM_INT_MSK_A9c0_DDATA_PA_MSK                                 BIT_4
#define	ARM_INT_MSK_A9c0_DEFLAG_MSK                                 BIT_3
#define	ARM_INT_MSK_SEAGULL_L2_UECC_INT_MSK                                 BIT_2
#define	ARM_INT_MSK_SEAGULL_L2_ECC_INT_MSK                                 BIT_1
#define	ARM_INT_MSK_SEAGULL_L2_PA_ECC_INT_MSK                                 BIT_0


//CA9 PARITYFAIL clear Register
#define	PA_CLR_A9_SCU_PA_CLEAR_MSK                                                     SHIFT16(0x3)
#define	PA_CLR_A9_SCU_PA_CLEAR_BASE                                                     16
#define	PA_CLR_A9c1_BTAC_PA_CLEAR                                                      BIT_15
#define	PA_CLR_A9c1_GHB_PA_CLEAR                                                      BIT_14
#define	PA_CLR_A9c1_ITAG_PA_CLEAR                                                      BIT_13
#define	PA_CLR_A9c1_IDATA_PA_CLEAR                                                      BIT_12
#define	PA_CLR_A9c1_TLB_PA_CLEAR                                                      BIT_11
#define	PA_CLR_A9c1_D_PA_CLEAR                                                      BIT_10
#define	PA_CLR_A9c1_DTAG_PA_CLEAR                                                      BIT_9
#define	PA_CLR_A9c1_DDATA_PA_CLEAR                                                      BIT_8
#define	PA_CLR_A9c0_BTAC_PA_CLEAR                                                      BIT_7
#define	PA_CLR_A9c0_GHB_PA_CLEAR                                                      BIT_6
#define	PA_CLR_A9c0_ITAG_PA_CLEAR                                                      BIT_5
#define	PA_CLR_A9c0_IDATA_PA_CLEAR                                                      BIT_4
#define	PA_CLR_A9c0_TLB_PA_CLEAR                                                      BIT_3
#define	PA_CLR_A9c0_D_PA_CLEAR                                                      BIT_2
#define	PA_CLR_A9c0_DTAG_PA_CLEAR                                                      BIT_1
#define	PA_CLR_A9c0_DDATA_PA_CLEAR                                                      BIT_0


//A9c1 FIQ Selected Interrupt Number Register  0X13C 0FFSET
#define	A9c1_FIQ_NUM_INT_PENDING                 BIT_6
#define	A9c1_FIQ_NUM_SEL_INT_NUM_MSK           SHIFT0(0x3f)
#define	A9c1_FIQ_NUM_SEL_INT_NUM_BASE                0

//A9c1 IRQ Selected Interrupt Number Register
#define	A9c1_IRQ_NUM_INT_PENDING                 BIT_6
#define	A9c1_IRQ_NUM_SEL_INT_NUM_MSK           SHIFT0(0x3f)
#define	A9c1_IRQ_NUM_SEL_INT_NUM_BASE                0



//A9c1 Global Interrupt Mask Register
#define	A9c1_GBL_INT_MSK_IRQ_MSK              BIT_1
#define	A9c1_GBL_INT_MSK_FIQ_MSK              BIT_0




#endif
