#include "LogFile.h"
#include "LogDisplay.h"

LogDisplay::LogDisplay ()
    : logEditor (0),
      startStopButton (0),
      midiButton (0),
      oscButton (0),
      pedalboardButton (0),
      filterLabel (0)
{
    addAndMakeVisible (logEditor = new juce::TextEditor ("logEditor"));
    logEditor->setMultiLine (true);
    logEditor->setReturnKeyStartsNewLine (true);
    logEditor->setReadOnly (true);
    logEditor->setScrollbarsShown (true);
    logEditor->setCaretVisible (false);
    logEditor->setPopupMenuEnabled (true);
    logEditor->setText (juce::String());

    addAndMakeVisible (startStopButton = new juce::TextButton ("startStopButton"));
    startStopButton->setButtonText ("Start Logging");
    startStopButton->addListener (this);

    addAndMakeVisible (midiButton = new juce::ToggleButton ("midiButton"));
    midiButton->setButtonText ("MIDI");
    midiButton->addListener (this);
    midiButton->setToggleState (true, juce::NotificationType::dontSendNotification);

    addAndMakeVisible (oscButton = new juce::ToggleButton ("oscButton"));
    oscButton->setButtonText ("OSC");
    oscButton->addListener (this);
    oscButton->setToggleState (true, juce::NotificationType::dontSendNotification);

    addAndMakeVisible (pedalboardButton = new juce::ToggleButton ("pedalboardButton"));
    pedalboardButton->setButtonText ("Pedalboard");
    pedalboardButton->addListener (this);
    pedalboardButton->setToggleState (true, juce::NotificationType::dontSendNotification);

    addAndMakeVisible (filterLabel = new juce::Label ("filterLabel",
                                                 "Filter:"));
    filterLabel->setFont (juce::Font (15.0000f, juce::Font::plain));
    filterLabel->setJustificationType (juce::Justification::centredLeft);
    filterLabel->setEditable (false, false, false);
    filterLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    filterLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x0));


	if(LogFile::getInstance().getIsLogging())
	{
		updateLog(true);

		startStopButton->setButtonText("Stop Logging");
	}

	LogFile::getInstance().addChangeListener(this);

    setSize (600, 400);

}

LogDisplay::~LogDisplay()
{
	LogFile::getInstance().removeChangeListener(this);

    delete logEditor;
    delete startStopButton;
    delete midiButton;
    delete oscButton;
    delete pedalboardButton;
    delete filterLabel;
    
    logEditor = nullptr;
    startStopButton = nullptr;
    midiButton = nullptr;
    oscButton = nullptr;
    pedalboardButton = nullptr;
    filterLabel = nullptr;
}

//==============================================================================
void LogDisplay::paint (Graphics& g)
{
    g.fillAll (juce::Colour (0xffeeece1));

}

void LogDisplay::resized()
{
    logEditor->setBounds (8, 8, getWidth() - 16, getHeight() - 40);
    startStopButton->setBounds (8, getHeight() - 28, 150, 24);
    midiButton->setBounds (208, getHeight() - 28, 56, 24);
    oscButton->setBounds (264, getHeight() - 28, 56, 24);
    pedalboardButton->setBounds (320, getHeight() - 28, 96, 24);
    filterLabel->setBounds (164, getHeight() - 28, 48, 24);
}

void LogDisplay::buttonClicked (juce::Button* buttonThatWasClicked)
{

    if (buttonThatWasClicked == startStopButton)
    {
		if(LogFile::getInstance().getIsLogging())
		{
			LogFile::getInstance().stop();
			logEditor->setText("");
			lastEvent = juce::Time();

			startStopButton->setButtonText("Start Logging");
		}
		else
		{
			LogFile::getInstance().start();

			startStopButton->setButtonText("Stop Logging");
		}
    }
    else if (buttonThatWasClicked == midiButton)
    {
		updateLog(true);
    }
    else if (buttonThatWasClicked == oscButton)
    {
		updateLog(true);
    }
    else if (buttonThatWasClicked == pedalboardButton)
    {
		updateLog(true);
    }
}

void LogDisplay::changeListenerCallback(ChangeBroadcaster *source)
{
	if(source == &(LogFile::getInstance()))
		updateLog(false);
}

void LogDisplay::updateLog(bool fromTheBeginning)
{
	juce::StringArray tempArr;

	if(fromTheBeginning)
	{
		lastEvent = juce::Time();
		logEditor->setText("");
	}
	else
		logEditor->moveCaretToEnd();

	if(midiButton->getToggleState())
		tempArr.add("MIDI");
	if(oscButton->getToggleState())
		tempArr.add("OSC");
	if(pedalboardButton->getToggleState())
		tempArr.add("Pedalboard");


	if(fromTheBeginning)
		logEditor->setText(LogFile::getInstance().getLogContents(tempArr, lastEvent));
	else
		logEditor->insertTextAtCaret(LogFile::getInstance().getLogContents(tempArr, lastEvent));

	lastEvent += juce::RelativeTime(0.001);
}

#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LogDisplay" componentName=""
                 parentClasses="public Component, public ChangeListener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330000013" fixedSize="0" initialWidth="600"
                 initialHeight="400">
  <BACKGROUND backgroundColour="ffeeece1"/>
  <TEXTEDITOR name="logEditor" id="6dbba2e3c0226e0" memberName="logEditor"
              virtualName="" explicitFocusOrder="0" pos="8 8 16M 40M" initialText=""
              multiline="1" retKeyStartsLine="1" readonly="1" scrollbars="1"
              caret="0" popupmenu="1"/>
  <TEXTBUTTON name="startStopButton" id="50e4fe174773b321" memberName="startStopButton"
              virtualName="" explicitFocusOrder="0" pos="8 28R 150 24" buttonText="Start Logging"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TOGGLEBUTTON name="midiButton" id="e128f4353765b317" memberName="midiButton"
                virtualName="" explicitFocusOrder="0" pos="208 28R 56 24" buttonText="MIDI"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="1"/>
  <TOGGLEBUTTON name="oscButton" id="6069ea976c3bf57c" memberName="oscButton"
                virtualName="" explicitFocusOrder="0" pos="264 28R 56 24" buttonText="OSC"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="1"/>
  <TOGGLEBUTTON name="pedalboardButton" id="8a2bcea281961e69" memberName="pedalboardButton"
                virtualName="" explicitFocusOrder="0" pos="320 28R 96 24" buttonText="Pedalboard"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="1"/>
  <LABEL name="filterLabel" id="c15fff90957c7ab0" memberName="filterLabel"
         virtualName="" explicitFocusOrder="0" pos="164 28R 48 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Filter:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
