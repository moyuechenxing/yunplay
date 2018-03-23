#pragma once

#include <stdio.h>

#include"dpipe.h"

#define __STDC_CONSTANT_MACROS  
#define VIDEO_FPS 30


extern "C"
{
#include "libavutil/opt.h"  
#include "libavcodec/avcodec.h"  
#include "libavformat/avformat.h"  
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
};

extern dpipe_t* dpipe;
extern dpipe_t* dpipe2;
extern int pb;
extern int pc;

int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index);
int play(int width,int height);
int play();
int wa();
int wb();
int tplay();
int playm();