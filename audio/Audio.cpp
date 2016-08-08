#include "Audio.hpp"


#define CHECK_RESULT(result, msg) \
do\
{\
	if (result != S_OK)\
	{\
		Exception ex(TRACE, msg);\
		ex << result;\
		throw ex;\
	}\
} while(0)


Audio::VoiceCallback Audio::callback;


Audio::Audio() :
	xAudio2(nullptr),
	masteringVoice(nullptr),
	sourceVoice(nullptr),
	bufCnt(0),
	currBufIdx(0)
{
}


Audio::~Audio()
{
	this->close();
}


void Audio::open(uint32_t flags)
{
	HRESULT result = XAudio2Create(&this->xAudio2, flags);
	CHECK_RESULT(result, "Failed to init XAudio2 engine: ");
	result = this->xAudio2->CreateMasteringVoice(&this->masteringVoice);
	CHECK_RESULT(result, "Failed creating mastering voice: ");
}


void Audio::close()
{
	if (this->sourceVoice != nullptr)
	{
		this->sourceVoice->DestroyVoice();
		this->sourceVoice = nullptr;
	}

	if (this->masteringVoice != nullptr)
	{
		this->masteringVoice->DestroyVoice();
		this->masteringVoice = nullptr;
	}

	if (this->xAudio2 != nullptr)
	{
		this->xAudio2->Release();
		this->xAudio2 = nullptr;
	}
}


void Audio::play(const WavFormat &format, const Audio::Buffer &buffer)
{
	Buffer buf = buffer;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	HRESULT result = this->xAudio2->CreateSourceVoice(&this->sourceVoice, &format);
	CHECK_RESULT(result, "Error creating source voice: ");

	result = this->sourceVoice->SubmitSourceBuffer(&buf);
	CHECK_RESULT(result, "Error submitting source buffer: ");

	this->sourceVoice->Start(0);
}


void Audio::play(const WavFormat &format, const char *buffer, uint32_t bufferSize)
{
	Buffer buf = this->arrayToBuffer(buffer, bufferSize);
	this->play(format, buf);
}


bool Audio::isPlaying()
{
	XAUDIO2_VOICE_STATE state;
	this->sourceVoice->GetState(&state);
	return state.BuffersQueued > 0;
}


Audio::Buffer Audio::arrayToBuffer(const char *array, uint32_t arraySize)
{
	Buffer buf = { 0 };
	buf.AudioBytes = arraySize;
	buf.pAudioData = (const BYTE *)array;
	return buf;
}


void Audio::stream(const WavFormat &format)
{
	HRESULT result = this->xAudio2->CreateSourceVoice(&this->sourceVoice, &format, 0, 1.0f, &this->callback);
	CHECK_RESULT(result, "Error creating stream source voice: ");
	this->sourceVoice->Start(0, 0);
}


void Audio::appendBuffer(uint32_t bufferSize)
{
	char *buf = new char[bufferSize];

	if (buf == nullptr)
		throw BadAllocationException(TRACE, "Creating buffer fail");

	bufPtrs.push_back(buf);
}


char * Audio::getBuffer(uint32_t bufferIndex)
{
	if (bufferIndex >= this->bufPtrs.size())
		throw IndexOutOfRangeException(TRACE, "bufferIndex");

	return this->bufPtrs[bufferIndex];
}


void Audio::submitBuffer(uint32_t bufferIndex, uint32_t bufferSize, bool isEndofStream)
{
	Buffer buf = this->arrayToBuffer(this->getBuffer(bufferIndex), bufferSize);

	if (isEndofStream == true)
		buf.Flags = XAUDIO2_END_OF_STREAM;

	HRESULT result = this->sourceVoice->SubmitSourceBuffer(&buf);
	CHECK_RESULT(result, "Error submitting source buffer: ");
}


void Audio::deleteBuffers()
{
	for (int i = 0; i < (int)this->bufPtrs.size(); ++i)
		delete this->bufPtrs[i];
	this->bufPtrs.clear();
}
