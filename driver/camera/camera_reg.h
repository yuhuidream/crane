#ifndef __CAMERA_REG_H__
#define __CAMERA_REG_H__

/* macro for regs */
#define REG_CAMERA_BASE 0xD420C000
#define RAM_BASE_REG 0xd1000000


/*
 * Y0-Base Address Register
 * Offset: 0x00
 * Bits    Field             Field (Code)    Type    Init    Reset    Description
 * 31:0    Y0 Base Address   YBASE0          RW      0x0              Y0 Base Address<p>This field can also be used as the RGB 0 Base Address.
 */
#define REG_Y0_BASE 0x00

/*
 * Y1-Base Address Register
 * Offset: 0x04
 * Bits    Field             Field (Code)    Type    Init    Reset    Description
 * 31:0    Y1 Base Address   YBASE1          RW      0x0              Y1 Base Address<p>This field can also be used as the RGB 1 Base Address.
 */
#define REG_Y1_BASE 0x04

/*
 * Y2-Base Address Register
 * Offset: 0x08
 * Bits    Field             Field (Code)    Type    Init    Reset    Description
 * 31:0    Y2 Base Address   YBASE2          RW      0x0              Y2 Base Address<p>This field can also be used as the RGB 2 Base Address.
 */
#define REG_Y2_BASE 0x08

/*
 * U0-Base Address Register
 * Offset: 0x0c
 * Bits    Field             Field (Code)    Type    Init    Reset    Description
 * 31:0    U0 Base Address   UBASE0          RW      0x0              U0 Base Address<p>This register is irrelevant when the image format is RGB or YUV packed.
 */
#define REG_U0_BASE 0x0c

/*
 * U1-Base Address Register
 * Offset: 0x10
 * Bits    Field             Field (Code)    Type    Init    Reset    Description
 * 31:0    U1 Base Address   UBASE1          RW      0x0              U1 Base Address<p>This register is irrelevant when the image format is RGB or YUV packed.
 */
#define REG_U1_BASE 0x10

/*
 * U2-Base Address Register
 * Offset: 0x14
 * Bits    Field             Field (Code)    Type    Init    Reset    Description
 * 31:0    U2 Base Address   UBASE2          RW      0x0              U2 Base Address<p>This register is irrelevant when the image format is RGB or YUV packed.
 */
#define REG_U2_BASE 0x14

/*
 * V0-Base Address Register
 * Offset: 0x18
 * Bits    Field             Field (Code)    Type    Init    Reset    Description
 * 31:0    V0 Base Address   VBASE0          RW      0x0              V0 Base Address<p>This register is irrelevant when the image format is RGB or YUV packed.
 */
#define REG_V0_BASE 0x18

/*
 * V1-Base Address Register
 * Offset: 0x1c
 * Bits    Field             Field (Code)    Type    Init    Reset    Description
 * 31:0    V1 Base Address   VBASE1          RW      0x0              V1 Base Address<p>This register is irrelevant when the image format is RGB or YUV packed.
 */
#define REG_V1_BASE 0x1c

/*
 * V2-Base Address Register
 * Offset: 0x20
 * Bits    Field             Field (Code)    Type    Init    Reset    Description
 * 31:0    V2 Base Address   VBASE2          RW      0x0              V2 Base Address<p>This register is irrelevant when the image format is RGB or YUV packed.
 */
#define REG_V2_BASE 0x20

/*
 * Image Pitch Register
 * Offset: 0x24
 * Bits    Field        Field (Code)    Type    Init    Reset    Description
 * 31:30   Reserved                     RSVD                     Reserved for future use
 * 29:18   UV Pitch		UVPITCH			RW      0x0 			 UV Pitch (distance in unit of 32-bit between two vertically adjacent pixels)<p>Must be programmed when output format is YUV planar. In JPEG capture, CCIC_IMG_PITCH = (bytesperline << 16) | bytesperline
 * 17:14   Reserved					    RSVD 					 Reserved for future use
 * 13:2    Y Pitch	    YPITCH		    RW	    0x0				 Y Pitch (distance in unit of 64-bit between two vertically adjacent pixels). In JPEG capture, CCIC_IMG_PITCH = (bytesperline << 16) | bytesperline.
 * 1:0     Reserved					    RSVD 					 Reserved for future use
*/
#define REG_IMG_PITCH_PIP0 0x24


/*
 * IRQ RAW Status Register
 * Offset: 0x28
 * 
*/
#define REG_IRQ_RAW_STATUS 0x28


/*
 * IRQ Mask Register
 * Offset: 0x2c
 * 
*/
#define REG_IRQ_MASK 0x2c

/*
 * IRQ Status Register
 * Offset: 0x30
 *
*/
#define REG_IRQ_STATUS 0x30

