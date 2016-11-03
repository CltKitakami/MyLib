#include <cstring>

#include "Wav.hpp"
#include "common/Exception.hpp"
#include "common/Convert.hpp"
#include "common/Log.hpp"


Wav::Wav(const char *fileName)
{
	this->open(fileName);
}


Wav::~Wav()
{
	this->close();
}


void Wav::close()
{
	this->file.close();
}


void Wav::open(const char *fileName)
{
	if (this->file.open(fileName, File::BINARY_READ) == false)
	{
		throw Exception(TRACE,
			std::string("Can not open wav: ") + fileName);
	}

	char header[Wav::HEADER_SIZE];
	int readSize = this->file.read(header, Wav::HEADER_SIZE);

	if (readSize == Wav::HEADER_SIZE &&
		compareQuadBytes("RIFF", &header[0]) &&
		compareQuadBytes("WAVE", &header[8]) &&
		compareQuadBytes("fmt ", &header[12]))
	{
		uint32_t chunkSize = Convert::combineQuadBytes(
			header[4], header[5], header[6], header[7]);
		uint32_t subChunk1Size = Convert::combineQuadBytes(
			header[16], header[17], header[18], header[19]);
		uint32_t subChunk2Size = Convert::combineQuadBytes(
			header[40], header[41], header[42], header[43]);

		if (4 + (8 + subChunk1Size) + (8 + subChunk2Size) != chunkSize)
			throw Exception(TRACE, "Wav chunk size error.");

		this->info.channel = Convert::combineDualBytes(
			header[22], header[23]);
		this->info.sampleRate = Convert::combineQuadBytes(
			header[24], header[25], header[26], header[27]);
		this->info.averageBytesPerSecond = Convert::combineQuadBytes(
			header[28], header[29], header[30], header[31]);
		this->info.blockAlign = Convert::combineDualBytes(
			header[32], header[33]);;
		this->info.bitsPerSample = Convert::combineDualBytes(
			header[34], header[35]);
		this->info.dataByteSize = subChunk2Size;
	}
	else
	{
		throw Exception(TRACE, "Wav header error.");
	}
}


uint32_t Wav::getSamples() const
{
	const uint32_t blockAlign = this->info.blockAlign;
	const uint32_t bytesPreSample = this->info.bitsPerSample / 8;
	uint32_t totalBlock = this->info.dataByteSize /
		(this->info.channel * blockAlign * bytesPreSample);

	return totalBlock * blockAlign;
}


uint32_t Wav::readAll(Wav::Raw &buffer)
{
	uint32_t dataSize = this->getSize();

	this->file.goToBegin(Wav::HEADER_SIZE);
	buffer.resize(dataSize);

	return (uint32_t)this->file.read(&buffer[0], (File::SizeType)dataSize);
}


Wav::Raw Wav::extractChannel(const Wav::Raw &wavBuffer,
	uint16_t channelIndex)
{
	if (this->info.channel == 1)
		return wavBuffer;

	if (channelIndex > this->info.channel)
		throw Exception(TRACE, "Channel does not exist.");

	const uint32_t bytesPreSample = this->info.bitsPerSample / 8;
	const uint32_t blockAlign = this->info.blockAlign;
	const uint32_t channelBlockSize = blockAlign * bytesPreSample;
	const uint32_t blockSize = channelBlockSize * this->info.channel;
	const uint32_t totalBlock = this->info.dataByteSize / blockSize;
	const uint32_t channelSize = this->info.dataByteSize / this->info.channel;
	const uint32_t channelSamples = channelSize / bytesPreSample;

	Raw channelBuffer;

	channelBuffer.resize(totalBlock * blockAlign * bytesPreSample);

	const char *block = wavBuffer.data() + blockAlign * channelIndex;
	char *out = &channelBuffer[0];

	// unroll loop
	if (bytesPreSample == 2)
	{
		for (uint32_t i = 0; i < channelSamples; ++i)
		{
			out[0] = block[0];
			out[1] = block[1];
			out += 2;
			block += blockAlign;
		}
	}
	else if (bytesPreSample == 1)
	{
		for (uint32_t i = 0; i < channelSamples; ++i)
		{
			out[0] = block[0];
			out += 1;
			block += blockAlign;
		}
	}
	else
	{
		for (uint32_t i = 0; i < channelSamples; ++i)
		{
			::memcpy(out, block, bytesPreSample);
			out += bytesPreSample;
			block += blockAlign;
		}
	}

	return channelBuffer;
}


