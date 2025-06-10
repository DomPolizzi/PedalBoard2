#ifndef __JUCER_HEADER_AUDIORECORDERCONTROL_AUDIORECORDERCONTROL_H__
#define __JUCER_HEADER_AUDIORECORDERCONTROL_AUDIORECORDERCONTROL_H__

#include <JuceHeader.h>

class RecorderProcessor;
class WaveformDisplayLite;

class AudioRecorderControl  : public juce::Component,
                              public juce::FilenameComponentListener,
                              public juce::ChangeListener,
                              public juce::Button::Listener
{

public:
    AudioRecorderControl (RecorderProcessor *proc, juce::AudioThumbnail& thumbnail);
    ~AudioRecorderControl();

	void filenameComponentChanged(juce::FilenameComponent *filenameComp);
	void changeListenerCallback(juce::ChangeBroadcaster *source);
	void setWaveformBackground(const juce::Colour& col);

    void paint (juce::Graphics& g);
    void resized();
    void buttonClicked (juce::Button* buttonThatWasClicked);

private:
	JUCE_LEAK_DETECTOR(AudioRecorderControl)

	RecorderProcessor *processor;

	std::unique_ptr<juce::Drawable> recordImage;
	std::unique_ptr<juce::Drawable> stopImage;
	bool recording;

    WaveformDisplayLite* fileDisplay;
    juce::FilenameComponent* filename;
    juce::ToggleButton* syncButton;
    juce::DrawableButton* recordButton;



    AudioRecorderControl (const AudioRecorderControl&);
    const AudioRecorderControl& operator= (const AudioRecorderControl&);
};

#endif   // __JUCER_HEADER_AUDIORECORDERCONTROL_AUDIORECORDERCONTROL_H__
