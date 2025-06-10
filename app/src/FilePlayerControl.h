/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  16 Feb 2012 11:00:24am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_FILEPLAYERCONTROL_FILEPLAYERCONTROL_54B42392__
#define __JUCER_HEADER_FILEPLAYERCONTROL_FILEPLAYERCONTROL_54B42392__

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "WaveformDisplay.h"

class FilePlayerProcessor;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class FilePlayerControl  : public juce::Component,
                           public juce::FilenameComponentListener,
                           public juce::Timer,
                           public juce::ChangeListener,
                           public juce::Button::Listener
{
public:
    //==========================================================================
    FilePlayerControl (FilePlayerProcessor *proc);
    ~FilePlayerControl();

    //==========================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Used to load a sound file.
	void filenameComponentChanged(juce::FilenameComponent *filenameComp);
	///	Used to update the read position graphic.
	void timerCallback();
	///	Used to change the player's read position when the user clicks on the WaveformDisplay.
	void changeListenerCallback(juce::ChangeBroadcaster *source);

	///	Changes the colour of the WaveformDisplay background.
	void setWaveformBackground(const juce::Colour& col);

	///	Used to keep track of the last directory the user loaded a sound from.
	static juce::File lastDir;

    //[/UserMethods]

    void paint (juce::Graphics& g);
    void resized();
    void buttonClicked (juce::Button* buttonThatWasClicked);



private:
	JUCE_LEAK_DETECTOR(FilePlayerControl)

    //[UserVariables]   -- You can add your own custom variables in this section.

	///	Our copy of the AudioProcessor pointer.
	FilePlayerProcessor *processor;

	///	The two drawables we use for the playButton.
	std::unique_ptr<juce::Drawable> playImage;
	std::unique_ptr<juce::Drawable> pauseImage;
	///	Whether the playPauseButton is currently displaying the play icon.
	bool playing;

    //[/UserVariables]

    //==========================================================================
    WaveformDisplay* fileDisplay;
    juce::FilenameComponent* filename;
    juce::ToggleButton* syncButton;
    juce::ToggleButton* loopButton;
    juce::DrawableButton* playPauseButton;
    juce::DrawableButton* rtzButton;


    //==========================================================================
    // (prevent copy constructor and operator= being generated..)
    FilePlayerControl (const FilePlayerControl&);
    const FilePlayerControl& operator= (const FilePlayerControl&);
};


#endif   // __JUCER_HEADER_FILEPLAYERCONTROL_FILEPLAYERCONTROL_54B42392__
