#include"ffmpeg.h"
#include<time.h>
#include<stdio.h>
#include<winsock2.h>
#include "win_t.h"
#include "encode.h"
#include "vencode.h"



int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index) {
	int ret;
	int got_frame;
	AVPacket enc_pkt;
	if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities &
		CODEC_CAP_DELAY))
		return 0;
	while (1) {
		enc_pkt.data = NULL;
		enc_pkt.size = 0;
		av_init_packet(&enc_pkt);
		ret = avcodec_encode_video2(fmt_ctx->streams[stream_index]->codec, &enc_pkt,
			NULL, &got_frame);
		av_frame_free(NULL);
		if (ret < 0)
			break;
		if (!got_frame) {
			ret = 0;
			break;
		}
		printf("Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n", enc_pkt.size);
		/* mux encoded frame */
		ret = av_write_frame(fmt_ctx, &enc_pkt);
		if (ret < 0)
			break;
	}
	return ret;
}

//int play(uint8_t * picture_buf)
int play(int width,int height)
{
	AVFormatContext* pFormatCtx;
	AVOutputFormat* fmt;
	AVStream* video_st;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;
	AVPacket pkt;
	AVFrame* pFrame;
	int picture_size;
	int y_size;
	//int framecnt = 0;
	//FILE *in_file = fopen("src01_480x272.yuv", "rb"); //Input raw YUV data   
	//FILE *in_file = fopen("../ds_480x272.yuv", "rb");   //Input raw YUV data  
	int in_w = width, in_h = height;                              //Input data's width and height  
	int framenum = 100;                                   //Frames to encode  
														  //const char* out_file = "src01.h264";              //Output Filepath   
														  //const char* out_file = "src01.ts";  
														  //const char* out_file = "src01.hevc";  
	const char* out_file = "ds.h264";




	av_register_all();
	avcodec_register_all();

	//Method1.  
	pFormatCtx = avformat_alloc_context();
	//Guess Format  
	fmt = av_guess_format(NULL, out_file, NULL);
	pFormatCtx->oformat = fmt;

	//Method 2.  
	//avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);  
	//fmt = pFormatCtx->oformat;  


	//Open output URL  
	if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
		printf("Failed to open output file! \n");
		return -1;
	}

	video_st = avformat_new_stream(pFormatCtx, 0);
	//video_st->time_base.num = 1;   
	//video_st->time_base.den = 25;    

	if (video_st == NULL) {
		return -1;
	}
	//Param that must set  
	pCodecCtx = video_st->codec;
	//pCodecCtx->codec_id =AV_CODEC_ID_HEVC;  
	pCodecCtx->codec_id = fmt->video_codec;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->width = in_w;
	pCodecCtx->height = in_h;
	pCodecCtx->bit_rate = 400000;
	pCodecCtx->gop_size = 250;

	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;

	//H264  
	//pCodecCtx->me_range = 16;  
	//pCodecCtx->max_qdiff = 4;  
	//pCodecCtx->qcompress = 0.6;  
	pCodecCtx->qmin = 10;
	pCodecCtx->qmax = 51;

	//Optional Param  
	pCodecCtx->max_b_frames = 3;

	// Set Option  
	AVDictionary *param = 0;
	//H.264  
	if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
		av_dict_set(&param, "preset", "slow", 0);
		av_dict_set(&param, "tune", "zerolatency", 0);
		//av_dict_set(?m, "profile", "main", 0);  
	}
	//H.265  
	if (pCodecCtx->codec_id == AV_CODEC_ID_H265) {
		av_dict_set(&param, "preset", "ultrafast", 0);
		av_dict_set(&param, "tune", "zero-latency", 0);
	}

	//Show some Information  
	av_dump_format(pFormatCtx, 0, out_file, 1);

	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!pCodec) {
		printf("Can not find encoder! \n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, &param) < 0) {
		printf("Failed to open encoder! \n");
		return -1;
	}

	uint8_t * picture_buf;
	pFrame = av_frame_alloc();
	picture_size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	picture_buf = (uint8_t *)av_malloc(picture_size);
	avpicture_fill((AVPicture *)pFrame, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

	//Write File Header  
	avformat_write_header(pFormatCtx, NULL);

	av_new_packet(&pkt, picture_size);

	y_size = pCodecCtx->width * pCodecCtx->height;

	for (int i = 0; i<framenum; i++) {
		//Read raw YUV data  
		/*if (fread(picture_buf, 1, y_size * 3 / 2, in_file) <= 0) {
		printf("Failed to read raw data! \n");
		return -1;
		}*/
		/*else if (feof(in_file)) {
		break;
		}*/

		dpipe_buffer_t *data = NULL;
		data = dpipe_load(dpipe, NULL);
		picture_buf = (uint8_t *)data->pointer;
		dpipe_put(dpipe, data);


		pFrame->data[0] = picture_buf;              // Y  
		pFrame->data[1] = picture_buf + y_size;      // U   
		pFrame->data[2] = picture_buf + y_size * 5 / 4;  // V  
														 //PTS  
														 //pFrame->pts=i;  
		pFrame->pts = i*(video_st->time_base.den) / ((video_st->time_base.num) * 25);
		int got_picture = 0;
		//Encode  
		int ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_picture);
		if (ret < 0) {
			printf("Failed to encode! \n");
			return -1;
		}
		if (got_picture == 1) {
			//printf("Succeed to encode frame: %5d\tsize:%5d\n", framecnt, pkt.size);
			//framecnt++;
			pkt.stream_index = video_st->index;
			ret = av_write_frame(pFormatCtx, &pkt);
			av_free_packet(&pkt);
		}
	}
	//Flush Encoder  
	int ret = flush_encoder(pFormatCtx, 0);
	if (ret < 0) {
		printf("Flushing encoder failed\n");
		return -1;
	}

	//Write file trailer  
	av_write_trailer(pFormatCtx);

	//Clean  
	if (video_st) {
		avcodec_close(video_st->codec);
		av_free(pFrame);
		//	av_free(picture_buf);
	}
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);

	/*fclose(in_file);*/

	return 0;
}

