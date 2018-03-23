#pragma once

#ifndef _H264_LIVE_VIDIO_SERVER_MEDIA_SUBSESSION_HH
#define _H264_LIVE_VIDIO_SERVER_MEDIA_SUBSESSION_HH

#ifndef _FILE_SERVER_MEDIA_SUBSESSION_HH
#include "H264VideoFileServerMediaSubsession.hh"
#endif

class H264LiveVideoServerMediaSubsession : public OnDemandServerMediaSubsession {
public:
	static H264LiveVideoServerMediaSubsession * createNew(UsageEnvironment &env,
		int cid,
		portNumBits initialPortNum = 6970,
		Boolean multiplexRTCPWithRTP = False);
protected:
	H264LiveVideoServerMediaSubsession(UsageEnvironment &env,
		int cid, /* channel Id */
		portNumBits initialPortNum = 6970,
		Boolean multiplexRTCPWithRTP = False);
	virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
		unsigned& estBitrate);
	// "estBitrate" is the stream's estimated bitrate, in kbps
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
		unsigned char rtpPayloadTypeIfDynamic,
		FramedSource* inputSource);
private:
	int channelId;

};
#endif