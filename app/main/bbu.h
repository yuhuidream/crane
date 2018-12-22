#ifndef bbu_h
#define bbu_h

#include "qp_port.h"
#include "predefines.h"
#include "print.h"
#include "utils.h"
#include "system_mrvl.h"
#include "bbu_help.h"
#include "bbu_test.h"
#include "interrupt.h"



#define MAX_CMD_NUM         256
#define MAX_LINE_LEN        512     //(256 - 4)  //to match uint8_t size
#define NHISTORY            16

#define CMDLINE_MAX_ARGS    16
#define SCREEN_TEST_DEADLINE 15  /* Each command execution time limit in screen test, in seconds */
#define BBU_TICKS_PER_SEC    (200)  /* 1 tick == 5ms */
/*
 * Macro for screen test result return
*/
#define SCREEN_TEST_PASS 1
#define SCREEN_TEST_FIAL 2
#define SCREEN_TEST_COMPLETED_NEED_CHECK 3
#define SCREEN_TEST_TIMEOUT 4
#define SCREEN_TEST_UNKNOWN_CMD 5

#ifdef MULTI_UART
#define UART_AP_ID          0
#define UART_CP_ID          1
#endif

#define BBU_PROMPT         "CR5> "
#define MY_NAME            "CR5"

/*..........................................................................*/
enum BBUSignals {
    TIME_TICK_SIG = Q_USER_SIG,

    /*insert other published signls here...*/
    MAX_PUB_SIG,

    COMPO_PUB_TEST_SIG,

    /*insert other component level published signls here...*/
    MAX_COMPO_PUB_SIG,

    COMPO_PUB_SIG,
    COMPO_SUBSCR_SIG,
    COMPO_UNSUBSCR_SIG,

    /*The commands signals*/
    SYSTEM_CMD_DEMO_SIG,
    SYSTEM_CMD_HELP_SIG,

    /*The console AO signals*/
    COM_PRINT_REQUEST_SIG,                  /* the print request signal */
    COM_TX_DATA_READY_SIG,                         /* the invented reminder signal */
    COM_DATA_REQUEST_SIG,                   /* Tx FIFO trigger interrupt notify signal */
    COM_PRINT_DONE_SIG,

    /*The cli AO signals*/
    COM_RX_REQUEST_SIG,                     /*Rx FIFO half full or time out send this event*/
    COM_RX_NEXT_BYTE_SIG,                       /*self-remainder to pre-parsing the next byte/block */
    COM_RX_CMD_READY_SIG,                   /*Auto Screen send this signal to tell related AOs to execute command*/

    CO_STRESS_PASS_SIG,
    CO_STRESS_FAIL_SIG,

    /*The PMU AO signals*/
    PMU_PPSET_SIG,                                    /*  The PP change signal*/
    PMU_OPSET_SIG,                                    /* The OP change signal*/
    PMU_LPMSET_SIG,                                   /*  LPM modes test signal*/
    PMU_IPWK_SIG,                                    /*  inter processor Wake up signal*/
    PMU_MSA_SIG,

    /* The Input AO signals */
    KEYP_DKIN_SIG,                                     /* Direct key input...  */
    KEYP_MKIN_SIG,                                     /* Matric key input     */
    KEYP_DECODE_SIG,                                   /* set key decode mode  */
    KEYP_REGDUMP_SIG,                                  /* register dump mode   */
    KEYP_INIT_SIG,                                     /* keypad init signal   */
    KEYP_READY_SIG,                                    /* keypad initialized   */
    TP_INIT_SIG,                                       /* touch init signal    */
    TP_WAKEUP_SIG,                                     /* touch wake up signal */
    TP_SUSPEND_SIG,                                    /* touch suspend signal */

    /* DMA AO Signals */
    DMA_TRAN_FROM_COM_SIG,                  /* DMA Transfer From Console */
    DMA_TRAN_SIG,                           /* DMA Transfer From Other AOs */
    DMA_STRESS_SIG,                         /* DMA stress signal from stress irq */

    /* Timer AO Signals */
    TIMER_FROM_COM_SIG,
    TIMER_HANDLE_SIG,
    TIMER_SIG,
    TIMER_RTC_SIG,
#ifdef CONFIG_GENERIC_TIME
    TIMER_GEN_TEST_SIG,
#endif

    /* Sensors AO signals */
    BBU_GSEN_SIG,                            /* G-sensor and e-compass test */
    BBU_LSEN_SIG,                            /* light sensor test signal    */
    BBU_GYRO_SIG,                            /*    gyroscope test signal    */
    BBU_TSEN_SIG,                            /* temperature sensor test sig */
    BBU_TTDM_SIG,                   /* temperature-time dynamic measurement */
    TTDM_CONT_SIG,                           /*   TTDM continuous signal    */
    BBU_BARM_SIG,                            /* baromter sensor test signal */
    BBU_PWM3_SIG,                            /* PWM4 output test signal */
    BBU_MAGN_SIG,                            /* Motion sensor magnetic test */
    BBU_MOTI_SIG,                            /* Motion sensor MPU 6050 test */
    BBU_ALSP_SIG,                            /* A & P sensor test */

