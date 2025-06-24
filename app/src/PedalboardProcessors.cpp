#include <JuceHeader.h>

#include "PedalboardProcessors.h"
#include "PedalboardProcessorEditors.h"
#include "AudioRecorderControl.h"
#include "AudioSingletons.h"
#include "MainTransport.h"
#include "LooperControl.h"

using namespace juce;

// Static background thread for audio recording
static TimeSliceThread backgroundThread("Audio Recording Thread");

//==============================================================================
// RecorderProcessor Implementation
//==============================================================================

RecorderProcessor::RecorderProcessor():
threadWriter(0),
thumbnail(512,
		  AudioFormatManagerSingleton::getInstance(),
		  AudioThumbnailCacheSingleton::getInstance()),
recording(false),
stopRecording(false),
syncToMainTransport(false),
currentRate(44100.0)
{
	setPlayConfigDetails(2, 0, 0, 0);
	MainTransport::getInstance()->addChangeListener(this);
	
	// Start the background thread if not already running
	if (!backgroundThread.isThreadRunning())
		backgroundThread.startThread();
}

//------------------------------------------------------------------------------
RecorderProcessor::~RecorderProcessor()
{
	removeAllChangeListeners();
	MainTransport::getInstance()->removeChangeListener(this);
}

//------------------------------------------------------------------------------
void RecorderProcessor::setFile(const File& phil)
{
	WavAudioFormat wavFormat;
	StringPairArray metadata;
	AudioFormatWriter *writer;
	FileOutputStream *philStream;

	if(recording)
	{
		stopRecording = true;
		Thread::sleep(100);
	}

	if(threadWriter)
	{
		delete threadWriter;
		threadWriter = 0;
	}

	soundFile = phil;

	if(soundFile.exists())
		soundFile.deleteFile();

	philStream = new FileOutputStream(soundFile);

	if(philStream->openedOk())
	{
		writer = wavFormat.createWriterFor(philStream,
										   currentRate,
										   2,
										   16,
										   metadata,
										   0);

		if(writer)
		{
			threadWriter = new AudioFormatWriter::ThreadedWriter(writer, 
																  backgroundThread,
																  32768);
		}
	}
}

//------------------------------------------------------------------------------
void RecorderProcessor::cacheFile(const File& phil)
{
	soundFile = phil;
}

//------------------------------------------------------------------------------
void RecorderProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	const int numSamples = buffer.getNumSamples();
	
	if(threadWriter && recording)
	{
		if(threadWriter->write(buffer.getArrayOfReadPointers(), numSamples))
		{
			thumbnail.addBlock(0, buffer, 0, numSamples);
		}
	}
	
	if(stopRecording)
	{
		if(threadWriter)
		{
			delete threadWriter;
			threadWriter = 0;
		}
		recording = false;
		stopRecording = false;
		sendChangeMessage();
	}
}

//------------------------------------------------------------------------------
Component *RecorderProcessor::getControls()
{
	return new AudioRecorderControl(this, thumbnail);
}

//------------------------------------------------------------------------------
void RecorderProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
	editorBounds = bounds;
}

