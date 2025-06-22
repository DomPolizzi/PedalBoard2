/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  19 Oct 2012 11:10:47am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_PREFERENCESDIALOG_PREFERENCESDIALOG_3C07F714__
#define __JUCER_HEADER_PREFERENCESDIALOG_PREFERENCESDIALOG_3C07F714__


#include <JuceHeader.h>
#include <memory>

using namespace juce;

class MainPanel;

class PreferencesDialog  : public juce::Component,
                           public juce::TextEditor::Listener,
                           public juce::Button::Listener
{
public:
    PreferencesDialog (MainPanel *panel, const juce::String& port, const juce::String& multicastAddress);
    ~PreferencesDialog();
	void textEditorTextChanged(juce::TextEditor &editor) {};
	void textEditorReturnKeyPressed(juce::TextEditor &editor);
	void textEditorEscapeKeyPressed(juce::TextEditor &editor);
	void textEditorFocusLost(juce::TextEditor &editor);
    void paint (juce::Graphics& g);
    void resized();
    void buttonClicked (juce::Button* buttonThatWasClicked);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PreferencesDialog)

private:
	MainPanel *mainPanel;
	juce::String currentPort;
	juce::String currentMulticast;
    juce::Label* oscPortLabel;
    juce::TextEditor* oscPortEditor;
    juce::Label* oscLabel;
    juce::Label* oscMulticastLabel;
    juce::TextEditor* oscMulticastEditor;
    juce::Label* multicastHintLabel;
    juce::Label* ioOptionsLabel;
    juce::ToggleButton* audioInputButton;
    juce::ToggleButton* midiInputButton;
    juce::ToggleButton* oscInputButton;
    juce::Label* otherLabel;
    juce::ToggleButton* mappingsWindowButton;
    juce::ToggleButton* loopPatchesButton;
    juce::ToggleButton* windowsOnTopButton;
    juce::ToggleButton* ignorePinNamesButton;
    juce::Label* midiLabel;
    juce::ToggleButton* midiProgramChangeButton;
    juce::ToggleButton* mmcTransportButton;
    juce::ToggleButton* useTrayIconButton;
    juce::ToggleButton* startInTrayButton;
    juce::ToggleButton* fixedSizeButton;
    juce::ToggleButton* pdlAudioSettingsButton;
};


#endif   // __JUCER_HEADER_PREFERENCESDIALOG_PREFERENCESDIALOG_3C07F714__
