/*
 *  mmc.h
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */
 
#ifndef _MMC_CARD_H
#define _MMC_CARD_H

#include "sdh_device.h"

/* Standard eMMC commands (4.51)                    type  argument         response */

/* class 0 --- basic commands */
#define MMC_CMD1_SEND_OP_COND                    1   /* bcr                      R3    */
#define MMC_CMD3_SET_RELATIVE_ADDR           	 3   /* ac                       R1    */
#define MMC_CMD5_SLEEP_AWAKE                     5   /* ac                       R1b   */
#define MMC_CMD6_SWITCH                       	 6   /* ac   [31:0] See below   R1b */
#define MMC_CMD8_SEND_EXT_CSD               	 	 8   /* adtc                        R1  */
#define MMC_CMD14_BUSTEST_R                     14   /* adtc                        R1  */
#define MMC_CMD19_BUSTEST_W                     19   /* adtc                        R1  */

/* class 2 --- Block-oriented read commands */
#define MMC_CMD21_SEND_TUNING_BLOCK         		21   /* adtc                       R1  */

/* class 4 --- Block-oriented write commands */
#define MMC_CMD26_PROGRAM_CID                   26   /* adtc                       R1  */

/* class 6 --- Block-oriented write protection commands */
#define MMC_CMD31_SEND_WRITE_PROI_TYPE          31   /* adtc                       R1  */

/* class 5 --- Erase commands */
#define MMC_CMD35_ERASE_GROUP_START             35   /* ac                       R1  */
#define MMC_CMD36_ERASE_GROUP_END               36   /* ac                       R1  */

/* class 9 --- I/O mode commands */
#define MMC_CMD39_FAST_IO                       39   /* ac                       R4  */
#define MMC_CMD40_GO_IRQ_STATE                  40   /* bcr                       R5  */

/* class 10 --- Security Protocols */
#define MMC_CMD53_PROTOCOL_RD                   53   /* adtc                       R1  */
#define MMC_CMD54_PROTOCOL_WR                   54   /* adtc                       R1  */

/* For auto command cmd12 and cmd23, used to end multi blocks transfer */
#define MMC_DISABLE_AUTO_CMD SDH_DISABLE_AUTO_CMD
#define MMC_AUTO_CMD23 SDH_AUTO_CMD23
#define MMC_AUTO_CMD12 SDH_AUTO_CMD12

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

struct mmc_cid
{
	uint8_t 		mid;
	uint8_t		cbx;
	uint16_t 	oid;
	uint8_t 		pnm[6];
	uint8_t 		prv_n;
	uint8_t 		prv_m;
	uint32_t 	psn;
	uint16_t 	mdt_year;
	uint8_t 		mdt_month;
};

struct mmc_csd
{
	uint8_t 		structure;
	uint8_t		spec_vers;  /* used by mmc card only */
	uint32_t		taac_ns;
	uint8_t		nsac_clks;

	uint32_t		max_dtr;	/* common SD card - 0x32,that is 25MHz, high speed - 0x5A, that is 50MHz */
	uint32_t		erase_size;  /* in sectors, that is 512bytes */

	uint16_t		cmd_class;
	uint8_t		read_blkbits;
	uint8_t		read_bl_partial:1;
	uint8_t		write_blk_misalign:1;
	uint8_t		read_blk_misalign:1;
	uint8_t		dsr_imp:1;
	uint8_t		erase_blk_en:1;				
	
	uint32_t		capacity;

	uint8_t		vdd_r_curr_min;
	uint8_t		vdd_r_curr_max;
	uint8_t		vdd_w_curr_min;
	uint8_t		vdd_w_curr_max;
	
	uint8_t		erase_grp_size:5;  /* used by mmc card only */
	uint8_t		erase_grp_mul:5;  /* used by mmc card only */
	
	uint8_t		wp_grp_size:7;
	uint8_t		wp_grp_en:1;
	
	uint8_t		default_ecc;  /* used by mmc onlu */

	uint8_t		r2w_factor:3;
	uint8_t		write_blkbits:4;
	uint8_t		write_bl_partial:1;	
	uint8_t		content_prot_app;  /* used by mmc only */
	
	uint16_t		file_format_grp:1;
	uint16_t		copy:1;
	uint16_t		perm_write_protect:1;
	uint16_t		tmp_write_protect:1;
	uint16_t		file_format:2;
	uint16_t		ecc:7;  /* used by mmc onlu */
	uint16_t		reserved:1;
};


