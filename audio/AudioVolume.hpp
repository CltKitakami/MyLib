#ifndef _AUDIOVOLUME_HPP_
#define _AUDIOVOLUME_HPP_

#include "pattern/NonCopyable.hpp"


class SimpleAudioVolume;


class AudioVolume : NonCopyable
{
public:
	AudioVolume() : sav(nullptr) {}
	~AudioVolume() { close(); }

	bool open();
	bool openByProcessId(unsigned int processId);
	void close();

	void setMute(bool isMute);
	void setVolume(float percentage);

private:
	SimpleAudioVolume *sav;
};


#endif
