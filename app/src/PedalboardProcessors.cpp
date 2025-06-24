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

//------------------------------------------------------------------------------
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