    /* i2c AO signals */
    I2C_INIT_SIG,
    I2C_READ_SIG,                            /*   read I2C                  */
    I2C_WRITE_SIG,                           /*   write I2C                 */
    I2C_ERROR_SIG,                           /*   read/write error          */
    I2C_SUC_SIG,                             /*   read/write success        */
    I2C_SCAN_SIG,                            /*   scan command              */
    I2C_MODE_SIG,                            /*   set mode command          */
    I2C_RD_TEST_SIG,                         /*   read command              */
    I2C_WR_TEST_SIG,                         /*   write command             */
    I2C_ADDR_SIG,                            /*   address i2c slave         */
    I2C_DUMP_SIG,                            /*   i2c dump command          */
    I2C_BED_SIG,                             /*   I2C error signal          */
    I2C_IRF_SIG,                             /*   I2C receive full          */
    I2C_ITE_SIG,                             /*   I2C transmit empty        */
    I2C_TIMEOUT_SIG,                         /*   I2C time out signal       */

    /* PCIE AO signals */
    PCIE_INIT_SIG,
    PCIE_TEST_SIG,

    /* NFC AO signals */
    NFC_IRQ_SIG,                              /*   NFC irq reach             */
    NFC_INIT_SIG,                             /*   NFC initialize            */
    NFC_TEST_SIG,                             /*   NFC  test signal          */

	/* SDH AO signals */
    SD_CARD_INIT_SIG,
    SD_SET_CARD_MODE_SIG,
    SD_CARD_INSERT_SIG,
    SD_CARD_REMOVE_SIG,
    SD_INIT_REQ_SIG,
    SD_INIT_SUC_SIG,
    SD_INIT_FAIL_SIG,
    SD_TRANS_REQ_SIG,
    SD_TRANS_SUC_SIG,
    SD_TRANS_FAIL_SIG,
    SD_READ_FROM_CLI_SIG,
    SD_WRITE_FROM_CLI_SIG,
    SD_ERASE_FROM_CLI_SIG,
    SD_DUMP_REG_SIG,
    SD_TEST_CMD_SIG,
    SD_STRESS_CMD_SIG,
    SD_STRESS_CONTINUE_SIG,

    MMC_CARD_INIT_SIG,
    MMC_SET_CARD_MODE_SIG,
    MMC_TRANS_REQ_SIG,
    MMC_TRANS_SUC_SIG,
    MMC_TRANS_FAIL_SIG,
    MMC_INIT_REQ_SIG,
    MMC_INIT_SUC_SIG,
    MMC_INIT_FAIL_SIG,
    MMC_READ_FROM_CLI_SIG,
    MMC_WRITE_FROM_CLI_SIG,
    MMC_ERASE_FROM_CLI_SIG,
    MMC_DUMP_REG_SIG,
    MMC_TEST_CMD_SIG,
    MMC_STRESS_CMD_SIG,
    MMC_STRESS_CONTINUE_SIG,
    MMC_SLEEP_SIG,
    MMC_AWAKE_SIG,
    MMC_SWITCH_PARTITION_SIG,
    MMC_WRITE_PROT_SIG,

    SDH_PHY_TEST_SIG,
    SDH_TRANS_SUC_SIG,
    SDH_TRANS_FAIL_SIG,
    BIO_TEST_SIG,
    SDH_CONFIG_SIG,
    SDH_TX_RX_TUNE_SIG,

    /*TEST AO signals */
    AO_TEST_TIMEOUT_SIG,

#ifdef CONFIG_ATE
    ATE_TEST_REQ_SIG,
    ATE_TEST_RESULI_SIG,
    ATE_TEST_TIMEOUT_SIG,
    ATE_TEST_CTRL_SIG,
#ifdef CONFIG_ATE_PE_FS
    LOAD_ATE_CMD_SIG,
#endif
#endif

#ifdef CONFIG_SCREEN_CHIP
    SCREEN_TEST_START_SIG,
    SCREEN_TEST_ACK_SIG,           /* auto screen response result signal from command related AOs */
    SCREEN_TIMEOUT_SIG,            /* signal for command timeout event of screen test */
#ifdef CONFIG_ATE_PE_FS
    LOAD_SLT_CMD_SIG,
#endif
#endif

    DTE_TEST_RES_SIG,
#ifdef CONFIG_SDH_API_TEST
    AUTO_EMMC_SD_TEST,
#endif

