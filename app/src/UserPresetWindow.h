#ifndef __JUCER_HEADER_USERPRESETWINDOW_USERPRESETWINDOW_525C7B3E__
#define __JUCER_HEADER_USERPRESETWINDOW_USERPRESETWINDOW_525C7B3E__

#include <JuceHeader.h>
#include <memory>
#include "ColourScheme.h"

class UserPresetWindow  : public juce::Component,
                          public juce::Button::Listener
{
public:
    UserPresetWindow (juce::KnownPluginList *knownPlugins);
    ~UserPresetWindow();

    void paint (juce::Graphics& g);
    void resized();
    void buttonClicked (juce::Button* buttonThatWasClicked);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UserPresetWindow)

	class PresetItem : public juce::TreeViewItem
	{
	  public:
		///	Constructor.
		PresetItem(const juce::File& preset):
		name(preset.getFileNameWithoutExtension()),
		presetFile(preset)
		{

		};
		///	Destructor.
		~PresetItem() {};

		///	Returns true, obviously.
		bool mightContainSubItems() {return false;};

		///	Draws the item.
		void paintItem(Graphics &g, int width, int height)
		{
			if(isSelected())
			{
				juce::Colour highlight = ColourScheme::getInstance().colours["List Selected Colour"];
				juce::ColourGradient basil(highlight.brighter(0.4f),
									 0.0f,
									 0.0f,
									 highlight.darker(0.125f),
									 0.0f,
									 (float)height,
									 false);

				g.setGradientFill(basil);

				//g.fillAll();
				g.fillRoundedRectangle(0.0f,
									   0.0f,
									   (float)width-4.0f,
									   (float)height,
									   4.0f);
			}

			g.setColour(ColourScheme::getInstance().colours["Text Colour"]);
			g.setFont(16.0f);
			g.drawText(name, 4, 0, width, height, juce::Justification::centredLeft, false);
		};

		///	Returns this preset's file.
		juce::File& getFile() {return presetFile;};

	  private:
		///	The name of this preset.
		juce::String name;

		///	This preset's file.
		juce::File presetFile;
	};
	///	The plugin item for the TreeView.
	class PluginItem : public juce::TreeViewItem
	{
	  public:
		///	Constructor.
		PluginItem(const juce::File& plugin):
		name(plugin.getFileName()),
		pluginDir(plugin)
		{
			setLinesDrawnForSubItems(true);
			setOpen(true);
		};
		///	Destructor.
		~PluginItem() {};

		///	Returns true, obviously.
		bool mightContainSubItems() {return true;};
		///	Returns false.
		bool canBeSelected() const {return false;};

		///	Adds all the sub-items (mappings in this category).
		void itemOpennessChanged(bool isNowOpen)
		{
			int i;
			juce::Array<juce::File> presets;

			clearSubItems();

			pluginDir.findChildFiles(presets, juce::File::findFiles, false, "*.fxp");
			for(i=0;i<presets.size();++i)
				addSubItem(new PresetItem(presets[i]));
		};

		///	Draws the item.
		void paintItem(Graphics &g, int width, int height)
		{
			g.setColour(ColourScheme::getInstance().colours["Text Colour"]);
			g.setFont(juce::Font(16.0f, juce::Font::bold));
			g.drawText(name, 0, 0, width, height, juce::Justification::centredLeft, false);
		};
	  private:
		///	The name of this plugin.
		juce::String name;

		///	This plugin's preset directory.
		juce::File pluginDir;
	};
	///	The root item in the mappingsTree.
	class RootItem : public juce::TreeViewItem
	{
	  public:
		///	Constructor.
		RootItem()
		{
			setLinesDrawnForSubItems(false);
			setOpen(true);
		};
		///	Destructor.
		~RootItem() {};

		///	Returns true, obviously.
		bool mightContainSubItems() {return true;};

		///	Adds all the sub-items (mapping categories).
		void itemOpennessChanged(bool isNowOpen)
		{
			int i;
			juce::File presetDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("Pedalboard2").getChildFile("presets");
			juce::Array<juce::File> pluginDirs;

			clearSubItems();

			presetDir.findChildFiles(pluginDirs, juce::File::findDirectories, false);
			for(i=0;i<pluginDirs.size();++i)
				addSubItem(new PluginItem(pluginDirs[i]));
		};
	};

	///	The root TreeViewItem.
	RootItem treeRoot;

	///	Used by the Import... button.
	juce::KnownPluginList *knownPluginList;

    juce::TreeView* presetList;
    juce::TextButton* copyButton;
    juce::TextButton* removeButton;
    juce::TextButton* importButton;
    juce::TextButton* exportButton;
    juce::TextButton* renameButton;
};


#endif   // __JUCER_HEADER_USERPRESETWINDOW_USERPRESETWINDOW_525C7B3E__
