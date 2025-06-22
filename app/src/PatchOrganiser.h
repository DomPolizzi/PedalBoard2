
#ifndef __JUCER_HEADER_PATCHORGANISER_PATCHORGANISER_B5AD9ECF__
#define __JUCER_HEADER_PATCHORGANISER_PATCHORGANISER_B5AD9ECF__

#include <JuceHeader.h>
#include <memory>

using namespace juce;

class MainPanel;

//==============================================================================
class PatchOrganiser  : public juce::Component,
                        public juce::ListBoxModel,
                        public juce::Label::Listener,
                        public juce::Button::Listener
{
public:
    //==============================================================================
    PatchOrganiser (MainPanel *p, juce::Array<juce::XmlElement*> &patches);
    ~PatchOrganiser();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Returns the number of active mappings.
	int getNumRows();
	///	Draws a row.
	void paintListBoxItem(int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected);
	///	Returns the component for a single row.
	juce::Component *refreshComponentForRow(int rowNumber, bool isRowSelected, juce::Component *existingComponentToUpdate);
	///	So the user can select rows.
	void listBoxItemDoubleClicked(int row, const juce::MouseEvent &e);
	///	So the user can select rows.
	void listBoxItemClicked(int row, const juce::MouseEvent &e);
	///	So the user can deselect rows.
	void backgroundClicked();

	///	Called when the user changes a patch name by editing the label.
	void labelTextChanged(juce::Label *labelThatHasChanged);

    //[/UserMethods]

    void paint (juce::Graphics& g);
    void resized();
    void buttonClicked (juce::Button* buttonThatWasClicked);


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchOrganiser)

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	The MainPanel.
	MainPanel *mainPanel;
	///	Our copy of the available patches.
	juce::Array<juce::XmlElement *>& patches;

    //[/UserVariables]

    //==============================================================================
    juce::ListBox* patchList;
    juce::TextButton* addButton;
    juce::TextButton* copyButton;
    juce::TextButton* removeButton;
    juce::TextButton* moveUpButton;
    juce::TextButton* moveDownButton;
    juce::TextButton* importButton;

    //==============================================================================
};


#endif   // __JUCER_HEADER_PATCHORGANISER_PATCHORGANISER_B5AD9ECF__
