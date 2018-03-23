#include "Qossink.h"

QoSH264VideoRTPSink * QoSH264VideoRTPSink::createNew(UsageEnvironment & env, Groupsock * RTPgs, unsigned char rtpPayloadFormat)
{
	return new QoSH264VideoRTPSink(env, RTPgs, rtpPayloadFormat);
}

QoSH264VideoRTPSink * QoSH264VideoRTPSink::createNew(UsageEnvironment & env, Groupsock * RTPgs, unsigned char rtpPayloadFormat, u_int8_t const * sps, unsigned spsSize, u_int8_t const * pps, unsigned ppsSize)
{
	return new QoSH264VideoRTPSink(env, RTPgs, rtpPayloadFormat, sps, spsSize, pps, ppsSize);
}

QoSH264VideoRTPSink::QoSH264VideoRTPSink(UsageEnvironment & env, Groupsock * RTPgs, unsigned char rtpPayloadFormat, u_int8_t const * sps, unsigned spsSize, u_int8_t const * pps, unsigned ppsSize)
	: H264VideoRTPSink(env, RTPgs, rtpPayloadFormat, sps, spsSize, pps, ppsSize)
{
	std::map<unsigned/*SSRC*/, qos_server_record_t> x;
	sinkmap[this] = x;
}

QoSH264VideoRTPSink::~QoSH264VideoRTPSink()
{
	sinkmap.erase(this);
}