    /* COMMON AO signals */
    BBU_WDT_SIG,                                /*  watchdog test signal       */
    BBU_PWM_SIG,                                /*       pwm test signal       */
    BBU_IPC_SIG,                                /*       IPC test signal       */
    BBU_RIPC_SIG,                               /*      RIPC test signal       */
    BBU_SCMD_SIG,                               /*  send command to other core */
    BBU_CMDA_SIG,                               /*   send command to AP        */
    BBU_CMDC_SIG,                               /*   send command to CP        */
    BBU_CMDD_SIG,                               /*   send command to DSP       */
    BBU_UART_SIG,                               /* change UART owner signal    */
    BBU_XTC_SIG,			/*   RTC/WTC config signal     */
    BBU_DPEEK_SIG,                               /* 2MSA_DSP:  Examine memory/register   */
    BBU_DPOKE_SIG,                               /*2MSA_DSP:  Load data into memory     */
    BBU_DFMEM_SIG,                              /*2MSA_DSP:  Fill memory area with specified data*/
    BBU_DVMEM_SIG,                               /*2MSA_DSP:  verify the memory area correctness, pair with DFMEM*/
    BBU_DMEMT_SIG,                               /*2MSA_DSP:  write and readback and compare data into memory  area*/
    BBU_DRC_SIG,                                /*2MSA: test DSP's latency in MSA CPU cycles for reading one DDR word*/
    BBU_DWC_SIG,                                /*2MSA: test DSP's latency in MSA CPU cycles for writting one DDR word*/
    BBU_PEEK_SIG,                               /*   Examine memory/register   */
    BBU_POKE_SIG,                               /*   Load data into memory     */
    BBU_DUMP_SIG,                               /*   dump Memory/register      */
    BBU_IVFP_SIG,                               /*   initialize VFP signal     */
    BBU_EICA_SIG,                               /*   enable  caches            */
    BBU_DICA_SIG,                               /*   disable caches            */
    BBU_CTST_SIG,                               /*   cache test signal         */
    BBU_L2CT_SIG,                               /*   L2 cache test signal      */
    BBU_MCPS_SIG,                               /*   measure core frequency    */
    BBU_ICPK_SIG,                               /*   I cache peek              */
    BBU_DCPK_SIG,                               /*   D cache peek              */
    BBU_VTPA_SIG,                               /*   virtual address to PA     */
    BBU_MEMCOPY_SIG,                            /* Copy mem from source to dest*/
    BBU_FMEM_SIG,                               /*   Fill memory with data     */
    BBU_MEMT_SIG,                               /*   Standard BBU memory test  */
    BBU_DTC_SIG,                                /*   DTC snow 3G test signal   */
    BBU_TEST_SIG,
    BBU_SBIT_SIG,
    BBU_CBIT_SIG,
    BBU_RBIT_SIG,
    BBU_MGWP_SIG,                               /* MCU generate word pattern   */
    BBU_MFILE_SIG,                              /*      MFILE test signal      */
    BBU_LEDT_SIG,                               /*    Tri-LED test signal      */
    BBU_SETV_SIG,                               /*  set V_BUCK voltage signal  */
    BBU_SLDO_SIG,                               /*  set V_LDO voltage signal   */
    BBU_TMARK_SIG,                              /* Tmark baremetal test signal */
    BBU_CMARK_SIG,                              /* CoreMark baremetal signal   */
    BBU_GPIO_SIG,                               /*    GPIO set test signal     */
    BBU_CPUT_SIG,                               /*    CPU test signal          */
    BBU_CACHEBENCH_SIG,                         /*   cachebench test signal    */
    BBU_MTSP_SIG,                               /* memory test package signal  */
    BBU_SMTD_SIG,                               /* set memory test defaults    */
    BBU_SCRT_SIG,                               /*    Screening Test signal    */
    BBU_PID_SIG,                                /*   Get Processor ID signal   */
    BBU_CLRS_SIG,                               /*      clear uart screen      */
    BBU_PMIC_SIG,                               /*        Get PMIC revision    */
    BBU_DDRP_SIG,                               /*    ddr priority set signal  */
    BBU_DDRW_SIG,                               /*    ddr weight set signal    */
    BBU_DDR_SWEEP_SIG,                       /* ddr driver strength sweep test */
    BBU_DDRT_SIG,                               /* DDR write-read-check test   */
    BBU_DURM_SIG,                          /* DDR utilization rate measurement */
    BBU_CURM_SIG,                          /* CPU utilization rate measurement */
    BBU_VIBRA_ST,                               /* Vibrator start              */
    BBU_VIBRA_SP,                               /*  Vibrator stop              */
    BBU_STRESS_SIG,                             /*      BBU stress test        */
    BBU_LINPACK_SIG,                            /* Linpack n=100 benchmark sig */
    BBU_WHET_SIG,                               /* whetstone benchmark sig     */
    BBU_NEON_SPEED_SIG,                         /*     NEON speed benchmark    */
    BBU_DHRY_SIG,                               /* Dhrystone benchmar signal   */
    BBU_FFTS_SIG,                               /*       FFTS test signal      */
    BBU_PRINT_SIG,                              /*    set print level signal   */
    BBU_MEMCPY_SIG,                             /*      Memcpy test signal     */
    BBU_DELAY_SIG,                            	/* delay x senconds sig */
    BBU_MDELAY_SIG,                            	/* delay x senconds sig */
    BBU_DDR_POWER_SIG,                          /* ddr power sig*/
    BBU_HSI_SIG,                          	/* hsi sig		       */
    BBU_CPLD_SIG,                               /* load CP image & release CP */
#ifdef CONFIG_H264
    BBU_H264_SIG,                               /*   H264 decoder test signal  */
#endif
#ifdef CONFIG_TZ
    BBU_STTZ_SIG,                               /*     TrustZone test signal   */
    BBU_SPEEK_SIG,
    BBU_SPOKE_SIG,
#endif
    BBU_RESET,                                  /*     Software reset          */
    BBU_PDOWN,                                  /*     Software power down     */
    BBU_MFPTEST_SIG,  				/*     mfp test                */
    BBU_DEVPM_SIG,				/*     dev pm api test         */
    LCD_ILCD_SIG,				/*	lcd initialize		*/
    LCD_LCDG_SIG,				/*	lcd display green	*/
    LCD_LCDR_SIG,				/*	lcd display red		*/
    LCD_LCDB_SIG,				/*	lcd display yellow	*/
    LCD_VLCD_SIG,				/*	lcd video_win on/off	*/
    LCD_CLCD_SIG,				/*	lcd video_win on/off	*/
    LCD_SIG,					/*   lcd_crane_test*/
    CAM_ICAM_SIG,				/*	camera initialize	*/
    CAM_READ_ID_SIG,			/*	camera read i2c id	  */
    CAM_ONLINE_SIG,			/*	camera online	  */
    CAM_REG_SIG,				/*	camera reg read and write	*/
    CAM_ONLINE_IRQ_SIG,		 /*	camera online irq   */
    CAM_CAMV_SIG,				/*	camera dma on/off	*/
    CAM_CCAM_SIG,				/*	camera dma on/off	*/
    CAM_CAMD_SIG,				/*	camera debug		*/
    CAM_CAMF_SIG,				/*	ccic frequency change	*/
    CAM_ISPP_SIG,				/*	isp aclk clk change	*/
    CAM_ISPM_SIG,				/*	isp capture/preview mode change	*/
    CAM_ISPA_SIG,				/*	isp ahbdma test		*/
    CAM_IFVT_SIG,				/*	isp fvts init test	*/
    CAM_FVTS_SIG,				/*	isp fvts test		*/
    CAM_ISPF_SIG,				/*	isp fvts test, NO sd CASE*/
    CAM_OFFLINE_SIG,				/*	isp offline test	*/
    CAM_SCL_SIG,				/*	isp scaler test		*/
    CAM_IRE_SIG,				/*	isp rotation test	*/
#ifdef	CONFIG_GCU
	GCU_2D_IINT_SIG,
	GCU_3D_IINT_SIG,
	GCU_2D_TEST_SIG,
	GCU_3D_TEST_SIG,
	GCU_2D_STRESS_SIG,
	GCU_3D_STRESS_SIG,
    GCU_2D_STOP_SIG,
    GCU_3D_STOP_SIG,
    GCU_2D_ROT_SIG,
    GCU_3D_ROT_SIG,
    GCU_COMMON_SIG,
#endif

