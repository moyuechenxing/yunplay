#include "liveMedia.hh"  
#include "BasicUsageEnvironment.hh"  
#include "H264LiveVideoServerMediaSubssion.hh"
#include "encode.h"
#include "Qossink.h"
#include "win_t.h"
#include "live555server.h"


static UsageEnvironment* env = NULL;

void *
liveserver_main(void *arg) {
	int cid;
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	UserAuthenticationDatabase* authDB = NULL;
	RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554, authDB);
	if (rtspServer == NULL) {
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}

	encoder_pktqueue_init(VIDEO_SOURCE_CHANNEL_MAX + 1, 3 * 1024 * 1024/*3MB*/);

	ServerMediaSession* sms = ServerMediaSession::createNew(*env, "game", "game", "gameserver");

	qos_server_init();


	cid = 0;

	sms->addSubsession(H264LiveVideoServerMediaSubsession::createNew(*env,cid));
	rtspServer->addServerMediaSession(sms);

	if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
		*env << "(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling, or for HTTP live streaming (for indexed Transport Stream files only).)\n";
	}
	else {
		*env << "(RTSP-over-HTTP tunneling is not available.)\n";
	}

	qos_server_start();

	env->taskScheduler().doEventLoop();

	qos_server_stop();
	qos_server_deinit();
}



void *
liveserver_taskscheduler() {
	if(env == NULL)
		return NULL;
	return &env->taskScheduler();
}

int
qos_server_init() {
	if (env == NULL) {
		return -1;
	}
	sinkmap.clear();
	return 0;
}

int
qos_server_start() {
	if (env == NULL)
		return -1;
	gettimeofday(&qos_tv, NULL);
	qos_started = 1;
	qos_server_schedule();
	return 0;
}

int
qos_server_stop() {
	qos_started = 0;
	return 0;
}

int
qos_server_deinit() {
	if (env != NULL) {
		env->taskScheduler().unscheduleDelayedTask(qos_task);
	}
	qos_task = NULL;
	sinkmap.clear();
	return 0;
}

int
qos_server_add_sink(const char *prefix, RTPSink *rtpsink) {
	std::map<unsigned/*SSRC*/, qos_server_record_t> x;
	sinkmap[rtpsink] = x;
	return 0;
}

static void
qos_server_schedule() {
	struct timeval now, timeout;
	if(qos_started == 0)
		return;
	timeout.tv_sec = qos_tv.tv_sec;
	timeout.tv_usec = qos_tv.tv_usec + QOS_SERVER_CHECK_INTERVAL_MS * 1000;
	timeout.tv_sec += (timeout.tv_usec / 1000000);
	timeout.tv_usec %= 1000000;
	gettimeofday(&now, NULL);
	qos_task = env->taskScheduler().scheduleDelayedTask(
			tvdiff_us(&timeout, &now), (TaskFunc*) qos_server_report, NULL);
	return;
}

static void
qos_server_report(void *clientData) {
	struct timeval now;
	std::map<RTPSink*, std::map<unsigned, qos_server_record_t> >::iterator mi;
	//
	gettimeofday(&now, NULL);
	for (mi = sinkmap.begin(); mi != sinkmap.end(); mi++) {
		RTPTransmissionStatsDB& db = mi->first->transmissionStatsDB();
		RTPTransmissionStatsDB::Iterator statsIter(db);
		RTPTransmissionStats *stats = NULL;
		while ((stats = statsIter.next()) != NULL) {
			unsigned ssrc = stats->SSRC();
			std::map<unsigned, qos_server_record_t>::iterator mj;
			unsigned long long pkts_lost, d_pkt_lost;
			unsigned long long pkts_sent, d_pkt_sent;
			unsigned long long bytes_sent, d_byte_sent;
			unsigned pkts_sent_hi, pkts_sent_lo;
			unsigned bytes_sent_hi, bytes_sent_lo;
			long long elapsed;
			//
			if ((mj = mi->second.find(ssrc)) == mi->second.end()) {
				qos_server_record_t qr;
				ZeroMemory(&qr, sizeof(qr));
				qr.timestamp = now;
				mi->second[ssrc] = qr;
				continue;
			}
			//
			elapsed = tvdiff_us(&now, &mj->second.timestamp);
			if (elapsed < QOS_SERVER_REPORT_INTERVAL_MS * 1000)
				continue;
			mj->second.timestamp = now;
			//
			pkts_lost = stats->totNumPacketsLost();
			stats->getTotalPacketCount(pkts_sent_hi, pkts_sent_lo);
			stats->getTotalOctetCount(bytes_sent_hi, bytes_sent_lo);
			pkts_sent = pkts_sent_hi;
			pkts_sent = (pkts_sent << 32) | pkts_sent_lo;
			bytes_sent = bytes_sent_hi;
			bytes_sent = (bytes_sent << 32) | bytes_sent_lo;
			// delta
			d_pkt_lost = pkts_lost - mj->second.pkts_lost;
			d_pkt_sent = pkts_sent - mj->second.pkts_sent;
			d_byte_sent = bytes_sent - mj->second.bytes_sent;
			//
			if (d_pkt_lost > d_pkt_sent) {
				*env << "qos: invalid packet loss count detected (%d)\n", d_pkt_lost;
				d_pkt_lost = 0;
			}
			// nothing updated?
			if (d_pkt_lost == 0 && d_pkt_sent == 0 && d_byte_sent == 0)
				continue;
			// report
			*env<<"%s-report: %lldKB sent; pkt-loss=%lld/%lld,%.2f%%; bitrate=%.0fKbps; rtt=%u (%.3fms); jitter=%u (freq=%uHz)\n",
				mi->first->rtpPayloadFormatName(),
				d_byte_sent / 1024,
				d_pkt_lost, d_pkt_sent, 100.0*d_pkt_lost / d_pkt_sent,
				(7812.5/*8000000.0/1024*/)*d_byte_sent / elapsed,
				stats->roundTripDelay(),
				1000.0 * stats->roundTripDelay() / 65536,
				stats->jitter(),
				mi->first->rtpTimestampFrequency();
			//
			mj->second.pkts_lost = pkts_lost;
			mj->second.pkts_sent = pkts_sent;
			mj->second.bytes_sent = bytes_sent;
		}
	}
	// schedule next qos
	qos_tv = now;
	qos_server_schedule();
	return;
}

//long long
//tvdiff_us(struct timeval *tv1, struct timeval *tv2) {
//	struct timeval delta;
//	delta.tv_sec = tv1->tv_sec - tv2->tv_sec;
//	delta.tv_usec = tv1->tv_usec - tv2->tv_usec;
//	if (delta.tv_usec < 0) {
//		delta.tv_sec--;
//		delta.tv_usec += 1000000;
//	}
//	return 1000000LL * delta.tv_sec + delta.tv_usec;
//}