int play()
{
	AVFormatContext* pFormatCtx;
	AVOutputFormat* fmt;
	AVStream* video_st;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;
	AVFrame* pFrame;
	uint8_t *buff;
	int picture_size;
	int y_size;
	int iid=0;
	//int framecnt = 0;
	//FILE *in_file = fopen("src01_480x272.yuv", "rb"); //Input raw YUV data   
	//FILE *in_file = fopen("../ds_480x272.yuv", "rb");   //Input raw YUV data  
	int in_w = 1366, in_h = 768;                              //Input data's width and height  
	int framenum = 500;                                   //Frames to encode  
														  //const char* out_file = "src01.h264";              //Output Filepath   
														  //const char* out_file = "src01.ts";  
														  //const char* out_file = "src01.hevc";  
	const char* out_file = "ffmpeg.h264";
	long long basePts = -1LL, newpts = 0LL, pts = -1LL, ptsSync = 0LL;

	av_register_all();
	avcodec_register_all();
	//Method1.  
	pFormatCtx = avformat_alloc_context();
	//Guess Format  
	fmt = av_guess_format(NULL, out_file, NULL);
	pFormatCtx->oformat = fmt;

	//Method 2.  
	//avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);  
	//fmt = pFormatCtx->oformat;  


	//Open output URL  
	if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
		printf("Failed to open output file! \n");
		return -1;
	}

	video_st = avformat_new_stream(pFormatCtx, 0);
	//video_st->time_base.num = 1;   
	//video_st->time_base.den = 25;    

	if (video_st == NULL) {
		return -1;
	}
	//Param that must set  
	pCodecCtx = video_st->codec;
	//pCodecCtx->codec_id =AV_CODEC_ID_HEVC;  
	pCodecCtx->codec_id = fmt->video_codec;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->width = in_w;
	pCodecCtx->height = in_h;
	pCodecCtx->bit_rate = 400000;
	pCodecCtx->gop_size = 20;

	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 30;

	//H264  
	//pCodecCtx->me_range = 16;  
	//pCodecCtx->max_qdiff = 4;  
	//pCodecCtx->qcompress = 0.6;  
	pCodecCtx->qmin = 10;
	pCodecCtx->qmax = 51;

	//Optional Param  
	pCodecCtx->max_b_frames = 3;

	// Set Option  
	AVDictionary *param = 0;
	//H.264  
	if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
		av_dict_set(&param, "preset", "superfast", 0);
		av_dict_set(&param, "tune", "zerolatency", 0);
		//av_dict_set(?m, "profile", "main", 0);  
	}
	//H.265  
	//if (pCodecCtx->codec_id == AV_CODEC_ID_H265) {
	//	av_dict_set(&param, "x265-params", "qp=20", 0);
	//	av_dict_set(&param, "preset", "ultrafast", 0);
	//	av_dict_set(&param, "tune", "zero-latency", 0);
	//}

	//Show some Information  
	av_dump_format(pFormatCtx, 0, out_file, 1);

	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!pCodec) {
		printf("Can not find encoder! \n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, &param) < 0) {
		printf("Failed to open encoder! \n");
		return -1;
	}
	vencode_put(iid,pCodecCtx);

	pFrame = av_frame_alloc();
	picture_size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	uint8_t *picture_buf = (uint8_t *)av_malloc(picture_size);
	avpicture_fill((AVPicture *)pFrame, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

	//Write File Header  
	avformat_write_header(pFormatCtx, NULL);

	//av_new_packet(&pkt, picture_size);

	y_size = pCodecCtx->width * pCodecCtx->height;

	/*uint8_t *data0 = (uint8_t *)av_malloc(in_w*in_h);
	uint8_t *data1 = (uint8_t *)av_malloc(in_w*in_h/4);
	uint8_t *data2 = (uint8_t *)av_malloc(in_w*in_h/4);*/

	for (;;) {
		//Read raw YUV data  
		/*if (fread(picture_buf, 1, y_size * 3 / 2, in_file) <= 0) {
		printf("Failed to read raw data! \n");
		return -1;
		}*/
		/*else if (feof(in_file)) {
		break;
		}*/
		AVPacket pkt;

		struct timeval tv;
		struct timespec to;
		gettimeofday(&tv, NULL);
		to.tv_sec = tv.tv_sec + 1;
		to.tv_nsec = tv.tv_usec * 1000;


		dpipe_buffer_t *data2 = NULL;
		data2 = dpipe_load(dpipe2, &to);
		buff = (uint8_t *)data2->pointer;
		
		if (basePts == -1LL) {
			basePts = data2->imgpts;
			ptsSync = pts_sync(VIDEO_FPS);
			newpts = ptsSync;
		}
		else {
			newpts = ptsSync + data2->imgpts - basePts;
		}
		tv = data2->timestamp;

		//pFrame->data[0] = buff;             // Y  
		//pFrame->data[1] = buff+ picture_size*2/3;      // U   
		//pFrame->data[2] = buff+ picture_size*5/6;  // V  
		memcpy(buff, picture_buf, picture_size);
		dpipe_put(dpipe2, data2);
								  //PTS  
								  //pFrame->pts=i;  
	//	pFrame->pts = i*(video_st->time_base.den) / ((video_st->time_base.num) * 25);
		int got_picture = 0;

		if (newpts > pts) {
			pts = newpts;
		}
		else {
			pts++;
		}


		//Encode  
		int ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_picture);
		if (ret < 0) {
			printf("Failed to encode! \n");
			return -1;
		}
		if (got_picture) {
			//printf("Succeed to encode frame: %5d\tsize:%5d\n", framecnt, pkt.size);
			//framecnt++;
			pkt.stream_index = 0;//video_st->index;
			/*ret = av_write_frame(pFormatCtx, &pkt);
			av_free_packet(&pkt);*/

			pkt.pts = pts;
			if (pkt.pts != AV_NOPTS_VALUE) {
				if (encoder_ptv_get(iid, pkt.pts, &tv, 0) == NULL) {
					gettimeofday(&tv, NULL);
				}
			}
			else {
				gettimeofday(&tv, NULL);
			}

			encoder_send_packet("video-encoder",iid, &pkt,pkt.pts, &tv);

		}
		
		pc++;
	}
	//Flush Encoder  
	int ret = flush_encoder(pFormatCtx, 0);
	if (ret < 0) {
		printf("Flushing encoder failed\n");
		return -1;
	}

	//Write file trailer  
	av_write_trailer(pFormatCtx);

	//Clean  
	if (video_st) {
		avcodec_close(video_st->codec);
		av_free(pFrame);
		//	av_free(picture_buf);
	}
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);

	/*fclose(in_file);*/

	return 0;
}

