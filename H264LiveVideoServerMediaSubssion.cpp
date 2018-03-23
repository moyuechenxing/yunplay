#include "H264LiveVideoServerMediaSubssion.hh"
#include "H264VideoStreamDiscreteFramer.hh"
#include "H264FramedLiveSource.hh"
#include "vencode.h"
#include "Qossink.h"


H264LiveVideoServerMediaSubsession * H264LiveVideoServerMediaSubsession::createNew(UsageEnvironment & env, int cid, portNumBits initialPortNum, Boolean multiplexRTCPWithRTP)
{
	return new H264LiveVideoServerMediaSubsession(env,cid,initialPortNum,multiplexRTCPWithRTP);
}

H264LiveVideoServerMediaSubsession::H264LiveVideoServerMediaSubsession(UsageEnvironment & env, int cid, portNumBits initialPortNum, Boolean multiplexRTCPWithRTP):OnDemandServerMediaSubsession(env,True,initialPortNum,multiplexRTCPWithRTP)
{
	this->channelId = cid;
}

FramedSource * H264LiveVideoServerMediaSubsession::createNewStreamSource(unsigned clientSessionId, unsigned & estBitrate)
{
	FramedSource *result = NULL;
	//struct RTSPConf *rtspconf = rtspconf_global();

	estBitrate = 3000000 / 1000; /* Kbps */
	OutPacketBuffer::increaseMaxSizeTo(8000000);
	result = H264FramedLiveSource::createNew(envir(), this->channelId);
	do {
		result = H264VideoStreamDiscreteFramer::createNew(envir(), result);
	} while (0);
	return result;
}

RTPSink * H264LiveVideoServerMediaSubsession::createNewRTPSink(Groupsock * rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource * inputSource)
{
	RTPSink	*result = NULL;
	//struct RTSPConf *rtspconf = rtspconf_global();
	int err;

	//H.264
	unsigned profile_level_id = 0;
	u_int8_t SPS[256]; int SPSSize = 0;
	u_int8_t PPS[256]; int PPSSize = 0;
	ga_ioctl_buffer_t mb;
	mb.id = this->channelId;
	mb.ptr = SPS;
	mb.size = sizeof(SPS);
	err = v_ioctl(IOCTL_GETSPS,sizeof(mb),&mb);
	SPSSize = mb.size;

	mb.id = this->channelId;
	mb.ptr = PPS;
	mb.size = sizeof(PPS);
	err = v_ioctl(IOCTL_GETPPS, sizeof(mb), &mb);
	PPSSize = mb.size;

	if (SPSSize >= 1/*'profile_level_id' offset within SPS*/ + 3/*num bytes needed*/) {
		profile_level_id = (SPS[1] << 16) | (SPS[2] << 8) | SPS[3];
	}
	result = QoSH264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic,
		SPS, SPSSize, PPS, PPSSize/*, profile_level_id*/);
	return result;
}
