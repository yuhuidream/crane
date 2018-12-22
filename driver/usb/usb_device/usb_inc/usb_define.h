/******************************************************************************
	Copyright (C) 2001 UTStarcom (Hangzhou) Telecom Co., Ltd.
	All rights reserved.
    Contains confidential proprietary information of UTStarcom, Inc.
    Reverse engineering is prohibited.
    The copyright notice does not imply publication.
	
	Project Name: F16
	Project No.	: 
	File Name	: define.h
	Module		: apframe
	Description	: The first header file to be included
	Compiler	: TLCS-900 or Microsoft Visual C++ 6.0
  
    ************** REVISION HISTORY *******************************************
	Date     Author			Reference
	======== ==========		==========================
	7.2002	 Inherited from J22

******************************************************************************/

#ifndef _USB_DEFINE_H_
#define _USB_DEFINE_H_

#ifdef __cplusplus
extern "C" {
#endif
	
#define _ASSERT_ON	//open the assert
	
#ifndef S8
	typedef	 char				S8;			/*  signed 8 bit data  */
#endif
#ifndef S16
	typedef	short				S16;		/*  signed 16 bit data */
#endif
#ifndef S32
	typedef	long				S32;		/*  signed 32 bit data */
#endif
#ifndef VOID
	typedef	void				VOID;		/* void */
#endif
#ifndef FLOAT
	typedef	float				FLOAT;		/* float */
#endif
#ifndef DOUBLE
	typedef	double				DOUBLE;		/* double */
#endif
	
#ifndef U8
	typedef	unsigned char		U8;			/*  unsigned 8 bit data  */
#endif
#ifndef U16
	typedef	unsigned short		U16;		/*  unsigned 16 bit data */
#endif
#ifndef U32
	typedef	unsigned long		U32;		/*  unsigned 32 bit data */
#endif
#ifndef U64
    typedef	unsigned long long	U64;		/*  unsigned 64 bit data */
#endif

typedef S8 *            LP_S8;      
typedef S16*           LP_S16;    
typedef S32*           LP_S32;    
typedef U8*            LP_U8;      
typedef U16*           LP_U16;    
typedef U32*           LP_U32;    
typedef VOID*          LP_VOID; 

typedef void*           v_pointer;
	
//=======================================================================================	
//=======================================================================================
	/* To support both PC Platform (32 bit int) and  DO please use following data type */
	/* Basic type difinitions */
#ifndef _VOID
	typedef	void				_VOID;		/* _VOID */
#endif
#ifndef _FLOAT
	typedef	float				_FLOAT;		/* float */
#endif
#ifndef _DOUBLE
	typedef	double				_DOUBLE;		/* double */
#endif


	typedef	char				_CHAR;			/*  signed 8 bit data  */
	typedef	short				_SHORT;		/*  signed 16 bit data */
	typedef	int					_INT;			/*  signed 32 bit data */
	typedef	long				_LONG;			/*  signed 32 bit data in rvct1.2  */
	
	typedef	unsigned char		_BYTE;			/*  unsigned 8 bit data  */
	typedef	unsigned short		_WORD;			/*  unsigned 16 bit data */
	typedef	unsigned int		_DWORD;		/*  unsigned 32 bit data */

	typedef 	volatile _CHAR		_VCHAR;		// Volatile type
	typedef 	volatile _SHORT 	_VSHORT;
	//typedef 	volatile _INT		_VINT;

	typedef 	volatile _BYTE		_VBYTE;		// Volatile type
	typedef 	volatile _WORD 		_VWORD;
	typedef 	volatile _DWORD		_VDWORD;
	
//=======================================================================================	
//=======================================================================================

#ifndef BOOLEAN
	typedef U8 BOOLEAN;
#endif

#ifndef _BOOLEAN
	typedef _BYTE _BOOLEAN;
#endif

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE    0
#endif
	
#ifndef SUCCESS
	typedef enum
	{
		FAILURE=0,
		SUCCESS
	}RESULT;
#endif
	
#ifndef NULL
#define NULL			((LP_VOID)0)// null pointer
#endif

#define HAL_READ_UINT8( _register_, _value_ ) \
        ((_value_) = *((volatile U8 *)(_register_)))

#define HAL_WRITE_UINT8( _register_, _value_ ) \
        (*((volatile U8 *)(_register_)) = (_value_))

#define HAL_READ_UINT16( _register_, _value_ ) \
        ((_value_) = *((volatile U16 *)(_register_)))

#define HAL_WRITE_UINT16( _register_, _value_ ) \
        (*((volatile U16 *)(_register_)) = (_value_))

#define HAL_READ_UINT32( _register_, _value_ ) \
        ((_value_) = *((volatile U32 *)(_register_)))

#define HAL_WRITE_UINT32( _register_, _value_ ) \
        (*((volatile U32 *)(_register_)) = (_value_))


#define BB_REGS32(_reg_)  (*(volatile U32*)(_reg_))	
	
#ifdef __cplusplus
}
#endif

#endif /* END of _DEFINE_H */