    BBU_LUCDWT_SIG,            /* Mersenne number test of DR. Richard Crandall */
#ifdef CONFIG_MLUCAS
    BBU_MLUCAS_SIG,             /* Mersenne number test program of Ernst Mayer */
#endif

#ifdef CONFIG_OMX
    BBU_OMXT_SIG,
#endif

#ifdef CONFIG_VFP
    BBU_VFPT_SIG,                               /*   VFP test signal           */
    BBU_UCBT_SIG,                               /*   UCB test signal           */
#endif

#ifdef CONFIG_SMP
    SMP_PLUG_SIG,                                           /* Core hot unplug */
    SMP_UNPLUG_SIG,                                           /* Core hot plug */
    SMP_WKSL_SIG,                             /* Display and switch work steal */
    SMP_SETAO_SIG,                                       /* Display and set AO */
    SMP_SINT_SIG,                                 /* Display and set interrupt */
#endif

    HDMI_HDMI_SIG,				/*	HDMI initialize		*/
    MHL_IMHL_SIG,				/*	MHL initialize		*/
    VMETA_VDEC_SIG,				/*	VMETA decode test	*/
#ifdef CONFIG_AUDIO
    AUDIO_TEST_SIG, /* Audio buffer unit test sig */
    AUDIO_DECODE_SIG,
#endif
#ifdef CONFIG_VPU
    VPU_START_SIG,
    VPU_STOP_SIG,
    VPU_TEST_SIG,
    VPU_DECODE_SIG,
    VPU_DECODE_DONE_SIG,
    VPU_ENCODE_SIG,
    VPU_ENCODE_DONE_SIG,
    VPU_CAM_CODER_SIG,
    VPU_STREAM_EXTRACT_SIG,
#endif
#ifndef CONFIG_MMT
    USB_IUSB_STORAGE, /*Simple USB test*/
    USB_CHARGER,
    USB_CLOSE,
    USB_SPEAKER,
    USB_RESET,
    USB_PORT_CHANGER,
    USB_STS_ERR,
    USB_STS_SUSPEND,
    USB_STS_COMPLETE,
    USB_REQ_SIG,
    USB_ENABLE_ISR,
    USB_PROCESS_COMPLETE,
    USB_READ_HANDLE,
    USB_WRITE_HANDLE,
    USB_2_SD_TRAN,
    USB_MMC_STORAGE,
    USB_SERIAL,
    USB_ECM,
    USB_DR,
    USB_DQH,
    USB_CDC2,
    USB_SERIAL_READ,
    USB_TIMEOUT_SIG,
    USB_RNDIS,
    USB_HOST_TEST,
    USB_CHARGER_INIT,
    USB_ETHER_SEND_SIG,
    USB_ETHER_RECE_SIG,
#endif
    GEU_TEST_SIG,
    FUSE_READ_SIG,
    FUSE_WRITE_SIG,
    FUSE_READ_DRO_SIG,
    FUSE_HELP,

