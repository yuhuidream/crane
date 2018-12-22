#if defined(HARDWARE_EVB)	//aquilac evb
#include "plat_basic_api.h"
#include <string.h>
#else 				//crane fpga
#include "predefines.h"
#include "bbu.h"
#include <string.h>
#endif

#include "scaler_rotation.h"
#include "dp650_fns.h"

#define SRC_BUFFER_1_ADDR 0x20000000
#define SRC_BUFFER_2_ADDR 0x23000000
#define SRC_BUFFER_3_ADDR 0x26000000
#define SRC_BUFFER_4_ADDR 0x29000000
#define SRC_BUFFER_5_ADDR 0x2C000000

#define DEST_BUFFER_1_ADDR 0x30000000
#define DEST_BUFFER_2_ADDR 0x33000000
#define DEST_BUFFER_3_ADDR 0x36000000
#define DEST_BUFFER_4_ADDR 0x39000000
#define DEST_BUFFER_5_ADDR 0x3C000000
#define DEST_BUFFER_6_ADDR 0x41000000

struct offline_rotation_item {
	enum offline_rot type;
	char *name;
};

static struct offline_rotation_item rot_item[] = {
	{ROT_90_DEG,  "offline rotation 90'"},
	{ROT_180_DEG, "offline rotation 180'"},
	{ROT_270_DEG, "offline rotation 270'"},
	{ROT_MIR,     "offline rotation mirror"},
	{ROT_FLIP,    "offline rotation flip"},
	{ROT_MAX,     "offline rotation invalid"},
};

int offline_scaler_test(int argc, uint8_t **argv)
{
	int rc = 0;
	uint32_t w_in = 640;
	uint32_t h_in = 480;
	uint32_t w_ot = 640;
	uint32_t h_ot = 480;
	uint32_t pitch_in = w_in;
	uint32_t pitch_ot = w_ot;
	uint32_t fmt = 0;
	uint32_t stride_c3_p1 = 0, stride_c3_p2 = 0;
	uint64_t addr_c3_p2 = 0;

	dprint("before draw_image!\n");
	draw_images(SRC_BUFFER_3_ADDR, &addr_c3_p2, NULL, DP650_FORMAT_YUV420_P2, 
		IMG_PATTERN_VCB_5, w_in, h_in, false, &stride_c3_p1, &stride_c3_p2, NULL);
	fmt = V4L2_PIX_FMT_NV12M;
	pitch_in = stride_c3_p1;
	pitch_ot = stride_c3_p1;
	dprint("after draw_image!\n");

	// power up and clk
	offline_s_power_clock(1);

	// irq init
	scl_ire_irq_init();

	// scl cfg
	rc = scl_ctrl_cfg(w_in, h_in, w_ot, h_ot, pitch_in, pitch_ot, fmt);
	if (rc < 0) {
		dprint("%s: scaler configure err\n", __func__);
		return SCREEN_TEST_FIAL;
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

	return SCREEN_TEST_PASS;
}

int offline_rotation_test(int argc, uint8_t **argv)
{
	int rc = 0;
	uint32_t w_in = 640;
	uint32_t h_in = 640;
	uint32_t pitch_in = w_in;
	uint32_t pitch_ot = w_in;
	uint32_t fmt = 0;
	enum offline_rot rot = 0;
	uint32_t stride_c3_p1 = 0, stride_c3_p2 = 0;
	uint64_t addr_c3_p2 = 0;

	dprint("before draw_image!\n");
	draw_images(SRC_BUFFER_3_ADDR, &addr_c3_p2, NULL, DP650_FORMAT_YUV420_P2, 
		IMG_PATTERN_VCB_5, w_in, h_in, false, &stride_c3_p1, &stride_c3_p2, NULL);
	fmt = V4L2_PIX_FMT_NV12M;
	pitch_in = stride_c3_p1;
	dprint("after draw_image!\n");

	//power up and clk
	offline_s_power_clock(1);

	//irq init and dmac cfg
	scl_ire_irq_init();
	ire_dmac_cfg();

	//ire cfg
	for (int i = 0; rot_item[i].type < ROT_MAX; i++) {
		rc = ire_ctrl_cfg(w_in, h_in, pitch_in, pitch_ot, fmt, rot_item[i].type);
		if (rc < 0) {
			dprint("%s: rotation cfg err\n", __func__);
			return SCREEN_TEST_FIAL;
		}
		ire_set_addr(SRC_BUFFER_3_ADDR, addr_c3_p2, NULL,
			     DEST_BUFFER_3_ADDR, addr_c3_p2 - SRC_BUFFER_3_ADDR, NULL);

		//5. ire stream on
		ire_trigger();
		dprint("%s start \n", rot_item[i].name);
	}

	//power off and clk
	offline_s_power_clock(0);

	return SCREEN_TEST_PASS;
}

void offline_regs_walk_through(void)
{
	//power up and clk
	offline_s_power_clock(1);

	dprint("%s E", __func__);
	scl_ire_regs_dump();
	scl_ire_regs_check();
	scl_ire_regs_dump();
	dprint("%s X", __func__);

	offline_s_power_clock(0);
}
