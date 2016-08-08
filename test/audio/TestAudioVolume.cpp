#include <stdlib.h>
#include <windows.h>
#include "audfile/AudioVolume.hpp"

int main()
{
	PlaySound("D:\\openal\\ost.wav", nullptr, SND_FILENAME | SND_ASYNC);
	Sleep(1000);
	AudioVolume volume;

	if (volume.open() == true)
	{
		volume.setMute(false);
		Sleep(1000);
		volume.setMute(true);
		volume.setVolume(0.2);
		Sleep(1000);
		volume.setMute(false);
	}

	system("pause");

	return 0;
}
