/******************************************************************************
 *
 * Name:	PMR.h
 * Project:	MORONA ( PXAxxx )
 * Purpose:	Testing
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  (C)Copyright 2005 - 2009 Marvell. All Rights Reserved.
 *  
 *  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF MARVELL.
 *  The copyright notice above does not evidence any actual or intended 
 *  publication of such source code.
 *  This Module contains Proprietary Information of Marvell and should be
 *  treated as Confidential.
 *  The information in this file is provided for the exclusive use of the 
 *  licensees of Marvell.
 *  Such users have the right to use, modify, and incorporate this code into 
 *  products for purposes authorized by the license agreement provided they 
 *  include this notice and the associated copyright notice with any such
 *  product. 
 *  The information in this file is provided "AS IS" without warranty.
 *
 ******************************************************************************/


#ifndef	__INC_PMR_H
#define	__INC_PMR_H

 /*Private Memory Region 
 * The Private Memory Region (PMR) includes the remappable memory region used to internally 
access the private PJ4B-MP Subsystem peripherals, the Interrupt Distributor, the PJ4B-MP CPU 
interrupt interfaces, the Timers and Watchdog, and the MP Control Registers.  
 Any transaction to addresses of the form {PERIPHBASE[18:0], 13’address_low_bits} is redirected to 
the MPCore private memory region.  
 Plese refer to PJ4B-MP datasheet, chapter 8
 */

/*
 *
 *	THE BASE ADDRESSES
 *
 */
// for silicon
/* 
   The PMR Base can be configured through CA9_CPU_CONF_SCU of CIU
   CA9_CPU_CONF_SCU reset value is 0x68EFF, and 0x68EFF<<13 =  0xD1DFE000 
   Just use 0xD1DFE000 as PMR_BASE
   */
#define PMR_BASE      0xD1DFE000


#define	MP_CTRL_BASE	(PMR_BASE+0x0000)
#define	INT_IF_BASE		(PMR_BASE+0x0100)
#define	TMR_WD_BASE		(PMR_BASE+0x0600)
#define	GIC_BASE		(PMR_BASE+0x1000)


/*
 *
 *	THE REGISTER DEFINES
 *
 */
#define	MP_CTRL					(MP_CTRL_BASE+0x0000)
#define	MP_CONF					(MP_CTRL_BASE+0x0004)

#define	MP_TMR_LOAD				(TMR_WD_BASE+0x0000)
#define	MP_TMR_CNTR				(TMR_WD_BASE+0x0004)
#define	MP_TMR_CTRL				(TMR_WD_BASE+0x0008)
#define	MP_TMR_INT_STATUS		(TMR_WD_BASE+0x000C)
#define	MP_WD_LOAD				(TMR_WD_BASE+0x0020)
#define	MP_WD_CNTR				(TMR_WD_BASE+0x0024)
#define	MP_WD_CTRL				(TMR_WD_BASE+0x0028)
#define	MP_WD_INT_STATUS		(TMR_WD_BASE+0x002C)
#define	MP_WD_RESET				(TMR_WD_BASE+0x0030)
#define	MP_WD_DISABLE			(TMR_WD_BASE+0x0034)

#define	GIC_DIST_CTRL			(GIC_BASE+0x0000)
#define	GIC_CTRLR_TYPE			(GIC_BASE+0x0004)
#define	GIC_SECURITY_REG		(GIC_BASE+0x0080)
#define	GIC_ENA_SET				(GIC_BASE+0x0100)
#define	GIC_ENA_CLEAR			(GIC_BASE+0x0180)
#define	GIC_PEND_SET			(GIC_BASE+0x0200)
#define	GIC_PEND_CLEAR			(GIC_BASE+0x0280)
#define	GIC_ACTIVE_BIT			(GIC_BASE+0x0300)
#define	GIC_PRIORITY			(GIC_BASE+0x0400)
#define	GIC_CPU_TARGET			(GIC_BASE+0x0800)
#define	GIC_CONFIG				(GIC_BASE+0x0C00)
#define	GIC_LINE_LEVEL			(GIC_BASE+0x0D00)
#define	GIC_SW_INT				(GIC_BASE+0x0F00)


#define GIC_CPU_IF_CTRL			(INT_IF_BASE+0x0000)
#define GIC_CPU_IF_PMR			(INT_IF_BASE+0x0004)
#define GIC_CPU_IF_BPR			(INT_IF_BASE+0x0008)
#define GIC_CPU_ACK_REG			(INT_IF_BASE+0x000C)
#define GIC_CPU_EOI_REG			(INT_IF_BASE+0x0010)

/*
 *
 *	THE BIT DEFINES
 *
 */
//	MP_TMR_CTRL					(TMR_WD_BASE+0x0008)
#define	MP_TMR_CTRL_PRESCALER_MSK	SHIFT8(0xff)
#define	MP_TMR_CTRL_PRESCALER_BASE	8
#define	MP_TMR_CTRL_IT_ENA			BIT_2
#define	MP_TMR_CTRL_AUTO_RELD		BIT_1
#define	MP_TMR_CTRL_ENA				BIT_0

//	MP_WD_CTRL					(TMR_WD_BASE+0x0028)
#define	MP_WD_CTRL_PRESCALER_MSK	SHIFT8(0xff)
#define	MP_WD_CTRL_PRESCALER_BASE	8
#define	MP_WD_CTRL_MODE				BIT_3
#define	MP_WD_CTRL_IT_ENA			BIT_2
#define	MP_WD_CTRL_AUTO_RELD		BIT_1
#define	MP_WD_CTRL_ENA				BIT_0

#endif	/* __INC_PMR_H */
