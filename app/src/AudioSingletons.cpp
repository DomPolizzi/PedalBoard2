//	AudioSingletons.cpp - A couple of singleton wrappers to some of JUCE' audio
//						  classes.

#include "AudioSingletons.h"

#include <cassert>

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AudioPluginFormatManagerSingleton AudioPluginFormatManagerSingleton::singletonInstance;

//------------------------------------------------------------------------------
AudioPluginFormatManager& AudioPluginFormatManagerSingleton::getInstance()
{
	return *(singletonInstance.instance);
}

//------------------------------------------------------------------------------
void AudioPluginFormatManagerSingleton::killInstance()
{
	assert(singletonInstance.instance != 0);

	if(singletonInstance.instance)
	{
		delete singletonInstance.instance;
		singletonInstance.instance = 0;
	}
}

//------------------------------------------------------------------------------
AudioPluginFormatManagerSingleton::AudioPluginFormatManagerSingleton():
instance(0)
{
	instance = new AudioPluginFormatManager();
	instance->addDefaultFormats();
}

//------------------------------------------------------------------------------
AudioPluginFormatManagerSingleton::~AudioPluginFormatManagerSingleton()
{
	if(instance)
	{
		assert(1);
		delete instance;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AudioFormatManagerSingleton AudioFormatManagerSingleton::singletonInstance;

//------------------------------------------------------------------------------
AudioFormatManager& AudioFormatManagerSingleton::getInstance()
{
	return *(singletonInstance.instance);
}

//------------------------------------------------------------------------------
void AudioFormatManagerSingleton::killInstance()
{
	assert(singletonInstance.instance != 0);

	if(singletonInstance.instance)
	{
		delete singletonInstance.instance;
		singletonInstance.instance = 0;
	}
}

//------------------------------------------------------------------------------
AudioFormatManagerSingleton::AudioFormatManagerSingleton():
instance(0)
{
	instance = new AudioFormatManager();
	instance->registerBasicFormats();
}

//------------------------------------------------------------------------------
AudioFormatManagerSingleton::~AudioFormatManagerSingleton()
{
	if(instance)
	{
		assert(1);
		delete instance;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AudioThumbnailCacheSingleton AudioThumbnailCacheSingleton::singletonInstance;

//------------------------------------------------------------------------------
AudioThumbnailCache& AudioThumbnailCacheSingleton::getInstance()
{
	return *(singletonInstance.instance);
}

//------------------------------------------------------------------------------
void AudioThumbnailCacheSingleton::killInstance()
{
	assert(singletonInstance.instance != 0);

	if(singletonInstance.instance)
	{
		delete singletonInstance.instance;
		singletonInstance.instance = 0;
	}
}

//------------------------------------------------------------------------------
AudioThumbnailCacheSingleton::AudioThumbnailCacheSingleton():
instance(0)
{
	instance = new AudioThumbnailCache(32);
}

//------------------------------------------------------------------------------
AudioThumbnailCacheSingleton::~AudioThumbnailCacheSingleton()
{
	if(instance)
	{
		assert(1);
		delete instance;
	}
}
