#pragma once

#ifndef _H264FRAMEDLIVESOURCE_HH  
#define _H264FRAMEDLIVESOURCE_HH  

#include <FramedSource.hh>  

class H264FramedLiveSource : public FramedSource
{
public:
	static H264FramedLiveSource * createNew(UsageEnvironment& env, int cid/* TODO: more params */);
protected:
	H264FramedLiveSource(UsageEnvironment& env, int cid);
	~H264FramedLiveSource();
private:
	/*static unsigned referenceCount;*/
	int remove_startcode;
	//static ga_module_t *m;
	int channelId;
	//
	static void deliverFrame0(void* clientData);
	void doGetNextFrame();
	//virtual void doStopGettingFrames(); // optional
	void deliverFrame();

};
#endif