
#include <windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <stdio.h>
#include <functiondiscoverykeys_devpkey.h>


#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>
#include <iostream>
#include <windows.h>
#include <xaudio2.h>

#include "Debug.hpp"
#include "Exception.hpp"
#include "Wav.hpp"
#include "Audio.hpp"
#include "Thread.hpp"


#define TEST_BASIC
#define TEST_BASIC
#define TEST_CATPURE
#define BUFFER_SIZE (1024 * 1024 * 10)	// 10 MB


Audio audio;


#ifdef TEST_CATPURE
#define SafeRelease(f) { free(f); f = nullptr; }

LPWSTR GetDeviceName(IMMDeviceCollection *DeviceCollection, UINT DeviceIndex)
{
    IMMDevice *device;
    LPWSTR deviceId;
    HRESULT hr;

    hr = DeviceCollection->Item(DeviceIndex, &device);
    if (FAILED(hr))
    {
        printf("Unable to get device %d: %x\n", DeviceIndex, hr);
        return nullptr;
    }
    hr = device->GetId(&deviceId);
    if (FAILED(hr))
    {
        printf("Unable to get device %d id: %x\n", DeviceIndex, hr);
        return nullptr;
    }

    IPropertyStore *propertyStore;
    hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
    if (FAILED(hr))
    {
        printf("Unable to open device %d property store: %x\n", DeviceIndex, hr);
        return nullptr;
    }

    PROPVARIANT friendlyName;
    PropVariantInit(&friendlyName);
    hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);

    if (FAILED(hr))
    {
        printf("Unable to retrieve friendly name for device %d : %x\n", DeviceIndex, hr);
        return nullptr;
    }

    DVAR(friendlyName.vt);
    if (friendlyName.vt != VT_LPWSTR)
    	DVAR("Unknown");
    else
    	DVAR((const wchar_t *)friendlyName.pwszVal);

    PropVariantClear(&friendlyName);
    CoTaskMemFree(deviceId);

    return nullptr;
}


HRESULT listDevices() {
    HRESULT hr = S_OK;

    // get an enumerator
    IMMDeviceEnumerator *pMMDeviceEnumerator;

    hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        (void**)&pMMDeviceEnumerator
    );

    if (FAILED(hr)) {
        printf("CoCreateInstance(IMMDeviceEnumerator) failed: hr = 0x%08x\n", hr);
        return hr;
    }

    IMMDeviceCollection *pMMDeviceCollection;

    // get all the active render endpoints
    hr = pMMDeviceEnumerator->EnumAudioEndpoints(
        eRender, DEVICE_STATE_ACTIVE, &pMMDeviceCollection
    );
    pMMDeviceEnumerator->Release();
    if (FAILED(hr)) {
        printf("IMMDeviceEnumerator::EnumAudioEndpoints failed: hr = 0x%08x\n", hr);
        return hr;
    }

    UINT count;
    hr = pMMDeviceCollection->GetCount(&count);
    if (FAILED(hr)) {
        pMMDeviceCollection->Release();
        printf("IMMDeviceCollection::GetCount failed: hr = 0x%08x\n", hr);
        return hr;
    }
    printf("Active render endpoints found: %u\n", count);

    for (UINT i = 0; i < count; i++) {
        IMMDevice *pMMDevice;
GetDeviceName(pMMDeviceCollection, i);
        // get the "n"th device
        hr = pMMDeviceCollection->Item(i, &pMMDevice);
        if (FAILED(hr)) {
            pMMDeviceCollection->Release();
            printf("IMMDeviceCollection::Item failed: hr = 0x%08x\n", hr);
            return hr;
        }

        // open the property store on that device
        IPropertyStore *pPropertyStore;
        hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
        pMMDevice->Release();
        if (FAILED(hr)) {
            pMMDeviceCollection->Release();
            printf("IMMDevice::OpenPropertyStore failed: hr = 0x%08x\n", hr);
            return hr;
        }

        // get the long name property
        PROPVARIANT pv; PropVariantInit(&pv);
        hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &pv);
        pPropertyStore->Release();
        if (FAILED(hr)) {
            pMMDeviceCollection->Release();
            printf("IPropertyStore::GetValue failed: hr = 0x%08x\n", hr);
            return hr;
        }

        if (VT_LPWSTR != pv.vt) {
            printf("PKEY_Device_FriendlyName variant type is %u - expected VT_LPWSTR", pv.vt);

            PropVariantClear(&pv);
            pMMDeviceCollection->Release();
            return E_UNEXPECTED;
        }

        printf("\nname =%ls\n", pv.pwszVal);

        PropVariantClear(&pv);
    }
    pMMDeviceCollection->Release();

    return S_OK;
}