//------------------------------------------------------------------------------
void RecorderProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
	if(source == MainTransport::getInstance())
	{
		if(syncToMainTransport)
		{
			if(MainTransport::getInstance()->getState())
			{
				if(!recording)
				{
					recording = true;
					sendChangeMessage();
				}
			}
			else
			{
				if(recording)
				{
					stopRecording = true;
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
void RecorderProcessor::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
	currentRate = sampleRate;
}

//------------------------------------------------------------------------------
void RecorderProcessor::fillInPluginDescription(PluginDescription &description) const
{
	description.name = getName();
	description.descriptiveName = getName();
	description.pluginFormatName = "Internal";
	description.category = "Effect";
	description.manufacturerName = "Niall Moody";
	description.version = "1.0";
	description.fileOrIdentifier = "RecorderProcessor";
	description.lastFileModTime = Time::getCurrentTime();
	description.isInstrument = false;
	description.numInputChannels = 2;
	description.numOutputChannels = 0;
}

//------------------------------------------------------------------------------
AudioProcessorEditor *RecorderProcessor::createEditor()
{
	return new AudioRecorderEditor(this, editorBounds, thumbnail);
}

//------------------------------------------------------------------------------
const String RecorderProcessor::getParameterName(int parameterIndex)
{
	switch(parameterIndex)
	{
		case Record:
			return "Record";
		case SyncToMainTransport:
			return "Sync To Main Transport";
		default:
			return "";
	}
}

//------------------------------------------------------------------------------
float RecorderProcessor::getParameter(int parameterIndex)
{
	switch(parameterIndex)
	{
		case Record:
			return recording ? 1.0f : 0.0f;
		case SyncToMainTransport:
			return syncToMainTransport ? 1.0f : 0.0f;
		default:
			return 0.0f;
	}
}

//------------------------------------------------------------------------------
const String RecorderProcessor::getParameterText(int parameterIndex)
{
	switch(parameterIndex)
	{
		case Record:
			return recording ? "Recording" : "Not Recording";
		case SyncToMainTransport:
			return syncToMainTransport ? "Synced" : "Not Synced";
		default:
			return "";
	}
}

//------------------------------------------------------------------------------
void RecorderProcessor::setParameter(int parameterIndex, float newValue)
{
	switch(parameterIndex)
	{
		case Record:
			if(newValue > 0.5f)
			{
				if(!recording)
				{
					recording = true;
					sendChangeMessage();
				}
			}
			else
			{
				if(recording)
				{
					stopRecording = true;
				}
			}
			break;
		case SyncToMainTransport:
			syncToMainTransport = (newValue > 0.5f);
			break;
	}
}

//------------------------------------------------------------------------------
void RecorderProcessor::getStateInformation(MemoryBlock &destData)
{
	XmlElement xml("RecorderProcessor");
	xml.setAttribute("syncToMainTransport", syncToMainTransport);
	xml.setAttribute("editorX", editorBounds.getX());
	xml.setAttribute("editorY", editorBounds.getY());
	xml.setAttribute("editorWidth", editorBounds.getWidth());
	xml.setAttribute("editorHeight", editorBounds.getHeight());
	
	copyXmlToBinary(xml, destData);
}

//------------------------------------------------------------------------------
void RecorderProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
	
	if(xml != nullptr)
	{
		syncToMainTransport = xml->getBoolAttribute("syncToMainTransport", false);
		
		int x = xml->getIntAttribute("editorX", 0);
		int y = xml->getIntAttribute("editorY", 0);
		int width = xml->getIntAttribute("editorWidth", 300);
		int height = xml->getIntAttribute("editorHeight", 100);
		
		editorBounds.setBounds(x, y, width, height);
	}
}

//==============================================================================
// LevelProcessor Implementation
//==============================================================================

LevelProcessor::LevelProcessor()
{
    setPlayConfigDetails(2, 2, 0, 0);
}

LevelProcessor::~LevelProcessor()
{
}

Component* LevelProcessor::getControls()
{
    return nullptr; // Will be implemented by editor
}

void LevelProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
    editorBounds = bounds;
}

void LevelProcessor::fillInPluginDescription(PluginDescription& description) const
{
    description.name = "Level";
    description.descriptiveName = "Level Control";
    description.pluginFormatName = "Internal";
    description.category = "Effect";
    description.manufacturerName = "Pedalboard2";
    description.version = "1.0";
    description.fileOrIdentifier = "Level";
    description.lastFileModTime = Time();
    description.lastInfoUpdateTime = Time();
    description.isInstrument = false;
    description.numInputChannels = 2;
    description.numOutputChannels = 2;
}

void LevelProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Simple level control - multiply by parameter value
    float level = getParameter(0) * 2.0f; // 0-1 parameter becomes 0-2 multiplier
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        buffer.applyGain(channel, 0, buffer.getNumSamples(), level);
    }
}

AudioProcessorEditor* LevelProcessor::createEditor()
{
    return nullptr; // Editor created externally
}

const String LevelProcessor::getParameterText(int parameterIndex)
{
    if (parameterIndex == 0)
        return String(level * 200.0f, 1) + "%";
    return "";
}

void LevelProcessor::setParameter(int parameterIndex, float newValue)
{
    if (parameterIndex == 0)
        level = newValue;
}

void LevelProcessor::getStateInformation(MemoryBlock& destData)
{
    // State saving would be implemented here
}

void LevelProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // State loading would be implemented here
}

//==============================================================================
// FilePlayerProcessor Implementation
//==============================================================================

FilePlayerProcessor::FilePlayerProcessor() :
    looping(false),
    syncToMainTransport(false),
    justPaused(false)
{
    setPlayConfigDetails(0, 2, 0, 0);
}

