//	BranchesLAF.h - LookAndFeel class implementing some different buttons.
//	----------------------------------------------------------------------------
//	This file is part of Branches, a branching story editor.
//	Copyright (c) 2008 Niall Moody.
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//	----------------------------------------------------------------------------

#ifndef BRANCHESLAF_H_
#define BRANCHESLAF_H_

#include <JuceHeader.h>

///	LookAndFeel class implementing some different buttons.

class BranchesLAF : public juce::LookAndFeel_V4
{
  public:
	///	Constructor.
	BranchesLAF();
	///	Destructor.
	~BranchesLAF();

	///	Draws the buttons.
	void drawButtonBackground(juce::Graphics &g,
							  juce::Button &button,
							  const juce::Colour &backgroundColour,
							  bool isMouseOverButton,
							  bool isButtonDown);
	///	Draws button text.
	void drawButtonText(juce::Graphics &g,
						juce::TextButton &button,
						bool isMouseOverButton,
						bool isButtonDown);

	///	Draws the scrollbar buttons.
	void drawScrollbarButton(juce::Graphics &g,
							 juce::ScrollBar &scrollbar,
							 int width,
							 int height,
							 int buttonDirection,
							 bool isScrollbarVertical,
							 bool isMouseOverButton,
							 bool isButtonDown);
	///	Draws the scrollbar.
	void drawScrollbar(juce::Graphics &g,
					   juce::ScrollBar &scrollbar,
					   int x,
					   int y,
					   int width,
					   int height,
					   bool isScrollbarVertical,
					   int thumbStartPosition,
					   int thumbSize,
					   bool isMouseOver,
					   bool isMouseDown);

	///	Draws the menubar.
	void drawMenuBarBackground(juce::Graphics &g,
							   int width,
							   int height,
							   bool isMouseOverBar,
							   juce::MenuBarComponent &menuBar);
	///	Returns the menubar font.
	juce::Font getMenuBarFont(juce::MenuBarComponent &menuBar,
						int itemIndex,
						const juce::String &itemText);
	///	Draws the menubar items.
	void drawMenuBarItem(juce::Graphics &g,
					 int width,
					 int height,
					 int itemIndex,
					 const juce::String &itemText,
					 bool isMouseOverItem,
					 bool isMenuOpen,
					 bool isMouseOverBar,
					 juce::MenuBarComponent &menuBar);
	///	The width of a menubar item.
	int getMenuBarItemWidth(juce::MenuBarComponent &menuBar,
						int itemIndex,
						const juce::String &itemText);
	///	Returns the popup meun font.
	juce::Font getPopupMenuFont() {return juce::Font(15.0f);};
	///	Draws the popup menu background.
	void drawPopupMenuBackground(juce::Graphics &g, int width, int height);
	///	Cancels menus' drop shadow.
	int getMenuWindowFlags() {return 0;};

	///	Returns the image of a folder for the file chooser.
	const juce::Drawable *getDefaultFolderImage();
	///	Draws a combobox (used in the file chooser).
	void drawComboBox(juce::Graphics& g,
					  int width,
					  int height,
                      bool isButtonDown,
                      int buttonX,
					  int buttonY,
                      int buttonW,
					  int buttonH,
                      juce::ComboBox& box);

	///	Draws the ProgressBar.
	void drawProgressBar(juce::Graphics& g,
					 juce::ProgressBar& progressBar,
                         int width,
					 int height,
					 double progress,
					 const juce::String& textToShow);

	///	Draws the KeymapChange button.
	void drawKeymapChangeButton(juce::Graphics &g,
							int width,
							int height,
							juce::Button &button,
							const juce::String &keyDescription);

	///	Draws a Label.
	void drawLabel(juce::Graphics& g, juce::Label& label);

	///	Draws a ToggleButton.
	void drawToggleButton(juce::Graphics& g,
                          juce::ToggleButton& button,
                          bool isMouseOverButton,
                          bool isButtonDown);

	///	Drwas a tick box.
	void drawTickBox(juce::Graphics& g,
                     juce::Component& component,
                     float x, float y, float w, float h,
                     bool ticked,
                     bool isEnabled,
                     bool isMouseOverButton,
                     bool isButtonDown);

	///	Fills in the TextEditor background.
	void fillTextEditorBackground(juce::Graphics& g, int width, int height,
                                  juce::TextEditor& textEditor);

	///	Draws the callout box.
	void drawCallOutBoxBackground(juce::CallOutBox &box,
						  juce::Graphics &g,
						  const juce::Path &path,
						  juce::Image &cachedImage);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BranchesLAF)
};

#endif
