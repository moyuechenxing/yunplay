#pragma once

#include <pthread.h>
#include<stdint.h>
#include<WinSock2.h>
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include "libavcodec/avcodec.h"  

#define VIDEO_SOURCE_CHANNEL_MAX 2
enum ioctl_commands {
	IOCTL_NULL = 0,		/**< Not used */
	IOCTL_RECONFIGURE,		/**< Reconfiguration */
	IOCTL_GETSPS = 0x100,	/**< Get SPS: for H.264 and H.265 */
	IOCTL_GETPPS,		/**< Get PPS: for H.264 and H.265 */
	IOCTL_GETVPS,		/**< Get VPS: for H.265 */
	IOCTL_CUSTOM = 0x40000000	/**< For user customization */
};

typedef struct encoder_packet_s {
	char *data;		/**< Pointer to the data buffer */
	unsigned size;		/**< Size of the buffer */
	int64_t pts_int64;	/**< Packet timestamp in a 64-bit integer */
	struct timeval pts_tv;	/**< Packet timestamp in \a timeval structure */
							// internal data structure - do not touch
	int padding;		/**< Padding area: internal used */
}	encoder_packet_t;


typedef struct encoder_packet_queue_s {
	pthread_mutex_t mutex;	/**< Per-queue mutex */
	char *buf;		/**< Pointer to the packet queue buffer */
	int bufsize;		/**< Size of the queue buffer */
	int datasize;		/**< Size of occupied data size */
	int head;		/**< Position of queue head */
	int tail;		/**< Position of queue tail */
}	encoder_packet_queue_t;

typedef struct encoder_pts_s {
	long long pts;
	struct timeval ptv;
}	encoder_pts_t;

typedef struct ga_ioctl_buffer_s {
	int id;
	unsigned char *ptr;	/**< Pointer to the buffer */
	int size;		/**< Size of the buffer */
}	ga_ioctl_buffer_t;

typedef void(*qcallback_t)(int);

int encoder_send_packet(const char *prefix, int channelId, AVPacket *pkt, int64_t encoderPts, struct timeval *ptv);

int encoder_pktqueue_init(int channels, int qsize);
int encoder_pktqueue_reset();
int encoder_pktqueue_reset_channel(int channelId);
int encoder_pktqueue_size(int channelId);
int encoder_pktqueue_append(int channelId, AVPacket *pkt, int64_t encoderPts, struct timeval *ptv);
char * encoder_pktqueue_front(int channelId, encoder_packet_t *pkt);
void encoder_pktqueue_split_packet(int channelId, char *offset);
void encoder_pktqueue_pop_front(int channelId);
int encoder_pktqueue_register_callback(int channelId, qcallback_t cb);
int encoder_pktqueue_unregister_callback(int channelId, qcallback_t cb);

int pts_sync(int samplerate);

int encoder_pts_clear(unsigned queueid);

int encoder_pts_put(unsigned queueid, long long pts, timeval * ptv);

struct timeval * encoder_ptv_get(unsigned queueid, long long pts, timeval * ptv, int interpolation);