FilePlayerProcessor::FilePlayerProcessor(const File& phil) :
    FilePlayerProcessor()
{
    setFile(phil);
}

FilePlayerProcessor::~FilePlayerProcessor()
{
}

void FilePlayerProcessor::setFile(const File& phil)
{
    soundFile = phil;
    
    if (phil.existsAsFile())
    {
        AudioFormatManager& formatManager = AudioFormatManagerSingleton::getInstance();
        AudioFormatReader* reader = formatManager.createReaderFor(phil);
        
        if (reader != nullptr)
        {
            soundFileSource = std::make_unique<AudioFormatReaderSource>(reader, true);
            transportSource.setSource(soundFileSource.get());
        }
    }
}

Component* FilePlayerProcessor::getControls()
{
    return nullptr; // Will be implemented by editor
}

void FilePlayerProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
    editorBounds = bounds;
}

void FilePlayerProcessor::fillInPluginDescription(PluginDescription& description) const
{
    description.name = "File Player";
    description.descriptiveName = "Audio File Player";
    description.pluginFormatName = "Internal";
    description.category = "Generator";
    description.manufacturerName = "Pedalboard2";
    description.version = "1.0";
    description.fileOrIdentifier = "FilePlayer";
    description.lastFileModTime = Time();
    description.lastInfoUpdateTime = Time();
    description.isInstrument = false;
    description.numInputChannels = 0;
    description.numOutputChannels = 2;
}

void FilePlayerProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    if (soundFileSource != nullptr)
    {
        AudioSourceChannelInfo info;
        info.buffer = &buffer;
        info.startSample = 0;
        info.numSamples = buffer.getNumSamples();
        
        transportSource.getNextAudioBlock(info);
    }
    else
    {
        buffer.clear();
    }
}

void FilePlayerProcessor::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    transportSource.prepareToPlay(estimatedSamplesPerBlock, sampleRate);
}

void FilePlayerProcessor::releaseResources()
{
    transportSource.releaseResources();
}

AudioProcessorEditor* FilePlayerProcessor::createEditor()
{
    return nullptr; // Editor created externally
}

const String FilePlayerProcessor::getParameterName(int parameterIndex)
{
    switch (parameterIndex)
    {
        case Play: return "Play";
        case ReturnToZero: return "Return to Zero";
        case Looping: return "Looping";
        case ReadPosition: return "Read Position";
        case SyncToMainTransport: return "Sync to Main Transport";
        case Trigger: return "Trigger";
        default: return "";
    }
}

float FilePlayerProcessor::getParameter(int parameterIndex)
{
    switch (parameterIndex)
    {
        case Play: return transportSource.isPlaying() ? 1.0f : 0.0f;
        case ReturnToZero: return 0.0f;
        case Looping: return looping ? 1.0f : 0.0f;
        case ReadPosition: return (float)getReadPosition();
        case SyncToMainTransport: return syncToMainTransport ? 1.0f : 0.0f;
        case Trigger: return 0.0f;
        default: return 0.0f;
    }
}

void FilePlayerProcessor::setParameter(int parameterIndex, float newValue)
{
    switch (parameterIndex)
    {
        case Play:
            if (newValue > 0.5f)
                transportSource.start();
            else
                transportSource.stop();
            break;
        case ReturnToZero:
            if (newValue > 0.5f)
                transportSource.setPosition(0.0);
            break;
        case Looping:
            looping = newValue > 0.5f;
            break;
        case SyncToMainTransport:
            syncToMainTransport = newValue > 0.5f;
            break;
    }
}

void FilePlayerProcessor::changeListenerCallback(ChangeBroadcaster* source)
{
    // Handle transport changes
}

const String FilePlayerProcessor::getParameterText(int parameterIndex)
{
    switch (parameterIndex)
    {
        case Play: return transportSource.isPlaying() ? "Playing" : "Stopped";
        case Looping: return looping ? "On" : "Off";
        case SyncToMainTransport: return syncToMainTransport ? "On" : "Off";
        default: return "";
    }
}

void FilePlayerProcessor::getStateInformation(MemoryBlock& destData)
{
    // State saving would be implemented here
}

void FilePlayerProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // State loading would be implemented here
}

//==============================================================================
// OutputToggleProcessor Implementation
//==============================================================================

