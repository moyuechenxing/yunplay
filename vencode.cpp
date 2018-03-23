#include  <stdio.h>

#include "dpipe.h"
#include "vencode.h"

static char *_sps[VIDEO_SOURCE_CHANNEL_MAX];
static int _spslen[VIDEO_SOURCE_CHANNEL_MAX];
static char *_pps[VIDEO_SOURCE_CHANNEL_MAX];
static int _ppslen[VIDEO_SOURCE_CHANNEL_MAX];
static char *_vps[VIDEO_SOURCE_CHANNEL_MAX];
static int _vpslen[VIDEO_SOURCE_CHANNEL_MAX];

static AVCodecContext *vencoder[VIDEO_SOURCE_CHANNEL_MAX];

#define SOURCE 1

static int vencode_init() {
	int iid;
	for (iid = 0; iid < SOURCE; iid++) {
		_sps[iid] = _pps[iid] = NULL;
		_spslen[iid] = _ppslen[iid] = 0;
	}
}

static unsigned char *
find_startcode(unsigned char *data, unsigned char *end) {
	unsigned char *r;
	for (r = data; r < end - 4; r++) {
		if (r[0] == 0
			&& r[1] == 0
			&& r[2] == 0
			&& r[3] == 1)
			return r;
	}
	return end;
}

static int
h264or5_get_vparam(int type, int channelId, unsigned char *data, int datalen) {
	int ret = -1;
	int p = 0;
	unsigned char *r;
	unsigned char *sps = NULL, *pps = NULL, *vps = NULL;
	int spslen = 0, ppslen = 0, vpslen = 0;
	if (_sps[channelId] != NULL)
		return 0;
	r = find_startcode(data, data + datalen);
	while (r < data + datalen) {
		unsigned char nal_type;
		unsigned char *r1;
		while (0 == (*r++));
		r1 = find_startcode(r, data + datalen);
		if (type == 265) {
			nal_type = ((*r) >> 1) & 0x3f;
			if (nal_type == 32) {		// VPS
				vps = r;
				vpslen = r1 - r;
			}
			else if (nal_type == 33) {	// SPS
				sps = r;
				spslen = r1 - r;
			}
			else if (nal_type == 34) {	// PPS
				pps = r;
				ppslen = r1 - r;
			}
		}
		else {
			// assume default is 264
			nal_type = *r & 0x1f;
			if (nal_type == 7) {		// SPS
				sps = r;
				spslen = r1 - r;
			}
			else if (nal_type == 8) {	// PPS
				pps = r;
				ppslen = r1 - r;
			}
		}
		r = r1;
	}
	if (sps != NULL && pps != NULL) {
		// alloc and copy SPS
		if ((_sps[channelId] = (char*)malloc(spslen)) == NULL)
			goto error_get_h264or5_vparam;
		_spslen[channelId] = spslen;
		CopyMemory(_sps[channelId],sps , spslen);
		// alloc and copy PPS
		if ((_pps[channelId] = (char*)malloc(ppslen)) == NULL) {
			goto error_get_h264or5_vparam;
		}
		_ppslen[channelId] = ppslen;
		CopyMemory(_pps[channelId],pps , ppslen);
		// alloc and copy VPS
		if (vps != NULL) {
			if ((_vps[channelId] = (char*)malloc(vpslen)) == NULL) {
				goto error_get_h264or5_vparam;
			}
			_vpslen[channelId] = vpslen;
			CopyMemory(_vps[channelId],vps , vpslen);
		}
		//
		if (type == 265) {
			if (vps == NULL)
				goto error_get_h264or5_vparam;
		}
		//
		ret = 0;
	}
	return ret;
error_get_h264or5_vparam:
	if (_sps[channelId])	free(_sps[channelId]);
	if (_pps[channelId])	free(_pps[channelId]);
	if (_vps[channelId])	free(_vps[channelId]);
	_sps[channelId] = _pps[channelId] = _vps[channelId] = NULL;
	_spslen[channelId] = _ppslen[channelId] = _vpslen[channelId] = 0;
	return -1;
}

static int
vencoder_ioctl(int command, int argsize, void *arg) {
	int ret = 0;
	ga_ioctl_buffer_t *buf = (ga_ioctl_buffer_t*)arg;
	AVCodecContext *ve = NULL;
	ve = vencoder[buf->id];
	h264or5_get_vparam( 264 , buf->id, ve->extradata, ve->extradata_size);
	if (command == IOCTL_GETPPS) {
		if (buf->size < _spslen[buf->id])
			return 0;
		buf->size = _spslen[buf->id];
		CopyMemory(buf->ptr,_sps[buf->id] , buf->size);
	}
	else if (command == IOCTL_GETPPS) {
		if (buf->size < _ppslen[buf->id])
			return 0;
		buf->size = _ppslen[buf->id];
		CopyMemory( buf->ptr,_pps[buf->id], buf->size);
	}
	else if (command == IOCTL_GETVPS) {
		if (buf->size < _vpslen[buf->id])
			return 0;
		buf->size = _vpslen[buf->id];
		CopyMemory( buf->ptr,_vps[buf->id], buf->size);
	}
}

int v_ioctl(int command, int argsize, void *arg) {
	vencoder_ioctl(command, argsize,&arg);
	return 0;
}

int vencode_put(int iid,AVCodecContext *encode) {
	vencoder[iid] = encode;
	return 0;
}