int wa()
{
	int w = 1366, h = 768;
	AVPixelFormat src_pixfmt = AV_PIX_FMT_BGRA;
	const int src_w = w, src_h = h;
	int src_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(src_pixfmt));

	//FILE *dst_file = fopen("sintel.yuv", "wb");
	AVPixelFormat dst_pixfmt = AV_PIX_FMT_YUV420P;
	const int dst_w = w, dst_h = h;
	int dst_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(dst_pixfmt));

	

	uint8_t *src_data[4];
	int src_linesize[4];

	uint8_t *dst_data[4];
	int dst_linesize[4];


	dpipe_buffer_t *data = NULL;
	data = dpipe_load(dpipe, NULL);
	uint8_t *temp_buffer = (uint8_t *)data->pointer;
	
	dpipe_buffer_t *data2 = NULL;
	data2 = dpipe_get(dpipe2);
	uint8_t *dst_buffer = (uint8_t *)data2->pointer;



	int rescale_method = SWS_BICUBIC;
	struct SwsContext *img_convert_ctx;


	int frame_idx = 0;
	int ret = 0;
	ret = av_image_alloc(src_data, src_linesize, src_w, src_h, src_pixfmt, 1);
	if (ret< 0) {
		printf("Could not allocate source image\n");
		return -1;
	}
	ret = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pixfmt, 1);
	if (ret< 0) {
		printf("Could not allocate destination image\n");
		return -1;
	}

	//Init Method 1  
	img_convert_ctx = sws_alloc_context();
	//Show AVOption  
	//	av_opt_show2(img_convert_ctx, stdout, AV_OPT_FLAG_VIDEO_PARAM, 0);
	//Set Value  
	av_opt_set_int(img_convert_ctx, "sws_flags", SWS_BICUBIC | SWS_PRINT_INFO, 0);
	av_opt_set_int(img_convert_ctx, "srcw", src_w, 0);
	av_opt_set_int(img_convert_ctx, "srch", src_h, 0);
	av_opt_set_int(img_convert_ctx, "src_format", src_pixfmt, 0);
	//'0' for MPEG (Y:0-235);'1' for JPEG (Y:0-255) 
	av_opt_set_int(img_convert_ctx, "src_range", 1, 0);
	av_opt_set_int(img_convert_ctx, "dstw", dst_w, 0);
	av_opt_set_int(img_convert_ctx, "dsth", dst_h, 0);
	av_opt_set_int(img_convert_ctx, "dst_format", dst_pixfmt, 0);
	av_opt_set_int(img_convert_ctx, "dst_range", 1, 0);
	sws_init_context(img_convert_ctx, NULL, NULL);



	switch (src_pixfmt) {
	case AV_PIX_FMT_GRAY8: {
		memcpy(src_data[0], temp_buffer, src_w*src_h);
		break;
	}
	case AV_PIX_FMT_YUV420P: {
		memcpy(src_data[0], temp_buffer, src_w*src_h);                    //Y  
		memcpy(src_data[1], temp_buffer + src_w*src_h, src_w*src_h / 4);      //U  
		memcpy(src_data[2], temp_buffer + src_w*src_h * 5 / 4, src_w*src_h / 4);  //V  
		break;
	}
	case AV_PIX_FMT_YUV422P: {
		memcpy(src_data[0], temp_buffer, src_w*src_h);                    //Y  
		memcpy(src_data[1], temp_buffer + src_w*src_h, src_w*src_h / 2);      //U  
		memcpy(src_data[2], temp_buffer + src_w*src_h * 3 / 2, src_w*src_h / 2);  //V  
		break;
	}
	case AV_PIX_FMT_YUV444P: {
		memcpy(src_data[0], temp_buffer, src_w*src_h);                    //Y  
		memcpy(src_data[1], temp_buffer + src_w*src_h, src_w*src_h);        //U  
		memcpy(src_data[2], temp_buffer + src_w*src_h * 2, src_w*src_h);      //V  
		break;
	}
	case AV_PIX_FMT_YUYV422: {
		memcpy(src_data[0], temp_buffer, src_w*src_h * 2);                  //Packed  
		break;
	}
	case AV_PIX_FMT_RGB24: {
		memcpy(src_data[0], temp_buffer, src_w*src_h * 3);                  //Packed  
		break;
	}
	case AV_PIX_FMT_BGRA: {
		memcpy(src_data[0], temp_buffer, src_w*src_h * 4);                  //Packed  
		break;
	}
	default: {
		printf("Not Support Input Pixel Format.\n");
		break;
	}
	}

	sws_scale(img_convert_ctx, src_data, src_linesize, 0, src_h, dst_data, dst_linesize);
	printf("Finish process frame %5d\n", frame_idx);
	frame_idx++;

	//switch (dst_pixfmt) {
	//case AV_PIX_FMT_GRAY8: {
	//	fwrite(dst_data[0], 1, dst_w*dst_h, dst_file);
	//	break;
	//}
	//case AV_PIX_FMT_YUV420P: {
	//	fwrite(dst_data[0], 1, dst_w*dst_h, dst_file);                 //Y  
	//	fwrite(dst_data[1], 1, dst_w*dst_h / 4, dst_file);               //U  
	//	fwrite(dst_data[2], 1, dst_w*dst_h / 4, dst_file);               //V  
	//	break;
	//}
	//case AV_PIX_FMT_YUV422P: {
	//	fwrite(dst_data[0], 1, dst_w*dst_h, dst_file);                 //Y  
	//	fwrite(dst_data[1], 1, dst_w*dst_h / 2, dst_file);               //U  
	//	fwrite(dst_data[2], 1, dst_w*dst_h / 2, dst_file);               //V  
	//	break;
	//}
	//case AV_PIX_FMT_YUV444P: {
	//	fwrite(dst_data[0], 1, dst_w*dst_h, dst_file);                 //Y  
	//	fwrite(dst_data[1], 1, dst_w*dst_h, dst_file);                 //U  
	//	fwrite(dst_data[2], 1, dst_w*dst_h, dst_file);                 //V  
	//	break;
	//}
	//case AV_PIX_FMT_YUYV422: {
	//	fwrite(dst_data[0], 1, dst_w*dst_h * 2, dst_file);               //Packed  
	//	break;
	//}
	//case AV_PIX_FMT_RGB24: {
	//	fwrite(dst_data[0], 1, dst_w*dst_h * 3, dst_file);               //Packed  
	//	break;
	//}
	//default: {
	//	printf("Not Support Output Pixel Format.\n");
	//	break;
	//}
	//}
	/*play(dst_data[0], dst_data[1], dst_data[2]);*/
	/*data0 = dst_data[0];
	data1 = dst_data[1];
	data2 = dst_data[2];*/
	memcpy(dst_buffer, dst_data[0], src_w*src_h);
	memcpy(dst_buffer+ src_w*src_h, dst_data[1], src_w*src_h/4);
	memcpy(dst_buffer+ src_w*src_h*5/4, dst_data[2], src_w*src_h/4);

	dpipe_put(dpipe, data);
	dpipe_store(dpipe2, data2);

	sws_freeContext(img_convert_ctx);

	//free(temp_buffer);
	/*fclose(dst_file);*/
	av_freep(&src_data[0]);
	av_freep(&dst_data[0]);
	pb++;
	return 0;

}

