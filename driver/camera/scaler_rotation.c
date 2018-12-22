#include "predefines.h"
#include "interrupt.h"
#include "scaler_rotation.h"

#define IRQ_MMP_SCL_IRE 61

void offline_s_power_clock(int32_t on)
{
#define AP_PMU_BASE 0xd4282800
	uint32_t val = 0;

	if (on) {
		//power on
		dprint("multimedia power on!");
		//REG32(AP_PMU_BASE+0x4c) = 0x7003f; // refer to ap-pmu register map
		//REG32(AP_PMU_BASE+0x50) = 0xc1c1033f;// refer to ap-pmu register map
		BU_REG_WRITE(AP_PMU_BASE + 0x4c, 0x7003f);
		dprint("APMU: 0x%08x = 0x%08x", AP_PMU_BASE + 0x4c,
			BU_REG_READ(AP_PMU_BASE + 0x4c));
		BU_REG_WRITE(AP_PMU_BASE + 0x50, 0xc1c1033f);
		dprint("APMU: 0x%08x = 0x%08x", AP_PMU_BASE + 0x50,
			BU_REG_READ(AP_PMU_BASE + 0x50));
	} else {
		//power off
		dprint("multimedia power off!");
		BU_REG_WRITE(AP_PMU_BASE + 0x4c, 0x0);
		BU_REG_WRITE(AP_PMU_BASE + 0x50, 0x0);
	}
}

static void scl_ire_isr_handler(void *arg)
{
	uint32_t irqs;

	dprint("%s IN!!!\n", __func__);

	irqs = scl_ire_reg_read(REG_OFFLINE_IRQ_STATUS);
	scl_ire_reg_write(REG_OFFLINE_IRQ_RAW, irqs);
	dprint("REG_OFFLINE_IRQ_STATUS = 0x%08x\n", irqs);

	if (irqs & IRQ_IRE_EOF) {
		dprint("IRE eof");
	}

	if (irqs & IRQ_SCL_EOF) {
		dprint("SCL eof");
	}

	if (irqs & IRQ_DMAC_RD_CH0_ERR) {
		dprint("dmac_rd_chnl0_err");
	}

	if (irqs & IRQ_DMAC_RD_CH1_ERR) {
		dprint("dmac_rd_chnl1_err");
	}

	if (irqs & IRQ_DMAC_WR_CH0_ERR) {
		dprint("dmac_wr_chnl0_err");
	}

	if (irqs & IRQ_DMAC_WR_CH1_ERR) {
		dprint("dmac_wr_chnl1_err");
	}

	if (irqs & IRQ_SCL_WF_OVERRUN) {
		dprint("scl_wfifo_overrun");
	}
}

void scl_ire_irq_init(void)
{
	dprint("%s E\n", __func__);
	/* Interupt Mask */
	//scl_ire_set_bit(REG_OFFLINE_IRQ_MASK, IRQ_IRE_EOF | IRQ_SCL_EOF);
	ISR_Connect(IRQ_MMP_SCL_IRE, scl_ire_isr_handler, NULL);
	INT_Enable(IRQ_MMP_SCL_IRE, 0, 15);
}

