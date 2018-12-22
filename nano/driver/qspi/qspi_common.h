#ifndef _QSPI_COMMON_H
#define _QSPI_COMMON_H

// ---------------------------------------------------------------
// Memory Map
// ---------------------------------------------------------------
#define QSPI0_REG_BASE       0xd420B000 //QSPI1_IPS_BASE_ADDR
#define QSPI0_ARDB_BASE      0xa0000000 //AHB RX Data Buffer base addr(QSPI_ARDB0 to QSPI_ARDB31)
#define QSPI0_AMBA_BASE      0x80000000 //AHB base addr
#define QSPI0_FLASH_A1_BASE  0x80000000
#define QSPI0_FLASH_A1_TOP   0x88000000
#define QSPI0_FLASH_A2_BASE  0x88000000
#define QSPI0_FLASH_A2_TOP   0x90000000
#define QSPI0_FLASH_B1_BASE  0x90000000
#define QSPI0_FLASH_B1_TOP   0x98000000
#define QSPI0_FLASH_B2_BASE  0x98000000
#define QSPI0_FLASH_B2_TOP   0xa0000000

// ---------------------------------------------------------------
// Register definitions 
// ---------------------------------------------------------------
#define QSPI_MCR_OFFSET                      0x000
#define QSPI_IPCR_OFFSET                     0x008
#define QSPI_FLSHCR_OFFSET                   0x00C
#define QSPI_BUF0CR_OFFSET                   0x010
#define QSPI_BUF1CR_OFFSET                   0x014
#define QSPI_BUF2CR_OFFSET                   0x018
#define QSPI_BUF3CR_OFFSET                   0x01C
#define QSPI_BFGENCR_OFFSET                  0x020 
#define QSPI_SOCCR_OFFSET                    0x024 
#define QSPI_BUF0IND_OFFSET                  0x030 
#define QSPI_BUF1IND_OFFSET                  0x034 
#define QSPI_BUF2IND_OFFSET                  0x038 
#define QSPI_SFAR_OFFSET                     0x100 
#define QSPI_SMPR_OFFSET                     0x108 
#define QSPI_RBSR_OFFSET                     0x10C 
#define QSPI_RBCT_OFFSET                     0x110 
#define QSPI_TBSR_OFFSET                     0x150 
#define QSPI_TBDR_OFFSET                     0x154 
#define QSPI_TBCT_OFFSET                     0x158 
#define QSPI_SR_OFFSET                       0x15C 
#define QSPI_FR_OFFSET                       0x160 
#define QSPI_RSER_OFFSET                     0x164 
#define QSPI_SPNDST_OFFSET                   0x168 
#define QSPI_SPTRCLR_OFFSET                  0x16C 
#define QSPI_SFA1AD_OFFSET                   0x180 
#define QSPI_SFA2AD_OFFSET                   0x184 
#define QSPI_SFB1AD_OFFSET                   0x188 
#define QSPI_SFB2AD_OFFSET                   0x18C 
#define QSPI_DLPV_OFFSET                     0x190 
#define QSPI_RBDR0_OFFSET                    0x200 
#define QSPI_LUTKEY_OFFSET                   0x300 
#define QSPI_LCKCR_OFFSET                    0x304 
#define QSPI_LUT0_OFFSET                     0x310
#define QSPI_LUT1_OFFSET                     0x314
#define QSPI_LUT2_OFFSET                     0x318
#define QSPI_LUT3_OFFSET                     0x31C

