//	WaveformDisplay.h - A component that displays a waveform.

#ifndef WAVEFORMDISPLAY_H_
#define WAVEFORMDISPLAY_H_

#include <JuceHeader.h>

///	A component that displays a waveform.
class WaveformDisplay : public juce::Component,
						public juce::ChangeListener,
						public juce::ChangeBroadcaster
{
  public:
	///	Constructor.
	WaveformDisplay(juce::AudioThumbnail *thumb = 0, bool deleteThumb = true);
	///	Destructor.
	~WaveformDisplay();

	///	Draws the background and the waveform.
	void paint(juce::Graphics& g);
	///	So we can update the thumbnail as it is loaded.
	void changeListenerCallback(juce::ChangeBroadcaster *source);

	///	So the user can click and drag to move through the sound file.
	void mouseDown(const juce::MouseEvent &e);
	///	So the user can click and drag to move through the sound file.
	void mouseDrag(const juce::MouseEvent &e);

	///	Loads an audio file.
	void setFile(const juce::File& file);
	///	Sets the position of the readPointer.
	void setReadPointer(float val);
	///	Returns where the user clicked to move the readPointer.
	float getReadPointer() const {return newReadPointer;};

	///	Sets the background colour.
	void setBackgroundColour(const juce::Colour& col);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
  private:
	///	The current thumbnail.
    juce::AudioThumbnail *thumbnail;

	///	The current position of the read pointer.
	float readPointer;
	///	Where the user clicked to move the read pointer.
	float newReadPointer;

	///	The background colour.
	juce::Colour backgroundColour;

	///	True if we should delete the thumbnail in our constructor.
	bool deleteThumbnail;
};

//------------------------------------------------------------------------------
///	A simplified component that displays a waveform.
class WaveformDisplayLite : public juce::Component,
							public juce::ChangeListener
{
  public:
	///	Constructor.
	WaveformDisplayLite(juce::AudioThumbnail& thumb);
	///	Destructor.
	~WaveformDisplayLite();

	///	Draws the background and the waveform.
	void paint(juce::Graphics& g);
	///	So we can update the thumbnail as it is loaded.
	void changeListenerCallback(juce::ChangeBroadcaster *source);

	///	Sets the background colour.
	void setBackgroundColour(const juce::Colour& col);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplayLite)
  private:
	///	The current thumbnail.
    juce::AudioThumbnail &thumbnail;

	///	The background colour.
	juce::Colour backgroundColour;
};

#endif
