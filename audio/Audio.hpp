#ifndef _AUDIO_HPP_
#define _AUDIO_HPP_

#include <cstdint>
#include <vector>
#include <xaudio2.h>

#include "Debug.hpp"
#include "Exception.hpp"
#include "Wav.hpp"


class Audio
{
public:
	typedef XAUDIO2_BUFFER Buffer;

	Audio();
	virtual ~Audio();

	void open(uint32_t flags = 0);
	void close();

	void play(const WavFormat &format, const Buffer &buffer);
	void play(const WavFormat &format, const char *buffer, uint32_t bufferSize);
	void stop() { this->sourceVoice->Stop(0); }
	bool isPlaying();
	Buffer arrayToBuffer(const char *array, uint32_t arraySize);

	void stream(const WavFormat &format);
	void appendBuffer(uint32_t bufferSize);
	char * getBuffer(uint32_t bufferIndex);
	void submitBuffer(uint32_t bufferIndex, uint32_t bufferSize, bool isEndofStream = false);
	void deleteBuffers();

	IXAudio2 *xAudio2;
	IXAudio2MasteringVoice *masteringVoice;
	IXAudio2SourceVoice *sourceVoice;
private:
	std::vector<char *> bufPtrs;
	uint32_t bufCnt;
	uint32_t currBufIdx;


	class VoiceCallback : public IXAudio2VoiceCallback
	{
	public:
	    HANDLE bufferEndEvent;
	    VoiceCallback(): bufferEndEvent(CreateEvent(nullptr, FALSE, FALSE, nullptr)) {}
	    ~VoiceCallback() { CloseHandle(bufferEndEvent); }

	    STDMETHOD_(void, OnStreamEnd)() { SetEvent(bufferEndEvent); }

	    STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32) {}
	    STDMETHOD_(void, OnVoiceProcessingPassEnd)() {}
	    STDMETHOD_(void, OnBufferStart)(void *) {}
	    STDMETHOD_(void, OnBufferEnd)(void *) { DMSG("buffer end"); }
	    STDMETHOD_(void, OnLoopEnd)(void *) {}
	    STDMETHOD_(void, OnVoiceError)(void *, HRESULT) {}
	};


	static VoiceCallback callback;
};


#endif