int tplay()
{
	AVFormatContext* pFormatCtx;
	AVOutputFormat* fmt;
	AVStream* video_st;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;
	AVPacket pkt;
	AVFrame* pFrame;
	AVFrame* pictureRGB;
	int picture_size;
	int y_size;
	//int framecnt = 0;
	//FILE *in_file = fopen("src01_480x272.yuv", "rb"); //Input raw YUV data   
	//FILE *in_file = fopen("../ds_480x272.yuv", "rb");   //Input raw YUV data  
	int in_w = 1366, in_h = 768;                              //Input data's width and height  
	int framenum = 1000;                                   //Frames to encode  
														  //const char* out_file = "src01.h264";              //Output Filepath   
														  //const char* out_file = "src01.ts";  
														  //const char* out_file = "src01.hevc";  
	const char* out_file = "tplay.h264";

	av_register_all();
	//Method1.  
	pFormatCtx = avformat_alloc_context();
	//Guess Format  
	fmt = av_guess_format(NULL, out_file, NULL);
	pFormatCtx->oformat = fmt;

	//Method 2.  
	//avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);  
	//fmt = pFormatCtx->oformat;  


	//Open output URL  
	if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
		printf("Failed to open output file! \n");
		return -1;
	}

	video_st = avformat_new_stream(pFormatCtx, 0);
	//video_st->time_base.num = 1;   
	//video_st->time_base.den = 25;    

	if (video_st == NULL) {
		return -1;
	}
	//Param that must set  
	pCodecCtx = video_st->codec;
	//pCodecCtx->codec_id =AV_CODEC_ID_HEVC;  
	pCodecCtx->codec_id = fmt->video_codec;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->width = in_w;
	pCodecCtx->height = in_h;
	pCodecCtx->bit_rate = 400000;
	pCodecCtx->gop_size = 250;

	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;

	//H264  
	//pCodecCtx->me_range = 16;  
	//pCodecCtx->max_qdiff = 4;  
	//pCodecCtx->qcompress = 0.6;  
	pCodecCtx->qmin = 10;
	pCodecCtx->qmax = 51;

	//Optional Param  
	pCodecCtx->max_b_frames = 3;

	// Set Option  
	AVDictionary *param = 0;
	//H.264  
	if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
		av_dict_set(&param, "preset", "slow", 0);
		av_dict_set(&param, "tune", "zerolatency", 0);
		//av_dict_set(?m, "profile", "main", 0);  
	}
	//H.265  
	if (pCodecCtx->codec_id == AV_CODEC_ID_H265) {
		av_dict_set(&param, "preset", "ultrafast", 0);
		av_dict_set(&param, "tune", "zero-latency", 0);
	}

	//Show some Information  
	av_dump_format(pFormatCtx, 0, out_file, 1);

	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!pCodec) {
		printf("Can not find encoder! \n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, &param) < 0) {
		printf("Failed to open encoder! \n");
		return -1;
	}


	pFrame = av_frame_alloc();
	pictureRGB = av_frame_alloc();
	picture_size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	uint8_t *picture_buff = (uint8_t *)av_malloc(picture_size);
	uint8_t *picture_buf = (uint8_t *)av_malloc(picture_size);
	avpicture_fill((AVPicture *)pFrame, picture_buff, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	avpicture_fill((AVPicture *)pictureRGB, picture_buf, AV_PIX_FMT_BGRA, pCodecCtx->width, pCodecCtx->height);
	//Write File Header  
	avformat_write_header(pFormatCtx, NULL);

	av_new_packet(&pkt, picture_size);

	y_size = pCodecCtx->width * pCodecCtx->height;

	for (int i = 0; i<framenum; i++) {
		//Read raw YUV data  
		/*if (fread(picture_buf, 1, y_size * 3 / 2, in_file) <= 0) {
		printf("Failed to read raw data! \n");
		return -1;
		}*/
		/*else if (feof(in_file)) {
		break;
		}*/
		dpipe_buffer_t *data = NULL;
		data = dpipe_load(dpipe, NULL);
		picture_buf = (uint8_t *)data->pointer;
		
		pictureRGB->data[0] = picture_buf;


		SwsContext* img_convert_ctx;
		img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGRA, pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);//SWS_BICUBIC
		sws_scale(img_convert_ctx, pictureRGB->data, pictureRGB->linesize, 0, pCodecCtx->height, pFrame->data, pFrame->linesize);
		//PTS  
		//pFrame->pts=i;  
		pFrame->pts = i*(video_st->time_base.den) / ((video_st->time_base.num) * 25);
		int got_picture = 0;
		//Encode  
		int ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_picture);
		if (ret < 0) {
			printf("Failed to encode! \n");
			return -1;
		}
		if (got_picture == 1) {
			//printf("Succeed to encode frame: %5d\tsize:%5d\n", framecnt, pkt.size);
			//framecnt++;
			pkt.stream_index = video_st->index;
			ret = av_write_frame(pFormatCtx, &pkt);
			av_free_packet(&pkt);
		}
		dpipe_put(dpipe, data);
	}
	//Flush Encoder  
	int ret = flush_encoder(pFormatCtx, 0);
	if (ret < 0) {
		printf("Flushing encoder failed\n");
		return -1;
	}

	//Write file trailer  
	av_write_trailer(pFormatCtx);

	//Clean  
	if (video_st) {
		avcodec_close(video_st->codec);
		av_free(pFrame);
		//	av_free(picture_buf);
	}
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);

	/*fclose(in_file);*/

	return 0;
}