    CHARGER_INIT_SIG,
    CHARGER_PULLOUT_SIG,
    CHARGER_DISABLE_SIG,
    CHARGER_STARTTIMER_SIG,
    CHARGER_DUMPREG_SIG,
    CHARGER_ICRESET_SIG,
    CHARGER_READ_SIG,
    CHARGER_CONFIG_SIG,
    CHARGER_TIMEOUT_SIG,
    STRESS_STRT_SIG,                            /* stress test start signal */
    STRESS_CONT_SIG,                         /* stress test continue signal */
    STRESS_OVER_SIG,                             /* stress test over signal */
    STRESS_AUTO_SIG,                        /* stress auto run start signal */
    STRESS_PASS_SIG,                             /* stress test pass signal */
    STRESS_FAIL_SIG,                             /* stress test fail signal */
    STRESS_ACK_SIG,
    STRESS_CHECK_SIG,
    STRESS_CPBP_SIG,                    /* calc and print the biggest prime */
    TRANS_TEST_SIG,
    IPC_RREQ_SIG,
    IPC_RECV_SIG,
    IPC_SEND_SIG,
    IPC_SREQ_SIG,
    IPC_SACK_SIG,

// LQ:
#ifdef  CONFIG_FAT32
    QF_CMD_WRITE_FILE_SIG, //210?
    QF_CMD_COPY_FILE_1_SIG,
    QF_CMD_COPY_FILE_2_SIG,
    QF_CMD_DEBUG_SIG,
    QF_CMD_RM_SIG,
    QF_CMD_UNMOUNT_SIG,
    QF_CMD_RMDIR_SIG,
    QF_CMD_CD_SIG,
    QF_CMD_CREATE_FILE_SIG,
    QF_UBLIO_BLOCK_WRITE_LOOP_SIG,
    QF_CMD_READ_FILE_SIG,
    QF_CMD_HELP_SIG,
    QF_CMD_LS_SIG,
    QF_UBLIO_BLOCK_IO_SIG,
    QF_UBLIO_BLOCK_IO_ACK_SIG,
    QF_UBLIO_DC_DATA_IO,
    QF_WRITE_DATA_ACK_SIG,
    QF_UBLIO_BLOCK_SYNC_LOOP_SIG,
    QF_DC_DATA_VEC_IO_SIG,
    QF_DISC_DATA_VEC_ACK_SIG,
    SD_IO_ACK_SIG,
    SD_IO_SIG,
    QF_READ_DATA_VEC_LOOP_SIG,
    QF_UBLIO_DC_DATA_LOOP_SIG,
    QF_CLOSE_FILE_SKIP_TRUNC_SIG,
    QF_CLOSE_FILE_FREE_SIG,
    QF_UBLIO_DISC_DATA_ACK_SIG,
    QF_DATA_VEC_IO_ACK_SIG,
    QF_DATA_VEC_IO_SIG,
    QF_DATA_VEC_IO_LOOP_SIG,
    QF_ENTRY_PUSH_SIG,
    QF_CLOSE_FILE_UPDATE_DIRENT_SIG,
    QF_CLOSE_FILE_HANDLE_LINK_SIG,
    QF_CLOSE_FILE_FLUSH_CACHE_SIG,
    QF_FLUSH_CACHE_LOOP_SIG,
    QF_FETCH_ENTRY_MEM_CPY_SIG,
    QF_CLEAN_UP_ENTRY_FETCH_SIG,
    QF_FETCH_ENTRY_WITH_CONTEXT_SIG,
    QF_ENTRY_FETCH_SIG,
    QF_ENTRY_FETCH_CLEAN_UP_SIG,
    QF_TRUNCATE_FILE_ACK_SIG,
    QF_PUSH_ENTRY_WITH_CONTEXT_SIG,
    QF_CMD_MKDIR_SIG,
    QF_CMD_PWD_SIG,
    QF_CMD_CAT_SIG,
    QF_CREATE_FILE_SIG,
    QF_CREATE_FILE_LOOP_SIG,
    QF_API_OPEN_FILE_SIG,
    QF_API_CREATE_FILE_SIG,
    QF_FILE_CREATE_SIG,
    QF_WRITE_FILE_LOOP_SIG,
    QF_UBLIO_BLOCK_SYNC_SIG,
    QF_UBLIO_PIO_ACK_SIG,
    QF_UBLIO_BLOCK_SYNC_ACK_SIG,
    QF_UBLIO_BLOCK_READ_LOOP_SIG,
    QF_UBLIO_BLOCK_READ_TRANS_SIG,
    QF_FIND_FREE_DIRENT_ACK_SIG,
    QF_FIND_FREE_DIRENT_LOOP_SIG,
    QF_FIND_FREE_DIRENT_TAIL_SIG,
    QF_EXTEND_DIRECTORY,
    QF_EXTEND_DIRECTORY_ACK_SIG,
    QF_CLEAR_CLUSTER_SIG,
    QF_CLEAR_CLUSTER_ACK_SIG,
    QF_CREATE_DIRENT_CREATE_SIG,
    QF_CLEAR_CLUSTER_LOOP_SIG,
    QF_CLEAR_CLUSTER_TAIL_SIG,
    QF_CREATE_LFNS_SIG,
    QF_FIND_FREE_DIRENT_SIG,
    QF_UBLIO_PIO_SIG,
    QF_FLUSH_NODE_SIG,
    QF_PUT_NODE_SIG,
    QF_PUT_NODE_ACK_SIG,
    QF_FLUSH_NODE_ACK_SIG,
    QF_API_READ_FILE_SIG,
    QF_API_WRITE_FILE_SIG,
    QF_API_CLOSE_FILE_SIG,
    QF_API_FILE_CLOSE_SIG,
    QF_API_FILE_OPEN_SIG,
    QF_API_FILE_READ_SIG,
    QF_API_FILE_WRITE_SIG,
    QF_CREATE_DIRENT_TAIL_SIG,
    QF_CREATE_CLUTER_CHAIN_SIG,
    QF_OPEN_FILE_SIG,
    QF_LOOKUP_LOOP_SIG,
    QF_CREATE_LFNS_TAIL_SIG,
    QF_CREATE_LFNS_LOOP_SIG,
    QF_CREATE_CLUTER_CHAIN_ACK_SIG,
    QF_UNLINK_CLUSTER_CHAIN_SIG,
    QF_UNLINK_CLUSTER_CHAIN_ACK_SIG,
    QF_CREATE_FILE_TAIL_SIG,
    QF_LOOKUP_NAME_SIG,
    QF_CACHE_DIRECTORY_ACK_SIG,
    QF_CREATE_LFNS_ACK_SIG,
    QF_CACHE_DIRECTORY_SIG,
    QF_CREATE_SHORT_NAME_SIG,
    QF_SHORT_NAME_CREATE_SIG,
    QF_CREATE_SHORT_NAME_TAIL_SIG,
    QF_CREATE_SHORT_NAME_LOOP_SIG,
    QF_SHORT_NAME_EXIST_SIG,
    QF_SHORT_NAME_EXIST_ACK_SIG,
    QF_SHORT_NAME_EXIST_LOOP_SIG,
    QF_SHORT_NAME_EXIST_TAIL,
    QF_LOOKUP_NAME_ACK_SIG,
    QF_FILE_INFO_SIG,
    QF_READ_FILE_SIG,
    QF_UBLIO_BLOCK_ACK,
    QF_READ_FILE_LOOP_SIG,
    QF_FIND_ENTRY_IN_DIR_LOOP_SIG,
    QF_FIND_ENTRY_IN_DIR_END_SIG,
    QF_POP_LDIR_LOOP_SIG,
    QF_POP_LDIR_END_SIG,
    QF_CREATE_DIRENT_SIG,
    QF_INC_FREE_CLUSTER_SIG,
    QF_INC_FREE_CLUSTER_ACK_SIG,
    QF_DIRENT_CREATE_SIG,
    QF_POP_LDIR_SIG,
    QF_POP_LDIR_ACK_SIG,
    QF_WRITE_FILE_SIG,
    QF_CLOSE_FILE_SIG,
    QF_FILE_CLOSE_SIG,
    QF_READ_BLOCK_SIG,
    QF_BLOCK_READ_SIG,
    QF_SET_CLUSTER_SIG,
    QF_CLUSTER_SET_SIG,
    QF_FIND_DIR_SIG,
    QF_DIR_INFO_SIG,
    QF_FIND_ENTRY_IN_DIR_SIG,
    QF_DIR_ENTRY_INFO_SIG,
    QF_DEC_CLUSTERS_SIG,
    QF_DECREASE_FREE_CLUSTER_SIG,
    QF_CLUSTER_DECREASE_SIG,
    QF_WRITE_CLUSTER_LOOP_SIG,
    QF_SEEK_FILE_SIG,
    QF_FILE_SEEK_SIG,
    QF_WRITE_CLUSTER_SIG,
    QF_CLUSTER_WRITE_SIG,
    QF_GET_BUFFER_SIG,
    QF_BUFFER_GET_SIG,
    QF_WRITE_BLOCK_SIG,
    QF_RELEASE_BUFFER_SIG,
    QF_BUFFER_RELEASE_SIG,
    QF_BLOCK_WRITE_SIG,
    QF_GET_ENTRY_SIG,
    QF_ENTRY_GET_SIG,
    QF_PUT_ENTRY_SIG,
    QF_ENTRY_PUT_SIG,
    QF_WRITE_TO_SEC_BOUND_SIG,
    QF_WRITE_TO_CLUSTER_BOUND_SIG,
    QF_WRITE_REMAIN_BLOCKS_SIG,
    QF_READ_TO_SECTOR_BOUNDARY,
    QF_WRITE_REMAIN_BYTES_SIG,
    QF_EXTEND_FILE_SIG,
    QF_EXT_FILE_SIG,
    QF_FILE_EXT_SIG,
    QF_READ_CLUSTERS_SIG,
    QF_CLUSTERS_READ_SIG,
    QF_CACHE_FLUSH_SIG,
    QF_FLUSH_CACHE_SIG,
    QF_WRITE_IN_BLOCK_FINISH,
    QF_BUFFER_LOOP_SIG,
    QF_BUFFER_REPLACE_SIG,
    QF_READ_CLUSTERS_LOOP_SIG,
    QF_FSYNC_SIG,
    QF_FSYNC_ACK_SIG,
    QF_READ_TO_CLUSTER_BOUNDARY,
    QF_READ_REMAIN_BLOCKS,
    QF_READ_REMAIN_BYTES,
    QF_INIT_FILE_SYSTEM_SIG,
    QF_AUTO_INIT_FILE_SYSTEM_SIG,
    QF_INIT_FS_SIG,
    QF_API_READ_LINE_SIG,
// #define FAT32_TEST
#ifdef FAT32_TEST
    QF_TEST_SIG,
    QF_TEST_RAW_COPY_SIG,
    QF_TEST_COPY_SIG,
#endif /* FAT32_TEST */
#endif /* FAT32 */

#ifdef CONFIG_TCPIP
    LWIP_SLOW_TICK_SIG,
    LWIP_INIT_SIG,
    LWIP_RX_READY_SIG,
    LWIP_TX_READY_SIG,
    LWIP_RX_OVERRUN_SIG,
    LWIP_SOCKET_SIG,
    TCPIP_LISTEN_ACCEPT,
    TCPIP_RECV_SIG,
    TCPIP_TRIGGER_SIG,
    LWIP_UDP_STATUS_SIG,
#ifdef CONFIG_TPERF
    TPERF_INIT_SIG,
    TPERF_SERVER_INIT_SIG,
    TPERF_SERVER_PROCESS,
    TPERF_SERVER_END,
    TPERF_SERVER_RECEIVE_BEGIN,
    TPERF_SERVER_RECEIVE_END,
    TPERF_SERVER_TICK_SIG,
#endif /* CONFIG_TPERF */
#endif /* CONFIG_TCPIP */
    SUPER_PI_SIG,                              /* Super Pi benchmark signal */
    BBU_COMMT_SIG,                      /*Seagull&MSA reset and boot and digRF3 external loopback test*/

#ifdef CONFIG_MEM_HOTPLUG
    MEM_ACQUIRE_SIG,
    MEM_RELEASE_SIG,
#endif

#ifdef QF_STACK_PER_AO
    BBU_STACK_SIG,
#endif

#ifdef CONFIG_SUSP_DBG
    AO_WAKE_SIG,
    SENS_SUSP_SIG,
    I2C_SUSP_SIG,
    INPUT_SUSP_SIG,
    COMPO_TEST_SIG,
    COMPO_ORDER_SIG,
    TICK_TEST_SIG,
    TIME_TEST0_SIG,
    TIME_TEST1_SIG,
    TIME_TEST2_SIG,
#endif
    CPU_FUNC_SIG,
#ifdef CONFIG_USB3
    USB3_TEST_SIG,
    USB3_SCHED_SIG,
#endif