HRESULT getDefaultDevice(IMMDevice **ppMMDevice) {
    HRESULT hr = S_OK;
    IMMDeviceEnumerator *pMMDeviceEnumerator;

    // activate a device enumerator
    hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        (void**)&pMMDeviceEnumerator
    );
    if (FAILED(hr)) {
        printf("CoCreateInstance(IMMDeviceEnumerator) failed: hr = 0x%08x\n", hr);
        return hr;
    }

    // get the default render endpoint
    hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, ppMMDevice);
    pMMDeviceEnumerator->Release();
    if (FAILED(hr)) {
        printf("IMMDeviceEnumerator::GetDefaultAudioEndpoint failed: hr = 0x%08x\n", hr);
        return hr;
    }

    return S_OK;
}


int main(int argc, char *argv[])
{
	CoInitialize(nullptr);

	listDevices();

	IAudioClient *pAudioClient;
	IMMDevice *device;

	getDefaultDevice(&device);

    HRESULT hr = device->Activate(__uuidof(IAudioClient),
        CLSCTX_ALL, nullptr, (void**)&pAudioClient);
    if (FAILED(hr)) {
        printf("IMMDevice::Activate(IAudioClient) failed: hr = 0x%08x", hr);
        return hr;
    }

	REFERENCE_TIME hnsDefaultDevicePeriod;
    hr = pAudioClient->GetDevicePeriod(&hnsDefaultDevicePeriod, nullptr);
    if (FAILED(hr)) {
        printf("IAudioClient::GetDevicePeriod failed: hr = 0x%08x\n", hr);
        pAudioClient->Release();
        return hr;
    }

	// get the default device format
    WAVEFORMATEX *pwfx;
    hr = pAudioClient->GetMixFormat(&pwfx);
    if (FAILED(hr)) {
        printf("IAudioClient::GetMixFormat failed: hr = 0x%08x\n", hr);
        CoTaskMemFree(pwfx);
        pAudioClient->Release();
        return hr;
    }

    DVAR(pwfx->wFormatTag);
    DVAR(pwfx->wBitsPerSample);
    DVAR(pwfx->nBlockAlign);
    DVAR(pwfx->nAvgBytesPerSec);

    switch (pwfx->wFormatTag) {
        case WAVE_FORMAT_IEEE_FLOAT:
            pwfx->wFormatTag = WAVE_FORMAT_PCM;
            pwfx->wBitsPerSample = 16;
            pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
            pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
            break;

        case WAVE_FORMAT_EXTENSIBLE:
            {
                // naked scope for case-local variable
                PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);
                if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat)) {
                    pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
                    pEx->Samples.wValidBitsPerSample = 16;
                    pwfx->wBitsPerSample = 16;
                    pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
                    pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
                } else {
                    printf("Don't know how to coerce mix format to int-16\n");
                    CoTaskMemFree(pwfx);
                    pAudioClient->Release();
                    return E_UNEXPECTED;
                }
            }
            break;

        default:
            printf("Don't know how to coerce WAVEFORMATEX with wFormatTag = 0x%08x to int-16\n", pwfx->wFormatTag);
            CoTaskMemFree(pwfx);
            pAudioClient->Release();
            return E_UNEXPECTED;
    }

    DVAR(pwfx->wFormatTag);
    DVAR(pwfx->wBitsPerSample);
    DVAR(pwfx->nBlockAlign);
    DVAR(pwfx->nAvgBytesPerSec);

	hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, pwfx, 0 );
    if (FAILED(hr)) {
        printf("IAudioClient::Initialize failed: hr = 0x%08x\n", hr);
        pAudioClient->Release();
        return hr;
    }

	IAudioCaptureClient *pAudioCaptureClient;
    hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pAudioCaptureClient);
    if (FAILED(hr)) {
        printf("IAudioClient::GetService(IAudioCaptureClient) failed: hr 0x%08x\n", hr);
        pAudioClient->Release();
        return hr;
    }


    hr = pAudioClient->Start();
    if (FAILED(hr)) {
        printf("IAudioClient::Start failed: hr = 0x%08x\n", hr);
        pAudioCaptureClient->Release();
        pAudioClient->Release();
        return hr;
    }


