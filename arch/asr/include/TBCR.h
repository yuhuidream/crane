//Trackball Control Register

#ifndef __TBCR_H__
#define __TBCR_H__

typedef struct
{



    __IO uint32_t TBCR    ; 
    __I  uint32_t TBCNTR  ; 
    __O  uint32_t TBSBC   ; 
     __IO uint32_t ENH_ROTARY_CNT;

}TBCR_TypeDef;





#define    TBCR_BASE      0xD4013000
#define    TBCR  (( TBCR_TypeDef *) TBCR_BASE )


/*
 *
 *	THE BIT DEFINES
 *
 */
/*	TB_CR			0x0000	Trackball Control Register */
/*		Bit(s) TB_CR_RSRV_31_11 reserved */
/* Read value of the sticky bit */
#define	TB_CR_TBSB					BIT_10				
/* Y+ and Y- divider setting */
#define	TB_CR_Y_FILTER_MSK			SHIFT6(0xf)			
#define	TB_CR_Y_FILTER_BASE			6
/* X+ and X- divider setting */
#define	TB_CR_X_FILTER_MSK			SHIFT2(0xf)			
#define	TB_CR_X_FILTER_BASE			2
#define	TB_CR_TBRST					BIT_1				/* Trackball module enable */
/*		Bit(s) TB_CR_RSRV_0 reserved */

/*	TB_CNTR			0x0004	Trackball Counter Register */
#define	TB_CNTR_NY_CNT_MSK		SHIFT24(0xff)	/* -Y counter */
#define	TB_CNTR_NY_CNT_BASE		24
#define	TB_CNTR_PY_CNT_MSK		SHIFT16(0xff)	/* +Y counter */
#define	TB_CNTR_PY_CNT_BASE		16
#define	TB_CNTR_NX_CNT_MSK		SHIFT8(0xff)	/* -X counter */
#define	TB_CNTR_NX_CNT_BASE		8
#define	TB_CNTR_PX_CNT_MSK		SHIFT0(0xff)	/* +X counter */
#define	TB_CNTR_PX_CNT_BASE		0

/*	TB_SBC			0x0008	Trackball Sticky Bit Clear Register */
/*		Bit(s) TB_SBC_RSRV_31_1 reserved */
#define	TB_SBC_TBSBC				BIT_0				/* TBSBC */

/*	ENH_ROTARY_CNT	0x000C	Enhanced-Rotary Counter Register */
/*		Bit(s) ENH_ROTARY_CNT_RSRV_31_4 reserved */
/* Enhanced-Rotary Counter */
#define	ENH_ROTARY_CNT_CNT_MSK				SHIFT0(0xf)			
#define	ENH_ROTARY_CNT_CNT_BASE				0













#endif