struct mmc_csd_ext
{
	uint8_t			ext_security_err;
	uint8_t			s_cmd_set;
	uint8_t			hpi_features;
	uint8_t			bkops_support;
	uint8_t			max_packed_read;
	uint8_t			max_packed_writes;
	uint8_t			data_tag_support;
	uint8_t			tag_unit_size;
	uint8_t			tag_res_size;
	uint8_t			context_cap;
	uint8_t			large_unit_size_m1;
	uint8_t			ext_support;
	uint32_t			cache_size;
	uint8_t			generic_cmd6_time;
	uint8_t			power_off_long_tim;
	uint8_t			bkops_status;
	uint32_t			correctly_prg_sect;
	uint8_t			ini_timeout_ap;
	uint8_t			pwr_cl_ddr_52_360;
	uint8_t			pwr_cl_ddr_52_195;
	uint8_t			pwr_cl_ddr_200_195;
	uint8_t			pwr_cl_ddr_200_130;
	uint8_t			min_perf_ddr_w_8_52;
	uint8_t			min_perf_ddr_r_8_52;
	uint8_t			trim_mult;
	uint8_t			sec_feature_support;

	uint8_t			sec_erase_mult;
	uint8_t			sec_trim_mult;
	uint8_t			boot_info;
	uint8_t			boot_size_mult;
	uint8_t			acc_size;
	uint8_t			hc_erase_grp_size;
	uint8_t			erase_timeout_mult;
	uint8_t			rel_wr_sec_c;
	uint8_t			hc_wp_grp_size;
	uint8_t			s_c_vcc;
	uint8_t			s_c_vccq;
	uint8_t			s_a_timeout;
	uint32_t			sec_count;
	uint8_t 			min_perf_w_8_52;
	uint8_t			min_perf_r_8_52;
	uint8_t			min_perf_w_8_26_4_52;
	uint8_t			min_perf_r_8_26_4_52;
	uint8_t			min_perf_w_4_26;
	uint8_t			min_perf_r_4_26;
	uint8_t			pwr_cl_26_360;
	uint8_t			pwr_cl_52_360;
	uint8_t			pwr_cl_26_195;
	uint8_t			pwr_cl_52_195;
	uint8_t			partition_switch_time;
	uint8_t			out_of_interrupt_time;
	uint8_t			driver_strength;
	uint8_t			device_type;
	uint8_t			csd_str_ver;
	uint8_t			ext_csd_rev;

	uint8_t			cmd_set;
	uint8_t			cmd_set_rev;
	uint8_t			power_class;
	uint8_t			hs_timing;
	uint8_t			bus_width;
	uint8_t			erased_mem_cont;
	uint8_t			partition_config;
	uint8_t			boot_config_prot;
	uint8_t			boot_bus_conditions;
	uint8_t			erase_group_def;
	uint8_t			boot_wp_status;
	uint8_t			boot_wp;
	uint8_t			user_wp;
#define US_PWR_WP_EN	BIT_0
#define US_PERM_WP_EN	BIT_2
#define US_PWR_WP_DIS	BIT_3
#define US_PERM_WP_DIS	BIT_4
#define CD_PERM_WP_DIS	BIT_6
#define PERM_PSWD_DIS	BIT_7
	uint8_t			fw_config;
	uint8_t			rpmb_size_mult;
	uint8_t			wr_rel_set;
	uint8_t			wr_rel_param;
	uint8_t			sanitize_start;
	uint8_t			bkops_start;
	uint8_t			bkops_en;
	uint8_t			rst_n_function;
	uint8_t			hpi_mgmt;
	uint8_t			partition_support;
	uint8_t			max_enh_size_mult[3];
	uint8_t			partition_attribute;
	uint8_t			partition_setting_completed;

	uint8_t			gp_size_mult_gp0[3];
	uint8_t			gp_size_mult_gp1[3];
	uint8_t			gp_size_mult_gp2[3];
	uint8_t			gp_size_mult_gp3[3];
	uint8_t			enh_size_mult[3];
	uint32_t			enh_start_addr;
	uint8_t			sec_bad_blk_mgmnt;
	uint8_t			tcase_support;
	uint8_t			periodic_wakeup;
	uint8_t			program_cid_csd_ddr_support;
	uint8_t			native_sector_size;
	uint8_t			use_native_sector;
	uint8_t			data_sector_size;
	uint8_t			ini_timeout_emu;
	uint8_t			class_6_ctrl;
	uint8_t			dyncap_needed;
	
	uint8_t			exception_events_ctrl[2];
	uint8_t			exception_events_status[2];
	uint16_t			ext_partitions_attribute;
	uint8_t			context_conf[15];
	uint8_t			packed_command_status;
	uint8_t			packed_failure_index;
	uint8_t			power_off_notification;
	uint8_t			cache_ctrl;
	uint8_t			flush_cache;
	
	
#if 0
	uint8_t			rev;
	uint8_t			erase_group_def;
	uint32_t			sa_timeout;		/* Units: 100ns */
	uint32_t			hs_max_dtr;
	uint32_t			sectors;
	uint32_t			card_type;
	uint32_t			hc_erase_size;		/* In sectors */
	uint32_t			hc_erase_timeout;	/* In milliseconds */
	uint32_t			sec_trim_mult;	/* Secure trim multiplier  */
	uint32_t			sec_erase_mult;	/* Secure erase multiplier */
	uint32_t			trim_timeout;		/* In milliseconds */
#endif
};

