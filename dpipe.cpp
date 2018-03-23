#include"dpipe.h"
#include <windows.h>

//static pthread_mutex_t dpipemap_mutex = PTHREAD_MUTEX_INITIALIZER;
//static map<string, dpipe_t*> dpipemap;

dpipe_t *
dpipe_create(int nframe, int maxframesize) {
	int i;
	dpipe_t *dpipe;
	if ((dpipe = (dpipe_t*)malloc(sizeof(dpipe_t))) == NULL)
		return NULL;
	ZeroMemory(dpipe, sizeof(dpipe));
	pthread_mutex_init(&dpipe->cond_mutex, NULL);
	pthread_cond_init(&dpipe->cond, NULL);
	pthread_mutex_init(&dpipe->io_mutex, NULL);
	dpipe->in = NULL;
	dpipe->in_count = 0;
	for (i = 0; i < nframe; i++) {
		dpipe_buffer_t* dbuffer;
		if ((dbuffer = (dpipe_buffer_t*)malloc(sizeof(dpipe_buffer_t))) == NULL)
			return NULL;
		/*if ((dbuffer->internal = malloc(maxframesize)) == NULL)
			return NULL;
		dbuffer->offset = 16 - (((unsigned)dbuffer->internal) & 0x0f);*/
		/*dbuffer->pointer = (void*)(((char*)dbuffer->internal) + dbuffer->offset);*/
		dbuffer->pointer = (void *)malloc(maxframesize);
		dbuffer->next = dpipe->in;
		dpipe->in = dbuffer;
		dpipe->in_count++;
	}
	dpipe->out = NULL;
	dpipe->out_tail = NULL;
	dpipe->out_count = 0;
	dpipe->in_count = nframe;
	return dpipe;
}

int
dpipe_destroy(dpipe_t *dpipe) {
	dpipe_buffer_t *vbuf, *next;
	if (dpipe == NULL)
		return 0;
	pthread_mutex_destroy(&dpipe->cond_mutex);
	pthread_cond_destroy(&dpipe->cond);
	pthread_mutex_destroy(&dpipe->io_mutex);
	for (vbuf = dpipe->in; vbuf != NULL; vbuf = next) {
		next = vbuf->next;
		free(vbuf);
	}
	for (vbuf = dpipe->out; vbuf != NULL; vbuf = next) {
		next = vbuf->next;
		free(vbuf);
	}
	free(dpipe);
	return 0;
}

dpipe_buffer_t *
dpipe_get(dpipe_t *dpipe) {
	dpipe_buffer_t *vbuf = NULL;
	pthread_mutex_lock(&dpipe->io_mutex);

	if (dpipe->in != NULL) {
		// quick path: has available frame buffers
		if ((vbuf = dpipe->in) != NULL) {
			dpipe->in = vbuf->next;
			vbuf->next = NULL;
			dpipe->in_count--;
		}
	}
	else {
		// no available buffers: drop the eldest frame buffer from output pool
		if ((vbuf = dpipe->out) != NULL) {
			dpipe->out = vbuf->next;
			vbuf->next = NULL;
			if (dpipe->out == NULL) {
				dpipe->out_tail = NULL;
			}
			dpipe->out_count--;
		}
	}
	pthread_mutex_unlock(&dpipe->io_mutex);
	//
	return vbuf;
}

void
dpipe_put(dpipe_t *dpipe, dpipe_buffer_t *buffer) {
	pthread_mutex_lock(&dpipe->io_mutex);
	buffer->next = dpipe->in;
	dpipe->in = buffer;
	dpipe->in_count++;
	pthread_mutex_unlock(&dpipe->io_mutex);
	return;
}

dpipe_buffer_t *
dpipe_load(dpipe_t *dpipe, const struct timespec *abstime) {
	dpipe_buffer_t *vbuf = NULL;
	int failed = 0;
	pthread_mutex_lock(&dpipe->io_mutex);
again:
	if (dpipe->out != NULL) {
		vbuf = dpipe->out;
		dpipe->out = vbuf->next;
		vbuf->next = NULL;
		if (dpipe->out == NULL)
			dpipe->out_tail = NULL;
		dpipe->out_count--;
	}
	else if (abstime == NULL) {
		// no frame buffered
		pthread_cond_wait(&dpipe->cond, &dpipe->io_mutex);
		goto again;
	}
	else if (failed == 0) {
		pthread_cond_timedwait(&dpipe->cond, &dpipe->io_mutex, abstime);
		failed = 1;
		goto again;
	}
	pthread_mutex_unlock(&dpipe->io_mutex);
	//
	return vbuf;
}

dpipe_buffer_t *
dpipe_load_nowait(dpipe_t *dpipe) {
	dpipe_buffer_t *vbuf = NULL;
	//
	pthread_mutex_lock(&dpipe->io_mutex);
	if (dpipe->out != NULL) {
		vbuf = dpipe->out;
		dpipe->out = vbuf->next;
		vbuf->next = NULL;
		if (dpipe->out == NULL)
			dpipe->out_tail = NULL;
		dpipe->out_count--;
	}
	pthread_mutex_unlock(&dpipe->io_mutex);
	//
	return vbuf;
}

void
dpipe_store(dpipe_t *dpipe, dpipe_buffer_t *buffer) {
	pthread_mutex_lock(&dpipe->io_mutex);
	// put at the end
	if (dpipe->out_tail != NULL) {
		dpipe->out_tail->next = buffer;
		dpipe->out_tail = buffer;
	}
	else {
		dpipe->out = dpipe->out_tail = buffer;
	}
	buffer->next = NULL;
	dpipe->out_count++;
	//
	pthread_mutex_unlock(&dpipe->io_mutex);
	pthread_cond_signal(&dpipe->cond);
	return;
}