int scl_ire_regs_dump(void)
{
	dprint("%s E", __func__);
	scl_ire_reg_read(REG_SCALER_IN_SIZE);
	scl_ire_reg_read(REG_SCALER_OUT_SIZE);
	scl_ire_reg_read(REG_SCALER_SLICE_WIDTH);
	scl_ire_reg_read(REG_SCALER_H_PHASE);
	scl_ire_reg_read(REG_SCALER_V_PHASE);
	scl_ire_reg_read(REG_SCLAER_TOP_CTRL);
	scl_ire_reg_read(REG_SCALER_READ_PITCH);
	scl_ire_reg_read(REG_SCALER_WRITE_PITCH);
	scl_ire_reg_read(REG_SCALER_RD_Y_BASE_ADDR);
	scl_ire_reg_read(REG_SCALER_RD_U_BASE_ADDR);
	scl_ire_reg_read(REG_SCALER_RD_V_BASE_ADDR);
	scl_ire_reg_read(REG_SCALER_WR_Y_BASE_ADDR);
	scl_ire_reg_read(REG_SCALER_WR_U_BASE_ADDR);
	scl_ire_reg_read(REG_SCALER_WR_V_BASE_ADDR);
	scl_ire_reg_read(REG_OFFLINE_IRQ_RAW);
	scl_ire_reg_read(REG_OFFLINE_IRQ_MASK);
	scl_ire_reg_read(REG_OFFLINE_IRQ_STATUS);
	scl_ire_reg_read(REG_IRE_TOP_CTRL);
	scl_ire_reg_read(REG_IRE_IN_SIZE);
	scl_ire_reg_read(REG_IRE_PRE_PITCH);
	scl_ire_reg_read(REG_IRE_PST_PITCH);
	scl_ire_reg_read(REG_IRE_RD_Y_BASE_ADDR);
	scl_ire_reg_read(REG_IRE_RD_U_BASE_ADDR);
	scl_ire_reg_read(REG_IRE_RD_V_BASE_ADDR);
	scl_ire_reg_read(REG_IRE_WR_Y_BASE_ADDR);
	scl_ire_reg_read(REG_IRE_WR_U_BASE_ADDR);
	scl_ire_reg_read(REG_IRE_WR_V_BASE_ADDR);
	scl_ire_reg_read(REG_IRE_DMAC_CTRL);
	scl_ire_reg_read(REG_IRE_DEBUG_INFO_LSB);
	scl_ire_reg_read(REG_IRE_DEBUG_INFO_MSB);
	scl_ire_reg_read(REG_IRE_DMAC_DEBUG_INFO);

	return 0;
}

int scl_ire_regs_check(void)
{
	dprint("%s E", __func__);
	scl_ire_reg_write(REG_SCALER_IN_SIZE, 0xffffffff);
	scl_ire_reg_write(REG_SCALER_OUT_SIZE, 0xffffffff);
	scl_ire_reg_write(REG_SCALER_SLICE_WIDTH, 0xffffffff);
	scl_ire_reg_write(REG_SCALER_H_PHASE, 0xffffffff);
	scl_ire_reg_write(REG_SCALER_V_PHASE, 0xffffffff);
	scl_ire_reg_write(REG_SCLAER_TOP_CTRL, 0xffffffff);
	scl_ire_reg_write(REG_SCALER_READ_PITCH, 0xffffffff);
	scl_ire_reg_write(REG_SCALER_WRITE_PITCH, 0xffffffff);
	scl_ire_reg_write(REG_SCALER_RD_Y_BASE_ADDR, 0xffffffff);
	scl_ire_reg_write(REG_SCALER_RD_U_BASE_ADDR, 0xffffffff);
	scl_ire_reg_write(REG_SCALER_RD_V_BASE_ADDR, 0xffffffff);
	scl_ire_reg_write(REG_SCALER_WR_Y_BASE_ADDR, 0xffffffff);
	scl_ire_reg_write(REG_SCALER_WR_U_BASE_ADDR, 0xffffffff);
	scl_ire_reg_write(REG_SCALER_WR_V_BASE_ADDR, 0xffffffff);
	scl_ire_reg_write(REG_OFFLINE_IRQ_RAW, 0xffffffff);
	scl_ire_reg_write(REG_OFFLINE_IRQ_MASK, 0xffffffff);
	scl_ire_reg_write(REG_OFFLINE_IRQ_STATUS, 0xffffffff);
	scl_ire_reg_write(REG_IRE_TOP_CTRL, 0xffffffff);
	scl_ire_reg_write(REG_IRE_IN_SIZE, 0xffffffff);
	scl_ire_reg_write(REG_IRE_PRE_PITCH, 0xffffffff);
	scl_ire_reg_write(REG_IRE_PST_PITCH, 0xffffffff);
	scl_ire_reg_write(REG_IRE_RD_Y_BASE_ADDR, 0xffffffff);
	scl_ire_reg_write(REG_IRE_RD_U_BASE_ADDR, 0xffffffff);
	scl_ire_reg_write(REG_IRE_RD_V_BASE_ADDR, 0xffffffff);
	scl_ire_reg_write(REG_IRE_WR_Y_BASE_ADDR, 0xffffffff);
	scl_ire_reg_write(REG_IRE_WR_U_BASE_ADDR, 0xffffffff);
	scl_ire_reg_write(REG_IRE_WR_V_BASE_ADDR, 0xffffffff);
	scl_ire_reg_write(REG_IRE_DMAC_CTRL, 0xffffffff);
	scl_ire_reg_write(REG_IRE_DEBUG_INFO_LSB, 0xffffffff);
	scl_ire_reg_write(REG_IRE_DEBUG_INFO_MSB, 0xffffffff);
	scl_ire_reg_write(REG_IRE_DMAC_DEBUG_INFO, 0xffffffff);

	return 0;
}

