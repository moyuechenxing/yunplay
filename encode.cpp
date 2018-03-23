#include "encode.h"
#include <map> 
#include <list>
#include "win_t.h"
using namespace std;

static bool sync_reset = true;
static pthread_mutex_t syncmutex = PTHREAD_MUTEX_INITIALIZER;
static struct timeval synctv;


static int pktqueue_initqsize = -1;
static int pktqueue_initchannels = -1;
static encoder_packet_queue_t pktqueue[VIDEO_SOURCE_CHANNEL_MAX + 1];
static list<encoder_packet_t> pktlist[VIDEO_SOURCE_CHANNEL_MAX + 1];
static map<qcallback_t, qcallback_t>queue_cb[VIDEO_SOURCE_CHANNEL_MAX + 1];






int encoder_send_packet(const char * prefix, int channelId, AVPacket * pkt, int64_t encoderPts, timeval * ptv)
{
	return encoder_pktqueue_append(channelId, pkt, encoderPts, ptv);
}

int encoder_pktqueue_init(int channels, int qsize)
{
	int i;
	for (i = 0; i < channels; i++) {
		if (pktqueue[i].buf != NULL)
			free(pktqueue[i].buf);
		//
		ZeroMemory(&pktqueue[i], sizeof(encoder_packet_queue_t));
		pthread_mutex_init(&pktqueue[i].mutex, NULL);
		if ((pktqueue[i].buf = (char *)malloc(qsize)) == NULL) {
			exit(-1);
		}
		pktqueue[i].bufsize = qsize;
		pktqueue[i].datasize = 0;
		pktqueue[i].head = 0;
		pktqueue[i].tail = 0;
		pktlist[i].clear();
	}
	pktqueue_initqsize = qsize;
	pktqueue_initchannels = channels;
	return 0;

}

int encoder_pktqueue_reset()
{
	int i;
	if(pktqueue_initchannels <= 0)
		return -1;
	for(i = 0; i < pktqueue_initchannels; i++) {
		encoder_pktqueue_reset_channel(i);
	}
	return 0;
}

int encoder_pktqueue_reset_channel(int channelId)
{
	pthread_mutex_lock(&pktqueue[channelId].mutex);
	pktlist[channelId].clear();
	pktqueue[channelId].head = pktqueue[channelId].tail = 0;
	pktqueue[channelId].datasize = 0;
	pktqueue[channelId].bufsize = pktqueue_initqsize;
	pthread_mutex_unlock(&pktqueue[channelId].mutex);
	return 0;
}

int encoder_pktqueue_size(int channelId)
{
	return pktqueue[channelId].datasize;
	return 0;
}

int encoder_pktqueue_append(int channelId, AVPacket * pkt, int64_t encoderPts, timeval * ptv)
{
	encoder_packet_queue_t *q = &pktqueue[channelId];
	encoder_packet_t qp;
	map<qcallback_t, qcallback_t>::iterator mi;
	int padding = 0;
	pthread_mutex_lock(&q->mutex);
size_check:
	if (q->datasize + pkt->size > q->bufsize) {
		pthread_mutex_unlock(&q->mutex);
		return -1;
	}
	if (q->bufsize - q->tail < pkt->size) {
		if (pktlist[channelId].size() == 0) {
			q->datasize = q->tail = q->head = 0;
		}
		else {
			padding = q->bufsize - q->tail;
			pktlist[channelId].back().padding = padding;
			q->datasize += padding;
			q->tail = 0;
		}
		goto size_check;
	}
	CopyMemory(q->buf + q->tail, pkt->data, pkt->size);

	qp.data = q->buf + q->tail;
	qp.size = pkt->size;
	qp.pts_int64 = pkt->pts;
	if (ptv != NULL) {
		qp.pts_tv = *ptv;
	}
	else {
		gettimeofday(&qp.pts_tv, NULL);
	}
	//qp.pos = q->tail;
	qp.padding = 0;
	//
	q->tail += pkt->size;
	q->datasize += pkt->size;
	pktlist[channelId].push_back(qp);
	//
	if (q->tail == q->bufsize)
		q->tail = 0;
	//
	pthread_mutex_unlock(&q->mutex);
	// notify client
	for (mi = queue_cb[channelId].begin(); mi != queue_cb[channelId].end(); mi++) {
		mi->second(channelId);
	}

	return 0;
}

char * encoder_pktqueue_front(int channelId, encoder_packet_t * pkt)
{
	encoder_packet_queue_t *q = &pktqueue[channelId];
	pthread_mutex_lock(&q->mutex);
	if (pktlist[channelId].size() == 0) {
		pthread_mutex_unlock(&q->mutex);
		return NULL;
	}
	*pkt = pktlist[channelId].front();
	pthread_mutex_unlock(&q->mutex);
	return pkt->data;
}

