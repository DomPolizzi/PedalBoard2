//	WaveformDisplay.h - A component that displays a waveform.

#ifndef WAVEFORMDISPLAY_H_
#define WAVEFORMDISPLAY_H_

#include <JuceHeader.h>

///	A component that displays a waveform.
class WaveformDisplay : public Component,
						public ChangeListener,
						public ChangeBroadcaster
{
  public:
	///	Constructor.
	WaveformDisplay(AudioThumbnail *thumb = 0, bool deleteThumb = true);
	///	Destructor.
	~WaveformDisplay();

	///	Draws the background and the waveform.
	void paint(Graphics& g);
	///	So we can update the thumbnail as it is loaded.
	void changeListenerCallback(ChangeBroadcaster *source);

	///	So the user can click and drag to move through the sound file.
	void mouseDown(const MouseEvent &e);
	///	So the user can click and drag to move through the sound file.
	void mouseDrag(const MouseEvent &e);

	///	Loads an audio file.
	void setFile(const File& file);
	///	Sets the position of the readPointer.
	void setReadPointer(float val);
	///	Returns where the user clicked to move the readPointer.
	float getReadPointer() const {return newReadPointer;};

	///	Sets the background colour.
	void setBackgroundColour(const Colour& col);

	juce_UseDebuggingNewOperator
  private:
	///	The current thumbnail.
    AudioThumbnail *thumbnail;

	///	The current position of the read pointer.
	float readPointer;
	///	Where the user clicked to move the read pointer.
	float newReadPointer;

	///	The background colour.
	Colour backgroundColour;

	///	True if we should delete the thumbnail in our constructor.
	bool deleteThumbnail;
};

//------------------------------------------------------------------------------
///	A simplified component that displays a waveform.
class WaveformDisplayLite : public Component,
							public ChangeListener
{
  public:
	///	Constructor.
	WaveformDisplayLite(AudioThumbnail& thumb);
	///	Destructor.
	~WaveformDisplayLite();

	///	Draws the background and the waveform.
	void paint(Graphics& g);
	///	So we can update the thumbnail as it is loaded.
	void changeListenerCallback(ChangeBroadcaster *source);

	///	Sets the background colour.
	void setBackgroundColour(const Colour& col);

	juce_UseDebuggingNewOperator
  private:
	///	The current thumbnail.
    AudioThumbnail &thumbnail;

	///	The background colour.
	Colour backgroundColour;
};

#endif
