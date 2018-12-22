#ifndef __AXIFab_H__
#define __AXIFab_H__


typedef struct
{
     __IO uint32_t RESERVED0[0X200/4];
     __IO uint32_t FAB_CP_ADDR_REMAP;//0X200
     __IO uint32_t RESERVED1[(0x220-0x200)/4-1];
     __IO uint32_t FAB_TIMEOUT;//0X220
     __IO uint32_t RESERVED2[(0x220-0x200)/4-1];
     __IO uint32_t FAB_TIMOUT_STATUS;//0X240
     __IO uint32_t RESERVED3[(0x220-0x200)/4-1];
     __IO uint32_t FAB_CTRL;//0X260
     __IO uint32_t RESERVED4[(0x408-0x260)/4-1];
     __IO uint32_t FAB_ARB_CTRL_READ_0;//0X408
     __IO uint32_t FAB_ARB_CTRL_WRITE_0;//0X40C
     __IO uint32_t RESERVED5[(0x428-0x40C)/4-1];
     __IO uint32_t FAB_ARB_CTRL_READ_1;//0X428
     __IO uint32_t FAB_ARB_CTRL_WRITE_1;//0X42C
     __IO uint32_t RESERVED6[(0x428-0x40C)/4-1];
     __IO uint32_t FAB_ARB_CTRL_READ_2;//0X448
     __IO uint32_t FAB_ARB_CTRL_WRITE_2;//0X44C
     __IO uint32_t RESERVED7[(0x428-0x40C)/4-1];
     __IO uint32_t FAB_ARB_CTRL_READ_3;//0X468
     __IO uint32_t FAB_ARB_CTRL_WRITE_3;//0X46C
     __IO uint32_t RESERVED8[(0x428-0x40C)/4-1];
     __IO uint32_t FAB_ARB_CTRL_READ_4;//0X488
     __IO uint32_t FAB_ARB_CTRL_WRITE_4;//0X48C
     __IO uint32_t RESERVED9[(0x428-0x40C)/4-1];
     __IO uint32_t FAB_ARB_CTRL_READ_5;//0X4A8
     __IO uint32_t FAB_ARB_CTRL_WRITE_5;//0X4AC
     __IO uint32_t RESERVED10[(0x428-0x40C)/4-1];
     __IO uint32_t FAB_ARB_CTRL_READ_6;//0X4C8
     __IO uint32_t FAB_ARB_CTRL_WRITE_6;//0X4CC



}MAIN_AXIFab_TypeDef;


#define   MAIN_AXIFab_BASE 0xD4200000  
#define   MAIN_AXIFab ((MAIN_AXIFab_TypeDef *)MAIN_AXIFab_BASE)

/*		Bit(s) FAB_CP_ADDR_REMAP_RSRV_31_8 reserved */
#define	FAB_CP_ADDR_REMAP_BIT_SAMPL_MSK			SHIFT0(0xff)		/* Bit Sample */
#define	FAB_CP_ADDR_REMAP_BIT_SAMPL_BASE		0

/*	FAB_TIMEOUT				0x0220	AXI Fabric Time-Out Register */
/*		Bit(s) FAB_TIMEOUT_RSRV_31_16 reserved */
#define	FAB_TIMEOUT_BIT_SAMPL_MSK		SHIFT0(0xffff)	/* Time-out Value */
#define	FAB_TIMEOUT_BIT_SAMPL_BASE		0