OutputToggleProcessor::OutputToggleProcessor() :
    toggle(false),
    fade(0.0f)
{
    setPlayConfigDetails(2, 2, 0, 0);
}

OutputToggleProcessor::~OutputToggleProcessor()
{
}

Component* OutputToggleProcessor::getControls()
{
    return nullptr; // Will be implemented by editor
}

void OutputToggleProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
    editorBounds = bounds;
}

void OutputToggleProcessor::fillInPluginDescription(PluginDescription& description) const
{
    description.name = "Output Toggle";
    description.descriptiveName = "Output Toggle Switch";
    description.pluginFormatName = "Internal";
    description.category = "Effect";
    description.manufacturerName = "Pedalboard2";
    description.version = "1.0";
    description.fileOrIdentifier = "OutputToggle";
    description.lastFileModTime = Time();
    description.lastInfoUpdateTime = Time();
    description.isInstrument = false;
    description.numInputChannels = 2;
    description.numOutputChannels = 2;
}

void OutputToggleProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    if (!toggle)
    {
        buffer.clear();
    }
}

AudioProcessorEditor* OutputToggleProcessor::createEditor()
{
    return nullptr; // Editor created externally
}

const String OutputToggleProcessor::getParameterText(int parameterIndex)
{
    if (parameterIndex == 0)
        return toggle ? "On" : "Off";
    return "";
}

void OutputToggleProcessor::setParameter(int parameterIndex, float newValue)
{
    if (parameterIndex == 0)
        toggle = newValue > 0.5f;
}

void OutputToggleProcessor::getStateInformation(MemoryBlock& destData)
{
    // State saving would be implemented here
}

void OutputToggleProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // State loading would be implemented here
}

//==============================================================================
// VuMeterProcessor Implementation
//==============================================================================

VuMeterProcessor::VuMeterProcessor() :
    levelLeft(0.0f),
    levelRight(0.0f)
{
    setPlayConfigDetails(2, 2, 0, 0);
}

VuMeterProcessor::~VuMeterProcessor()
{
}

Component* VuMeterProcessor::getControls()
{
    return nullptr; // Will be implemented by editor
}

void VuMeterProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
    editorBounds = bounds;
}

void VuMeterProcessor::fillInPluginDescription(PluginDescription& description) const
{
    description.name = "VU Meter";
    description.descriptiveName = "VU Level Meter";
    description.pluginFormatName = "Internal";
    description.category = "Analyser";
    description.manufacturerName = "Pedalboard2";
    description.version = "1.0";
    description.fileOrIdentifier = "VuMeter";
    description.lastFileModTime = Time();
    description.lastInfoUpdateTime = Time();
    description.isInstrument = false;
    description.numInputChannels = 2;
    description.numOutputChannels = 2;
}

void VuMeterProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Calculate RMS levels
    if (buffer.getNumChannels() > 0)
        levelLeft = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
    if (buffer.getNumChannels() > 1)
        levelRight = buffer.getRMSLevel(1, 0, buffer.getNumSamples());
    else
        levelRight = levelLeft;
    
    // Pass audio through unchanged
}

AudioProcessorEditor* VuMeterProcessor::createEditor()
{
    return nullptr; // Editor created externally
}

void VuMeterProcessor::getStateInformation(MemoryBlock& destData)
{
    // State saving would be implemented here
}

void VuMeterProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // State loading would be implemented here
}

//==============================================================================
// MetronomeProcessor Implementation
//==============================================================================

MetronomeProcessor::MetronomeProcessor() :
    playing(false),
    syncToMainTransport(false),
    numerator(4),
    denominator(4),
    sineX0(0.0f),
    sineX1(0.0f),
    sineCoeff(0.0f),
    sineEnv(0.0f),
    clickCount(0.0f),
    clickDec(0.0f),
    measureCount(0),
    isAccent(false)
{
    setPlayConfigDetails(0, 2, 0, 0);
}

MetronomeProcessor::~MetronomeProcessor()
{
}

void MetronomeProcessor::setAccentFile(const File& phil)
{
    files[0] = phil;
    
    if (phil.existsAsFile())
    {
        AudioFormatManager& formatManager = AudioFormatManagerSingleton::getInstance();
        AudioFormatReader* reader = formatManager.createReaderFor(phil);
        
        if (reader != nullptr)
        {
            soundFileSource[0] = std::make_unique<AudioFormatReaderSource>(reader, true);
            transportSource[0].setSource(soundFileSource[0].get());
        }
    }
}

