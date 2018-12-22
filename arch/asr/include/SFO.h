#ifndef __SFO_H__
#define __SFO_H__

typedef struct
{

    __IO uint32_t FUSE_OVR_REG0;
    __IO uint32_t FUSE_OVR_REG1;
    __IO uint32_t FUSE_OVR_REG2;
    __IO uint32_t FUSE_OVR_REG3;
    __IO uint32_t FUSE_OVR_REG4;
    __IO uint32_t FUSE_OVR_REG5;
    __IO uint32_t OVRD_CONF_REG0;
    __IO uint32_t OVRD_CONF_REG1;
    __IO uint32_t OVRD_CONF_REG2;
    __IO uint32_t OVRD_CONF_REG3;
    __IO uint32_t OVRD_CONF_REG4;
    __IO uint32_t OVRD_CONF_REG5;






}SFO_TypeDef;



#define    SFO_BASE      0xD401EC00


#define    SFO  (( SFO_TypeDef *) SFO_BASE )

//Bitfield Constants Register SFO_FUSE_OVR_REG0
#define FUSE_OVR_REG0_TOP_FUSE_OVR_MASK                     0x0000FFFF
#define FUSE_OVR_REG0_TOP_FUSE_OVR_SHIFT                    0


//Bitfield Constants Register SFO_FUSE_OVR_REG1
#define FUSE_OVR_REG1_APP_FUSE_OVR0_MASK                    0x0000FFFF
#define FUSE_OVR_REG1_APP_FUSE_OVR0_SHIFT                   0


//Bitfield Constants Register SFO_FUSE_OVR_REG2
#define FUSE_OVR_REG2_APP_FUSE_OVR1_MASK                    0x0000FFFF
#define FUSE_OVR_REG2_APP_FUSE_OVR1_SHIFT                   0


//Bitfield Constants Register SFO_FUSE_OVR_REG3
#define FUSE_OVR_REG3_APP_FUSE_OVR2_MASK                    0x0000FFFF
#define FUSE_OVR_REG3_APP_FUSE_OVR2_SHIFT                   0


//Bitfield Constants Register SFO_FUSE_OVR_REG4
#define FUSE_OVR_REG4_APP_FUSE_OVR3_MASK                    0x0000FFFF
#define FUSE_OVR_REG4_APP_FUSE_OVR3_SHIFT                   0


//Bitfield Constants Register SFO_FUSE_OVR_REG5
#define FUSE_OVR_REG5_AIB_FUSE_OVR_MASK                     0x00008000
#define FUSE_OVR_REG5_AIB_FUSE_OVR_SHIFT                    15
#define FUSE_OVR_REG5_APP_FUSE_OVR4_MASK                    0x00007FFF
#define FUSE_OVR_REG5_APP_FUSE_OVR4_SHIFT                   0




//Bitfield Constants Register SFO_OVRD_CONF_REG0
#define OVRD_CONF_REG0_TOP_OVRD_CONF_MASK                    0x0000FFFF
#define OVRD_CONF_REG0_TOP_OVRD_CONF_SHIFT                   0



//Bitfield Constants Register SFO_OVRD_CONF_REG1
#define OVRD_CONF_REG1_APP_OVRD_CONF0_MASK                   0x0000FFFF
#define OVRD_CONF_REG1_APP_OVRD_CONF0_SHIFT                  0



//Bitfield Constants Register SFO_OVRD_CONF_REG2
#define OVRD_CONF_REG2_APP_OVRD_CONF1_MASK                   0x0000FFFF
#define OVRD_CONF_REG2_APP_OVRD_CONF1_SHIFT                  0



//Bitfield Constants Register SFO_OVRD_CONF_REG3
#define OVRD_CONF_REG3_APP_OVRD_CONF2_MASK                   0x0000FFFF
#define OVRD_CONF_REG3_APP_OVRD_CONF2_SHIFT                  0





//Bitfield Constants Register SFO_OVRD_CONF_REG4
#define OVRD_CONF_REG4_APP_OVRD_CONF3_MASK                   0x0000FFFF
#define OVRD_CONF_REG4_APP_OVRD_CONF3_SHIFT                  0




//Bitfield Constants Register SFO_OVRD_CONF_REG5
#define OVRD_CONF_REG5_DISABLE_STRAP_OVR_MASK                0x00008000
//#define OVRD_CONF_REG5_DISABLE_STRAP_OVR_SHIFT               15
#define OVRD_CONF_REG5_DISABLE_AIB_OVR_MASK                  0x00004000
//#define OVRD_CONF_REG5_DISABLE_AIB_OVR_SHIFT                 14
#define OVRD_CONF_REG5_APP_OVRD_CONF4_MASK                   0x00003FFF
#define OVRD_CONF_REG5_APP_OVRD_CONF4_SHIFT                  0






#endif