/*
 * Image Size Register
 * Offset: 0x34
 * Bits    Field                      Field (Code)    Type    Init    Reset    Description
 * 31:29   Reserved                                   RSVD                     Reserved for future use
 * 28:16   Image Length in Scanline	  VSIZE			  RW      0x0 			   Image Length in Scanline<p>Total size is VSIZE * bytescanline
 * 15:14   Reserved					                  RSVD 					   Reserved for future use
 * 13:0    Image Width in byte Unit	  HSIZE		      RW	  0x0              Image Width in byte
*/
#define REG_IMG_SIZE_PIP0 0x34

/*
 * Image Offset Register
 * Offset: 0x38
 * Bits    Field                          Field (Code)    Type    Init    Reset    Description
 * 31:29   Reserved                                       RSVD                     Reserved for future use
 * 28:16   Image Line Offset in Scanline  VOFF			  RW      0x0 			   Image Line Offset in Scanline<p>Starts capturing external CMOS sensor image at VOFF line. <p>0 = CCIC starts capturing at line 0 for every frame
 * 15:14   Reserved					                      RSVD 					   Reserved for future use
 * 13:0    Image Pixel Offset in PIXCLK	  HOFF		      RW	  0x0              Image Pixel Offset in PIXCLK<p>Starts capturing external CMOS sensor image at HOFF pixel. <p>0 = CCIC starts capturing at pixel 0 for every line in the frame
*/
#define REG_IMG_OFFSET 0x38


/*
 * Control 0 Register
 * Offset: 0x3c
 *
*/
#define REG_CTRL_0 0x3c

/*
 * Control 1 Register
 * Offset: 0x40
 *
*/
#define REG_CTRL_1 0x40

/*
 * Control 2 Register
 * Offset: 0x44
 * Bits    Field                            Field (Code)           Type    Init    Reset    Description
 * 31      ISIM Fix                         ISIM_HMUCSTFIX         RW      0X0              ISIM Fix<p>This field is used to enable the fix for enabling/disabling CCIC.<p>1 = Enable this fix<p>0 = Disable this fix<p>Bug description:when isim_en=1->0, isim_en_s2 toggles 1->0 before vend_csi2 comes, hardware will mis the vend_csi2 and cause internal state machine hang
 * 30      Frame Done DMA IRQ Select VOFF   FRAME_DONEDMA_IRQ_SEL  RW      0x0 			    Frame Done DMA IRQ Select<p>This field is used to select the interrupt generation for frame done on DMA side<p>0 = Interrupt is generated after the last byte of the last line is sent out by DMA (default)<p>1 = Frame byte count is used to generate the frame done
 *                                                                                          This bit has nothing to do with JPEG mode. When in JPEG mode, frame byte count will always be used to generate the frame done interrupt on the DMA side.
 * 16      Line Buffer Number Enable	    LINEBUFNUMENA		   RW	  0x0               Line Buffer Number Enable<p>This field enables/disables the feature to switch buffer after writing a certain number of lines. When enabled, CCIC is configured to write pixel data to two line buffers. Each line buffer size is defined in <Line Buffer Number>. The line buffer start address is defined by CCIC_Y0/Y1/U0/U1/V0/V1_BASE_ADDR. <p> 0 = Disable <p>1 = Enable
 * 15:13   Reserved                                                RSVD			            Reserved for future use
 * 12:0    LINECNTIRQ Interrupt				LINECNTIRQ			   RW	  0x0 				LINECNTIRQ Interrupt<p>This field generates an interrupt after writing LINECNTIRQ lines.<p>0 = No interrupt is generated<p>1 = Interrupt is generated
*/
#define REG_CTRL_2 0x44

/*
 * Camera IRQ RAW Status Register
 * Offset: 0x48
 *
*/
#define REG_CAMERA_IRQ_RAW_STATUS 0x48

/*
 * Camera IRQ MASK Register
 * Offset: 0x4c
 *
*/
#define REG_CAMERA_IRQ_RAW_MASK 0x4c

/*
 * Camera IRQ status Register
 * Offset: 0x50
 *
*/
#define REG_CAMERA_IRQ_STATUS 0x50


/*
 * Line Number Register
 * Offset: 0x60
 * Bits    Field        Field (Code)    Type    Init    Reset    Description
 * 31:13   Reserved                     RSVD                     Reserved for future use
 * 12:0   Line Number	LINENUM			RO      0x0 			 Line Number<p>This field shows the line number on the current frame being written to memory.
*/
#define REG_LNNUM 0x60

