#include "common/Log.hpp"
#include "common/Exception.hpp"
#include "audfile/Wav.hpp"


int main()
{
	try
	{
		Wav::Raw wavData;
		Wav wav("D:\\openal\\ring.wav");
		size_t readSize = wav.readAll(wavData);
		LOG_VAR(readSize);
		LOG_VAR(wav.getSize());
		LOG_VAR(wav.getSamples());

		Wav::Channels channels;
		Wav::Raw left = wav.extractChannel(wavData, 0);
		Wav::Raw right = wav.extractChannel(wavData, 1);
		channels.push_back(left);
		channels.push_back(right);
		channels.push_back(left);
		channels.push_back(right);
		channels.push_back(left);

		Wav out;
		out.setInfo(wav.getInfo());
		out.save("E:\\out.wav", channels);
	}
	catch (Exception ex)
	{
		MSG(ex.getMessage());
	}

	return 0;
}