static int scl_parse_input_fmt(uint32_t fmt, uint32_t *data_fmt, uint32_t *yuv422_order)
{
	uint8_t _fmt, _order;

	switch (fmt) {
	case V4L2_PIX_FMT_YUYV:
		_fmt = 0;
		_order = 0;
		break;
	case V4L2_PIX_FMT_YVYU:
		_fmt = 0;
		_order = 1;
		break;
	case V4L2_PIX_FMT_UYVY:
		_fmt = 0;
		_order = 2;
		break;
	case V4L2_PIX_FMT_VYUY:
		_fmt = 0;
		_order = 3;
		break;
	case V4L2_PIX_FMT_GREY:
		_fmt = 1;
		_order = 0;
		break;
	case V4L2_PIX_FMT_NV12M:
	case V4L2_PIX_FMT_NV21M:
		_fmt = 2;
		_order = 0;
		break;
	case V4L2_PIX_FMT_YUV420M:
	case V4L2_PIX_FMT_YVU420M:
		_fmt = 3;
		_order = 0;
		break;
	case V4L2_PIX_FMT_RGB24:
		_fmt = 4;
		_order = 0;
		break;
	case V4L2_PIX_FMT_RGB565:
		_fmt = 5;
		_order = 0;
		break;
	case V4L2_PIX_FMT_ARGB32:
		_fmt = 6;
		_order = 0;
		break;
	default:
		dprint("%s: input format not support: %d", fmt);
		return -1;
	}

	dprint("%s: data_fmt = %d, yuv422_order = %d n", __func__, _fmt, _order);
	if (data_fmt)
		*data_fmt = _fmt;

	if (yuv422_order)
		*yuv422_order = _order;

	return 0;
}

//TODO
static int scl_calc_coef_tab(uint32_t w_in, uint32_t h_in, uint32_t w_ot,
			     uint32_t h_ot, uint32_t *coef_sel, uint32_t *subsam)
{
	int rc = 0;

	dprint("%s E!!!!!!!!!!!!!!!!!!!!!!!! TODO\n", __func__);
	return -1;
}

void scl_enable(void)
{
	dprint("%s E\n", __func__);
	scl_ire_set_bit(REG_SCLAER_TOP_CTRL, SCL_CTRL_EN);
}

void scl_disable(void)
{
	dprint("%s E\n", __func__);
	scl_ire_clr_bit(REG_SCLAER_TOP_CTRL, SCL_CTRL_EN);
}

void scl_trigger(void)
{
	dprint("%s E\n", __func__);
	scl_ire_set_bit(REG_SCLAER_TOP_CTRL, SCL_CTRL_TRIGGER_FRAME);
}

void scl_set_phase(uint32_t w_in, uint32_t w_ot)
{
	int init_ph = 0;
	int delta_ph = 0;
	uint32_t val;

	dprint("%s E\n", __func__);
	init_ph = (w_in * 16 / w_ot + 1) / 2;
	dprint("w_init_ph = %d \n", init_ph);
	delta_ph = (w_in << 16) / w_ot;  // 4.12
	dprint("w_delta_ph = %d \n", delta_ph);
	val = (delta_ph & 0x1FFFFF) | ((init_ph & 0x7F) << 24);

	scl_ire_reg_write(REG_SCALER_H_PHASE, val);
	scl_ire_reg_write(REG_SCALER_V_PHASE, val);
}

