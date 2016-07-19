#include <fcntl.h>
#include <linux/kd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/vt.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

/*
 * for simplicity, I ignore the errno check when
 * deal with ioctls. since cirrus vga is currently
 * driven by modesetting driver, it's free to create
 * dumb bo to mangle with any pix value. Also, this
 * code assumes there's only one connector/output for
 * display, then crtc<->encoder<-->connector relationship
 * can be directly simple!
 *
 * after we get landed in text mode with screen size fb,
 * I use a fontdata of 8x8 to map char. Most work is bit
 * twist with byte unit inside the fb.
 */

extern unsigned char fontdata_8x8[2048];
extern unsigned char fontdata_8x16[4096];

void draw_8x8_char(uint32_t x, uint32_t y, uint8_t *fb_virt, uint32_t pitch, uint8_t chars)
{
	int i, j;
	uint8_t *tmp;
	uint32_t index = chars * 8;

	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			*((uint32_t *)(fb_virt + ( j + y ) * pitch + (x + i) * 4)) = ((fontdata_8x8[index + j] >> (7 - i)) & 0x1) ? 0 : 0xFFFFFF;
		}
	}
}

void draw_8x16_char(uint32_t x, uint32_t y, uint8_t *fb_virt, uint32_t pitch, uint8_t chars)
{
	int i, j;
	uint8_t *tmp;
	uint32_t index = chars * 16;

	for (j = 0; j < 16; j++) {
		for (i = 0; i < 8; i++) {
			*((uint32_t *)(fb_virt + ( j + y ) * pitch + (x + i) * 4)) = ((fontdata_8x16[index + j] >> (7 - i)) & 0x1) ? 0 : 0xFFFFFF;
		}
	}
}