int wb() {
	int w = 1366, h = 768;
	AVPixelFormat src_pixfmt = AV_PIX_FMT_BGRA;
	const int src_w = w, src_h = h;
	int src_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(src_pixfmt));

	//FILE *dst_file = fopen("sintel.yuv", "wb");
	AVPixelFormat dst_pixfmt = AV_PIX_FMT_YUV420P;
	const int dst_w = w, dst_h = h;
	int dst_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(dst_pixfmt));



	uint8_t *src_data[4];
	int src_linesize[4];

	uint8_t *dst_data[4];
	int dst_linesize[4];

	int rescale_method = SWS_BICUBIC;
	struct SwsContext *img_convert_ctx;


	int frame_idx = 0;
	int ret = 0;
	ret = av_image_alloc(src_data, src_linesize, src_w, src_h, src_pixfmt, 1);
	if (ret< 0) {
		printf("Could not allocate source image\n");
		return -1;
	}
	ret = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pixfmt, 1);
	if (ret< 0) {
		printf("Could not allocate destination image\n");
		return -1;
	}

	//Init Method 1  
	img_convert_ctx = sws_alloc_context();
	//Show AVOption  
	//	av_opt_show2(img_convert_ctx, stdout, AV_OPT_FLAG_VIDEO_PARAM, 0);
	//Set Value  
	av_opt_set_int(img_convert_ctx, "sws_flags", SWS_BICUBIC | SWS_PRINT_INFO, 0);
	av_opt_set_int(img_convert_ctx, "srcw", src_w, 0);
	av_opt_set_int(img_convert_ctx, "srch", src_h, 0);
	av_opt_set_int(img_convert_ctx, "src_format", src_pixfmt, 0);
	//'0' for MPEG (Y:0-235);'1' for JPEG (Y:0-255) 
	av_opt_set_int(img_convert_ctx, "src_range", 1, 0);
	av_opt_set_int(img_convert_ctx, "dstw", dst_w, 0);
	av_opt_set_int(img_convert_ctx, "dsth", dst_h, 0);
	av_opt_set_int(img_convert_ctx, "dst_format", dst_pixfmt, 0);
	av_opt_set_int(img_convert_ctx, "dst_range", 1, 0);
	sws_init_context(img_convert_ctx, NULL, NULL);

	while(1) {
		dpipe_buffer_t *data = NULL;
		data = dpipe_load(dpipe, NULL);
		uint8_t *temp_buffer = (uint8_t *)data->pointer;

		dpipe_buffer_t *data2 = NULL;
		data2 = dpipe_get(dpipe2);
		uint8_t *dst_buffer = (uint8_t *)data2->pointer;


		memcpy(src_data[0], temp_buffer, src_w*src_h * 4);

		sws_scale(img_convert_ctx, src_data, src_linesize, 0, src_h, dst_data, dst_linesize);
		printf("Finish process frame %5d\n", frame_idx);
		frame_idx++;

		memcpy(dst_buffer, dst_data[0], src_w*src_h);
		memcpy(dst_buffer + src_w*src_h, dst_data[1], src_w*src_h / 4);
		memcpy(dst_buffer + src_w*src_h * 5 / 4, dst_data[2], src_w*src_h / 4);

		dpipe_put(dpipe, data);
		dpipe_store(dpipe2, data2);
		pb++;
	}
	sws_freeContext(img_convert_ctx);

	//free(temp_buffer);
	/*fclose(dst_file);*/
	av_freep(&src_data[0]);
	av_freep(&dst_data[0]);
	pb++;
	return 0;
}