void scl_set_size(uint32_t w_in, uint32_t h_in, uint32_t w_ot, uint32_t h_ot)
{
	uint32_t in_size, ot_size;

	dprint("%s E, w_in=0x%08x, h_in=0x%08x, w_ot=0x%08x, h_ot=0x%08x\n",
	       __func__, w_in, h_in, w_ot, h_ot);
	in_size = (w_in & 0x1FFF) | ((h_in & 0x1FFF) << 16);
	ot_size = (w_ot & 0x1FFF) | ((h_ot & 0x1FFF) << 16);

	scl_ire_reg_write(REG_SCALER_IN_SIZE, in_size);
	scl_ire_reg_write(REG_SCALER_OUT_SIZE, ot_size);
}

int scl_set_slice(uint32_t slice_in, uint32_t slice_ot)
{
	uint32_t slice = 0;

	if (slice_in > 512) {
		dprint("%s: slice in must less than 512 err\n", __func__);
		return -1;
	}
	slice = (slice_in & 0x1FFF) | ((slice_ot & 0x1FFF) << 16);
	scl_ire_reg_write(REG_SCALER_SLICE_WIDTH, slice);

	return 0;
}

void scl_set_pitch(uint32_t pitch_in, uint32_t pitch_ot)
{
	scl_ire_reg_write(REG_SCALER_READ_PITCH, (pitch_in & 0x3FFFF));
	scl_ire_reg_write(REG_SCALER_WRITE_PITCH, (pitch_ot & 0x3FFFF));
}

int scl_set_top(uint32_t fmt, uint32_t w_in, uint32_t h_in,
		uint32_t w_ot, uint32_t h_ot)
{
	int rc = 0;
	uint32_t data_fmt, yuv422_order, coef_sel_h, coef_sel_v, subsam;
	uint32_t val = 0, mask = 0;

	dprint("%s E\n", __func__);

	/* data_fmt, yuv422 order */
	rc = scl_parse_input_fmt(fmt, &data_fmt, &yuv422_order);
	if (rc < 0) {
		dprint("%s: parse fmt err\n", __func__);
		return rc;
	}

	/*
 *         [> subsumple <]
 *         switch (sub) {
 *         case SUBSAM_1_1:
 *                 sam = SCL_1_SUBSAMPLE;
 *                 break;
 *         case SUBSAM_1_2:
 *                 sam = SCL_2_SUBSAMPLE;
 *                 break;
 *         case SUBSAM_1_4:
 *                 sam = SCL_4_SUBSAMPLE;
 *                 break;
 *         case SUBSAM_1_8:
 *                 sam = SCL_8_SUBSAMPLE;
 *                 break;
 *         case SUBSAM_1_16:
 *                 sam = SCL_16_SUBSAMPLE;
 *                 break;
 *         case SUBSAM_1_32:
 *                 sam = SCL_32_SUBSAMPLE;
 *                 break;
 *         default:
 *                 dprint("%s: subsample %d err", sub);
 *                 return -1;
 *         }
 *
 */
	rc = scl_calc_coef_tab(w_in, h_in, w_ot, h_ot, &coef_sel_h, &subsam);
	if (rc < 0) {
		dprint("%s: calc coef tab h err\n", __func__);
		return rc;
	}
	rc = scl_calc_coef_tab(w_in, h_in, w_ot, h_ot, &coef_sel_v, &subsam);
	if (rc < 0) {
		dprint("%s: calc coef tab v err\n", __func__);
		return rc;
	}

	val = data_fmt | yuv422_order | subsam | coef_sel_v | coef_sel_h;
	mask = SCL_CTRL_DATA_FMT_MASK | SCL_CTRL_YUV422_ORDER_MASK | SCL_CTRL_SUBSAMLE_MASK |
		SCL_CTRL_COEF_TAB_SEL_V_MASK | SCL_CTRL_COEF_TAB_SEL_H_MASK;
	scl_ire_write_mask(REG_SCLAER_TOP_CTRL, val, mask);

	return rc;
}

void scl_set_addr(uint32_t addr_rd_y, uint32_t addr_rd_u, uint32_t addr_rd_v,
		  uint32_t addr_wr_y, uint32_t addr_wr_u, uint32_t addr_wr_v)
{
	scl_ire_reg_write(REG_SCALER_RD_Y_BASE_ADDR, addr_rd_y);
	scl_ire_reg_write(REG_SCALER_RD_U_BASE_ADDR, addr_rd_u);
	scl_ire_reg_write(REG_SCALER_RD_V_BASE_ADDR, addr_rd_v);

	scl_ire_reg_write(REG_SCALER_WR_Y_BASE_ADDR, addr_wr_y);
	scl_ire_reg_write(REG_SCALER_WR_U_BASE_ADDR, addr_wr_u);
	scl_ire_reg_write(REG_SCALER_WR_V_BASE_ADDR, addr_wr_v);
}