struct sdh_device;

struct mmc_card
{
	struct sdh_device dev;
	uint32_t		state;		/* (our) card state */
#define CARD_STATE_PRESENT	(1<<0)		/* present in sysfs */
#define CARD_STATE_READONLY	(1<<1)		/* card is read-only */
#define CARD_STATE_HIGHSPEED	(1<<2)		/* card is in high speed mode */
#define CARD_STATE_BLOCKADDR	(1<<3)		/* card uses block-addressing */
#define CARD_STATE_HIGHSPEED_DDR (1<<4)	/* card is in high speed mode */
#define CARD_CAPACITY_MASK		SHIFT5(0x03)  /* 0 -- SDSC, 1 -- SDHC, 2 -- SDXC */
#define CARD_CAPACITY_BASE		5

	uint32_t 		card_version;  /* 0 -- ver1.x, 1 -- ver2.00 or later */
	
	struct mmc_cid 		cid;
	struct mmc_csd 		csd;
	struct mmc_csd_ext 	csd_ext;
	
	uint32_t partition;
	uint32_t partition_addr_offset;  /* only read/write begin with this offset */
	uint32_t partition_avail_size;  /* available parition size in blocks */
	
	uint8_t in_sleep;
};

enum parition_id {
	MMC_PARTITION_DEFAULT = 0x0,
	MMC_PARTITION_BOOT1,
	MMC_PARTITION_BOOT2,
	MMC_PARTITION_RPMB,
	MMC_PARTITION_GP1,
	MMC_PARTITION_GP2,
	MMC_PARTITION_GP3,
	MMC_PARTITION_GP4,
	MMC_TOTAL_PARTITION_NUM,
};

#define MMC_INIT_FREQ 400000

#define MMC_DRIVER_STRENGTH_TYPE0 (1<<0)
#define MMC_DRIVER_STRENGTH_TYPE1 (1<<1)
#define MMC_DRIVER_STRENGTH_TYPE2 (1<<2)
#define MMC_DRIVER_STRENGTH_TYPE3 (1<<3)
#define MMC_DRIVER_STRENGTH_DEFAULT (1<<7)

/* definition in extended CSD byte 185*/
#define MMC_TIMEING_INTERFACE_BACKWARDS 0
#define MMC_TIMEING_INTERFACE_HS 1
#define MMC_TIMEING_INTERFACE_HS200 2


#define MMC_DEVICE_TYPE_HS200_SDR_1V2 (1<<5)
#define MMC_DEVICE_TYPE_HS200_SDR_1V8 (1<<4)
#define MMC_DEVICE_TYPE_HS52_DDR_1V2 (1<<3)
#define MMC_DEVICE_TYPE_HS52_DDR_1V8_3V (1<<2)
#define MMC_DEVICE_TYPE_HS52_SDR_ALL (1<<1)
#define MMC_DEVICE_TYPE_HS26_SDR_ALL (1<<0)

#define MMC_BUS_WIDTH_BIT1 (1<<0)
#define MMC_BUS_WIDTH_BIT4 (1<<2)
#define MMC_BUS_WIDTH_BIT8 (1<<3)


/* speed switch macro */
#define MMC_BUS_SPEED_HS26        SDH_BUS_SPEED_DEFAULT
#define MMC_BUS_SPEED_HS52_DDR    SDH_BUS_SPEED_DDR_CLK_50M
#define MMC_BUS_SPEED_HS52_SDR    SDH_BUS_SPEED_SDR_CLK_50M
#define MMC_BUS_SPEED_HS200       SDH_BUS_SPEED_SDR_CLK_208M
#define MMC_BUS_SPEED_SEARCH      SDH_BUS_SPEED_SEARCH

#define MMC_CMD_RETRIES 3

#define MMC_WP_TYPE_PWR  BIT_0
#define MMC_WP_TYPE_PERM BIT_1
#define MMC_WP_TYPE_TEMP BIT_2

struct mmc_card *get_mmc_card(void);

uint32_t __mmc_card_init_and_ident(struct mmc_card *card);

uint32_t mmc_set_erase_group_start(struct mmc_card *card, uint32_t block_start);
uint32_t mmc_set_erase_group_end(struct mmc_card *card, uint32_t block_end);
uint32_t mmc_start_erase(struct mmc_card *card, uint32_t argu);
uint32_t __mmc_switch_partiton(struct mmc_card *card, uint32_t partition);

#endif


