#ifndef __BUF_MGR_H__
#define __BUF_MGR_H__

/* aviliable range */
#define SQU_VALID_ADDR_S 0xD1200000
#define SQU_VALID_ADDR_E 0xD1300000

#define SET_BIT(reg,bit)    ((reg) |= (1<<(bit)))
#define CLR_BIT(reg,bit)    ((reg) &= ~(1<<(bit)))

/*  Four-character-code (FOURCC) */
#define v4l2_fourcc(a, b, c, d)\
	((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))
//0x0 yuv422
#define V4L2_PIX_FMT_YUYV    v4l2_fourcc('Y', 'U', 'Y', 'V') /* 16  YUV 4:2:2     */
#define V4L2_PIX_FMT_YVYU    v4l2_fourcc('Y', 'V', 'Y', 'U') /* 16 YVU 4:2:2 */
#define V4L2_PIX_FMT_UYVY    v4l2_fourcc('U', 'Y', 'V', 'Y') /* 16  YUV 4:2:2     */
#define V4L2_PIX_FMT_VYUY    v4l2_fourcc('V', 'Y', 'U', 'Y') /* 16  YUV 4:2:2     */
//0x1 yuv400
#define V4L2_PIX_FMT_GREY    v4l2_fourcc('G', 'R', 'E', 'Y') /*  8  Greyscale     */
//0x2 yuv420 2planar
/* two planes -- one Y, one Cr + Cb interleaved  */
#define V4L2_PIX_FMT_NV12    v4l2_fourcc('N', 'V', '1', '2') /* 12  Y/CbCr 4:2:0  */
#define V4L2_PIX_FMT_NV21    v4l2_fourcc('N', 'V', '2', '1') /* 12  Y/CrCb 4:2:0  */
/* two non contiguous planes - one Y, one Cr + Cb interleaved  */
#define V4L2_PIX_FMT_NV12M   v4l2_fourcc('N', 'M', '1', '2') /* 12  Y/CbCr 4:2:0  */
#define V4L2_PIX_FMT_NV21M   v4l2_fourcc('N', 'M', '2', '1') /* 21  Y/CrCb 4:2:0  */
//0x3 yuv420 3planar
#define V4L2_PIX_FMT_YUV420M v4l2_fourcc('Y', 'M', '1', '2') /* 12  YUV420 planar */
#define V4L2_PIX_FMT_YVU420M v4l2_fourcc('Y', 'M', '2', '1') /* 12  YVU420 planar */
//0x4 rgb888
#define V4L2_PIX_FMT_RGB24   v4l2_fourcc('R', 'G', 'B', '3') /* 24  RGB-8-8-8     */
//0x5 rgb565
#define V4L2_PIX_FMT_RGB565  v4l2_fourcc('R', 'G', 'B', 'P') /* 16  RGB-5-6-5     */
//0x6 rgba/argb
#define V4L2_PIX_FMT_ARGB32  v4l2_fourcc('B', 'A', '2', '4') /* 32  ARGB-8-8-8-8  */

#define VIDEO_MAX_PLANES 3

struct v4l2_plane {
	uint32_t	addr;
	uint32_t	sizeimage;
	uint32_t	pitch;
	uint32_t	bytesperline;
	uint32_t	usedperline;
};

struct offline_buf {
	/* buf */
	uint32_t	index;
	uint32_t	num_planes;
	/* format */
	uint32_t	width;
	uint32_t	height;
	uint32_t	stride;
	uint32_t	pitch;
	uint32_t	bpp;
	uint32_t	pixelformat;
	struct v4l2_plane plane[VIDEO_MAX_PLANES];
};

struct offline_buf *buf_construct(uint32_t width, uint32_t height, uint32_t pixelformat, uint32_t stride);

int buf_destruct(struct offline_buf *buf);

int buf_set_base_addr(struct offline_buf *buf, uint32_t base);

int buf_fill_by_img(struct offline_buf *buf);

int buf_info_dump(struct offline_buf *buf);
#endif