for (int i = 0; i < 10; ++i)
{

	UINT32 nNextPacketSize;
    hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize);
    if (FAILED(hr)) {
        printf("IAudioCaptureClient::GetNextPacketSize failed on pass %u after %u frames: hr = 0x%08x\n", 0, 0, hr);
        pAudioClient->Stop();
        pAudioCaptureClient->Release();
        pAudioClient->Release();
        return hr;
    }


    // get the captured data
    BYTE *pData;
    UINT32 nNumFramesToRead;
    DWORD dwFlags;

    hr = pAudioCaptureClient->GetBuffer(&pData, &nNumFramesToRead, &dwFlags, nullptr,
        nullptr);
    if (FAILED(hr)) {
        printf("IAudioCaptureClient::GetBuffer failed on pass %u after %u frames: hr = 0x%08x\n", 0, 0, hr);
        pAudioClient->Stop();
        pAudioCaptureClient->Release();
        pAudioClient->Release();
        return hr;
    }

    DVAR(nNumFramesToRead);


    // if (bFirstPacket && AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY == dwFlags) {
        // printf("Probably spurious glitch reported on first packet\n");
    if (0 != dwFlags && AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY != dwFlags) {
        printf("IAudioCaptureClient::GetBuffer set flags to 0x%08x on pass %u after %u frames\n", dwFlags, 0, 0);
        // pAudioClient->Stop();
        // pAudioCaptureClient->Release();
        // pAudioClient->Release();
        // return E_UNEXPECTED;
    }
    else
    	DVAR((int)*pData);

    if (0 == nNumFramesToRead) {
        printf("IAudioCaptureClient::GetBuffer said to read 0 frames on pass %u after %u frames\n", 0, 0);
        pAudioClient->Stop();
        pAudioCaptureClient->Release();
        pAudioClient->Release();
        return E_UNEXPECTED;
    }


    UINT32 nBlockAlign = pwfx->nBlockAlign;
    LONG lBytesToWrite = nNumFramesToRead * nBlockAlign;
    hr = pAudioCaptureClient->ReleaseBuffer(nNumFramesToRead);
}

	pAudioClient->Stop();
    pAudioCaptureClient->Release();
    pAudioClient->Release();


	CoUninitialize();

	return 0;
}

#elif defined(TEST_STREAM)

int main(int argc, char *argv[])
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	try
	{
		audio.open(XAUDIO2_DEBUG_ENGINE);
		DMSG("open");
		Wav wav("D:\\openAL\\11k16bitpcm.wav");
		audio.stream(wav.getFormat());

		for (int i = 0; i < 2; ++i)
		{
			DMSG("submit buffer " << i);
			uint32_t size = wav.getSize();
			audio.appendBuffer(size);
			wav.read(audio.getBuffer(i), size);
			audio.submitBuffer(i, size);
		}

		wav.close();

		DMSG("sleep");
		Thread::sleep(10000);

		DMSG("stop");
		audio.stop();
	}
	catch (...)
	{
		std::cout << "error" << std::endl;
	}

	DMSG("close");
	audio.close();

	CoUninitialize();

	return 0;
}

#else

int main(int argc, char *argv[])
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	char *buffer = new char[BUFFER_SIZE];

	if (buffer == nullptr)
		return -11;

	try
	{
		audio.open(XAUDIO2_DEBUG_ENGINE);
		DMSG("open");
		Wav wav("D:\\openAL\\11k16bitpcm.wav");
		uint32_t readSize = wav.read(buffer, BUFFER_SIZE);
		DVAR(readSize);
		wav.close();
		DMSG("play");
		audio.play(wav.getFormat(), buffer, wav.getSize());

		while (audio.isPlaying())
			Thread::sleep(10);
	}
	catch (...)
	{
		std::cout << "error" << std::endl;
	}

	DMSG("close");
	audio.close();

	CoUninitialize();

	return 0;
}
#endif