    /* SSP AO signals*/
    SSP_INIT_SIG,
    SSP_TEST_SIG,
    SSP_LPBK_SIG,

    MAX_SIG                            /* The last signal (keep always last)*/
};

/*..............................................................................*/
typedef struct CmdLineEntryTag{
    const char *Cmd; //pointer to the name string of the command
    QActive * const AO_Cmd; //pointer to the target AO handling this command
    uint32_t   EvtSig; //related signal for the command event
    const char *Help; //pointer to a string of brief description of the command
}CmdLineEntry;


typedef struct ComCliEvtTag {           /*the event handle the complete line*/
    QEvent  super;
    uint8_t CliBuffer[MAX_LINE_LEN]; //current command
    uint8_t source;  /* 0 -- event come from uart, 1 -- event come from screen test */
    uint8_t len; //character index of the current command, eventually the whole length when it finished
    uint8_t cursor; //cursor position, which may be not same as len
} ComCliEvt;

typedef struct ScreenCmdEvtTag {       /*the event of auto screen script for both board and chip, the first three members must in the same location as "ComCliEvt" above so that AO can judge commands come from UART or screen test request.*/
    QEvent  super;
    uint8_t cmd[MAX_LINE_LEN]; //current command
    uint8_t source;  /* 0 -- event come from uart, 1 -- event come from screen test */
    uint8_t len; //character index of the current command, eventually the whole length when it finished
    uint8_t cmd_index; /*Command index located in the auto screen command table*/
} ScreenCmdEvt;

