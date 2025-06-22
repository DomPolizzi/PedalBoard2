/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  20 Oct 2012 7:09:35pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_LOOPEREDITOR_LOOPEREDITOR_89E8DC78__
#define __JUCER_HEADER_LOOPEREDITOR_LOOPEREDITOR_89E8DC78__

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "WaveformDisplay.h"

class LooperProcessor;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class LooperEditor  : public juce::AudioProcessorEditor,
                      public juce::FilenameComponentListener,
                      public juce::Timer,
                      public juce::ChangeListener,
                      public juce::Button::Listener,
                      public juce::Label::Listener,
                      public juce::Slider::Listener
{
public:
    //==============================================================================
    LooperEditor (LooperProcessor *proc, AudioThumbnail *thumbnail);
    ~LooperEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Used to load a sound file.
	void filenameComponentChanged(juce::FilenameComponent *filenameComp);
	///	Used to update the read position graphic.
	void timerCallback();
	///	Used to change the player's read position when the user clicks on the WaveformDisplay.
	void changeListenerCallback(juce::ChangeBroadcaster *source);

	///	Changes the colour of the WaveformDisplay background.
	void setWaveformBackground(const juce::Colour& col);
	///	Clears the waveform display.
	void clearDisplay();

    //[/UserMethods]

    void paint (juce::Graphics& g);
    void resized();
    void buttonClicked (juce::Button* buttonThatWasClicked);
    void labelTextChanged (juce::Label* labelThatHasChanged);
    void sliderValueChanged (juce::Slider* sliderThatWasMoved);



    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LooperEditor)

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	Our copy of the AudioProcessor pointer.
	LooperProcessor *processor;

	///	The two drawables we use for the playButton.
	std::unique_ptr<juce::Drawable> playImage;
	std::unique_ptr<juce::Drawable> pauseImage;
	///	Whether the playPauseButton is currently displaying the play icon.
	bool playing;

	///	The two drawables we use for the recordButton.
	std::unique_ptr<juce::Drawable> recordImage;
	std::unique_ptr<juce::Drawable> stopImage;
	///	Whether the recordButton is currently displaying the record icon.
	bool recording;

    //[/UserVariables]

    //==============================================================================
    WaveformDisplay* fileDisplay;
    juce::FilenameComponent* filename;
    juce::ToggleButton* syncButton;
    juce::ToggleButton* stopAfterBarButton;
    juce::DrawableButton* playPauseButton;
    juce::DrawableButton* rtzButton;
    juce::DrawableButton* recordButton;
    juce::ToggleButton* autoPlayButton;
    juce::Label* barLengthLabel;
    juce::Label* separatorLabel;
    juce::Label* numeratorLabel;
    juce::Label* denominatorLabel;
    juce::Label* loopLevelLabel;
    juce::Slider* loopLevelSlider;
    juce::Label* inputLevelLabel;
    juce::Slider* inputLevelSlider;


    //==============================================================================
    // Note: Copy constructor and operator= are handled by JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR
};


#endif   // __JUCER_HEADER_LOOPEREDITOR_LOOPEREDITOR_89E8DC78__
