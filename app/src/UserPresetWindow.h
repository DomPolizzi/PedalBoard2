#ifndef __JUCER_HEADER_USERPRESETWINDOW_USERPRESETWINDOW_525C7B3E__
#define __JUCER_HEADER_USERPRESETWINDOW_USERPRESETWINDOW_525C7B3E__

#include "ColourScheme.h"
#include <JuceHeader.h>

class UserPresetWindow  : public Component,
                          public ButtonListener
{
public:
    //==========================================================================
    UserPresetWindow (KnownPluginList *knownPlugins);
    ~UserPresetWindow();

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



private:
	JUCE_LEAK_DETECTOR(UserPresetWindow)

	class PresetItem : public TreeViewItem
	{
	  public:
		///	Constructor.
		PresetItem(const File& preset):
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
				Colour highlight = ColourScheme::getInstance().colours[L"List Selected Colour"];
				ColourGradient basil(highlight.brighter(0.4f),
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

			g.setColour(ColourScheme::getInstance().colours[L"Text Colour"]);
			g.setFont(16.0f);
			g.drawText(name, 4, 0, width, height, Justification::centredLeft, false);
		};

		///	Returns this preset's file.
		File& getFile() {return presetFile;};

	  private:
		///	The name of this preset.
		String name;

		///	This preset's file.
		File presetFile;
	};
	///	The plugin item for the TreeView.
	class PluginItem : public TreeViewItem
	{
	  public:
		///	Constructor.
		PluginItem(const File& plugin):
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
			Array<File> presets;

			clearSubItems();

			pluginDir.findChildFiles(presets, File::findFiles, false, L"*.fxp");
			for(i=0;i<presets.size();++i)
				addSubItem(new PresetItem(presets[i]));
		};

		///	Draws the item.
		void paintItem(Graphics &g, int width, int height)
		{
			g.setColour(ColourScheme::getInstance().colours[L"Text Colour"]);
			g.setFont(Font(16.0f, Font::bold));
			g.drawText(name, 0, 0, width, height, Justification::centredLeft, false);
		};
	  private:
		///	The name of this plugin.
		String name;

		///	This plugin's preset directory.
		File pluginDir;
	};
	///	The root item in the mappingsTree.
	class RootItem : public TreeViewItem
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
			File presetDir = File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile(L"Pedalboard2").getChildFile(L"presets");
			Array<File> pluginDirs;

			clearSubItems();

			presetDir.findChildFiles(pluginDirs, File::findDirectories, false);
			for(i=0;i<pluginDirs.size();++i)
				addSubItem(new PluginItem(pluginDirs[i]));
		};
	};

	///	The root TreeViewItem.
	RootItem treeRoot;

	///	Used by the Import... button.
	KnownPluginList *knownPluginList;

    //[/UserVariables]

    //==========================================================================
    TreeView* presetList;
    TextButton* copyButton;
    TextButton* removeButton;
    TextButton* importButton;
    TextButton* exportButton;
    TextButton* renameButton;


    //==========================================================================
    // (prevent copy constructor and operator= being generated..)
    UserPresetWindow (const UserPresetWindow&);
    const UserPresetWindow& operator= (const UserPresetWindow&);
};


#endif   // __JUCER_HEADER_USERPRESETWINDOW_USERPRESETWINDOW_525C7B3E__