/*
 * Clock Control Register
 * Offset: 0x88
 * Bits    Field                             Field (Code)    Type    Init    Reset    Description
 * 31      Reserved                                          RSVD                     Reserved for future use
 * 30:29   Internal PIXCLK Select            INTPIXCLKSEL	 RW      0x0 			  Internal PIXCLK Select<p>0x0 = PIXMCLK (CCIC core clock with divider)<p>0x1 = PIXCLK input from IO <p>0x2 = AXI Clock<p>0x3 = CCIC Core clock (312M or 52M, set by PMU_CCIC_CLK_RES_CTRL reg)
 * 28      Reserved                                          RSVD                     Reserved for future use
 * 27:16   Clock Fine Tune                   CLKFT		     RW	     0x0              Clock Fine Tune
 * 15:0	   Clock Divider Value for PIXMCLK   CLKDIV		     RW	     0x0              Clock Divider Value for PIXMCLK<p>0 = PIXMCLK is gated (no clock)<p>Other = PIXMCLK is core clock divided by CLKDIV and CLKFT
*/
#define REG_CLOCK_CTRL 0x88

/*
 * SRAM TC0 Register (Test Only)
 * Offset: 0x8c
 *
*/
#define REG_SRAM_TC0_TEST_ONLY 0x8c

/*
 * SRAM TC1 Register (Test Only)
 * Offset: 0x90
 *
*/
#define REG_SRAM_TC1_TEST_ONLY 0x90

/*
 * MIPI CSI2 Control 0 Register
 * Offset: 0x100
 *
*/
#define REG_CSI2_CTRL_0 0x100

/*
 * MIPI CSI2 IRQ Raw Status Register
 * Offset: 0x108
 *
*/
#define REG_CSI2_IRQ_RAW_STATUS 0x108

/*
 * MIPI CSI2 GSP FIFO Register
 * Offset: 0x110
 * Bits    Field                             Field (Code)    Type    Init    Reset    Description
 * 31:24   Reserved                                          RSVD                     Reserved for future use
 * 23:0    Generic Short Packet FIFO Data    GSPFF_DATA  	 RW      0x0 			  Internal PIXCLK Select<p>0x0 = PIXMCLK (CCIC core clock with divider)<p>0x1 = PIXCLK input from IO <p>0x2 = AXI Clock<p>0x3 = CCIC Core clock (312M or 52M, set by PMU_CCIC_CLK_RES_CTRL reg)
*/
#define REG_CSI2_GSPFF 0x110

/*
 * MIPI CSI2 Virtual Channel Control Register
 * Offset: 0x114
 * Bits    Field                Field (Code)    Type    Init    Reset    Description
 * 31:24   Reserved                             RSVD                     Reserved for future use
 * 23:22   Virtual Channel 1    VC1  	        RW      0x0 			 Virtual Channel for the other CCIC.<p>When <MD> = Virtual Channel, long packets whose Virtual-Channel fields match VC1 are directed to the other CCIC.
 * 21:16   Data Type 1	        DT1		        RW		0x0 			 Data Type for the other CCIC.<p>When <MD> = Data-Type Interleaving, long packets whose Data-Type match DT1, are directed to the other CCIC. Non matching packets are directed to main CCIC.
 * 15:14   Virtual Channel 0	VC0		        RW		0x0 			 Virtual Channel for main CCIC.<p>When <MD> = Virtual Channel, long packets whose Virtual-Channel fields match VC0 are directed to main CCIC.
 * 13:2    Reserved							    RSVD 					 Reserved for future use
 * 1:0	   Mode             	MD      		RW		0x0 			 This field is valid when CCIC_CSI2_CTRL_0's <EXT_TIM_ENA> = 1 and <ENA> = 1.<p>Select Virtual Channel or Data-Type Interleaving.<p>0 = Normal mode<p>1 = Virtual Channel<p>2 = Data-Type Interleaving.
*/
#define REG_CSI2_VCCTRL 0x114

/*
 * MIPI CSI2 DPHY 1 Register
 * Offset: 0x124
 * Bits    Field                Field (Code)    Type    Init    Reset    Description
 * 31:1    Reserved                             RSVD                     Reserved for future use
 * 0       ana_pu                               RW      0x1 			 analog power up  1: Enable    0: Disable
*/
#define REG_CSI2_DPHY1 0x124

/*
 * MIPI CSI2 DPHY 2 Register
 * Offset: 0x128
 *
*/
#define REG_CSI2_DPHY2 0x128

/*
 * MIPI CSI2 DPHY 3 Register
 * Offset: 0x12c
 *
*/
#define REG_CSI2_DPHY3 0x12c

/*
 * MIPI CSI2 DPHY 4 Register
 * Offset: 0x130
 *
*/
#define REG_CSI2_DPHY4 0x130

/*
 * MIPI CSI2 DPHY 5 Register
 * Offset: 0x134
 *
*/
#define REG_CSI2_DPHY5 0x134

/*
 * MIPI CSI2 DPHY 6 Register
 * Offset: 0x138
 *
*/
#define REG_CSI2_DPHY6 0x138