void Wav::save(const char *fileName, const Wav::Channels &channels)
{
	if (channels.size() == 0)
		return;

	if (this->info.channel != channels.size())
	{
		this->info.channel = (uint16_t)channels.size();
		this->info.dataByteSize = this->info.channel * channels[0].size();
		this->info.averageBytesPerSecond =
			Wav::getAverageBytesPerSecond(this->info);
	}

	this->file.open(fileName, File::BINARY_WRITE);
	this->writeHeader();
	this->writeChannels(channels);
}


bool Wav::compareQuadBytes(const char *quad, const char *compared)
{
	return compared[0] == quad[0] && compared[1] == quad[1] &&
		compared[2] == quad[2] && compared[3] == quad[3];
}


void Wav::writeQuadBytes(const char *quad, char *out)
{
	out[0] = quad[0];
	out[1] = quad[1];
	out[2] = quad[2];
	out[3] = quad[3];
}


void Wav::writeHeader()
{
	char header[Wav::HEADER_SIZE];
	const uint16_t subChunk1Size = 16;
	const uint32_t chunkSize = 4 + (8 + subChunk1Size) +
		(8 + this->info.dataByteSize);

	writeQuadBytes("RIFF", &header[0]);
	Convert::splitQuadBytes(chunkSize, &header[4]);
	writeQuadBytes("WAVE", &header[8]);
	writeQuadBytes("fmt ", &header[12]);
	Convert::splitQuadBytes(subChunk1Size, &header[16]);
	Convert::splitDualBytes(0x01, &header[20]);
	Convert::splitDualBytes(this->info.channel, &header[22]);
	Convert::splitQuadBytes(this->info.sampleRate, &header[24]);
	Convert::splitQuadBytes(this->info.averageBytesPerSecond, &header[28]);
	Convert::splitDualBytes(this->info.blockAlign, &header[32]);
	Convert::splitDualBytes(this->info.bitsPerSample, &header[34]);
	writeQuadBytes("data", &header[36]);
	Convert::splitQuadBytes(this->info.dataByteSize, &header[40]);

	this->file.write(header, Wav::HEADER_SIZE);
}


void Wav::writeChannels(const Wav::Channels &channels)
{
	if (channels.size() == 0)
		return;

	const uint32_t bytesPreSample = this->info.bitsPerSample / 8;
	const uint32_t blockAlign = this->info.blockAlign;
	const uint32_t channelBlockSize = blockAlign * bytesPreSample;
	const uint32_t blockSize = channelBlockSize * this->info.channel;
	const uint32_t totalBlock = this->info.dataByteSize / blockSize;
	const uint32_t channelDataLength = channels[0].size();

	std::vector<const char *> channelPtr;
	channelPtr.reserve(channels.size());

	for (uint32_t i = 0; i < channels.size(); ++i)
	{
		channelPtr[i] = &channels[i][0];

		if (channels[i].size() != channelDataLength)
			throw Exception(TRACE, "Channel data size does not match.");
	}

	for (uint32_t i = 0; i < totalBlock; ++i)
	{
		for (uint32_t a = 0; a < blockAlign; ++a)
		{
			for (uint32_t c = 0; c < channels.size(); ++c)
			{
				this->file.write(channelPtr[c], (File::SizeType)bytesPreSample);
				channelPtr[c] += bytesPreSample;
			}
		}
	}
}
