//	WaveformDisplay.cpp - A component that displays a waveform.

#include "WaveformDisplay.h"
#include "AudioSingletons.h"
#include "ColourScheme.h"


WaveformDisplay::WaveformDisplay(AudioThumbnail *thumb, bool deleteThumb):
/*thumbnail(512,
		  AudioFormatManagerSingleton::getInstance(),
		  AudioThumbnailCacheSingleton::getInstance()),*/
readPointer(0.0f),
newReadPointer(0.0f),
backgroundColour(ColourScheme::getInstance().colours["Window Background"]),
deleteThumbnail(deleteThumb)
{
	if(thumb)
		thumbnail = thumb;
	else
	{
		thumbnail = new AudioThumbnail(512,
									   AudioFormatManagerSingleton::getInstance(),
									   AudioThumbnailCacheSingleton::getInstance());
	}
	thumbnail->addChangeListener(this);
}

//------------------------------------------------------------------------------
WaveformDisplay::~WaveformDisplay()
{
	//Necessary?
	thumbnail->removeChangeListener(this);

	if(deleteThumbnail)
		delete thumbnail;
}

//------------------------------------------------------------------------------
void WaveformDisplay::paint(juce::Graphics& g)
{
	float x;
	String tempstr;
	const double length = thumbnail->getTotalLength();
	const Rectangle<int> tempRect(1, 1, getWidth()-2, getHeight()-2);

	g.fillAll(ColourScheme::getInstance().colours["Window Background"]);

	g.setColour(Colours::black.withAlpha(0.25f));
	g.drawRect(0, 0, getWidth(), getHeight());

	if(length > 0)
	{
		//Draw the thumbnail.
		g.setColour(ColourScheme::getInstance().colours["Waveform Colour"]);
		thumbnail->drawChannels(g, tempRect, 0.0, length, 1.0f);

		//Draw the readPointer.
		g.setColour(ColourScheme::getInstance().colours["Text Colour"].withAlpha(0.25f));
		x = 1.0f + (readPointer * (float)(getWidth()-2));
		g.drawLine(x, 1.0f, x, (float)(getHeight()-1), 1.0f);

		//Draw the length of the file.
		g.setColour(ColourScheme::getInstance().colours["Text Colour"].withAlpha(0.25f));
		g.setFont(10.0f);
		tempstr << (int)(length/60.0) << ":" << String(((int)length)%60).paddedLeft('0', 2);
		g.drawText(tempstr,
				   tempRect.getRight()-52,
				   tempRect.getY(),
				   50,
				   12,
				   Justification(Justification::right),
				   false);
	}
}

//------------------------------------------------------------------------------
void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster *source)
{
	repaint();
}

//------------------------------------------------------------------------------
void WaveformDisplay::mouseDown(const juce::MouseEvent &e)
{
	float x = (float)e.x;

	if((x > 0.0f) && (x < (getWidth()-1)) && (thumbnail->getTotalLength() > 0))
	{
		newReadPointer = x/(float)(getWidth()-2);
		sendChangeMessage();
	}
}

//------------------------------------------------------------------------------
void WaveformDisplay::mouseDrag(const juce::MouseEvent &e)
{
	float x = (float)e.x;

	if((x > 0.0f) && (x < (getWidth()-1)) && (thumbnail->getTotalLength() > 0))
	{
		newReadPointer = x/(float)(getWidth()-2);
		sendChangeMessage();
	}
}

//------------------------------------------------------------------------------
void WaveformDisplay::setFile(const juce::File& file)
{
	AudioThumbnailCacheSingleton::getInstance().clear();
	thumbnail->setSource(new juce::FileInputSource(file));
	if(file == juce::File())
		thumbnail->reset(2, 44100.0);
	readPointer = 0.0f;
}

//------------------------------------------------------------------------------
void WaveformDisplay::setReadPointer(float val)
{
	readPointer = val;
	repaint();
}

//------------------------------------------------------------------------------
void WaveformDisplay::setBackgroundColour(const juce::Colour& col)
{
	backgroundColour = col;
}


WaveformDisplayLite::WaveformDisplayLite(AudioThumbnail& thumb):
thumbnail(thumb),
backgroundColour(ColourScheme::getInstance().colours["Window Background"])
{
	thumbnail.addChangeListener(this);
}

//------------------------------------------------------------------------------
WaveformDisplayLite::~WaveformDisplayLite()
{
	thumbnail.removeChangeListener(this);
}

//------------------------------------------------------------------------------
void WaveformDisplayLite::paint(juce::Graphics& g)
{
	String tempstr;
	const double length = thumbnail.getTotalLength();
	const Rectangle<int> tempRect(1, 1, getWidth()-2, getHeight()-2);

	g.fillAll(ColourScheme::getInstance().colours["Window Background"]);

	g.setColour(Colours::black.withAlpha(0.25f));
	g.drawRect(0, 0, getWidth(), getHeight());

	if(length > 0)
	{
		//Draw the thumbnail.
		g.setColour(ColourScheme::getInstance().colours["Waveform Colour"]);
		thumbnail.drawChannels(g, tempRect, 0.0, length, 1.0f);

		//Draw the length of the file.
		g.setColour(ColourScheme::getInstance().colours["Text Colour"].withAlpha(0.25f));
		g.setFont(10.0f);
		tempstr << (int)(length/60.0) << ":" << String(((int)length)%60).paddedLeft('0', 2);
		g.drawText(tempstr,
				   tempRect.getRight()-52,
				   tempRect.getY(),
				   50,
				   12,
				   Justification(Justification::right),
				   false);
	}
}

//------------------------------------------------------------------------------
void WaveformDisplayLite::changeListenerCallback(juce::ChangeBroadcaster *source)
{
	repaint();
}

//------------------------------------------------------------------------------
void WaveformDisplayLite::setBackgroundColour(const juce::Colour& col)
{
	backgroundColour = col;
}

