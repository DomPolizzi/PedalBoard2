/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  6 Feb 2013 8:46:23pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_LOGDISPLAY_LOGDISPLAY_FCAF45DE__
#define __JUCER_HEADER_LOGDISPLAY_LOGDISPLAY_FCAF45DE__

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class LogDisplay  : public juce::Component,
                    public juce::ChangeListener,
                    public juce::Button::Listener
{
public:
    //==============================================================================
    LogDisplay ();
    ~LogDisplay();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Called when the LogFile is updated.
	void changeListenerCallback(juce::ChangeBroadcaster *source);

    //[/UserMethods]

    void paint (juce::Graphics& g);
    void resized();
    void buttonClicked (juce::Button* buttonThatWasClicked);



    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LogDisplay)

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	Helper method. Updates logEditor.
	void updateLog(bool fromTheBeginning);

	///	The timestamp of the most recent log event.
	juce::Time lastEvent;

    //[/UserVariables]

    //==============================================================================
    juce::TextEditor* logEditor;
    juce::TextButton* startStopButton;
    juce::ToggleButton* midiButton;
    juce::ToggleButton* oscButton;
    juce::ToggleButton* pedalboardButton;
    juce::Label* filterLabel;
};


#endif   // __JUCER_HEADER_LOGDISPLAY_LOGDISPLAY_FCAF45DE__
