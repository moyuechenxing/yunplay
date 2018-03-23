#pragma once

#define DISCRETE_FRAMER		/* use discrete framer */

void * liveserver_main(void *arg);

void * liveserver_taskscheduler();

int qos_server_init();

int qos_server_start();

int qos_server_stop();

int qos_server_deinit();

int qos_server_add_sink(const char * prefix, RTPSink * rtpsink);

static void qos_server_schedule();

static void qos_server_report(void * clientData);