/*
 * MIPI CSI2 Reset  Register
 * Offset: 0x1fc
 * Bits    Field                Field (Code)        Type    Init    Reset    Description
 * 31:7    Reserved                                 RSVD                     Reserved for future use
 * 6       hresetn_ahb          hresetn_ahbs        RW      0x1 			 module ahb clk reset
 * 5 	   ipe_hclkgate resetn	ipe_hclkgate_rstn   RW	    0x1				 reset ipe_hclkgate, active low
 * 4 	   ipe_vclkgate_reset   ipe_vclkgate_rst    RW	    0x1				 active low
 * 3 	   csi2_hresetn         csi2_rstn           RW	    0x1				 active low
 * 2 	   prst_mux             preset              RW	    0x1				 active low
 * 1 	   isim_vreset          isim_vreset         RW	    0x1				 active low
 * 0 	   csi2_vreset          csi2_vreset         RW	    0x1				 active low
*/
#define REG_CSI2_RESET 0x1fc

/*
 * Start Descriptor Chain Address Y Register
 * Offset: 0x200
 * Bits    Field                               Field (Code)        Type    Init    Reset    Description
 * 31:2    Start Descriptor Chain Address Y    SDCADDRY            RW      0x0              Descriptor Chain Y Start Address bit [31:2] of [31:0]
 * 1:0     Reserved                                                RSVD    0x0              Reserved for future use
*/
#define REG_START_DESC_CHAIN_ADDR_Y 0x200

/*
 * Start Descriptor Chain Address U Register
 * Offset: 0x204
 * Bits    Field                               Field (Code)        Type    Init    Reset    Description
 * 31:2    Start Descriptor Chain Address U    SDCADDRY            RW      0x0              Descriptor Chain U Start Address bit [31:2] of [31:0]
 * 1:0     Reserved                                                RSVD    0x0              Reserved for future use
*/
#define REG_START_DESC_CHAIN_ADDR_U 0x204

/*
 * Start Descriptor Chain Address V Register
 * Offset: 0x208
 * Bits    Field                               Field (Code)        Type    Init    Reset    Description
 * 31:2    Start Descriptor Chain Address V    SDCADDRY            RW      0x0              Descriptor Chain V Start Address bit [31:2] of [31:0]
 * 1:0     Reserved                                                RSVD    0x0              Reserved for future use
*/
#define REG_START_DESC_CHAIN_ADDR_V 0x208

/*
 * Descriptor Chain Length Y Register
 * Offset: 0x20c
 * Bits    Field                               Field (Code)        Type    Init    Reset    Description
 * 31:24   Reserved                                                RO                       Reserved for future use
 * 23:3    Descriptor Chain Length Y           DCLENY              RW      0x0              Descriptor Chain Length Y in bytes bit [23:3] of [23:0]<p>Indicates length of 2-word descriptor chain in memory
 * 2:0     Reserved                                                RSVD                     Reserved for future use
*/
#define REG_DESC_CHAIN_LEN_Y 0x20c

/*
 * Descriptor Chain Length U Register
 * Offset: 0x210
 * Bits    Field                               Field (Code)        Type    Init    Reset    Description
 * 31:24   Reserved                                                RO                       Reserved for future use
 * 23:3    Descriptor Chain Length U           DCLENU              RW      0x0              Descriptor Chain Length U in bytes bit [23:3] of [23:0]<p>Indicates length of 2-word descriptor chain in memory
 * 2:0     Reserved                                                RSVD                     Reserved for future use
*/
#define REG_DESC_CHAIN_LEN_Y 0x210

/*
 * Descriptor Chain Length V Register
 * Offset: 0x214
 * Bits    Field                               Field (Code)        Type    Init    Reset    Description
 * 31:24   Reserved                                                RO                       Reserved for future use
 * 23:3    Descriptor Chain Length V           DCLENV              RW      0x0              Descriptor Chain Length V in bytes bit [23:3] of [23:0]<p>Indicates length of 2-word descriptor chain in memory
 * 2:0     Reserved                                                RSVD                     Reserved for future use
*/
#define REG_DESC_CHAIN_LEN_Y 0x214

/*
 * Descriptor Chain Target Address Y Register
 * Offset: 0x218
 * Bits    Field                               Field (Code)        Type    Init    Reset    Description
 * 31:0    Descriptor Chain Target Address Y   TADDRY              RO      0x0              Descriptor Chain Target Address Y<p>This field indicates the current Y target address in memory.
*/
#define REG_DESC_CHAIN_TADDR_Y 0x218

/*
 * Descriptor Chain Target Address U Register
 * Offset: 0x21c
 * Bits    Field                               Field (Code)        Type    Init    Reset    Description
 * 31:0    Descriptor Chain Target Address U   TADDRU              RO      0x0              Descriptor Chain Target Address U<p>This field indicates the current U target address in memory.
*/
#define REG_DESC_CHAIN_TADDR_Y 0x21c