int scl_ctrl_cfg(uint32_t w_in, uint32_t h_in, uint32_t w_ot, uint32_t h_ot,
		 uint32_t pitch_in, uint32_t pitch_ot, uint32_t fmt)
{
	int rc = 0;

	dprint("%s E\n", __func__);
	scl_enable();  //FIXME: mv enable to power on
	scl_set_phase(w_in, w_ot);
	scl_set_size(w_in, h_in, w_ot, h_ot);
	scl_set_pitch(pitch_in, pitch_ot);
	rc = scl_set_slice(512, 512);
	if (rc < 0) {
		dprint("%s: slice err\n", __func__);
		return rc;
	}
	rc = scl_set_top(fmt, w_in, h_in, w_ot, h_ot);
	if (rc < 0) {
		dprint("%s: top err\n", __func__);
		return rc;
	}

	return rc;
}

void scl_func(void)
{
	int rc = 0;
	uint32_t w_in = 640;
	uint32_t h_in = 480;
	uint32_t w_ot = 640;
	uint32_t h_ot = 480;
	uint32_t pitch_in = w_in;
	uint32_t pitch_ot = w_ot;
	uint32_t fmt = 0;

	//1. power up and clk
	offline_s_power_clock(1);

	//2. irq init
	scl_ire_irq_init();

	//3. scl cfg
	rc = scl_ctrl_cfg(w_in, h_in, w_ot, h_ot, pitch_in, pitch_ot, fmt);
	if (rc < 0) {
		dprint("%s: scaler configure err\n", __func__);
		return;
	}
	//TODO addr:
	/*
	 * scl_set_addr(addr_rd_y, addr_rd_u, addr_rd_v,
	 *         addr_wr_y, addr_wr_u, addr_wr_v);
	 */

	//4. stream on
	scl_trigger();

	//power off and clk
	offline_s_power_clock(0);
}

static int ire_parse_input_fmt(uint32_t fmt, uint32_t *bpp, uint32_t *rot_mod,
			       uint32_t *yuv_fmt, uint32_t *yuv422_edian)
{
	uint32_t _bpp, _rot_mod, _yuv_fmt, _yuv422_edian;

	switch (fmt) {
	case V4L2_PIX_FMT_YUYV:
		_bpp = IRE_16_BPP;
		_rot_mod = IRE_MOD_PACKED;
		_yuv_fmt = IRE_FMT_YUV422;
		_yuv422_edian = IRE_CTRL_YUYV_EDIAN;
		break;
	case V4L2_PIX_FMT_YVYU:
		_bpp = IRE_16_BPP;
		_rot_mod = IRE_MOD_PACKED;
		_yuv_fmt = IRE_FMT_YUV422;
		_yuv422_edian = IRE_CTRL_YVYU_EDIAN;
		break;
	case V4L2_PIX_FMT_UYVY:
		_bpp = IRE_16_BPP;
		_rot_mod = IRE_MOD_PACKED;
		_yuv_fmt = IRE_FMT_YUV422;
		_yuv422_edian = IRE_CTRL_YUYV_EDIAN;
		break;
	case V4L2_PIX_FMT_VYUY:
		_bpp = IRE_16_BPP;
		_rot_mod = IRE_MOD_PACKED;
		_yuv_fmt = IRE_FMT_YUV422;
		_yuv422_edian = IRE_CTRL_YVYU_EDIAN;
		break;
	case V4L2_PIX_FMT_GREY:
		_bpp = IRE_8_BPP;
		_rot_mod = IRE_MOD_PACKED;
		_yuv_fmt = IRE_FMT_YUV400;
		_yuv422_edian = 0;  //invalid
		break;
	case V4L2_PIX_FMT_NV12M:
	case V4L2_PIX_FMT_NV21M:
		_bpp = IRE_8_BPP;
		_rot_mod = IRE_MOD_2PLANAR;
		_yuv_fmt = IRE_FMT_YUV420;
		_yuv422_edian = 0;  //invalid
		break;
	case V4L2_PIX_FMT_YUV420M:
	case V4L2_PIX_FMT_YVU420M:
		_bpp = IRE_8_BPP;
		_rot_mod = IRE_MOD_3PLANAR;
		_yuv_fmt = IRE_FMT_YUV420;
		_yuv422_edian = 0;  //invalid
		break;
	case V4L2_PIX_FMT_RGB24:
		_bpp = IRE_24_BPP;
		_rot_mod = IRE_MOD_PACKED;
		_yuv_fmt = IRE_FMT_RSVD;
		_yuv422_edian = 0;  //invalid
		break;
	case V4L2_PIX_FMT_RGB565:
		_bpp = IRE_16_BPP;
		_rot_mod = IRE_MOD_PACKED;
		_yuv_fmt = IRE_FMT_RSVD;
		_yuv422_edian = 0;  //invalid
		break;
	case V4L2_PIX_FMT_ARGB32:
		_bpp = IRE_32_BPP;
		_rot_mod = IRE_MOD_PACKED;
		_yuv_fmt = IRE_FMT_RSVD;
		_yuv422_edian = 0;  //invalid
		break;
	default:
		dprint("%s: input format not support: %d", fmt);
		return -1;
	}

	dprint("%s: bpp = 0x%08x, rot_mod = 0x%08x, yuv_fmt = 0x%08x, yuv422_edian = 0x%08x\n",
	       __func__, _bpp, _rot_mod, _yuv_fmt, _yuv422_edian);
	if (bpp)
		*bpp = _bpp;

	if (rot_mod)
		*rot_mod = _rot_mod;

	if (yuv_fmt)
		*yuv_fmt = _yuv_fmt;

	if (yuv422_edian)
		*yuv422_edian = _yuv422_edian;

	return 0;
}