// ---------------------------------------------------------------
// Register definitions 
// ---------------------------------------------------------------
#define QSPI0_MCR            (QSPI0_REG_BASE+QSPI_MCR_OFFSET)
#define QSPI0_IPCR           (QSPI0_REG_BASE+QSPI_IPCR_OFFSET)
#define QSPI0_FLSHCR         (QSPI0_REG_BASE+QSPI_FLSHCR_OFFSET)
#define QSPI0_BUF0CR         (QSPI0_REG_BASE+QSPI_BUF0CR_OFFSET)
#define QSPI0_BUF1CR         (QSPI0_REG_BASE+QSPI_BUF1CR_OFFSET)
#define QSPI0_BUF2CR         (QSPI0_REG_BASE+QSPI_BUF2CR_OFFSET)
#define QSPI0_BUF3CR         (QSPI0_REG_BASE+QSPI_BUF3CR_OFFSET)
#define QSPI0_BFGENCR        (QSPI0_REG_BASE+QSPI_BFGENCR_OFFSET)
#define QSPI0_SOCCR          (QSPI0_REG_BASE+QSPI_SOCCR_OFFSET)
#define QSPI0_BUF0IND        (QSPI0_REG_BASE+QSPI_BUF0IND_OFFSET)
#define QSPI0_BUF1IND        (QSPI0_REG_BASE+QSPI_BUF1IND_OFFSET)
#define QSPI0_BUF2IND        (QSPI0_REG_BASE+QSPI_BUF2IND_OFFSET)
#define QSPI0_SFAR           (QSPI0_REG_BASE+QSPI_SFAR_OFFSET)
#define QSPI0_SMPR           (QSPI0_REG_BASE+QSPI_SMPR_OFFSET)
#define QSPI0_RBSR           (QSPI0_REG_BASE+QSPI_RBSR_OFFSET)
#define QSPI0_RBCT           (QSPI0_REG_BASE+QSPI_RBCT_OFFSET)
#define QSPI0_TBSR           (QSPI0_REG_BASE+QSPI_TBSR_OFFSET)
#define QSPI0_TBDR           (QSPI0_REG_BASE+QSPI_TBDR_OFFSET)
#define QSPI0_TBCT           (QSPI0_REG_BASE+QSPI_TBCT_OFFSET)
#define QSPI0_SR             (QSPI0_REG_BASE+QSPI_SR_OFFSET)
#define QSPI0_FR             (QSPI0_REG_BASE+QSPI_FR_OFFSET)
#define QSPI0_RSER           (QSPI0_REG_BASE+QSPI_RSER_OFFSET)
#define QSPI0_SPNDST         (QSPI0_REG_BASE+QSPI_SPNDST_OFFSET)
#define QSPI0_SPTRCLR        (QSPI0_REG_BASE+QSPI_SPTRCLR_OFFSET)
#define QSPI0_SFA1AD         (QSPI0_REG_BASE+QSPI_SFA1AD_OFFSET)
#define QSPI0_SFA2AD         (QSPI0_REG_BASE+QSPI_SFA2AD_OFFSET)
#define QSPI0_SFB1AD         (QSPI0_REG_BASE+QSPI_SFB1AD_OFFSET)
#define QSPI0_SFB2AD         (QSPI0_REG_BASE+QSPI_SFB2AD_OFFSET)
#define QSPI0_DLPV           (QSPI0_REG_BASE+QSPI_DLPV_OFFSET)
#define QSPI0_RBDR0          (QSPI0_REG_BASE+QSPI_RBDR0_OFFSET)
#define QSPI0_LUTKEY         (QSPI0_REG_BASE+QSPI_LUTKEY_OFFSET)
#define QSPI0_LCKCR          (QSPI0_REG_BASE+QSPI_LCKCR_OFFSET)
#define QSPI0_LUT0           (QSPI0_REG_BASE+QSPI_LUT0_OFFSET)
#define QSPI0_LUT1           (QSPI0_REG_BASE+QSPI_LUT1_OFFSET)
#define QSPI0_LUT2           (QSPI0_REG_BASE+QSPI_LUT2_OFFSET)
#define QSPI0_LUT3           (QSPI0_REG_BASE+QSPI_LUT3_OFFSET)

// ---------------------------------------------------------------
// Enumeration & Structure
// ---------------------------------------------------------------
enum QSPI_INST_E {
	QSPI_INSTR_STOP = 0x0,
	QSPI_INSTR_CMD = 0x1,
	QSPI_INSTR_ADDR = 0x2,
	QSPI_INSTR_DUMMY = 0x3,
	QSPI_INSTR_MODE = 0x4,
	QSPI_INSTR_MODE2 = 0x5,
	QSPI_INSTR_MODE4 = 0x6,
	QSPI_INSTR_READ = 0x7,
	QSPI_INSTR_WRITE = 0x8,
	QSPI_INSTR_JMP_ON_CS = 0x9,
	QSPI_INSTR_ADDR_DDR = 0xA,
	QSPI_INSTR_MODE_DDR = 0xB,
	QSPI_INSTR_MODE2_DDR = 0xC,
	QSPI_INSTR_MODE4_DDR = 0xD,
	QSPI_INSTR_READ_DDR = 0xE,
	QSPI_INSTR_WRITE_DDR = 0xF,
	QSPI_INSTR_DATA_LEARN = 0x10
};

enum QSPI_PAD_E {
	QSPI_PAD_1X = 0x0,
	QSPI_PAD_2X = 0x1,
	QSPI_PAD_4X = 0x2,
	QSPI_PAD_RSVD = 0x3
};

#define	EIO		5	/* I/O error */
#define	ENOMEM		12	/* Out of memory */
#define	ENODEV		19	/* No such device */
#define	EINVAL		22	/* Invalid argument */
#define	EBADMSG		74	/* Not a data message */
#define	ETIMEDOUT	110	/* Connection timed out */
#define	EUCLEAN		117	/* Chip needs cleaning */
#define ENOTSUPP	524	/* Operation is not supported */

#endif // QSPI_COMMON_H