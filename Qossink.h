#pragma once

#include "H264VideoRTPSink.hh"
#include <map>

typedef struct qos_server_record_s {
	unsigned long long pkts_lost;
	unsigned long long pkts_sent;
	unsigned long long bytes_sent;
	struct timeval timestamp;
}	qos_server_record_t;

static std::map<RTPSink*, std::map<unsigned/*SSRC*/, qos_server_record_t> > sinkmap;
static TaskToken qos_task = NULL;
static int qos_started = 0;
static struct timeval qos_tv;

#define	QOS_SERVER_CHECK_INTERVAL_MS	(1 * 1000)	/* check every N seconds */
#define	QOS_SERVER_REPORT_INTERVAL_MS	(30 * 1000)	/* report every N seconds */
#define QOS_SERVER_PREFIX_LEN		64

class QoSH264VideoRTPSink : public H264VideoRTPSink {
public:
	static QoSH264VideoRTPSink*
		createNew(UsageEnvironment& env, Groupsock* RTPgs, unsigned char rtpPayloadFormat);
	static QoSH264VideoRTPSink*
		createNew(UsageEnvironment& env, Groupsock* RTPgs, unsigned char rtpPayloadFormat,
			u_int8_t const* sps, unsigned spsSize, u_int8_t const* pps, unsigned ppsSize);
protected:
	QoSH264VideoRTPSink(UsageEnvironment& env, Groupsock* RTPgs, unsigned char rtpPayloadFormat,
		u_int8_t const* sps = NULL, unsigned spsSize = 0,
		u_int8_t const* pps = NULL, unsigned ppsSize = 0);
	~QoSH264VideoRTPSink();
};