/*
 * Descriptor Chain Target Address V Register
 * Offset: 0x220
 * Bits    Field                               Field (Code)        Type    Init    Reset    Description
 * 31:0    Descriptor Chain Target Address V   TADDRU              RO      0x0              Descriptor Chain Target Address V<p>This field indicates the current V target address in memory.
*/
#define REG_DESC_CHAIN_TADDR_Y 0x220

/*
 * Descriptor Chain Control Y Register
 * Offset: 0x224
 * Bits    Field                   Field (Code)   Type    Init    Reset    Description
 * 31:28   Target Number of Line   TNL            RO      0x0              Target Number of Line bit [5:2] of [5:0]<p>Bit [1:0] is always 0. When set to values other than 0, it guarantees that Target Length (bit [23:0]) fits TNL*4 scanline.
 * 27:25   Reserved                               RSVD			           Reserved for future use
 * 24      Interrupt Control       IC       	  RO 	  0x0			   Interrupt Control<p>1 = Generate interrupt once all bytes are written to current target buffer.<p>0 = Don't generate interrupt.
 * 23:3    Target Length		   TL			  RO 	  0x0			   Target Length in bytes bit [23:3] of [23:0]
 * 2:0	   Reserved                               RSVD			           Reserved for future use
*/
#define REG_DESC_CHAIN_CTRLY 0x224

/*
 * Descriptor Chain Control U Register
 * Offset: 0x228
 * Bits    Field                   Field (Code)   Type    Init    Reset    Description
 * 31:28   Target Number of Line   TNL            RO      0x0              Target Number of Line bit [5:2] of [5:0]<p>Bit [1:0] is always 0. When set to values other than 0, it guarantees that Target Length (bit [23:0]) fits TNL*4 scanline.
 * 27:25   Reserved                               RSVD			           Reserved for future use
 * 24      Interrupt Control       IC       	  RO 	  0x0			   Interrupt Control<p>1 = Generate interrupt once all bytes are written to current target buffer.<p>0 = Don't generate interrupt.
 * 23:3    Target Length		   TL			  RO 	  0x0			   Target Length in bytes bit [23:3] of [23:0]
 * 2:0	   Reserved                               RSVD			           Reserved for future use
*/
#define REG_DESC_CHAIN_CTRLU 0x228

/*
 * Descriptor Chain Control V Register
 * Offset: 0x22c
 * Bits    Field                   Field (Code)   Type    Init    Reset    Description
 * 31:28   Target Number of Line   TNL            RO      0x0              Target Number of Line bit [5:2] of [5:0]<p>Bit [1:0] is always 0. When set to values other than 0, it guarantees that Target Length (bit [23:0]) fits TNL*4 scanline.
 * 27:25   Reserved                               RSVD			           Reserved for future use
 * 24      Interrupt Control       IC       	  RO 	  0x0			   Interrupt Control<p>1 = Generate interrupt once all bytes are written to current target buffer.<p>0 = Don't generate interrupt.
 * 23:3    Target Length		   TL			  RO 	  0x0			   Target Length in bytes bit [23:3] of [23:0]
 * 2:0	   Reserved                               RSVD			           Reserved for future use
*/
#define REG_DESC_CHAIN_CTRLV 0x22c

/*
 * Next Descriptor Chain Address Y Register
 * Offset: 0x230
 * Bits    Field                               Field (Code)        Type    Init    Reset    Description
 * 31:2    Next Descriptor Chain Address Y     NDCADDRY            RO      0x0              Next Descriptor Chain Address in byte bit [31:2] of [31:0]
 * 1:0	   Reserved								                   RSVD					    Reserved for future use
*/
#define REG_NEXT_DESC_CHAIN_ADDR_Y 0x230

/*
 * Next Descriptor Chain Address U Register
 * Offset: 0x234
 * Bits    Field                               Field (Code)        Type    Init    Reset    Description
 * 31:2    Next Descriptor Chain Address U     NDCADDRY            RO      0x0              Next Descriptor Chain Address in byte bit [31:2] of [31:0]
 * 1:0	   Reserved								                   RSVD					    Reserved for future use
*/
#define REG_NEXT_DESC_CHAIN_ADDR_U 0x234

/*
 * Next Descriptor Chain Address V Register
 * Offset: 0x238
 * Bits    Field                               Field (Code)        Type    Init    Reset    Description
 * 31:2    Next Descriptor Chain Address V     NDCADDRY            RO      0x0              Next Descriptor Chain Address in byte bit [31:2] of [31:0]
 * 1:0	   Reserved								                   RSVD					    Reserved for future use
*/
#define REG_NEXT_DESC_CHAIN_ADDR_V 0x238