int playm()
{
	AVFormatContext* pFormatCtx;
	AVOutputFormat* fmt;
	AVStream* video_st;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;
	AVPacket pkt;
	AVFrame* pFrame;
	int picture_size;
	int y_size;
	int framecnt = 0;
	//FILE *in_file = fopen("src01_480x272.yuv", "rb"); //Input raw YUV data   
	//FILE *in_file = fopen("../ds_480x272.yuv", "rb");   //Input raw YUV data  
	int in_w = 1366, in_h = 768;                              //Input data's width and height  
	int framenum = 1000;                                   //Frames to encode  
														  //const char* out_file = "src01.h264";              //Output Filepath   
														  //const char* out_file = "src01.ts";  
														  //const char* out_file = "src01.hevc";  
	const char* out_file = "ffmpeg.h264";

	av_register_all();
	//Method1.  
	pFormatCtx = avformat_alloc_context();
	//Guess Format  
	fmt = av_guess_format(NULL, out_file, NULL);
	pFormatCtx->oformat = fmt;

	//Method 2.  
	//avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);  
	//fmt = pFormatCtx->oformat;  


	//Open output URL  
	if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
		printf("Failed to open output file! \n");
		return -1;
	}

	video_st = avformat_new_stream(pFormatCtx, 0);
	//video_st->time_base.num = 1;   
	//video_st->time_base.den = 25;    

	if (video_st == NULL) {
		return -1;
	}
	//Param that must set  
	pCodecCtx = video_st->codec;
	//pCodecCtx->codec_id =AV_CODEC_ID_HEVC;  
	pCodecCtx->codec_id = fmt->video_codec;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->width = in_w;
	pCodecCtx->height = in_h;
	pCodecCtx->bit_rate = 400000;
