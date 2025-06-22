/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  20 Oct 2012 7:09:23pm

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
#include "JuceHelperStuff.h"
#include "ColourScheme.h"
#include "Vectors.h"

//[/Headers]

#include "LooperControl.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

juce::File LooperControl::lastDir(juce::File::getSpecialLocation(juce::File::userHomeDirectory));

//[/MiscUserDefs]

//==============================================================================
LooperControl::LooperControl (LooperProcessor *proc, AudioThumbnail *thumbnail)
    : processor(proc),
      fileDisplay (0),
      filename (0),
      syncButton (0),
      stopAfterBarButton (0),
      playPauseButton (0),
      rtzButton (0),
      recordButton (0)
{
    addAndMakeVisible (fileDisplay = new WaveformDisplay (thumbnail, false));
    fileDisplay->setName ("fileDisplay");

    addAndMakeVisible (filename = new juce::FilenameComponent ("filename", juce::File(), true, false, true, "*.wav;*.aif", "", "<no file loaded>"));
    filename->setName ("filename");

    addAndMakeVisible (syncButton = new juce::ToggleButton ("syncButton"));
    syncButton->setTooltip ("Sync file playback to the main transport");
    syncButton->setButtonText ("Sync to main transport");
    syncButton->addListener (this);

    addAndMakeVisible (stopAfterBarButton = new juce::ToggleButton ("stopAfterBarButton"));
    stopAfterBarButton->setTooltip ("Stop recording after a bar has elapsed.");
    stopAfterBarButton->setButtonText ("Stop after bar");
    stopAfterBarButton->addListener (this);

    addAndMakeVisible (playPauseButton = new juce::DrawableButton ("playPauseButton", juce::DrawableButton::ImageOnButtonBackground));
    playPauseButton->setName ("playPauseButton");

    addAndMakeVisible (rtzButton = new juce::DrawableButton ("rtzButton", juce::DrawableButton::ImageOnButtonBackground));
    rtzButton->setName ("rtzButton");

    addAndMakeVisible (recordButton = new juce::DrawableButton ("recordButton", juce::DrawableButton::ImageOnButtonBackground));
    recordButton->setName ("recordButton");


    //[UserPreSize]
	std::unique_ptr<juce::Drawable> rtzImage(JuceHelperStuff::loadSVGFromMemory(Vectors::rtzbutton_svg,
																	    Vectors::rtzbutton_svgSize));

	playing = false;

	playImage.reset(JuceHelperStuff::loadSVGFromMemory(Vectors::playbutton_svg,
												   Vectors::playbutton_svgSize));
	pauseImage.reset(JuceHelperStuff::loadSVGFromMemory(Vectors::pausebutton_svg,
												    Vectors::pausebutton_svgSize));
	playPauseButton->setImages(playImage.get());
	playPauseButton->setColour(juce::DrawableButton::backgroundColourId,
							   ColourScheme::getInstance().colours["Button Colour"]);
	playPauseButton->setColour(juce::DrawableButton::backgroundOnColourId,
							   ColourScheme::getInstance().colours["Button Colour"]);
	playPauseButton->addListener(this);
	playPauseButton->setTooltip("Play/pause audio file");

	recording = false;

	recordImage.reset(JuceHelperStuff::loadSVGFromMemory(Vectors::recordbutton_svg,
													 Vectors::recordbutton_svgSize));
	stopImage.reset(JuceHelperStuff::loadSVGFromMemory(Vectors::stopbutton_svg,
												   Vectors::stopbutton_svgSize));
	recordButton->setImages(recordImage.get());
	recordButton->setColour(juce::DrawableButton::backgroundColourId,
						    ColourScheme::getInstance().colours["Button Colour"]);
	recordButton->setColour(juce::DrawableButton::backgroundOnColourId,
						    ColourScheme::getInstance().colours["Button Colour"]);
	recordButton->addListener(this);
	recordButton->setTooltip("Record/stop recording a loop");

	//Used to make sure we're playing if the processor is already playing.
	changeListenerCallback(processor);

	rtzButton->setImages(rtzImage.get());
	rtzButton->setColour(juce::DrawableButton::backgroundColourId,
					     ColourScheme::getInstance().colours["Button Colour"]);
	rtzButton->setColour(juce::DrawableButton::backgroundOnColourId,
					     ColourScheme::getInstance().colours["Button Colour"]);
	rtzButton->addListener(this);
	rtzButton->setTooltip("Return to the start of the audio file");

	const File& soundFile = processor->getFile();
	if(soundFile != juce::File())
	{
		filename->setCurrentFile(soundFile, true, juce::dontSendNotification);
		fileDisplay->setFile(soundFile);
		fileDisplay->setReadPointer((float)processor->getReadPosition());
	}
	else
		filename->setDefaultBrowseTarget(lastDir);

	syncButton->setToggleState(processor->getParameter(LooperProcessor::SyncToMainTransport) > 0.5f,
						   juce::sendNotification);
	stopAfterBarButton->setToggleState(processor->getParameter(LooperProcessor::StopAfterBar) > 0.5f,
							   juce::sendNotification);

	filename->addListener(this);
	fileDisplay->addChangeListener(this);
	processor->addChangeListener(this);

	startTimer(60);

    //[/UserPreSize]

    setSize (300, 100);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

LooperControl::~LooperControl()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	processor->removeChangeListener(this);

    //[/Destructor_pre]

    delete fileDisplay; fileDisplay = nullptr;
    delete filename; filename = nullptr;
    delete syncButton; syncButton = nullptr;
    delete stopAfterBarButton; stopAfterBarButton = nullptr;
    delete playPauseButton; playPauseButton = nullptr;
    delete rtzButton; rtzButton = nullptr;
    delete recordButton; recordButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void LooperControl::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void LooperControl::resized()
{
    fileDisplay->setBounds (0, 28, getWidth() - 2, getHeight() - 48);
    filename->setBounds (0, 0, getWidth() - 84, 24);
    syncButton->setBounds (0, getHeight() - 23, 168, 24);
    stopAfterBarButton->setBounds (176, getHeight() - 23, 112, 24);
    playPauseButton->setBounds (getWidth() - 80, 0, 24, 24);
    rtzButton->setBounds (getWidth() - 52, 0, 24, 24);
    recordButton->setBounds (getWidth() - 24, 0, 24, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void LooperControl::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == syncButton)
    {
        //[UserButtonCode_syncButton] -- add your button handler code here..

		bool val = syncButton->getToggleState();

		processor->setParameter(LooperProcessor::SyncToMainTransport,
								val ? 1.0f : 0.0f);

        //[/UserButtonCode_syncButton]
    }
    else if (buttonThatWasClicked == stopAfterBarButton)
    {
        //[UserButtonCode_stopAfterBarButton] -- add your button handler code here..

		bool val = stopAfterBarButton->getToggleState();

		processor->setParameter(LooperProcessor::StopAfterBar,
								val ? 1.0f : 0.0f);

        //[/UserButtonCode_stopAfterBarButton]
    }

    //[UserbuttonClicked_Post]
	else if(buttonThatWasClicked == playPauseButton)
	{
		if(!playing)
			playPauseButton->setImages(pauseImage.get());
		else
			playPauseButton->setImages(playImage.get());
		playing = !playing;
		processor->setParameter(LooperProcessor::Play, 1.0f);
	}
	else if(buttonThatWasClicked == rtzButton)
	{
		processor->setParameter(LooperProcessor::ReturnToZero, 1.0f);
		fileDisplay->setReadPointer(0.0f);
	}
	else if(buttonThatWasClicked == recordButton)
	{
		recording = !recording;

		if(recording)
		{
			//fileDisplay->setFile(File::nonexistent);
			if(playing)
			{
				playPauseButton->setImages(playImage.get());
				playing = false;
				processor->setParameter(LooperProcessor::Play, 1.0f);
			}

			//processor->setFile(filename->getCurrentFile());
		}

		processor->setParameter(LooperProcessor::Record, 1.0f);
	}

	//So LooperEditor gets updated too.
	processor->sendChangeMessage();

    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
void LooperControl::filenameComponentChanged(juce::FilenameComponent *filenameComp)
{
	juce::File phil = filenameComp->getCurrentFile();
	fileDisplay->setFile(phil);
	processor->setFile(phil);
	lastDir = phil.getParentDirectory();

	//So LooperEditor gets updated too.
	processor->sendChangeMessage();
}

//------------------------------------------------------------------------------
void LooperControl::timerCallback()
{
	if(playing)
		fileDisplay->setReadPointer((float)processor->getReadPosition());
}

//------------------------------------------------------------------------------
void LooperControl::changeListenerCallback(juce::ChangeBroadcaster *source)
{
	if(source == fileDisplay)
	{
		processor->setParameter(LooperProcessor::ReadPosition,
								fileDisplay->getReadPointer());
		fileDisplay->setReadPointer((float)processor->getReadPosition());
	}
	else if(source == processor)
	{
		if(processor->getNewFileLoaded())
			fileDisplay->setFile(processor->getFile());

		if(filename->getCurrentFile() != processor->getFile())
			filename->setCurrentFile(processor->getFile(), true, juce::dontSendNotification);

		if(processor->isRecording())
		{
			playPauseButton->setImages(playImage.get());
			playPauseButton->setEnabled(false);

			if(!recording)
				fileDisplay->setFile(juce::File());
			recordButton->setImages(stopImage.get());
			recording = true;
		}
		else
		{
			playPauseButton->setEnabled(true);
			recordButton->setImages(recordImage.get());
			recording = false;

			if(processor->isPlaying())
			{
				playPauseButton->setImages(pauseImage.get());
				playing = true;
			}
			else
			{
				playPauseButton->setImages(playImage.get());
				playing = false;
			}
		}
		fileDisplay->setReadPointer((float)processor->getReadPosition());
		syncButton->setToggleState(processor->getParameter(LooperProcessor::SyncToMainTransport) > 0.5f, juce::sendNotification);
		stopAfterBarButton->setToggleState(processor->getParameter(LooperProcessor::StopAfterBar) > 0.5f, juce::sendNotification);
	}
}

//------------------------------------------------------------------------------
void LooperControl::setWaveformBackground(const juce::Colour& col)
{
	fileDisplay->setBackgroundColour(col);
}

//------------------------------------------------------------------------------
void LooperControl::clearDisplay()
{
	fileDisplay->setFile(juce::File());
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LooperContro" componentName=""
                 parentClasses="public Component, public FilenameComponentListener, public Timer, public ChangeListener"
                 constructorParams="LooperProcessor *proc, AudioThumbnail *thumbnai"
                 variableInitialisers="processor(proc)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330000013" fixedSize="0" initialWidth="300"
                 initialHeight="100">
  <BACKGROUND backgroundColour="eeece1"/>
  <GENERICCOMPONENT name="fileDisplay" id="ca679484fd8a4509" memberName="fileDisplay"
                    virtualName="" explicitFocusOrder="0" pos="0 28 2M 48M" class="WaveformDisplay"
                    params="thumbnail, false"/>
  <GENERICCOMPONENT name="filename" id="f1e80797e31ff742" memberName="filename" virtualName=""
                    explicitFocusOrder="0" pos="0 0 84M 24" class="FilenameComponent"
                    params="&quot;filename&quot;, juce::File(), true, false, true, &quot;*.wav;*.aif&quot;, &quot;&quot;, &quot;&lt;no file loaded&gt;&quot;"/>
  <TOGGLEBUTTON name="syncButton" id="b0788f5602fb3d7f" memberName="syncButton"
                virtualName="" explicitFocusOrder="0" pos="0 23R 168 24" tooltip="Sync file playback to the main transport"
                buttonText="Sync to main transport" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="stopAfterBarButton" id="eb49769414af66b2" memberName="stopAfterBarButton"
                virtualName="" explicitFocusOrder="0" pos="176 23R 112 24" tooltip="Stop recording after a bar has elapsed."
                buttonText="Stop after bar" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="0"/>
  <GENERICCOMPONENT name="playPauseButton" id="da66be207abe8144" memberName="playPauseButton"
                    virtualName="" explicitFocusOrder="0" pos="80R 0 24 24" class="DrawableButton"
                    params="&quot;playPauseButton&quot;, DrawableButton::ImageOnButtonBackground"/>
  <GENERICCOMPONENT name="rtzButton" id="8087375604e93800" memberName="rtzButton"
                    virtualName="" explicitFocusOrder="0" pos="52R 0 24 24" class="DrawableButton"
                    params="&quot;rtzButton&quot;, DrawableButton::ImageOnButtonBackground"/>
  <GENERICCOMPONENT name="recordButton" id="9d6c14be2cdfd38e" memberName="recordButton"
                    virtualName="" explicitFocusOrder="0" pos="24R 0 24 24" class="DrawableButton"
                    params="&quot;recordButton&quot;, DrawableButton::ImageOnButtonBackground"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