typedef struct ScreenCmdAckTag{  /*The auto screen response event from command executor*/
    QEvent super;
    uint32_t cmd_index; /*Command index located in the auto screen command table*/
    uint8_t result; /*Command execution result report from command executor*/
} ScreenCmdAck;

typedef struct ComCmdEvtTag {
    QEvent  super;  /*Command signal inside*/
    uint8_t cmdline[MAX_LINE_LEN];
    uint32_t argc;
    uint8_t *argv[CMDLINE_MAX_ARGS]; /* argv[i] is the pointer to args in cmdline, e.g. argv[0] point to the cmd name */
    uint32_t cmd_number;
    uint8_t scr_cmd_index;  /* location of screen test command in the screen test table, begin from 1, 0 means do not care */
} ComCmdEvt;


typedef struct ComRxEvtTag {
    QEvent  super;
    uint8_t data[MAX_LINE_LEN]; /*half FIFO size where interrupt, but it can have input during the process*/
    int len;
} ComRxEvt;

typedef struct ComTxEvtTag {
    QEvent  super;
    uint8_t ConsoleBuffer[MAX_LINE_LEN];
    int len;
} ComTxEvt;

typedef struct KeypEvtTag {
   QEvent super;                                     /* derives from QEvent */
   uint32_t kpas;                                      /* Value of reg KPAS */
   uint32_t kpdk;                                      /* Value of reg KPDK */
   uint32_t kpasmkp[3];                             /* Values of KPASMKP0-3 */
} KeypEvt;

typedef struct StressAckEvtTag {
   QEvent super;                                     /* derives from QEvent */
   char   cmd[20];
} StressAckEvt;

/* I2C read and write event between different AOs.......................... */
typedef struct I2cRwEvtTag{
   QEvent    super;                                   /* derives from QEvent */
   uint8_t   AO_callI2C;                 /* pointer to the AO which call I2C */
   uint8_t   i2c_index;                            /* index of specified I2C */
   uint8_t   slave_addr;                   /* slave address of target device */
   uint8_t   reg_addr_len;                   /* slave register address bytes */
   uint8_t   i2c_mode;                     /* I2C bus mode. default:standard */
   uint32_t  length;                                       /* length of data */
   uint8_t   *reg_addr;           /* register address of target device */
   uint8_t   *value;              /* pointer to data for transmit or receive */
}I2cRwEvt;

typedef struct I2cRetEvtTag{
	QEvent	super;				/* derives from QEvent */
	uint8_t	i2c_index;
}I2cRetEvt;

typedef struct USBHandleEvtTag
{
    QEvent super;
    void   *handle;
     uint32_t length;
}USBHandleEvt;

typedef struct USBEndpointEvtTag
{
      QEvent super;
      void   *            handle;
      uint8_t               ep_num;
      uint8_t    *     buffer_ptr;
      uint32_t              start_addr;
      uint32_t              size;
      uint8_t               usbcbw_process;
      void   *           csw_prt;
      uint8_t             tran_type;


} USBEndpointEvt;
typedef struct USBReqEvtTag
{
    QEvent super;

}USBReqEvt;

typedef struct USBEtherSendEvtTag{
    QEvent super;
    uint8_t *data;
    uint32_t length;

}USBEtherSendEvt;
typedef struct USBEtherReceiveEvtTag{
    QEvent super;
    uint8_t status;

}USBEtherReceiveEvt;