/*
 * Frame Byte Counter Register
 * Offset: 0x23c
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31:0    Frame Byte Count   FRAME_BYTE_CNT      RO      0x0              Frame Byte Count<p>Read this field and register when the frame done interrupt has been received.<p>frame_end_0_irq | frame_end_1_irq | frame_end_2_irq  (The interrupt should be generated when receiving one whole frame data at the input side, not indicating all the data are transferred to DDR )<p>Or, we can wait for the next frame’s SOF IRQ, then read the pixel byte count reg
*/
#define REG_FRAME_BYTE_CNT 0x23c

/*
 * Pipe1 Image Pitch Register
 * Offset: 0x240
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31:30   Reserved								  RSVD					   Reserved for future use
 * 29:18   UV Pitch           UVPITCH             RW      0x0              UV Pitch (distance in unit of 32-bit between two vertically adjacent pixels)<p>Must be programmed when output format is YUV planar. In JPEG capture, CCIC_IMG_PITCH = (bytesperline << 16) | bytesperline
 * 17:14   Reserved								  RSVD					   Reserved for future use
 * 13:2    Y Pitch			  YPITCH			  RW 	 0x0			   Y Pitch (distance in unit of 32-bit between two vertically adjacent pixels). In JPEG capture, CCIC_IMG_PITCH = (bytesperline << 16) | bytesperline.
 * 1:0     Reserved							  	  RSVD					   Reserved for future use
*/
#define REG_PIP1_CCIC_IMG_PITCH 0x240

/*
 * Pipe2 Image Pitch Register
 * Offset: 0x244
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31:30   Reserved								  RSVD					   Reserved for future use
 * 29:18   UV Pitch           UVPITCH             RW      0x0              UV Pitch (distance in unit of 32-bit between two vertically adjacent pixels)<p>Must be programmed when output format is YUV planar. In JPEG capture, CCIC_IMG_PITCH = (bytesperline << 16) | bytesperline
 * 17:14   Reserved								  RSVD					   Reserved for future use
 * 13:2    Y Pitch			  YPITCH			  RW 	 0x0			   Y Pitch (distance in unit of 32-bit between two vertically adjacent pixels). In JPEG capture, CCIC_IMG_PITCH = (bytesperline << 16) | bytesperline.
 * 1:0     Reserved							  	  RSVD					   Reserved for future use
*/
#define REG_PIP2_CCIC_IMG_PITCH 0x244

/*
 * PIP0_CROP_ROI_X
 * Offset: 0x248
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31:29   Reserved								  RSVD					   Reserved for future use
 * 28:16   roi_end_x                              RW      0x0              roi end x for crop function
 * 15:13   Reserved								  RSVD					   Reserved for future use
 * 12:0    roi_start_x			     			  RW 	  0x0			   roi start x for crop function
*/
#define REG_PIP0_CROP_ROI_X 0x248

/*
 * PIP0_CROP_ROI_Y
 * Offset: 0x24c
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31:29   Reserved								  RSVD					   Reserved for future use
 * 28:16   roi_end_y                              RW      0x0              roi end y for crop function
 * 15:13   Reserved								  RSVD					   Reserved for future use
 * 12:0    roi_start_y			     			  RW 	  0x0			   roi start y for crop function
*/
#define REG_PIP0_CROP_ROI_Y 0x24c

/*
 * PIP1_CROP_ROI_X
 * Offset: 0x250
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31:29   Reserved								  RSVD					   Reserved for future use
 * 28:16   roi_end_x                              RW      0x0              roi end x for crop function
 * 15:13   Reserved								  RSVD					   Reserved for future use
 * 12:0    roi_start_x			     			  RW 	  0x0			   roi start x for crop function
*/
#define REG_PIP1_CROP_ROI_X 0x250

/*
 * PIP1_CROP_ROI_Y
 * Offset: 0x254
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31:29   Reserved								  RSVD					   Reserved for future use
 * 28:16   roi_end_y                              RW      0x0              roi end y for crop function
 * 15:13   Reserved								  RSVD					   Reserved for future use
 * 12:0    roi_start_y			     			  RW 	  0x0			   roi start y for crop function
*/
#define REG_PIP1_CROP_ROI_Y 0x254

/*
 * PIP0_SCALER_IN_SIZE
 * Offset: 0x258
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31:29   Reserved								  RSVD					   Reserved for future use
 * 28:16   v_in_size                              RW      0x0              scaler input height
 * 15:13   Reserved								  RSVD					   Reserved for future use
 * 12:0    h_in_size			     			  RW 	  0x0			   scaler input width
*/
#define REG_PIP0_SCALER_IN_SIZE 0x258

/*
 * PIP0_SCALER_OUT_SIZE
 * Offset: 0x25c
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31:29   Reserved								  RSVD					   Reserved for future use
 * 28:16   v_out_size                             RW      0x0              scaler output height
 * 15:13   Reserved								  RSVD					   Reserved for future use
 * 12:0    h_out_size			     			  RW 	  0x0			   scaler output width
*/
#define REG_PIP0_SCALER_OUT_SIZE 0x25c

