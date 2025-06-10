#ifndef __JUCER_HEADER_COLOURSCHEMEEDITOR_COLOURSCHEMEEDITOR_4DDFE783__
#define __JUCER_HEADER_COLOURSCHEMEEDITOR_COLOURSCHEMEEDITOR_4DDFE783__

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
//[/Headers]
class ColourSchemeEditor  : public juce::Component,
                            public juce::ListBoxModel,
                            public juce::Button::Listener,
                            public juce::ChangeListener,
                            public juce::ChangeBroadcaster,
                            public juce::ComboBox::Listener
{
public:
    //==============================================================================
    ColourSchemeEditor ();
    ~ColourSchemeEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Returns the number of colours which can be edited.
	int getNumRows();
	///	Draws the colour selector list.
	void paintListBoxItem(int rowNumber,
						  juce::Graphics &g,
						  int width,
						  int height,
						  bool rowIsSelected);
	///	So we know when the user selects a new colour.
	void listBoxItemClicked(int row, const juce::MouseEvent &e);

	///	Handles the user clicking the save or delete preset buttons.
	void buttonClicked(juce::Button *button);

	///	Called when the colour changes.
	void changeListenerCallback(juce::ChangeBroadcaster *source);

    //[/UserMethods]

    void paint (juce::Graphics& g);
    void resized();
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged);



    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColourSchemeEditor)

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	Helper method to load an SVG file from a binary chunk of data.
	juce::Drawable *loadSVGFromMemory(const void *dataToInitialiseFrom, size_t sizeInBytes);

    //[/UserVariables]

    //==============================================================================
    juce::ColourSelector* colourEditor;
    juce::ListBox* colourSelector;
    juce::ComboBox* presetSelector;
    juce::DrawableButton* deleteButton;
    juce::DrawableButton* saveButton;
    juce::DrawableButton* newButton;

    //==============================================================================
    // Copy constructor and operator= are handled by JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR
};

#endif   // __JUCER_HEADER_COLOURSCHEMEEDITOR_COLOURSCHEMEEDITOR_4DDFE783__