//	pCodecCtx->gop_size = 20;
	pCodecCtx->keyint_min = 25;
	pCodecCtx->gop_size = 25 * 10;

	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;

	//H264  
	//pCodecCtx->me_range = 16;  
	//pCodecCtx->max_qdiff = 4;  
	//pCodecCtx->qcompress = 0.6;  
	pCodecCtx->qmin = 10;
	pCodecCtx->qmax = 51;

	//Optional Param  
	pCodecCtx->max_b_frames = 0;

	// Set Option  
	AVDictionary *param = 0;
	//H.264  
	if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
		av_dict_set(&param, "preset", "superfast", 0);
		av_dict_set(&param, "tune", "zerolatency", 0);
		//av_dict_set(?m, "profile", "main", 0);  
	}
	//H.265  
	//if (pCodecCtx->codec_id == AV_CODEC_ID_H265) {
	//	av_dict_set(&param, "x265-params", "qp=20", 0);
	//	av_dict_set(&param, "preset", "ultrafast", 0);
	//	av_dict_set(&param, "tune", "zero-latency", 0);
	//}

	//Show some Information  
	av_dump_format(pFormatCtx, 0, out_file, 1);

	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!pCodec) {
		printf("Can not find encoder! \n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, &param) < 0) {
		printf("Failed to open encoder! \n");
		return -1;
	}


	pFrame = av_frame_alloc();
	picture_size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	uint8_t *picture_buf = (uint8_t *)av_malloc(picture_size);
	avpicture_fill((AVPicture *)pFrame, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

	//Write File Header  
	avformat_write_header(pFormatCtx, NULL);

	av_new_packet(&pkt, picture_size);

	y_size = pCodecCtx->width * pCodecCtx->height;

	/*uint8_t *data0 = (uint8_t *)av_malloc(in_w*in_h);
	uint8_t *data1 = (uint8_t *)av_malloc(in_w*in_h/4);
	uint8_t *data2 = (uint8_t *)av_malloc(in_w*in_h/4);*/

	for (int i = 0; i<framenum; i++) {
		//Read raw YUV data  
		/*if (fread(picture_buf, 1, y_size * 3 / 2, in_file) <= 0) {
		printf("Failed to read raw data! \n");
		return -1;
		}*/
		/*else if (feof(in_file)) {
		break;
		}*/

		dpipe_buffer_t *data2 = NULL;
		data2 = dpipe_load(dpipe2, NULL);
		picture_buf = (uint8_t *)data2->pointer;




		pFrame->data[0] = picture_buf;             // Y  
		pFrame->data[1] = picture_buf + picture_size * 2 / 3;      // U   
		pFrame->data[2] = picture_buf + picture_size * 5 / 6;  // V  

		dpipe_put(dpipe2, data2);
															   //PTS  
															   //pFrame->pts=i;  
		pFrame->pts = i*(video_st->time_base.den) / ((video_st->time_base.num) * 25);
		
		int got_picture = 0;
		//Encode  
		int ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_picture);
		if (ret < 0) {
			printf("Failed to encode! \n");
			return -1;
		}
		if (got_picture) {
			//printf("Succeed to encode frame: %5d\tsize:%5d\n", framecnt, pkt.size);
			framecnt++;
			pkt.stream_index = video_st->index;
			ret = av_write_frame(pFormatCtx, &pkt);
			av_free_packet(&pkt);
		}
		
		pc++;
	}
	//Flush Encoder  
	int ret = flush_encoder(pFormatCtx, 0);
	if (ret < 0) {
		printf("Flushing encoder failed\n");
		return -1;
	}

	//Write file trailer  
	av_write_trailer(pFormatCtx);

	//Clean  
	if (video_st) {
		avcodec_close(video_st->codec);
		av_free(pFrame);
		//	av_free(picture_buf);
	}
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);

	/*fclose(in_file);*/

	return 0;
}