void ire_enable(void)
{
	dprint("%s E\n", __func__);
	scl_ire_set_bit(REG_IRE_TOP_CTRL, IRE_CTRL_EN);
}

void ire_disable(void)
{
	dprint("%s E\n", __func__);
	scl_ire_clr_bit(REG_IRE_TOP_CTRL, IRE_CTRL_EN);
}

void ire_trigger(void)
{
	dprint("%s E\n", __func__);
	scl_ire_set_bit(REG_IRE_TOP_CTRL, IRE_CTRL_PREFETCH);
}

void ire_set_size(uint32_t w_in, uint32_t h_in)
{
	uint32_t in_size;

	in_size = (w_in & 0x1FFF) | ((h_in & 0xFFF) << 16);
	dprint("%s E, w_in=0x%08x, h_in=0x%08x, in_size=0x%08x\n",
	       __func__, w_in, h_in, in_size);

	scl_ire_reg_write(REG_IRE_IN_SIZE, in_size);
}

void ire_set_pitch(uint32_t pitch_in, uint32_t pitch_ot)
{
	scl_ire_reg_write(REG_IRE_PRE_PITCH, (pitch_in & 0x3FFFF));
	scl_ire_reg_write(REG_IRE_PST_PITCH, (pitch_ot & 0x3FFFF));
}

int ire_set_top(uint32_t fmt, enum offline_rot rot)
{
	int rc = 0;
	uint32_t bpp, angle, rot_mod, yuv_fmt, yuv422_edian;
	uint32_t val = 0, mask = 0;

	switch (rot) {
	case ROT_90_DEG:
		angle = IRE_DEGREE_90;
		break;
	case ROT_180_DEG:
		angle = IRE_DEGREE_180;
		break;
	case ROT_270_DEG:
		angle = IRE_DEGREE_270;
		break;
	case ROT_MIR:
		angle = IRE_MIRROR;
		break;
	case ROT_FLIP:
		angle = IRE_FLIP;
		break;
	default:
		dprint("%s: rot %d err", rot);
		return -1;
	}

	rc = ire_parse_input_fmt(fmt, &bpp, &rot_mod, &yuv_fmt, &yuv422_edian);
	if (rc < 0) {
		dprint("%s: parse input fmt err\n", __func__);
		return -1;
	}

	val = bpp | angle | rot_mod | yuv_fmt | yuv422_edian;
	mask = IRE_CTRL_BPP_MASK | IRE_CTRL_ANGLE_MASK | IRE_CTRL_YUV_MOD_MASK | IRE_CTRL_YUV_FMT_MASK | IRE_CTRL_422_EDIAN_MASK;

	scl_ire_write_mask(REG_IRE_TOP_CTRL, val, mask);

	return rc;
}

