#include "predefines.h"
#include "buf_mgr.h"
#include "dp650_fns.h"

#define BUF_MGR_DBG
#ifdef BUF_MGR_DBG
#define dprint(fmt, arg...)			\
	do {					\
		bbu_printf("[bufmgr]"fmt "\r\n",## arg);	\
	} while (0)
#else
#define dprint(...) do {} while(0)
#endif

#define MAX_VB2_BUFFER_NUM 30

struct buffer_poll {
	uint32_t bitmap;
	uint32_t free_bufs;
	struct offline_buf bufs[MAX_VB2_BUFFER_NUM];
};

static struct buffer_poll vb2_poll = {
	.bitmap = 0xFFFFFFFF,
	.free_bufs = MAX_VB2_BUFFER_NUM,
};

static struct offline_buf *get_free_buf_from_pool()
{
	struct offline_buf *vb = NULL;
	uint32_t valid_idx = 0;

	valid_idx = __builtin_ffs(vb2_poll.bitmap) - 1;
	if(valid_idx > MAX_VB2_BUFFER_NUM) {
		dprint("no enough buffer, idx = %d\n", valid_idx);
		return NULL;
	}
	vb = &vb2_poll.bufs[valid_idx];
	vb->index = valid_idx;
	CLR_BIT(vb2_poll.bitmap, valid_idx);
	vb2_poll.free_bufs--;

	dprint("###buffer %d, buf bitmap 0x%8x###",
			valid_idx, vb2_poll.bitmap);
	return vb;
}

static int release_buf_to_pool(struct offline_buf *vb)
{
	uint32_t used_idx = vb->index;

	vb = NULL; //point to NULL
	SET_BIT(vb2_poll.bitmap, used_idx);
	vb2_poll.free_bufs++;

	dprint("buffer rls %d, buf bitmap 0x%8x",
			used_idx, vb2_poll.bitmap);
	return 0;
}

static int init_buf(struct offline_buf *buf)
{
	int rc = 0;
	uint32_t pitch;

	if (buf->width == 0 || buf->height == 0 || buf->stride == 0) {
		dprint("%s, bad argument", __func__);
		return -1;
	}

	switch (buf->pixelformat) {
	case V4L2_PIX_FMT_YUYV:
		buf->num_planes = 1;
		buf->bpp = 16;
		pitch = buf->bpp * (buf->stride) >> 3;
		buf->pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->bpp * (buf->width) >> 3;
		buf->plane[0].sizeimage = pitch * buf->height;
		/* TODO
		 * if ((buf->width) & 0x07) {
		 *         dprint("err: width need 8 align\n");
		 *         rc = -1;
		 * }
		 */
		break;
	case V4L2_PIX_FMT_YVYU:
		buf->num_planes = 1;
		buf->bpp = 16;
		pitch = buf->bpp * (buf->stride) >> 3;
		buf->pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->bpp * (buf->width) >> 3;
		buf->plane[0].sizeimage = pitch * buf->height;
		/* TODO
		 * if ((buf->width) & 0x07) {
		 *         dprint("err: width need 8 align\n");
		 *         rc = -1;
		 * }
		 */
		break;
	case V4L2_PIX_FMT_UYVY:
		buf->num_planes = 1;
		buf->bpp = 16;
		pitch = buf->bpp * (buf->stride) >> 3;
		buf->pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->bpp * (buf->width) >> 3;
		buf->plane[0].sizeimage = pitch * buf->height;
		break;
	case V4L2_PIX_FMT_VYUY:
		buf->num_planes = 1;
		buf->bpp = 16;
		pitch = buf->bpp * (buf->stride) >> 3;
		buf->pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->bpp * (buf->width) >> 3;
		buf->plane[0].sizeimage = pitch * buf->height;
		break;
	case V4L2_PIX_FMT_GREY:
		buf->num_planes = 1;
		buf->bpp = 8;
		pitch = buf->bpp * (buf->stride) >> 3;
		buf->pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->bpp * (buf->width) >> 3;
		buf->plane[0].sizeimage = pitch * buf->height;
		break;
	case V4L2_PIX_FMT_RGB24:
		buf->num_planes = 1;
		buf->bpp = 24;
		pitch = buf->bpp * (buf->stride) >> 3;
		buf->pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->bpp * (buf->width) >> 3;
		buf->plane[0].sizeimage = pitch * buf->height;
		break;
	case V4L2_PIX_FMT_RGB565:
		buf->num_planes = 1;
		buf->bpp = 16;
		pitch = buf->bpp * (buf->stride) >> 3;
		buf->pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->bpp * (buf->width) >> 3;
		buf->plane[0].sizeimage = pitch * buf->height;
		break;
	case V4L2_PIX_FMT_ARGB32:
		buf->num_planes = 1;
		buf->bpp = 32;
		pitch = buf->bpp * (buf->stride) >> 3;
		buf->pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->bpp * (buf->width) >> 3;
		buf->plane[0].sizeimage = pitch * buf->height;
		break;
	case V4L2_PIX_FMT_NV12:
	case V4L2_PIX_FMT_NV21:
	case V4L2_PIX_FMT_NV12M:
	case V4L2_PIX_FMT_NV21M:
		buf->num_planes = 2;
		buf->bpp = 8;
		pitch = buf->stride;
		buf->pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->width;
		buf->plane[0].sizeimage = pitch * buf->height;

		pitch = buf->stride;
		buf->plane[1].bytesperline = pitch;
		buf->plane[1].usedperline = buf->width;
		buf->plane[1].sizeimage = pitch * buf->height / 2;
		break;
	case V4L2_PIX_FMT_YUV420M:
	case V4L2_PIX_FMT_YVU420M:
		buf->num_planes = 3;
		buf->bpp = 8;
		pitch = buf->stride;
		buf->pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->width;
		buf->plane[0].sizeimage = pitch * buf->height;

		pitch = buf->stride >> 1;
		buf->plane[1].bytesperline = pitch;
		buf->plane[1].usedperline = buf->width >> 1;
		buf->plane[1].sizeimage = pitch * buf->height / 2;

		buf->plane[2].bytesperline = pitch; 
		buf->plane[2].usedperline = buf->width >> 1;
		buf->plane[2].sizeimage = pitch * buf->height / 2;
		break;
	default:
		dprint("%s: input format not support: %d", __func__, buf->pixelformat);
		rc = -1;
	}

#ifdef BUF_MGR_DBG
	dprint("%s: p1 bytesperline=0x%08x, usedperline=0x%08x",
			__func__, buf->plane[0].bytesperline, buf->plane[0].usedperline);
	dprint("%s: p2 bytesperline=0x%08x, usedperline=0x%08x",
			__func__, buf->plane[1].bytesperline, buf->plane[1].usedperline);
	dprint("%s: p3 bytesperline=0x%08x, usedperline=0x%08x",
			__func__, buf->plane[2].bytesperline, buf->plane[2].usedperline);
#endif

	return rc;
}

/*========================buf func================================*/
struct offline_buf *buf_construct(uint32_t width, uint32_t height, uint32_t pixelformat, uint32_t stride)
{
	struct offline_buf *buf = NULL;
	 
	buf = get_free_buf_from_pool();

	if (buf) {
		buf->width = width;
		buf->height = height;
		buf->pixelformat = pixelformat;
		buf->stride = stride;
		init_buf(buf);
	}

	return buf;
}

int buf_destruct(struct offline_buf *buf)
{
	int rc = 0;

	rc = release_buf_to_pool(buf);

	return rc;
}

/* suppose compact planar storage */
int buf_set_base_addr(struct offline_buf *buf, uint32_t base)
{
	int rc = 0;

	switch (buf->num_planes) {
	case 1:
		buf->plane[0].addr = base;
		buf->plane[1].addr = 0;
		buf->plane[2].addr = 0;
		break;
	case 2:
		buf->plane[0].addr = base;
		buf->plane[1].addr = base + buf->plane[0].sizeimage;
		buf->plane[2].addr = 0;
		break;
	case 3:
		buf->plane[0].addr = base;
		buf->plane[1].addr = base + buf->plane[0].sizeimage;
		buf->plane[2].addr = base + buf->plane[0].sizeimage + buf->plane[1].sizeimage;
		break;
	default:
		dprint("%s: format not support: %d", __func__, buf->pixelformat);
		rc = -1;
	}

	/* check addr range */
	if ((base < SQU_VALID_ADDR_S) || ((buf->plane[2].addr + buf->plane[2].sizeimage) > SQU_VALID_ADDR_E)) {
		dprint("%s: exceed aviliable range 0x%08x--0x%08x",
		       __func__, SQU_VALID_ADDR_S, SQU_VALID_ADDR_E);
		rc = -1;
	}

#ifdef BUF_MGR_DBG
	dprint("%s: p1 addr=0x%08x, sizeimage=0x%08x",
			__func__, buf->plane[0].addr, buf->plane[0].sizeimage);
	dprint("%s: p2 addr=0x%08x, sizeimage=0x%08x",
			__func__, buf->plane[1].addr, buf->plane[1].sizeimage);
	dprint("%s: p3 addr=0x%08x, sizeimage=0x%08x",
			__func__, buf->plane[2].addr, buf->plane[2].sizeimage);
#endif

	return rc;
}

int buf_fill_by_img(struct offline_buf *buf)
{
	int rc;
	uint32_t fmt;

	switch (buf->pixelformat) {
	case V4L2_PIX_FMT_YUYV:
		fmt = DP650_FORMAT_YUYV;
		break;
	case V4L2_PIX_FMT_YVYU:
		fmt = DP650_FORMAT_YVYU;
		break;
	case V4L2_PIX_FMT_UYVY:
		fmt = DP650_FORMAT_UYVY;
		break;
	case V4L2_PIX_FMT_VYUY:
		fmt = DP650_FORMAT_VYUY;
		break;
	case V4L2_PIX_FMT_NV12:
	case V4L2_PIX_FMT_NV21:
	case V4L2_PIX_FMT_NV12M:
	case V4L2_PIX_FMT_NV21M:
		fmt = DP650_FORMAT_YUV420_P2;
		break;
	case V4L2_PIX_FMT_YUV420M:
	case V4L2_PIX_FMT_YVU420M:
		fmt = DP650_FORMAT_YUV420_P3;
		break;
	case V4L2_PIX_FMT_ARGB32:
		fmt = DP650_FORMAT_ARGB8888;
		break;
	case V4L2_PIX_FMT_GREY:
		fmt = DP650_FORMAT_YUV400;
		break;
	case V4L2_PIX_FMT_RGB24:
		fmt = DP650_FORMAT_RGB888;
		break;
	case V4L2_PIX_FMT_RGB565:
		fmt = DP650_FORMAT_RGB565;
		break;
	default:
		dprint("%s: format not support: %d", __func__, buf->pixelformat);
		return -1;
	}

	dprint("before draw_image!");
	rc = draw_images(buf->plane[0].addr, &(buf->plane[1].addr), &(buf->plane[2].addr), fmt, IMG_PATTERN_VCB_5,
		    buf->width, buf->height, false, &(buf->plane[0].pitch), &(buf->plane[1].pitch), &(buf->plane[2].pitch));
	dprint("buf%d: p1[0x%08x], p2[0x%08x], p3[0x%08x]",
			buf->index, buf->plane[0].addr, buf->plane[1].addr, buf->plane[2].addr);
	dprint("after draw_image!");

	return rc;
}

int buf_info_dump(struct offline_buf *buf)
{
	int rc = 0;
	uint32_t width, height;
	uint32_t start_addr, end_addr;
	char *suffix;

	width = buf->width;
	height = buf->height;
	start_addr = buf->plane[0].addr;

	switch (buf->pixelformat) {
	case V4L2_PIX_FMT_YUYV:
		suffix = "YUYV";
		end_addr = start_addr + buf->plane[0].sizeimage - 1;
		break;
	case V4L2_PIX_FMT_YVYU:
		suffix = "YVYU";
		end_addr = start_addr + buf->plane[0].sizeimage - 1;
		break;
	case V4L2_PIX_FMT_UYVY:
		suffix = "UYVY";
		end_addr = start_addr + buf->plane[0].sizeimage - 1;
		break;
	case V4L2_PIX_FMT_VYUY:
		suffix = "VYUY";
		end_addr = start_addr + buf->plane[0].sizeimage - 1;
		break;
	case V4L2_PIX_FMT_GREY:
		suffix = "GREY";
		end_addr = start_addr + buf->plane[0].sizeimage - 1;
		break;
	case V4L2_PIX_FMT_RGB24:
		suffix = "RGB24";
		end_addr = start_addr + buf->plane[0].sizeimage - 1;
		break;
	case V4L2_PIX_FMT_RGB565:
		suffix = "RGB565";
		end_addr = start_addr + buf->plane[0].sizeimage - 1;
		break;
	case V4L2_PIX_FMT_ARGB32:
		suffix = "RGB32";
		end_addr = start_addr + buf->plane[0].sizeimage - 1;
		break;
	case V4L2_PIX_FMT_NV12:
	case V4L2_PIX_FMT_NV12M:
		suffix = "NV12";
		end_addr = start_addr + buf->plane[0].sizeimage + buf->plane[1].sizeimage - 1;
		break;
	case V4L2_PIX_FMT_NV21:
	case V4L2_PIX_FMT_NV21M:
		suffix = "NV21";
		end_addr = start_addr + buf->plane[0].sizeimage + buf->plane[1].sizeimage - 1;
		break;
	case V4L2_PIX_FMT_YUV420M:
		suffix = "I420";
		end_addr = start_addr + buf->plane[0].sizeimage + buf->plane[1].sizeimage + buf->plane[1].sizeimage - 1;
		break;
	case V4L2_PIX_FMT_YVU420M:
		suffix = "YV12";
		end_addr = start_addr + buf->plane[0].sizeimage + buf->plane[1].sizeimage + buf->plane[1].sizeimage - 1;
		break;
	default:
		dprint("%s: input format not support: %d", __func__, buf->pixelformat);
		rc = -1;
	}

	bbu_printf("data.save.binary buf_%d_%dx%d.%s 0x%08x--0x%08x\r\n", buf->index, width, height, suffix, start_addr, end_addr);
	return rc;
}

/*========================buf func================================*/