/*
 * PIP0_SCALER_H_PHASE
 * Offset: 0x260
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31      Reserved								  RSVD					   Reserved for future use
 * 30:24   h_init_ph                              RW      0x0              h initital phase
 * 15:13   Reserved								  RSVD					   Reserved for future use
 * 12:0    h_delta_ph_delta			     		  RW 	  0x0		   h delta phase 
*/
#define REG_PIP0_SCALER_H_PHASE 0x260

/*
 * PIP0_SCALER_V_PHASE
 * Offset: 0x264
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31      Reserved								  RSVD					   Reserved for future use
 * 30:24   v_init_ph                              RW      0x0              v initital phase
 * 15:13   Reserved								  RSVD					   Reserved for future use
 * 12:0    v_delta_ph_delta			     		  RW 	  0x0		   v delta phase 
*/
#define REG_PIP0_SCALER_V_PHASE 0x264

/*
 * PIP1_SCALER_IN_SIZE
 * Offset: 0x268
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31:29   Reserved								  RSVD					   Reserved for future use
 * 28:16   v_in_size                              RW      0x0              scaler input height
 * 15:13   Reserved								  RSVD					   Reserved for future use
 * 12:0    h_in_size			     			  RW 	  0x0			   scaler input width
*/
#define REG_PIP1_SCALER_IN_SIZE 0x268

/*
 * PIP1_SCALER_OUT_SIZE
 * Offset: 0x26c
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31:29   Reserved								  RSVD					   Reserved for future use
 * 28:16   v_out_size                             RW      0x0              scaler output height
 * 15:13   Reserved								  RSVD					   Reserved for future use
 * 12:0    h_out_size			     			  RW 	  0x0			   scaler output width
*/
#define REG_PIP1_SCALER_OUT_SIZE 0x26c

/*
 * PIP1_SCALER_H_PHASE
 * Offset: 0x260
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31      Reserved								  RSVD					   Reserved for future use
 * 30:24   h_init_ph                              RW      0x0              h initital phase
 * 15:13   Reserved								  RSVD					   Reserved for future use
 * 12:0    h_delta_ph_delta			     		  RW 	  0x0		       h delta phase 
*/
#define REG_PIP1_SCALER_H_PHASE 0x270

/*
 * PIP1_SCALER_V_PHASE
 * Offset: 0x274
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31      Reserved								  RSVD					   Reserved for future use
 * 30:24   v_init_ph                              RW      0x0              v initital phase
 * 15:13   Reserved								  RSVD					   Reserved for future use
 * 12:0    v_delta_ph_delta			     		  RW 	  0x0		       v delta phase 
*/
#define REG_PIP1_SCALER_V_PHASE 0x274

/*
 * SUBSAMPLE_SCALER_CTRL
 * Offset: 0x278
 *
*/
#define REG_SUBSAMPLE_SCALER_CTRL 0x278

/*
 * DATA_FORMAT_CTRL
 * Offset: 0x27c
 *
*/
#define REG_DATA_FORMAT_CTRL 0x27c

/*
 * ISP_DVP_IF_CTRL
 * Offset: 0x280
 * Bits    Field              Field (Code)        Type    Init    Reset    Description
 * 31      BYPASS_ISP							  RW      0x0			   BYPASS GC ISP  '1' valid
 * 30:29   reserved                                                        reserved
 * 28	   IS_RAW8_MODE							  RW 	  0x0			   Is RAW8 mode, data[7:0] from repack module will be feed in to [9:2], and low 2 bits stuffing 0. 
 * 27      reserved														   reserved
 * 26:16   TRIG_WMARK							  RW 	  0x8			   Trigger wartermark for dvp fifo recommand same as image width
 * 15:8    TAIL_LEN						    	  RW 	  0x8			   Minimun Tail time for dvp interface
 * 7:0	   HBLANK_LEN							  RW 	  0x8			   Minimun Hblank time for dvp interface
*/
#define REG_ISP_DVP_IF_CTRL 0x280

/*
 * CCIC_IMG_SIZE_PIP1
 * Offset: 0x284
 * Bits    Field                      Field (Code)        Type    Init    Reset    Description
 * 31:29   Reserved								          RSVD					   Reserved for future use
 * 28:16   Image Length in Scanline   VSIZE               RW 	  0x0              Pipe 1 Image Length in Scanline<p>Total size is VSIZE * bytescanline
 * 15:14   Reserved								          RSVD					   Reserved for future use
 * 13:0	   Image Width in byte Unit	  HSIZE				  RW 	  0x0			   Pipe 1 Image Width in byte
*/
#define REG_IMG_SIZE_PIP1 0x284