typedef void (*timer_irq_func)(void*);
typedef struct TimerIrqEvtTag{
    QEvent super;
    timer_irq_func func_p;
    void *arg;
}TimerIrqEvt;

typedef struct DmaStressEvtTag{
    QEvent super;
    uint32_t channel;//start channel id.
    uint32_t num;    //channel number.
    uint32_t size;   //transfer size for each channel.
}DmaStressEvt;

/*Global identification of test case PASS or FAIL result
  Each test command has 1 bit flag.
  The assumption is the test can pass with bit value 0.
  If the test fails, set related bit to value 1, bit index is
  ComCmdEvt.cmd_number.
  After all test, print out the result and failed unit.
  We may need to add a message in the cmdtable in cli.c to show the unit name.
  so that we can print it out.
  */
uint32_t test_result[(MAX_CMD_NUM+31)/32];
uint8_t screen_handler(int argc, uint8_t **argv);

#ifdef MULTI_UART
uint8_t uart_id[2];
#endif

/* BBU malloc test function in driver/common/bbu_malloc.c */
void bbu_malloc_test(int argc, uint8_t **argv);

/* active objects' "constructors" */
void Cli_ctor(void);
void Console_ctor(void);
void Input_ctor(void);
void Dma_ctor(void);
void Timer_ctor(void);
void Sensors_ctor(void);
void I2C_ctor(void);
void Audio_ctor(void);
void LCD_ctor(void);
void CAM_ctor(void);
void HDMI_ctor(void);
void NFC_ctor(void);
void COMMON_ctor(void);
void GCU_ctor(void);
void VMETA_ctor(void);
void MHL_ctor(void);
void Vpu_ctor(void);
void pmu_ctor(void);
void USB_ctor(void);
void GEU_ctor(void);
void MMC_ctor(void);
void STRESS_ctor(void);
void SMP_ctor(void);
void CHARGER_ctor(void);
void SCHED_ctor(void);
void SDH_ctor(void);
void TEST_ctor(void);
void NTZ_ctor(void);
void IPC_ctor(void);
void SSP_ctor(void);
void PCIE_ctor(void);
// LQ:
#ifdef  CONFIG_FAT32
void Fat32_ctor(void);
void Exfat_ctor(void);
#ifdef FAT32_TEST
void FileTest_ctor1(void);
void FileTest_ctor2(void);
#endif
#endif
#ifdef CONFIG_TCPIP
void LwIPMgr_ctor(void);
#endif
#ifdef CONFIG_TPERF
void TPerf_ctor(void);
void ServerTPerf_ctor(void);
#endif
#ifdef CONFIG_SUSP_DBG
void COMPO_ctor(void);
extern QActive * const AO_COMPO;
#endif
/* opaque pointers to active objects in the application */
extern QActive * const AO_Cli;
extern QActive * const AO_Console;
extern QActive * const AO_Input;
extern QActive * const AO_Dma;
extern QActive * const AO_Timer;
extern QActive * const AO_Sensors;
extern QActive * const AO_I2C;
extern QActive * const AO_Audio;
extern QActive * const AO_LCD;
extern QActive * const AO_CAM;
extern QActive * const AO_HDMI;
extern QActive * const AO_GCU;
extern QActive * const AO_VMETA;
extern QActive * const AO_MHL;
extern QActive * const AO_NFC;
extern QActive * const AO_COMMON;
extern QActive * const AO_Vpu;
extern QActive * const AO_pmu;
extern QActive * const AO_USB;
extern QActive * const AO_GEU;
extern QActive * const AO_MMC;
extern QActive * const AO_STRESS;
extern QActive * const AO_SMP;
extern QActive * const AO_CHARGER;
extern QActive * const AO_SCHED;
extern QActive * const AO_SDH;
extern QActive * const AO_TEST;
extern QActive * const AO_NTZ;
extern QActive * const AO_IPC;
extern QActive * AO_CPU[CORE_NUM];
extern QActive * const AO_SSP;
extern QActive * const AO_PCIE;

// LQ:
#ifdef  CONFIG_FAT32
extern QActive * const AO_Fat32;
extern QActive * const AO_exfat;
#ifdef FAT32_TEST
extern QActive * const AO_FileTest1;
extern QActive * const AO_FileTest2;
#endif /* FAT32_TEST */
#endif /* CONFIG_FAT32 */
#ifdef CONFIG_TCPIP
extern QActive * const AO_LwIPMgr;
#endif /* CONFIG_TCPIP */
#ifdef CONFIG_TPERF
extern QActive * const AO_TPerf;
extern QActive * const AO_SerPerf;
#endif /* CONFIG_TPERF */

#ifdef CONFIG_USB3
extern QActive * const AO_Usb3;
#endif

extern void QActive_cpuStart(void);

/*  board version detect functions */
int board_is_nza3_dkb_v10(void);
int board_is_nza3_dkb_v20a(void);
int board_is_nza3_dkb_v20b(void);
int board_is_nza3_dkb_v21a(void);

extern void cpu_cycle_delay(uint32_t cycle);

extern int is_test_buffer_safe(uint32_t addr, uint32_t len);
extern int is_gcu_buffer(uint32_t addr);
#define bbu_msleep(ms)    QA_mSleep(ms)

enum PlatformType{
    REAL_SI = 0,
    HAPS,
    ZEBU_Z1,
    VDK,
};
int platform_is_zebu();
int platform_is_z1(void);
int platform_is_haps(void);
int platform_is_emulator(void);
int platform_is_realsi(void);
#endif