/*	FAB_TIMOUT_STATUS		0x0240	AXI Fabric Time-Out Status Register */
/*		Bit(s) FAB_TIMOUT_STATUS_RSRV_31_16 reserved */		
/* Status Master Port 6 Read */
#define	FAB_TIMOUT_STATUS_STATUS_DX8_PORT9_WR		BIT_14			
/* Status Master Port 5 Read */
#define	FAB_TIMOUT_STATUS_STATUS_USBPHY_PORT8_WR		BIT_13			
/* Status Master Port 7 Write */
#define	FAB_TIMOUT_STATUS_STATUS_SDH_PORT7_WR		BIT_12			
/* Status Master Port 6 Write */
#define	FAB_TIMOUT_STATUS_STATUS_USB_PORT6_WR		BIT_11			
/* Status Master Port 5 Write */
#define	FAB_TIMOUT_STATUS_STATUS_IRE_PORT5_WR		BIT_10			
/* Status Master Port 4 Read */
#define	FAB_TIMOUT_STATUS_STATUS_MAST_PORT4_RD		BIT_9			
/* Status Master Port 3 Read */
#define	FAB_TIMOUT_STATUS_STATUS_MAST_PORT3_RD		BIT_8			
/* Status Master Port 2 Read */
#define	FAB_TIMOUT_STATUS_STATUS_MAST_PORT2_RD		BIT_7			
/* Status Master Port 1 Read */
#define	FAB_TIMOUT_STATUS_STATUS_MAST_PORT1_RD		BIT_6			
/* Status Master Port 0 Read */
#define	FAB_TIMOUT_STATUS_STATUS_MAST_PORT0_RD		BIT_5			
/* Status Master Port 4 Write */
#define	FAB_TIMOUT_STATUS_STATUS_MAST_PORT4_WR		BIT_4			
/* Status Master Port 3 Write */
#define	FAB_TIMOUT_STATUS_STATUS_MAST_PORT3_WR		BIT_3			
/* Status Master Port 2 Write */
#define	FAB_TIMOUT_STATUS_STATUS_MAST_PORT2_WR		BIT_2			
/* Status Master Port 1 Write */
#define	FAB_TIMOUT_STATUS_STATUS_MAST_PORT1_WR		BIT_1			
/* Status Master Port 0 Write */
#define	FAB_TIMOUT_STATUS_STATUS_MAST_PORT0_WR		BIT_0			



/*	FAB_CTRL				0x0260	AXI Fabric 2 Master Wait State Control and
 *									Request Threshold Control Register
 */
/*		Bit(s) FAB_CTRL_RSRV_31_16 reserved */
/* Master Port 10 Write Channel Threshold Control */
#define	FAB_CTRL_MAST_PORT10_WR_CHAN_THRESH_CTRL_MSK		SHIFT13(0x7)	
#define	FAB_CTRL_MAST_PORT10_WR_CHAN_THRESH_CTRL_BASE		13
/* Master Port 10 Read Channel Threshold Control */
#define	FAB_CTRL_MAST_PORT10_RD_CHAN_THRESH_CTRL_MSK		SHIFT10(0x7)	
#define	FAB_CTRL_MAST_PORT10_RD_CHAN_THRESH_CTRL_BASE		10
/* Master Port 8 Write Channel Threshold Control */
#define	FAB_CTRL_MAST_PORT8_WR_CHAN_THRESH_CTRL_MSK			SHIFT7(0x7)		
#define	FAB_CTRL_MAST_PORT8_WR_CHAN_THRESH_CTRL_BASE		7
/* Master Port 8 Read Channel Threshold Control */
#define	FAB_CTRL_MAST_PORT8_RD_CHAN_THRESH_CTRL_MSK			SHIFT4(0x7)		
#define	FAB_CTRL_MAST_PORT8_RD_CHAN_THRESH_CTRL_BASE		4
/* Request Threshold Control Disable */
#define	FAB_CTRL_REQ_THRESH_CTRL_DIS						BIT_3		


