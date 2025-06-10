/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  7 Nov 2011 11:39:47am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...

#include "PedalboardProcessors.h"
#include "FilePlayerControl.h"
#include "JuceHelperStuff.h"
#include "WaveformDisplay.h"
#include "ColourScheme.h"
#include "Vectors.h"

//[/Headers]

#include "AudioRecorderControl.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AudioRecorderControl::AudioRecorderControl (RecorderProcessor *proc, AudioThumbnail& thumbnail)
    : processor(proc),
      fileDisplay (0),
      filename (0),
      syncButton (0),
      recordButton (0)
{
    addAndMakeVisible (fileDisplay = new WaveformDisplayLite (thumbnail));
    fileDisplay->setName (L"fileDisplay");

    addAndMakeVisible (filename = new FilenameComponent ("filename", juce::File(), true, false, true, "*.wav", ".wav", "<no file loaded>"));
    filename->setName (L"filename");

    addAndMakeVisible (syncButton = new ToggleButton (L"syncButton"));
    syncButton->setButtonText (L"Sync to main transport");
    syncButton->addListener (this);

    addAndMakeVisible (recordButton = new DrawableButton ("recordButton", DrawableButton::ImageOnButtonBackground));
    recordButton->setName (L"recordButton");


    //[UserPreSize]

	recording = false;

	recordImage.reset(JuceHelperStuff::loadSVGFromMemory(Vectors::recordbutton_svg,
															 Vectors::recordbutton_svgSize));
	stopImage.reset(JuceHelperStuff::loadSVGFromMemory(Vectors::stopbutton_svg,
									   Vectors::stopbutton_svgSize));
	recordButton->setImages(recordImage.get());
	recordButton->setColour(DrawableButton::backgroundColourId,
						    ColourScheme::getInstance().colours[L"Button Colour"]);
	recordButton->setColour(DrawableButton::backgroundOnColourId,
						    ColourScheme::getInstance().colours[L"Button Colour"]);
	recordButton->addListener(this);
	recordButton->setTooltip(L"Record audio input");

	const File& soundFile = processor->getFile();
	if(soundFile.existsAsFile())
		filename->setCurrentFile(soundFile, true, dontSendNotification);
	else
		filename->setDefaultBrowseTarget(FilePlayerControl::lastDir);

	syncButton->setToggleState(processor->getParameter(RecorderProcessor::SyncToMainTransport) > 0.5f,
							   juce::dontSendNotification);

	filename->addListener(this);

	processor->addChangeListener(this);

    //[/UserPreSize]

    setSize (300, 100);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

AudioRecorderControl::~AudioRecorderControl()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	processor->removeChangeListener(this);

    //[/Destructor_pre]

    delete fileDisplay;
    delete filename;
    delete syncButton;
    delete recordButton;
    
    fileDisplay = nullptr;
    filename = nullptr;
    syncButton = nullptr;
    recordButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AudioRecorderControl::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AudioRecorderControl::resized()
{
    fileDisplay->setBounds (0, 28, getWidth() - 2, getHeight() - 48);
    filename->setBounds (0, 0, getWidth() - 28, 24);
    syncButton->setBounds (0, getHeight() - 23, 168, 24);
    recordButton->setBounds (getWidth() - 26, 0, 24, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AudioRecorderControl::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == syncButton)
    {
        //[UserButtonCode_syncButton] -- add your button handler code here..

		bool val = syncButton->getToggleState();

		processor->setParameter(RecorderProcessor::SyncToMainTransport,
								val ? 1.0f : 0.0f);

        //[/UserButtonCode_syncButton]
    }

    //[UserbuttonClicked_Post]
	else if(buttonThatWasClicked == recordButton)
	{
		if(!recording)
			recordButton->setImages(stopImage.get());
		else
			recordButton->setImages(recordImage.get());
		recording = !recording;

		/*if(recording)
			processor->setFile(filename->getCurrentFile());*/

		processor->setParameter(RecorderProcessor::Record, 1.0f);
	}
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
void AudioRecorderControl::filenameComponentChanged(juce::FilenameComponent *filenameComp)
{
	juce::File phil = filenameComp->getCurrentFile();
	//processor->setFile(phil);
	processor->cacheFile(phil);
	FilePlayerControl::lastDir = phil.getParentDirectory();
}

//------------------------------------------------------------------------------
void AudioRecorderControl::changeListenerCallback(juce::ChangeBroadcaster *source)
{
	if(source == processor)
	{
		if(processor->isRecording())
		{
			recordButton->setImages(stopImage.get());
			recording = true;
		}
		else
		{
			recordButton->setImages(recordImage.get());
			recording = false;
		}
		syncButton->setToggleState(processor->getParameter(RecorderProcessor::SyncToMainTransport) > 0.5f, juce::dontSendNotification);

		filename->setCurrentFile(processor->getFile(), true, juce::dontSendNotification);
	}
}

//------------------------------------------------------------------------------
void AudioRecorderControl::setWaveformBackground(const juce::Colour& col)
{
	fileDisplay->setBackgroundColour(col);
}

#if 0

#endif
