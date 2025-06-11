//	AudioDeviceManagerSingleton.cpp - Singleton wrapper for AudioDeviceManager

#include "AudioDeviceManagerSingleton.h"

#include <cassert>

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AudioDeviceManagerSingleton AudioDeviceManagerSingleton::singletonInstance;

//------------------------------------------------------------------------------
juce::AudioDeviceManager& AudioDeviceManagerSingleton::getInstance()
{
	return *(singletonInstance.instance);
}

//------------------------------------------------------------------------------
void AudioDeviceManagerSingleton::killInstance()
{
	assert(singletonInstance.instance != nullptr);

	if(singletonInstance.instance)
	{
		delete singletonInstance.instance;
		singletonInstance.instance = nullptr;
	}
}

//------------------------------------------------------------------------------
AudioDeviceManagerSingleton::AudioDeviceManagerSingleton():
instance(nullptr)
{
	instance = new juce::AudioDeviceManager();
	instance->initialiseWithDefaultDevices(2, 2);
}

//------------------------------------------------------------------------------
AudioDeviceManagerSingleton::~AudioDeviceManagerSingleton()
{
	if(instance)
	{
		assert(1);
		delete instance;
	}
}