/*	FAB_ARB_CTRL_READ_0		0x0408	Fabric Master Port 0 Read Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_READ_0_READ_SN_ID_0_MSK		SHIFT24(0xff)	
#define	FAB_ARB_CTRL_READ_0_READ_SN_ID_0_BASE		24
/*		Bit(s) FAB_ARB_CTRL_READ_0_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_READ_0_READ_PRI_VAL_0_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_READ_0_READ_PRI_VAL_0_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_READ_0_READ_SP_ID_0_MSK		SHIFT0(0xff)	
#define	FAB_ARB_CTRL_READ_0_READ_SP_ID_0_BASE		0


/*	FAB_ARB_CTRL_WRITE_0	0x040C	Fabric Master Port 0 Write Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_WRITE_0_WRITE_SN_ID_0_MSK			SHIFT24(0xff)	
#define	FAB_ARB_CTRL_WRITE_0_WRITE_SN_ID_0_BASE			24
/*		Bit(s) FAB_ARB_CTRL_WRITE_0_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_WRITE_0_WRITE_PRI_VAL_0_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_WRITE_0_WRITE_PRI_VAL_0_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_WRITE_0_WRITE_SP_ID_0_MSK			SHIFT0(0xff)	
#define	FAB_ARB_CTRL_WRITE_0_WRITE_SP_ID_0_BASE			0


/*	FAB_ARB_CTRL_READ_1		0x0428	Fabric Master Port 1 Read Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_READ_1_READ_SN_ID_1_MSK		SHIFT24(0xff)	
#define	FAB_ARB_CTRL_READ_1_READ_SN_ID_1_BASE		24
/*		Bit(s) FAB_ARB_CTRL_READ_1_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_READ_1_READ_PRI_VAL_1_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_READ_1_READ_PRI_VAL_1_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_READ_1_READ_SP_ID_1_MSK		SHIFT0(0xff)	
#define	FAB_ARB_CTRL_READ_1_READ_SP_ID_1_BASE		0

/*	FAB_ARB_CTRL_WRITE_1	0x042C	Fabric Master Port 1 Write Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_WRITE_1_WRITE_SN_ID_1_MSK			SHIFT24(0xff)	
#define	FAB_ARB_CTRL_WRITE_1_WRITE_SN_ID_1_BASE			24
/*		Bit(s) FAB_ARB_CTRL_WRITE_1_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_WRITE_1_WRITE_PRI_VAL_1_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_WRITE_1_WRITE_PRI_VAL_1_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_WRITE_1_WRITE_SP_ID_1_MSK			SHIFT0(0xff)	
#define	FAB_ARB_CTRL_WRITE_1_WRITE_SP_ID_1_BASE			0


/*	FAB_ARB_CTRL_READ_2		0x0448	Fabric Master Port 2 Read Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_READ_2_READ_SN_ID_2_MSK		SHIFT24(0xff)	
#define	FAB_ARB_CTRL_READ_2_READ_SN_ID_2_BASE		24
/*		Bit(s) FAB_ARB_CTRL_READ_2_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_READ_2_READ_PRI_VAL_2_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_READ_2_READ_PRI_VAL_2_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_READ_2_READ_SP_ID_2_MSK		SHIFT0(0xff)	
#define	FAB_ARB_CTRL_READ_2_READ_SP_ID_2_BASE		0

/*	FAB_ARB_CTRL_WRITE_2	0x044C	Fabric Master Port 2 Write Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_WRITE_2_WRITE_SN_ID_2_MSK			SHIFT24(0xff)	
#define	FAB_ARB_CTRL_WRITE_2_WRITE_SN_ID_2_BASE			24
/*		Bit(s) FAB_ARB_CTRL_WRITE_2_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_WRITE_2_WRITE_PRI_VAL_2_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_WRITE_2_WRITE_PRI_VAL_2_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_WRITE_2_WRITE_SP_ID_2_MSK			SHIFT0(0xff)	
#define	FAB_ARB_CTRL_WRITE_2_WRITE_SP_ID_2_BASE			0


/*	FAB_ARB_CTRL_READ_3		0x0468	Fabric Master Port 3 Read Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_READ_3_READ_SN_ID_3_MSK		SHIFT24(0xff)	
#define	FAB_ARB_CTRL_READ_3_READ_SN_ID_3_BASE		24
/*		Bit(s) FAB_ARB_CTRL_READ_3_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_READ_3_READ_PRI_VAL_3_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_READ_3_READ_PRI_VAL_3_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_READ_3_READ_SP_ID_3_MSK		SHIFT0(0xff)	
#define	FAB_ARB_CTRL_READ_3_READ_SP_ID_3_BASE		0

/*	FAB_ARB_CTRL_WRITE_3	0x046C	Fabric Master Port 3 Write Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_WRITE_3_WRITE_SN_ID_3_MSK			SHIFT24(0xff)	
#define	FAB_ARB_CTRL_WRITE_3_WRITE_SN_ID_3_BASE			24
/*		Bit(s) FAB_ARB_CTRL_WRITE_3_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_WRITE_3_WRITE_PRI_VAL_3_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_WRITE_3_WRITE_PRI_VAL_3_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_WRITE_3_WRITE_SP_ID_3_MSK			SHIFT0(0xff)	
#define	FAB_ARB_CTRL_WRITE_3_WRITE_SP_ID_3_BASE			0


/*	FAB_ARB_CTRL_READ_4		0x0488	Fabric Master Port 4 Read Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_READ_4_READ_SN_ID_4_MSK		SHIFT24(0xff)	
#define	FAB_ARB_CTRL_READ_4_READ_SN_ID_4_BASE		24
/*		Bit(s) FAB_ARB_CTRL_READ_4_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_READ_4_READ_PRI_VAL_4_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_READ_4_READ_PRI_VAL_4_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_READ_4_READ_SP_ID_4_MSK		SHIFT0(0xff)	
#define	FAB_ARB_CTRL_READ_4_READ_SP_ID_4_BASE		0

/*	FAB_ARB_CTRL_WRITE_4	0x048C	Fabric Master Port 4 Write Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_WRITE_4_WRITE_SN_ID_4_MSK			SHIFT24(0xff)	
#define	FAB_ARB_CTRL_WRITE_4_WRITE_SN_ID_4_BASE			24
/*		Bit(s) FAB_ARB_CTRL_WRITE_4_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_WRITE_4_WRITE_PRI_VAL_4_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_WRITE_4_WRITE_PRI_VAL_4_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_WRITE_4_WRITE_SP_ID_4_MSK			SHIFT0(0xff)	
#define	FAB_ARB_CTRL_WRITE_4_WRITE_SP_ID_4_BASE			0


/*	FAB_ARB_CTRL_READ_5		0x04A8	Fabric Master Port 5 Read Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_READ_5_READ_SN_ID_5_MSK		SHIFT24(0xff)	
#define	FAB_ARB_CTRL_READ_5_READ_SN_ID_5_BASE		24
/*		Bit(s) FAB_ARB_CTRL_READ_5_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_READ_5_READ_PRI_VAL_5_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_READ_5_READ_PRI_VAL_5_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_READ_5_READ_SP_ID_5_MSK		SHIFT0(0xff)	
#define	FAB_ARB_CTRL_READ_5_READ_SP_ID_5_BASE		0

/*	FAB_ARB_CTRL_WRITE_5	0x04AC	Fabric Master Port 5 Write Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_WRITE_5_WRITE_SN_ID_5_MSK			SHIFT24(0xff)	
#define	FAB_ARB_CTRL_WRITE_5_WRITE_SN_ID_5_BASE			24
/*		Bit(s) FAB_ARB_CTRL_WRITE_5_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_WRITE_5_WRITE_PRI_VAL_5_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_WRITE_5_WRITE_PRI_VAL_5_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_WRITE_5_WRITE_SP_ID_5_MSK			SHIFT0(0xff)	
#define	FAB_ARB_CTRL_WRITE_5_WRITE_SP_ID_5_BASE			0


/*	FAB_ARB_CTRL_READ_6		0x04C8	Fabric Master Port 6 Read Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_READ_6_READ_SN_ID_6_MSK		SHIFT24(0xff)	
#define	FAB_ARB_CTRL_READ_6_READ_SN_ID_6_BASE		24
/*		Bit(s) FAB_ARB_CTRL_READ_6_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_READ_6_READ_PRI_VAL_6_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_READ_6_READ_PRI_VAL_6_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_READ_6_READ_SP_ID_6_MSK		SHIFT0(0xff)	
#define	FAB_ARB_CTRL_READ_6_READ_SP_ID_6_BASE		0

/*	FAB_ARB_CTRL_WRITE_6	0x04CC	Fabric Master Port 6 Write Arbitration
 *									Control Register
 */
/* Slave Port Slot Number */
#define	FAB_ARB_CTRL_WRITE_6_WRITE_SN_ID_6_MSK			SHIFT24(0xff)	
#define	FAB_ARB_CTRL_WRITE_6_WRITE_SN_ID_6_BASE			24
/*		Bit(s) FAB_ARB_CTRL_WRITE_6_RSRV_23_16 reserved */
/* Priority Value */
#define	FAB_ARB_CTRL_WRITE_6_WRITE_PRI_VAL_6_MSK		SHIFT8(0xff)	
#define	FAB_ARB_CTRL_WRITE_6_WRITE_PRI_VAL_6_BASE		8
/* Slave Port ID */
#define	FAB_ARB_CTRL_WRITE_6_WRITE_SP_ID_6_MSK			SHIFT0(0xff)	
#define	FAB_ARB_CTRL_WRITE_6_WRITE_SP_ID_6_BASE			0



#endif