void encoder_pktqueue_split_packet(int channelId, char * offset)
{
	encoder_packet_queue_t *q = &pktqueue[channelId];
	encoder_packet_t *pkt, newpkt;
	pthread_mutex_lock(&q->mutex);
	// has packet?
	if (pktlist[channelId].size() == 0) {
		pthread_mutex_unlock(&q->mutex);
		return;
	}
		
	pkt = &pktlist[channelId].front();
	// offset must be in the middle
	if (offset <= pkt->data || offset >= pkt->data + pkt->size) {
		pthread_mutex_unlock(&q->mutex);
		return;
	}
		
	// split the packet: the new one
	newpkt = *pkt;
	newpkt.size = offset - pkt->data;
	newpkt.padding = 0;
	//
	pkt->data = offset;
	pkt->size -= newpkt.size;
	//
	pktlist[channelId].push_front(newpkt);
	//
	pthread_mutex_unlock(&q->mutex);
	return;
	
}

void encoder_pktqueue_pop_front(int channelId)
{
	encoder_packet_queue_t *q = &pktqueue[channelId];
	encoder_packet_t qp;
	pthread_mutex_lock(&q->mutex);
	if (pktlist[channelId].size() == 0) {
		pthread_mutex_unlock(&q->mutex);
		return;
	}
	qp = pktlist[channelId].front();
	pktlist[channelId].pop_front();
	// update the packet queue
	q->head += qp.size;
	q->head += qp.padding;
	q->datasize -= qp.size;
	q->datasize -= qp.padding;
	if (q->head == q->bufsize) {
		q->head = 0;
	}
	if (q->head == q->tail) {
		q->head = q->tail = 0;
	}
	//
	pthread_mutex_unlock(&q->mutex);
	return;
}

int encoder_pktqueue_register_callback(int channelId, qcallback_t cb)
{
	queue_cb[channelId][cb] = cb;
	return 0;
}

int encoder_pktqueue_unregister_callback(int channelId, qcallback_t cb)
{
	queue_cb[channelId].erase(cb);
	return 0;
}



int	// XXX: need to be int64_t ?
pts_sync(int samplerate) {
	struct timeval tv;
	long long us;
	int ret;
	//
	pthread_mutex_lock(&syncmutex);
	if (sync_reset) {
		gettimeofday(&synctv, NULL);
		sync_reset = false;
		pthread_mutex_unlock(&syncmutex);
		return 0;
	}
	gettimeofday(&tv, NULL);
	us = tvdiff_us(&tv, &synctv);
	pthread_mutex_unlock(&syncmutex);
	ret = (int)(0.000001 * us * samplerate);
	return ret > 0 ? ret : 0;
}


// encoder pts to ptv mapping function
#define	MAX_PTS_QUEUE	8
static list<encoder_pts_t> pts_queue[MAX_PTS_QUEUE];	// up to 8 queues

int
encoder_pts_clear(unsigned queueid) {
	if (queueid >= MAX_PTS_QUEUE)
		return -1;
	pts_queue[queueid].clear();
	return 0;
}

int
encoder_pts_put(unsigned queueid, long long pts, struct timeval *ptv) {
	encoder_pts_t p;
	if (queueid >= MAX_PTS_QUEUE)
		return -1;
	p.pts = pts;
	p.ptv = *ptv;
	pts_queue[queueid].push_back(p);
	return 0;
}

struct timeval *
	encoder_ptv_get(unsigned queueid, long long pts, struct timeval *ptv, int interpolation) {
	if (ptv == NULL)
		return NULL;
	if (queueid >= MAX_PTS_QUEUE)
		return NULL;
	while (pts_queue[queueid].size() > 0) {
		if (pts > pts_queue[queueid].front().pts) {
			pts_queue[queueid].pop_front();
			continue;
		}
		if (pts_queue[queueid].front().pts == pts) {
			*ptv = pts_queue[queueid].front().ptv;
			pts_queue[queueid].pop_front();
			return ptv;
		}
		if (interpolation > 0) {
			long long delta_ts, delta_tv;
			delta_ts = pts_queue[queueid].front().pts - pts;
			delta_tv = (long long)(1.0 * delta_ts / interpolation);
			*ptv = pts_queue[queueid].front().ptv;
			ptv->tv_sec -= (delta_tv / 1000000LL);
			delta_tv %= 1000000LL;
			if (ptv->tv_usec < delta_tv) {
				ptv->tv_sec--;
				ptv->tv_usec += 1000000LL;
			}
			ptv->tv_usec -= delta_tv;
			return ptv;
		}
		break;
	}

	return NULL;
}