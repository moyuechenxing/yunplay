#pragma once
#ifndef HAVE_STRUCT_TIMESPEC
#define HAVE_STRUCT_TIMESPEC
#endif // !


#include<stdio.h>
#include<pthread.h>
#include <winsock2.h>


typedef struct dpipe_buffer_s {
	long long imgpts;
	struct timeval timestamp;
	void *pointer;
	struct dpipe_buffer_s *next;
}dpipe_buffer_t;

typedef struct dpipe_s {
	pthread_mutex_t cond_mutex;
	pthread_cond_t cond;

	pthread_mutex_t io_mutex;

	dpipe_buffer_t *in;
	dpipe_buffer_t *out;
	dpipe_buffer_t *out_tail;

	int in_count;
	int out_count;
}dpipe_t;

dpipe_t *	dpipe_create(int nframe, int maxframesize);
int		dpipe_destroy(dpipe_t *dpipe);
dpipe_buffer_t *	dpipe_get(dpipe_t *dpipe);
void		dpipe_put(dpipe_t *dpipe, dpipe_buffer_t *buffer);
dpipe_buffer_t *	dpipe_load(dpipe_t *dpipe, const struct timespec *abstime);
dpipe_buffer_t *	dpipe_load_nowait(dpipe_t *dpipe);
void	dpipe_store(dpipe_t *dpipe, dpipe_buffer_t *buffer);