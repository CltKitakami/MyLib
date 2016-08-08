#ifndef _WAV_HPP_
#define _WAV_HPP_

#include <cstdint>
#include <vector>
#include "file/File.hpp"


typedef struct WavInfo
{
	uint16_t channel;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	uint32_t sampleRate;
	uint32_t averageBytesPerSecond;
	uint32_t dataByteSize;
} WavInfo;


class Wav
{
public:
	enum { HEADER_SIZE = 44 };

	typedef std::vector<char> Raw;
	typedef std::vector<Raw> Channels;

	Wav() = default;
	Wav(const char *fileName);
	~Wav();

	void open(const char *fileName);
	void close();

	WavInfo getInfo() const { return this->info; }
	void setInfo(const WavInfo &info) { this->info = info; }

	uint32_t getSize() const { return this->info.dataByteSize; };
	uint32_t getSamples() const;

	uint32_t readAll(Raw &buffer);

	Raw extractChannel(const Raw &wavBuffer, uint16_t channelIndex);

	void save(const char *fileName, const Channels &channels);

	static uint32_t getAverageBytesPerSecond(const WavInfo &info)
	{ return info.sampleRate * info.bitsPerSample * info.channel / 8; }

private:
	bool compareQuadBytes(const char *quad, const char *compared);
	void writeQuadBytes(const char *quad, char *out);
	void writeHeader(File &file);
	void writeChannels(File &file, const Channels &channels);

	WavInfo info;
	File file;
};

#endif
