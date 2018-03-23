#pragma once
#include "encode.h"

static int vencode_init();
static unsigned char *
find_startcode(unsigned char *data, unsigned char *end);

static int vencoder_ioctl(int command, int argsize, void *arg);
int v_ioctl(int command, int argsize, void *arg);

int vencode_put(int iid, AVCodecContext * encode);