void MetronomeProcessor::setClickFile(const File& phil)
{
    files[1] = phil;
    
    if (phil.existsAsFile())
    {
        AudioFormatManager& formatManager = AudioFormatManagerSingleton::getInstance();
        AudioFormatReader* reader = formatManager.createReaderFor(phil);
        
        if (reader != nullptr)
        {
            soundFileSource[1] = std::make_unique<AudioFormatReaderSource>(reader, true);
            transportSource[1].setSource(soundFileSource[1].get());
        }
    }
}

Component* MetronomeProcessor::getControls()
{
    return nullptr; // Will be implemented by editor
}

void MetronomeProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
    editorBounds = bounds;
}

void MetronomeProcessor::fillInPluginDescription(PluginDescription& description) const
{
    description.name = "Metronome";
    description.descriptiveName = "Metronome Click Track";
    description.pluginFormatName = "Internal";
    description.category = "Generator";
    description.manufacturerName = "Pedalboard2";
    description.version = "1.0";
    description.fileOrIdentifier = "Metronome";
    description.lastFileModTime = Time();
    description.lastInfoUpdateTime = Time();
    description.isInstrument = false;
    description.numInputChannels = 0;
    description.numOutputChannels = 2;
}

void MetronomeProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    if (!playing)
    {
        buffer.clear();
        return;
    }
    
    // Simple metronome implementation - would need proper timing logic
    buffer.clear();
}

AudioProcessorEditor* MetronomeProcessor::createEditor()
{
    return nullptr; // Editor created externally
}

const String MetronomeProcessor::getParameterName(int parameterIndex)
{
    switch (parameterIndex)
    {
        case Play: return "Play";
        case Numerator: return "Numerator";
        case Denominator: return "Denominator";
        case SyncToMainTransport: return "Sync to Main Transport";
        default: return "";
    }
}

float MetronomeProcessor::getParameter(int parameterIndex)
{
    switch (parameterIndex)
    {
        case Play: return playing ? 1.0f : 0.0f;
        case Numerator: return (float)numerator / 16.0f;
        case Denominator: return (float)denominator / 16.0f;
        case SyncToMainTransport: return syncToMainTransport ? 1.0f : 0.0f;
        default: return 0.0f;
    }
}

void MetronomeProcessor::setParameter(int parameterIndex, float newValue)
{
    switch (parameterIndex)
    {
        case Play:
            playing = newValue > 0.5f;
            break;
        case Numerator:
            numerator = jmax(1, (int)(newValue * 16.0f));
            break;
        case Denominator:
            denominator = jmax(1, (int)(newValue * 16.0f));
            break;
        case SyncToMainTransport:
            syncToMainTransport = newValue > 0.5f;
            break;
    }
}

void MetronomeProcessor::changeListenerCallback(ChangeBroadcaster* source)
{
    // Handle transport changes
}

const String MetronomeProcessor::getParameterText(int parameterIndex)
{
    switch (parameterIndex)
    {
        case Play: return playing ? "Playing" : "Stopped";
        case Numerator: return String(numerator);
        case Denominator: return String(denominator);
        case SyncToMainTransport: return syncToMainTransport ? "On" : "Off";
        default: return "";
    }
}

void MetronomeProcessor::getStateInformation(MemoryBlock& destData)
{
    // State saving would be implemented here
}

void MetronomeProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // State loading would be implemented here
}

//==============================================================================
// LooperProcessor Implementation
//==============================================================================

LooperProcessor::LooperProcessor() :
    playing(false),
    stopPlaying(false),
    recording(false),
    stopRecording(false),
    syncToMainTransport(false),
    stopAfterBar(false),
    autoPlay(false),
    inputLevel(1.0f),
    loopLevel(1.0f),
    threadWriter(nullptr),
    thumbnail(512, AudioFormatManagerSingleton::getInstance(), AudioThumbnailCacheSingleton::getInstance()),
    numerator(4),
    denominator(4)
{
    setPlayConfigDetails(2, 2, 0, 0);
}

LooperProcessor::~LooperProcessor()
{
    if (threadWriter != nullptr)
    {
        delete threadWriter;
        threadWriter = nullptr;
    }
}

void LooperProcessor::setFile(const File& phil)
{
    soundFile = phil;
}

Component* LooperProcessor::getControls()
{
    return nullptr; // Will be implemented by editor
}

void LooperProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
    editorBounds = bounds;
}

void LooperProcessor::fillInPluginDescription(PluginDescription& description) const
{
    description.name = "Looper";
    description.descriptiveName = "Audio Looper";
    description.pluginFormatName = "Internal";
    description.category = "Effect";
    description.manufacturerName = "Pedalboard2";
    description.version = "1.0";
    description.fileOrIdentifier = "Looper";
    description.lastFileModTime = Time();
    description.lastInfoUpdateTime = Time();
    description.isInstrument = false;
    description.numInputChannels = 2;
    description.numOutputChannels = 2;
}

void LooperProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Simple pass-through for now - would implement proper looping logic
}

void LooperProcessor::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    // Prepare for playback
}

AudioProcessorEditor* LooperProcessor::createEditor()
{
    return nullptr; // Editor created externally
}

const String LooperProcessor::getParameterName(int parameterIndex)
{
    switch (parameterIndex)
    {
        case Play: return "Play";
        case ReturnToZero: return "Return to Zero";
        case Record: return "Record";
        case ReadPosition: return "Read Position";
        case SyncToMainTransport: return "Sync to Main Transport";
        case StopAfterBar: return "Stop After Bar";
        case AutoPlay: return "Auto Play";
        case BarNumerator: return "Bar Numerator";
        case BarDenominator: return "Bar Denominator";
        case InputLevel: return "Input Level";
        case LoopLevel: return "Loop Level";
        default: return "";
    }
}

float LooperProcessor::getParameter(int parameterIndex)
{
    switch (parameterIndex)
    {
        case Play: return playing ? 1.0f : 0.0f;
        case Record: return recording ? 1.0f : 0.0f;
        case ReadPosition: return (float)getReadPosition();
        case SyncToMainTransport: return syncToMainTransport ? 1.0f : 0.0f;
        case StopAfterBar: return stopAfterBar ? 1.0f : 0.0f;
        case AutoPlay: return autoPlay ? 1.0f : 0.0f;
        case BarNumerator: return (float)numerator / 16.0f;
        case BarDenominator: return (float)denominator / 16.0f;
        case InputLevel: return inputLevel;
        case LoopLevel: return loopLevel;
        default: return 0.0f;
    }
}

void LooperProcessor::setParameter(int parameterIndex, float newValue)
{
    switch (parameterIndex)
    {
        case Play:
            playing = newValue > 0.5f;
            break;
        case Record:
            recording = newValue > 0.5f;
            break;
        case SyncToMainTransport:
            syncToMainTransport = newValue > 0.5f;
            break;
        case StopAfterBar:
            stopAfterBar = newValue > 0.5f;
            break;
        case AutoPlay:
            autoPlay = newValue > 0.5f;
            break;
        case BarNumerator:
            numerator = jmax(1, (int)(newValue * 16.0f));
            break;
        case BarDenominator:
            denominator = jmax(1, (int)(newValue * 16.0f));
            break;
        case InputLevel:
            inputLevel = newValue;
            break;
        case LoopLevel:
            loopLevel = newValue;
            break;
    }
}

double LooperProcessor::getReadPosition() const
{
    return 0.0; // Would implement proper read position tracking
}

void LooperProcessor::handleAsyncUpdate()
{
    // Handle async recording start
}

int LooperProcessor::useTimeSlice()
{
    // Handle buffer allocation
    return 100; // Return time until next call
}

void LooperProcessor::changeListenerCallback(ChangeBroadcaster* source)
{
    // Handle transport changes
}

const String LooperProcessor::getParameterText(int parameterIndex)
{
    switch (parameterIndex)
    {
        case Play: return playing ? "Playing" : "Stopped";
        case Record: return recording ? "Recording" : "Stopped";
        case SyncToMainTransport: return syncToMainTransport ? "On" : "Off";
        case StopAfterBar: return stopAfterBar ? "On" : "Off";
        case AutoPlay: return autoPlay ? "On" : "Off";
        case BarNumerator: return String(numerator);
        case BarDenominator: return String(denominator);
        case InputLevel: return String(inputLevel * 100.0f, 1) + "%";
        case LoopLevel: return String(loopLevel * 100.0f, 1) + "%";
        default: return "";
    }
}

void LooperProcessor::getStateInformation(MemoryBlock& destData)
{
    // State saving would be implemented here
}

void LooperProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // State loading would be implemented here
}