void ire_set_addr(uint32_t addr_rd_y, uint32_t addr_rd_u, uint32_t addr_rd_v,
		  uint32_t addr_wr_y, uint32_t addr_wr_u, uint32_t addr_wr_v)
{
	scl_ire_reg_write(REG_IRE_RD_Y_BASE_ADDR, addr_rd_y);
	scl_ire_reg_write(REG_IRE_RD_U_BASE_ADDR, addr_rd_u);
	scl_ire_reg_write(REG_IRE_RD_V_BASE_ADDR, addr_rd_v);

	scl_ire_reg_write(REG_IRE_WR_Y_BASE_ADDR, addr_wr_y);
	scl_ire_reg_write(REG_IRE_WR_U_BASE_ADDR, addr_wr_u);
	scl_ire_reg_write(REG_IRE_WR_V_BASE_ADDR, addr_wr_v);
}

int ire_ctrl_cfg(uint32_t w_in, uint32_t h_in, uint32_t pitch_in, uint32_t pitch_ot,
		 uint32_t fmt, enum offline_rot rot)
{
	int rc = 0;

	dprint("%s E\n", __func__);

	ire_enable();
	ire_set_pitch(pitch_in, pitch_ot);
	ire_set_size(w_in, h_in);
	rc = ire_set_top(fmt, rot);
	if (rc < 0) {
		dprint("%s: top err\n", __func__);
		return rc;
	}

	return rc;
}

/*
 * cfg as george recomend
 * */
const uint32_t dmac_postwr_en = 0;
const uint32_t dmac_max_req_num = 4;
const uint32_t dmac_scan_mode = 0;
const uint32_t dmac_scan_enable = 0;
const uint32_t dmac_rd_int_clr = 0;
const uint32_t dmac_wr_int_clr = 0;
const uint32_t dmac_rst_req = 0;
const uint32_t dmac_rst_n_pwr = 1;
const uint32_t dmac_user_id = 0;
const uint32_t dmac_axi_sec = 0;
const uint32_t dmac_arqos = 4;
const uint32_t dmac_awqos = 4;
void ire_dmac_cfg(void)
{
	uint32_t val = 0;

	val = (dmac_postwr_en << 24) | (dmac_max_req_num << 19) | (dmac_scan_mode << 18) | (dmac_scan_enable << 17) | (dmac_rd_int_clr << 16) | (dmac_wr_int_clr < 15) | (dmac_rst_req << 14) | (dmac_rst_n_pwr << 13) | (dmac_user_id << 9) | (dmac_axi_sec << 8) | (dmac_arqos << 4) | (dmac_awqos << 0);
	dprint("%s :DMAC_CTRL = 0x%08x\n", __func__, val);
	scl_ire_reg_write(REG_IRE_DMAC_CTRL, val);
}

void ire_func(void)
{
	int rc = 0;
	uint32_t w_in = 640;
	uint32_t h_in = 640;
	uint32_t pitch_in = w_in;
	uint32_t pitch_ot = w_in;
	uint32_t fmt = 0;
	enum offline_rot rot = 0;

	//1. power up and clk
	offline_s_power_clock(1);

	//2. irq init and dmac cfg
	scl_ire_irq_init();
	ire_dmac_cfg();

	//4. ire cfg
	rc = ire_ctrl_cfg(w_in, h_in, pitch_in, pitch_ot, fmt, rot);
	if (rc < 0) {
		dprint("%s: rotation cfg err\n", __func__);
		return;
	}
	//TODO: addr
	/*
	 * scl_set_addr(addr_rd_y, addr_rd_u, addr_rd_v,
	 *         addr_wr_y, addr_wr_u, addr_wr_v);
	 */

	//5. ire stream on
	ire_trigger();

	//power off and clk
	offline_s_power_clock(0);
}