/*
 * CCIC_IMG_SIZE_PIP2
 * Offset: 0x288
 * Bits    Field                      Field (Code)        Type    Init    Reset    Description
 * 31:29   Reserved								          RSVD					   Reserved for future use
 * 28:16   Image Length in Scanline   VSIZE               RW 	  0x0              Pipe 2 Image Length in Scanline<p>Total size is VSIZE * bytescanline
 * 15:14   Reserved								          RSVD					   Reserved for future use
 * 13:0	   Image Width in byte Unit	  HSIZE				  RW 	  0x0			   Pipe 2 Image Width in byte
*/
#define REG_IMG_SIZE_PIP2 0x288

/*
 * ISP_IMG_SIZE
 * Offset: 0x28c
 * Bits    Field                      Field (Code)        Type    Init    Reset    Description
 * 31:29   Reserved								          RSVD					   Reserved for future use
 * 28:16   Image Length in Scanline   VSIZE               RW 	  0x0              ISP output Image Length in Scanline<p>Total size is VSIZE * bytescanline
 * 15:14   Reserved								          RSVD					   Reserved for future use
 * 13:0	   Image Width in byte Unit	  HSIZE				  RW 	  0x0			   ISP output Image Width in pixel
*/
#define REG_ISP_IMG_SIZE 0x28c

/*
 * DATA_RANGE_CTRL0
 * Offset: 0x290
 * Bits    Field          Field (Code)        Type    Init    Reset    Description
 * 31:24   pip0_u_top						  RW 	  0x0			   pip0_u_top
 * 23:16   pip0_u_bottom                      RW 	  0x0              pip0_u_bottom
 * 15:8    pip0_y_top						  RW 	  0x0              pip0_y_top
 * 7:0 	   pip0_y_bottom		              RW 	  0x0	           pip0_y_bottom
*/
#define REG_DATA_RANGE_CTRL0 0x290

/*
 * DATA_RANGE_CTRL1
 * Offset: 0x294
 * Bits    Field          Field (Code)        Type    Init    Reset    Description
 * 31:24   pip1_u_top						  RW 	  0x0			   pip1_u_top
 * 23:16   pip1_u_bottom                      RW 	  0x0              pip1_u_bottom
 * 15:8    pip1_y_top						  RW 	  0x0              pip1_y_top
 * 7:0 	   pip1_y_bottom		              RW 	  0x0	           pip1_y_bottom
*/
#define REG_DATA_RANGE_CTRL1 0x294

/*
 * DATA_RANGE_CTRL2
 * Offset: 0x298
 * Bits    Field          Field (Code)        Type    Init    Reset    Description
 * 31:25   Reserved							  RSVD					   Reserved for future use
 * 24:16   pip0_y_coef1                       RW 	  0x0              pip0_y_coef1
 * 15:9    Reserved							  RSVD					   Reserved for future use
 * 8:0 	   pip0_y_coef0		                  RW 	  0x0	           pip0_y_coef0
*/
#define REG_DATA_RANGE_CTRL2 0x298

/*
 * DATA_RANGE_CTRL3
 * Offset: 0x29c
 * Bits    Field          Field (Code)        Type    Init    Reset    Description
 * 31:25   Reserved							  RSVD					   Reserved for future use
 * 24:16   pip0_u_coef1                       RW 	  0x0              pip0_u_coef1
 * 15:9    Reserved							  RSVD					   Reserved for future use
 * 8:0 	   pip0_u_coef0		                  RW 	  0x0	           pip0_u_coef0
*/
#define REG_DATA_RANGE_CTRL3 0x29c


/*
 * DATA_RANGE_CTRL4
 * Offset: 0x2a0
 * Bits    Field          Field (Code)        Type    Init    Reset    Description
 * 31:25   Reserved							  RSVD					   Reserved for future use
 * 24:16   pip1_y_coef1                       RW 	  0x0              pip1_y_coef1
 * 15:9    Reserved							  RSVD					   Reserved for future use
 * 8:0 	   pip1_y_coef0		                  RW 	  0x0	           pip1_y_coef0
*/
#define REG_DATA_RANGE_CTRL4 0x2a0

/*
 * DATA_RANGE_CTRL5
 * Offset: 0x2a4
 * Bits    Field          Field (Code)        Type    Init    Reset    Description
 * 31:25   Reserved							  RSVD					   Reserved for future use
 * 24:16   pip1_u_coef1                       RW 	  0x0              pip1_u_coef1
 * 15:9    Reserved							  RSVD					   Reserved for future use
 * 8:0 	   pip1_u_coef0		                  RW 	  0x0	           pip1_u_coef0
*/
#define REG_DATA_RANGE_CTRL5 0x2a4


#define ISP_REG_BASE 0xd420e000


#endif
