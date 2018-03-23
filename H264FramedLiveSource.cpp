#include "H264FramedLiveSource.hh"
#include "encode.h"
#include "live555server.h"

static H264FramedLiveSource *vLiveSource[VIDEO_SOURCE_CHANNEL_MAX];
static EventTriggerId eventTriggerId[VIDEO_SOURCE_CHANNEL_MAX];
static void signalNewVideoFrameData(int channelId);


//unsigned H264FramedLiveSource::referenceCount = 0;


H264FramedLiveSource * H264FramedLiveSource::createNew(UsageEnvironment & env, int cid)
{
	return new H264FramedLiveSource(env, cid);
}

H264FramedLiveSource::H264FramedLiveSource(UsageEnvironment & env, int cid)
	:FramedSource(env)
{
	/*if (referenceCount == 0) {
		live_server_register_client(this);
	}*/
	remove_startcode = 1;
	this->channelId = cid;
	vLiveSource[cid] = this;
	if (eventTriggerId[cid] == 0) {
		eventTriggerId[cid] = envir().taskScheduler().createEventTrigger(deliverFrame0);
		encoder_pktqueue_register_callback(this->channelId, signalNewVideoFrameData);
	}


}

H264FramedLiveSource::~H264FramedLiveSource()
{
	vLiveSource[this->channelId] = NULL;

	remove_startcode = 0;
	encoder_pktqueue_unregister_callback(this->channelId, signalNewVideoFrameData);
	// Reclaim our 'event trigger'
	envir().taskScheduler().deleteEventTrigger(eventTriggerId[this->channelId]);
	eventTriggerId[this->channelId] = 0;

}

void H264FramedLiveSource::deliverFrame0(void * clientData)
{
	((H264FramedLiveSource*)clientData)->deliverFrame();
}

void H264FramedLiveSource::doGetNextFrame()
{
	if (0) {
		handleClosure(NULL);
		return;
	}
	if (encoder_pktqueue_size(this->channelId) > 0) {
		deliverFrame();
	}
}

void H264FramedLiveSource::deliverFrame()
{
	if (!isCurrentlyAwaitingData()) return;
	encoder_packet_t pkt;
	u_int8_t* newFrameDataStart = NULL; //%%% TO BE WRITTEN %%%
	unsigned newFrameSize = 0; //%%% TO BE WRITTEN %%%

	newFrameDataStart = (u_int8_t*)encoder_pktqueue_front(this->channelId, &pkt);
	if (newFrameDataStart == NULL)
		return;
	newFrameSize = pkt.size;
#ifdef DISCRETE_FRAMER	// special handling for packets with startcode
	if (remove_startcode != 0) {
		if (newFrameDataStart[0] == 0
			&& newFrameDataStart[1] == 0) {
			if (newFrameDataStart[2] == 0
				&& newFrameDataStart[3] == 1) {
				newFrameDataStart += 4;
				newFrameSize -= 4;
			}
			else if (newFrameDataStart[2] == 1) {
				newFrameDataStart += 3;
				newFrameSize -= 3;
			}
		}
	}
#endif
	if (newFrameSize > fMaxSize) {
		fFrameSize = fMaxSize;
#ifdef DISCRETE_FRAMER
		fNumTruncatedBytes = newFrameSize - fMaxSize;
#else		// for regular H264Framer
		encoder_pktqueue_split_packet(this->channelId, (char*)newFrameDataStart + fMaxSize);
#endif
	}
	else {
		fFrameSize = newFrameSize;
	}
	fPresentationTime = pkt.pts_tv;
	memmove(fTo, newFrameDataStart, fFrameSize);

	encoder_pktqueue_pop_front(channelId);
	FramedSource::afterGetting(this);
}

static void
signalNewVideoFrameData(int channelId) {
	TaskScheduler* ourScheduler = (TaskScheduler*)liveserver_taskscheduler(); //%%% TO BE WRITTEN %%%
	H264FramedLiveSource* ourDevice = vLiveSource[channelId]; //%%% TO BE WRITTEN %%%

	if (ourScheduler != NULL) { // sanity check
		ourScheduler->triggerEvent(eventTriggerId[channelId], ourDevice);
	}
}