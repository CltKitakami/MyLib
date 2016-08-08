#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <audioclient.h>
#include "common/Log.hpp"
#include "pattern/Singleton.hpp"


template <typename T>
inline void safeRelease(T &p)
{
	if (p != nullptr)
	{
		p->Release();
		p = nullptr;
	}
}


class ComLibrary
{
public:
	ComLibrary() { initialize(); }
	~ComLibrary() { release(); }

	void initialize() { ::CoInitialize(nullptr); }
	void release() { ::CoUninitialize(); }
};


class ImmDeviceEnumerator
{
public:
	ImmDeviceEnumerator() : deviceEnumerator(nullptr) { initialize(); }
	~ImmDeviceEnumerator() { safeRelease(deviceEnumerator); }

	inline IMMDeviceEnumerator * get() { return deviceEnumerator; }

private:
	void initialize()
	{
		(void)Singleton<ComLibrary>::instance();

		HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
			CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator);

		if (FAILED(hr))
			LOG.d(TRACE, "Error: CoCreateInstance");
	}

	IMMDeviceEnumerator *deviceEnumerator;
};


class SessionManager2
{
public:
	SessionManager2(IMMDevice *device) :
		sessionManager(nullptr) { initialize(device); }
	~SessionManager2() { safeRelease(sessionManager); }

	inline IAudioSessionManager2 * get() { return sessionManager; }

private:
	void initialize(IMMDevice *device)
	{
		if (device != nullptr)
		{
			HRESULT hr = device->Activate(__uuidof(IAudioSessionManager2),
				CLSCTX_ALL, nullptr, (void**)&sessionManager);

			if (FAILED(hr))
				LOG.d(TRACE, "Error: Activate IAudioSessionManager2");
		}
	}

	IAudioSessionManager2 *sessionManager;
};


class ImmDevice
{
public:
	ImmDevice(IMMDeviceEnumerator *deviceEnumerator) :
		device(nullptr) { GetDefaultDevice(deviceEnumerator); }
	~ImmDevice() { safeRelease(device); }

	inline IMMDevice * get() { return device; }

private:
	void GetDefaultDevice(IMMDeviceEnumerator *deviceEnumerator)
	{
		if (deviceEnumerator != nullptr)
		{
			HRESULT hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);

			if (FAILED(hr))
				LOG.d(TRACE, "Error: GetDefaultAudioEndpoint");
		}
	}

	IMMDevice *device;
};


class SimpleAudioVolume
{
public:
	SimpleAudioVolume() : simpleAudioVolume(nullptr) {}
	SimpleAudioVolume(IAudioSessionControl2 *sessionControl2) :
		simpleAudioVolume(nullptr) { initialize(sessionControl2); }

	void release() { safeRelease(simpleAudioVolume); }

	inline ISimpleAudioVolume * get() { return simpleAudioVolume; }

	inline void setMute(bool isMute)
	{ simpleAudioVolume->SetMute(isMute, nullptr); }

	inline void setVolume(float percentage)
	{ simpleAudioVolume->SetMasterVolume(percentage, nullptr); }

private:
	void initialize(IAudioSessionControl2 *sessionControl2)
	{
		if (sessionControl2 != nullptr)
		{
			HRESULT hr = sessionControl2->QueryInterface(
				__uuidof(ISimpleAudioVolume), (void**) &simpleAudioVolume);

			if (FAILED(hr))
				LOG.d(TRACE, "Error: QueryInterface ISimpleAudioVolume");
		}
	}

	ISimpleAudioVolume *simpleAudioVolume;
};


class SessionControl
{
public:
	SessionControl(IAudioSessionEnumerator *sessionEnumerator, int index) :
		sessionControl(nullptr) { initialize(sessionEnumerator, index); }
	~SessionControl() { safeRelease(sessionControl); }

	inline IAudioSessionControl * get() { return sessionControl; }

private:
	void initialize(IAudioSessionEnumerator *sessionEnumerator, int index)
	{
		if (sessionEnumerator != nullptr)
		{
			HRESULT hr = sessionEnumerator->GetSession(index, &sessionControl);

			if (FAILED(hr))
				LOG.d(TRACE, "Error: GetSession");
		}
	}

	IAudioSessionControl *sessionControl;
};


class SessionControl2
{
public:
	SessionControl2(IAudioSessionControl *sessionControl) :
		sessionControl2(nullptr) { initialize(sessionControl); }
	~SessionControl2() { safeRelease(sessionControl2); }

	inline IAudioSessionControl2 * get() { return sessionControl2; }

private:
	void initialize(IAudioSessionControl *sessionControl)
	{
		if (sessionControl != nullptr)
		{
			HRESULT hr = sessionControl->QueryInterface(
				__uuidof(IAudioSessionControl2), (void**)&sessionControl2);

			if (FAILED(hr))
				LOG.d(TRACE, "Error: QueryInterface IAudioSessionControl2");
		}
	}

	IAudioSessionControl2 *sessionControl2;
};


class SessionEnumerator
{
public:
	SessionEnumerator(IAudioSessionManager2 *audioSessionManager) :
		sessionEnumerator(nullptr) { initialize(audioSessionManager); }
	~SessionEnumerator() { safeRelease(sessionEnumerator); }

	SimpleAudioVolume getSimpleAudioVolumeByProcessId(DWORD processId)
	{
		if (sessionEnumerator == nullptr)
			return nullptr;

		int count = getSessionEnumeratorCount();

		for (int i = 0; i < count; ++i)
		{
			SessionControl sc(sessionEnumerator, i);
			SessionControl2 sc2(sc.get());

			if (getSessionControl2ProcessId(sc2.get()) == processId)
				return SimpleAudioVolume(sc2.get());
		}

		return nullptr;
	}

private:
	void initialize(IAudioSessionManager2 *audioSessionManager)
	{
		if (audioSessionManager != nullptr)
		{
			HRESULT hr = audioSessionManager->GetSessionEnumerator(&sessionEnumerator);

			if (FAILED(hr))
				LOG.d(TRACE, "Error: GetSessionEnumerator");
		}
	}

	int getSessionEnumeratorCount()
	{
		int count = 0;

		if (sessionEnumerator != nullptr)
			(void)sessionEnumerator->GetCount(&count);

		return count;
	}

	DWORD getSessionControl2ProcessId(IAudioSessionControl2 *sessionControl2)
	{
		DWORD processId = 0;

		if (sessionControl2 != nullptr)
		{
			if (SUCCEEDED(sessionControl2->GetProcessId(&processId)))
				return processId;
		}

		return 0;
	}

	IAudioSessionEnumerator *sessionEnumerator;
};


bool AudioVolume::open()
{
	return openByProcessId((unsigned int)::GetCurrentProcessId());
}


bool AudioVolume::openByProcessId(unsigned int processId)
{
	if (sav != nullptr)
		return false;
	else
	{
		ImmDeviceEnumerator ide;
		ImmDevice id(ide.get());
		SessionManager2 sm2(id.get());
		SessionEnumerator se(sm2.get());
		sav = new SimpleAudioVolume;
		*sav = se.getSimpleAudioVolumeByProcessId(processId);
		return sav->get() != nullptr;
	}
}


void AudioVolume::close()
{
	if (sav != nullptr && sav->get() != nullptr)
	{
		sav->release();
		sav = nullptr;
	}
}


void AudioVolume::setMute(bool isMute)
{
	sav->setMute(isMute);
}


void AudioVolume::setVolume(float percentage)
{
	if (percentage >= 0.0 && percentage <= 1.0)
		sav->setVolume(percentage);
}