int main(int argc, char **argv)
{
	int drmfd;
	int vtfd;
	unsigned int i, j;
	drmModeRes *res = NULL;
	drmModeEncoder *enc = NULL;
	drmModeConnector *conn = NULL;
	uint32_t fb_id, conn_id, crtc_id;
	long int vt_args;
	struct drm_mode_create_dumb fbo_create_args = {0};
	struct drm_mode_map_dumb fbo_map_args = {0};
	struct drm_mode_destroy_dumb fbo_close_args = {0};
	uint32_t gem_handle;
	uint8_t *fb_virt = NULL; /* start address of dumb fb */
	uint32_t fb_pitch;
	uint32_t fb_size;
	drmModeModeInfo curr_mode; /* store current mode infomation */

	vtfd = open("/dev/tty1", O_RDWR | O_CLOEXEC);

	/* trigger a vt switch to /dev/tty1 and wait for available */
	ioctl(vtfd, VT_ACTIVATE, 1);
	ioctl(vtfd, VT_WAITACTIVE, 1);

	ioctl(vtfd, KDGETMODE, &vt_args);
	printf("switch to %s mode on tty1\n", vt_args == KD_TEXT ? "TEXT" : "GFX");

	/* from now on, we can do pixel dance on dumb fb */
	drmfd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);

	res = drmModeGetResources(drmfd);

	for (i = 0; i < res->count_connectors; i++) {

		conn = drmModeGetConnector(drmfd, res->connectors[i]);
		conn_id = conn->connector_id;

		if (conn->connection != DRM_MODE_CONNECTED) {
			printf("conn:[%d] is offline\n", conn->connector_id);
			drmModeFreeConnector(conn);
			continue;
		}

		/* we choose max valid mode */
		memcpy(&curr_mode, &conn->modes[0], sizeof(curr_mode));
		printf("conn:[%d] is online %dx%d@%dHZ\n", conn->connector_id, curr_mode.hdisplay, curr_mode.vdisplay, curr_mode.vrefresh);

		enc = drmModeGetEncoder(drmfd, conn->encoder_id);

		crtc_id = enc->crtc_id;
		drmModeFreeEncoder(enc);

		/* create dumb fb and retrieve aperture */
		fbo_create_args.width = curr_mode.hdisplay;
		fbo_create_args.height = curr_mode.vdisplay;
		fbo_create_args.bpp = 32;

		drmIoctl(drmfd, DRM_IOCTL_MODE_CREATE_DUMB, &fbo_create_args);

		gem_handle = fbo_create_args.handle;
		fb_size = fbo_create_args.size;
		fb_pitch = fbo_create_args.pitch;

		drmModeAddFB(drmfd, curr_mode.hdisplay, curr_mode.vdisplay, 24, 32, fb_pitch, gem_handle, &fb_id);

		fbo_map_args.handle = gem_handle;
		drmIoctl(drmfd, DRM_IOCTL_MODE_MAP_DUMB, &fbo_map_args);

		fb_virt = mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, drmfd, fbo_map_args.offset);

		drmModeFreeConnector(conn);
	}

	drmModeFreeResources(res);

	drmModeSetCrtc(drmfd, crtc_id, fb_id, 0, 0, &conn_id, 1, &curr_mode);

	for (i = 0; i < curr_mode.vdisplay; i++) {
		for (j = 0; j < curr_mode.hdisplay; j++) {
			//*((uint32_t *)(fb_virt + fb_pitch * i + j * 4)) = i * j;
			*((uint32_t *)(fb_virt + fb_pitch * i + j * 4)) = 0xFFFFFF;
		}
	}

	/* say hello */
	draw_8x8_char(10, 10, fb_virt, fb_pitch, 'h');
	draw_8x8_char(18, 10, fb_virt, fb_pitch, 'e');
	draw_8x8_char(26, 10, fb_virt, fb_pitch, 'l');
	draw_8x8_char(34, 10, fb_virt, fb_pitch, 'l');
	draw_8x8_char(42, 10, fb_virt, fb_pitch, 'o');

	draw_8x8_char(50, 10, fb_virt, fb_pitch, ' ');
	draw_8x8_char(58, 10, fb_virt, fb_pitch, 'r');
	draw_8x8_char(66, 10, fb_virt, fb_pitch, 'e');
	draw_8x8_char(74, 10, fb_virt, fb_pitch, 'd');
	draw_8x8_char(82, 10, fb_virt, fb_pitch, 'h');
	draw_8x8_char(90, 10, fb_virt, fb_pitch, 'a');
	draw_8x8_char(98, 10, fb_virt, fb_pitch, 't');
	draw_8x8_char(106, 10, fb_virt, fb_pitch, '!');

	draw_8x16_char(110, 100, fb_virt, fb_pitch, 'h');
	draw_8x16_char(118, 100, fb_virt, fb_pitch, 'e');
	draw_8x16_char(126, 100, fb_virt, fb_pitch, 'l');
	draw_8x16_char(134, 100, fb_virt, fb_pitch, 'l');
	draw_8x16_char(142, 100, fb_virt, fb_pitch, 'o');

	draw_8x16_char(150, 100, fb_virt, fb_pitch, ' ');
	draw_8x16_char(158, 100, fb_virt, fb_pitch, 'r');
	draw_8x16_char(166, 100, fb_virt, fb_pitch, 'e');
	draw_8x16_char(174, 100, fb_virt, fb_pitch, 'd');
	draw_8x16_char(182, 100, fb_virt, fb_pitch, 'h');
	draw_8x16_char(190, 100, fb_virt, fb_pitch, 'a');
	draw_8x16_char(198, 100, fb_virt, fb_pitch, 't');
	draw_8x16_char(206, 100, fb_virt, fb_pitch, '!');

	usleep(5000000);

	/* unwind anything we done */
	munmap(fb_virt, fb_size);

	drmModeRmFB(drmfd, fb_id);

	fbo_close_args.handle = gem_handle;
	drmIoctl(drmfd, DRM_IOCTL_MODE_DESTROY_DUMB, &fbo_close_args);

	close(drmfd);

	/* switch back to /dev/tty7 for convinience */
	ioctl(vtfd, VT_ACTIVATE, 7);
	ioctl(vtfd, VT_WAITACTIVE, 7);

	close(vtfd);
	return 0;
}
