//	BranchesLAF.cpp - LookAndFeel class implementing some different buttons.
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

#include <JuceHeader.h>
#include "BranchesLAF.h"
#include "ColourScheme.h"
#include "LookAndFeelImages.h"

// Use a typedef to avoid confusion with juce::LookAndFeel_V4::ColourScheme
typedef ::ColourScheme CustomColourScheme;

using namespace std;

//------------------------------------------------------------------------------
BranchesLAF::BranchesLAF():
juce::LookAndFeel_V4()
{
	std::map<juce::String, juce::Colour>& colours = CustomColourScheme::getInstance().colours;

	setColour(juce::TextButton::buttonColourId, colours[L"Button Colour"]);
	setColour(juce::TextButton::buttonOnColourId, colours[L"Button Colour"]);
	setColour(juce::PopupMenu::highlightedBackgroundColourId, colours[L"Menu Selection Colour"]);
	setColour(juce::PopupMenu::backgroundColourId, colours[L"Window Background"]);
	setColour(juce::AlertWindow::backgroundColourId, colours[L"Window Background"]);
	setColour(juce::ComboBox::buttonColourId, colours[L"Button Colour"]);
	setColour(juce::TextEditor::highlightColourId, colours[L"Button Highlight"]);
	setColour(juce::TextEditor::focusedOutlineColourId, colours[L"Menu Selection Colour"]);
	setColour(juce::DirectoryContentsDisplayComponent::highlightColourId, juce::Colour(0xFFD7D1B5));
	setColour(juce::ProgressBar::backgroundColourId, colours[L"Window Background"]);
	setColour(juce::ProgressBar::foregroundColourId, colours[L"CPU Meter Colour"]);
}

//------------------------------------------------------------------------------
BranchesLAF::~BranchesLAF()
{
	
}

//------------------------------------------------------------------------------
void BranchesLAF::drawButtonBackground(juce::Graphics &g,
									   juce::Button &button,
									   const juce::Colour &backgroundColour,
									   bool isMouseOverButton,
									   bool isButtonDown)
{
	juce::Path highlight, shadow;
	juce::Colour buttonCol = CustomColourScheme::getInstance().colours[L"Button Colour"];
	
	juce::ColourGradient grad(buttonCol.brighter(0.8f),
				    0.0f,
				    0.0f,
				    buttonCol.darker(0.05f),
				    0.0f,
				    (float)(button.getHeight()),
				    false);

	g.setColour(juce::Colours::black);
	g.drawRoundedRectangle(1.0f,
						   1.0f,
						   (float)(button.getWidth()-2),
						   (float)(button.getHeight()-2),
						   4.0f,
						   1.0f);

	g.setGradientFill(grad);
	g.fillRoundedRectangle(1.0f,
						   1.0f,
						   (float)(button.getWidth()-2),
						   (float)(button.getHeight()-2),
						   4.0f);

	//Draw mouse over.
	if(isMouseOverButton)
	{
		g.setColour(CustomColourScheme::getInstance().colours[L"Button Highlight"]);
		g.drawRoundedRectangle(2.0f,
							   2.0f,
							   (float)(button.getWidth()-4),
							   (float)(button.getHeight()-4),
							   4.0f,
							   3.0f);
	}

	//Draw highlight.
	highlight.startNewSubPath(2.0f, (float)(button.getHeight()-3));
	highlight.lineTo(2.0f, 4.0f);
	highlight.quadraticTo(2.0f, 2.0f, 4.0f, 2.0f);
	highlight.lineTo((float)(button.getWidth()-3), 2.0f);
	if(!isButtonDown)
	{
		if(!isMouseOverButton)
			g.setColour(buttonCol.brighter(1.0f).withAlpha(0.69f));
		else
			g.setColour(buttonCol.brighter(1.0f).withAlpha(0.38f));
		g.strokePath(highlight, juce::PathStrokeType(2.0f,
									   juce::PathStrokeType::curved,
									   juce::PathStrokeType::rounded));
	}
	else
	{
		g.setColour(juce::Colour(0x20000000));
		g.strokePath(highlight, juce::PathStrokeType(1.0f,
									   juce::PathStrokeType::curved,
									   juce::PathStrokeType::rounded));
	}

	//Draw shadow.
	shadow.startNewSubPath(3.0f, (float)(button.getHeight()-2));
	shadow.lineTo((float)(button.getWidth()-4),
				  (float)(button.getHeight()-2));
	shadow.quadraticTo((float)(button.getWidth()-2),
						  (float)(button.getHeight()-2),
						  (float)(button.getWidth()-2),
						  (float)(button.getHeight()-4));
	shadow.lineTo((float)(button.getWidth()-2), 3.0f);
	if(!isButtonDown)
	{
		g.setColour(juce::Colour(0x20000000));
		g.strokePath(shadow, juce::PathStrokeType(1.0f,
									juce::PathStrokeType::curved,
									juce::PathStrokeType::rounded));
	}
	else
	{
		if(!isMouseOverButton)
			g.setColour(buttonCol.brighter(1.0f).withAlpha(0.69f));
		else
			g.setColour(buttonCol.brighter(1.0f).withAlpha(0.38f));
		g.strokePath(shadow, juce::PathStrokeType(2.0f,
									juce::PathStrokeType::curved,
									juce::PathStrokeType::rounded));
	}
}

//------------------------------------------------------------------------------
void BranchesLAF::drawButtonText(juce::Graphics &g,
								 juce::TextButton &button,
								 bool isMouseOverButton,
								 bool isButtonDown)
{
	int inc;

	g.setFont(juce::Font(15.0f));
    g.setColour (CustomColourScheme::getInstance().colours[L"Text Colour"]
                       .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

    const int yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
    const int cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;

    const int fontHeight = juce::roundToInt(juce::Font(15.0f).getHeight() * 0.6f);
    const int leftIndent  = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    const int rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));

	if(isButtonDown)
		inc = 1;
	else
		inc = 0;

    g.drawFittedText (button.getButtonText(),
                      leftIndent+inc,
                      yIndent+inc,
                      button.getWidth() - leftIndent - rightIndent,
                      button.getHeight() - yIndent * 2,
                      juce::Justification::centred, 2);
}

//------------------------------------------------------------------------------
void BranchesLAF::drawScrollbarButton(juce::Graphics &g,
									  juce::ScrollBar &scrollbar,
									  int width,
									  int height,
									  int buttonDirection,
									  bool isScrollbarVertical,
									  bool isMouseOverButton,
									  bool isButtonDown)
{
	float inc;
	juce::Path highlight, shadow, tri;
	std::map<juce::String, juce::Colour>& colours = CustomColourScheme::getInstance().colours;

	if(!isScrollbarVertical)
	{
		juce::ColourGradient grad(colours[L"Window Background"].darker(0.25f),
							0.0f,
							0.0f,
							colours[L"Window Background"],
							0.0f,
							(float)height,
							false);

		if(buttonDirection == 3)
		{
			g.setGradientFill(grad);
			g.fillRect((float)3, (float)0, (float)width, (float)height);

			g.setColour(juce::Colour(0x30000000));
			g.fillRect((float)width-3.0f, (float)0, 3.0f, 1.0f);
			g.fillRect((float)width-3.0f, (float)height-1.0f, 3.0f, 1.0f);
		}
		else if(buttonDirection == 1)
		{
			g.setGradientFill(grad);
			g.fillRect((float)0, (float)0, (float)width-3.0f, (float)height);

			g.setColour(juce::Colour(0x30000000));
			g.fillRect((float)0, (float)0, 3.0f, 1.0f);
			g.fillRect((float)0, (float)height-1.0f, 3.0f, 1.0f);
		}

		g.setColour(juce::Colour(0x80000000));
		g.drawRoundedRectangle(1.0f,
							   1.0f,
							   (float)(width-2),
							   (float)(height-2),
							   2.0f,
							   1.0f);
		g.setColour(colours[L"Button Colour"]);
		g.fillRoundedRectangle(1.0f,
							   1.0f,
							   (float)(width-2),
							   (float)(height-2),
							   2.0f);

		g.setColour(juce::Colour(0x08000000));
		g.fillRoundedRectangle(1.0f,
							   (float)(height/2),
							   (float)(width-2),
							   (float)(height-2),
							   2.0f);

		if(isMouseOverButton)
		{
			//Draw highlight.
			highlight.startNewSubPath(2.0f, (float)(height-3));
			highlight.lineTo(2.0f, 4.0f);
			highlight.quadraticTo(2.0f, 2.0f, 4.0f, 2.0f);
			highlight.lineTo((float)(width-3), 2.0f);
			if(!isButtonDown)
			{
				g.setColour(juce::Colour(0xB0FFFFFF));
				g.strokePath(highlight, juce::PathStrokeType(2.0f,
													   juce::PathStrokeType::curved,
													   juce::PathStrokeType::rounded));
			}
			else
			{
				g.setColour(juce::Colour(0x20000000));
				g.strokePath(highlight, juce::PathStrokeType(1.0f,
													   juce::PathStrokeType::curved,
													   juce::PathStrokeType::rounded));
			}

			//Draw shadow.
			shadow.startNewSubPath(3.0f, (float)(height-2));
			shadow.lineTo((float)(width-4),
						  (float)(height-2));
			shadow.quadraticTo((float)(width-2),
								  (float)(height-2),
								  (float)(width-2),
								  (float)(height-4));
			shadow.lineTo((float)(width-2), 3.0f);
			if(!isButtonDown)
			{
				g.setColour(juce::Colour(0x20000000));
				g.strokePath(shadow, juce::PathStrokeType(1.0f,
											juce::PathStrokeType::curved,
											juce::PathStrokeType::rounded));
			}
			else
			{
				g.setColour(juce::Colour(0xB0FFFFFF));
				g.strokePath(shadow, juce::PathStrokeType(2.0f,
											juce::PathStrokeType::curved,
											juce::PathStrokeType::rounded));
			}
		}
	}
	else
	{
		juce::ColourGradient grad(colours[L"Window Background"].darker(0.25f),
							0.0f,
							0.0f,
							colours[L"Window Background"],
							(float)width,
							0.0f,
							false);

		if(buttonDirection == 2)
		{
			g.setGradientFill(grad);
			g.fillRect((float)0, (float)0, (float)width, (float)height-3.0f);

			g.setColour(juce::Colour(0x30000000));
			g.fillRect((float)0, (float)0, 1.0f, 3.0f);
			g.fillRect((float)width-1.0f, 0.0f, 1.0f, 3.0f);
		}
		else if(buttonDirection == 0)
		{
			g.setGradientFill(grad);
			g.fillRect((float)0, (float)3, (float)width, (float)height);

			g.setColour(juce::Colour(0x30000000));
			g.fillRect((float)0, (float)height-3.0f, 1.0f, 3.0f);
			g.fillRect((float)width-1.0f, (float)height-3.0f, 1.0f, 3.0f);
		}

		g.setColour(juce::Colour(0x80000000));
		g.drawRoundedRectangle(1.0f,
							   1.0f,
							   (float)(width-2),
							   (float)(height-2),
							   2.0f,
							   1.0f);
		g.setColour(colours[L"Button Colour"]);
		g.fillRoundedRectangle(1.0f,
							   1.0f,
							   (float)(width-2),
							   (float)(height-2),
							   2.0f);

		g.setColour(juce::Colour(0x08000000));
		g.fillRoundedRectangle((float)(width/2),
							   1.0f,
							   (float)(width-2),
							   (float)(height-2),
							   2.0f);

		if(isMouseOverButton)
		{
			//Draw highlight.
			highlight.startNewSubPath(2.0f, (float)(height-3));
			highlight.lineTo(2.0f, 4.0f);
			highlight.quadraticTo(2.0f, 2.0f, 4.0f, 2.0f);
			highlight.lineTo((float)(width-3), 2.0f);
			if(!isButtonDown)
			{
				g.setColour(juce::Colour(0xB0FFFFFF));
				g.strokePath(highlight, juce::PathStrokeType(2.0f,
													   juce::PathStrokeType::curved,
													   juce::PathStrokeType::rounded));
			}
			else
			{
				g.setColour(juce::Colour(0x20000000));
				g.strokePath(highlight, juce::PathStrokeType(1.0f,
													   juce::PathStrokeType::curved,
													   juce::PathStrokeType::rounded));
			}

			//Draw shadow.
			shadow.startNewSubPath(3.0f, (float)(height-2));
			shadow.lineTo((float)(width-4),
						  (float)(height-2));
			shadow.quadraticTo((float)(width-2),
								  (float)(height-2),
								  (float)(width-2),
								  (float)(height-4));
			shadow.lineTo((float)(width-2), 3.0f);
			if(!isButtonDown)
			{
				g.setColour(juce::Colour(0x20000000));
				g.strokePath(shadow, juce::PathStrokeType(1.0f,
													juce::PathStrokeType::curved,
													juce::PathStrokeType::rounded));
			}
			else
			{
				g.setColour(juce::Colour(0xB0FFFFFF));
				g.strokePath(shadow, juce::PathStrokeType(2.0f,
													juce::PathStrokeType::curved,
													juce::PathStrokeType::rounded));
			}
		}
	}

	//Draw triangle.
	if(isButtonDown)
		inc = 1.0f;
	else
		inc = 0.0f;
	switch(buttonDirection)
	{
		case 0:
			tri.startNewSubPath((float)(width/2)-(float)(width/4)+inc,
								(float)(height/2)+(float)(height/8)+inc);
			tri.lineTo((float)(width/2)+inc,
					   (float)(height/2)-(float)(height/8)+inc);
			tri.lineTo((float)(width/2)+(float)(width/4)+inc,
					   (float)(height/2)+(float)(height/8)+inc);
			break;
		case 1:
			tri.startNewSubPath((float)(width/2)-(float)(width/8)+inc,
								(float)(height/2)-(float)(height/4)+inc);
			tri.lineTo((float)(width/2)+(float)(width/8)+inc,
					   (float)(height/2)+inc);
			tri.lineTo((float)(width/2)-(float)(width/8)+inc,
					   (float)(height/2)+(float)(height/4)+inc);
			break;
		case 2:
			tri.startNewSubPath((float)(width/2)-(float)(width/4)+inc,
								(float)(height/2)-(float)(height/8)+inc);
			tri.lineTo((float)(width/2)+inc,
					   (float)(height/2)+(float)(height/8)+inc);
			tri.lineTo((float)(width/2)+(float)(width/4)+inc,
					   (float)(height/2)-(float)(height/8)+inc);
			break;
		case 3:
			tri.startNewSubPath((float)(width/2)+(float)(width/8)+inc,
								(float)(height/2)+(float)(height/4)+inc);
			tri.lineTo((float)(width/2)-(float)(width/8)+inc,
					   (float)(height/2)+inc);
			tri.lineTo((float)(width/2)+(float)(width/8)+inc,
					   (float)(height/2)-(float)(height/4)+inc);
			break;
	}

	g.setColour(colours[L"Vector Colour"]);
	g.strokePath(tri, juce::PathStrokeType(2.0f));
}

//------------------------------------------------------------------------------
void BranchesLAF::drawScrollbar(juce::Graphics &g,
					   juce::ScrollBar &scrollbar,
					   int x,
					   int y,
					   int width,
					   int height,
					   bool isScrollbarVertical,
					   int thumbStartPosition,
					   int thumbSize,
					   bool isMouseOver,
					   bool isMouseDown)
{
	std::map<juce::String, juce::Colour>& colours = CustomColourScheme::getInstance().colours;

	if(!isScrollbarVertical)
	{
		juce::ColourGradient grad(colours[L"Window Background"].darker(0.25f),
							0.0f,
							(float)y,
							colours[L"Window Background"],
							0.0f,
							(float)height,
							false);

		g.setGradientFill(grad);
		g.fillRect((float)x, (float)y, (float)width, (float)height);

		g.setColour(juce::Colour(0x30000000));
		g.fillRect((float)x, (float)y, (float)width, 1.0f);
		g.fillRect((float)x, (float)height-1.0f, (float)width, 1.0f);

		g.setColour(juce::Colour(0x80000000));
		g.drawRoundedRectangle((float)(thumbStartPosition+1),
							   (float)(y+1),
							   (float)(thumbSize-2),
							   (float)(height-2),
							   2.0f,
							   1.0f);
		g.setColour(colours[L"Button Colour"]);
		g.fillRoundedRectangle((float)(thumbStartPosition+1),
							   (float)(y+1),
							   (float)(thumbSize-2),
							   (float)(height-2),
							   2.0f);

		g.setColour(juce::Colour(0x08000000));
		g.fillRoundedRectangle((float)(thumbStartPosition+1),
							   (float)(height/2),
							   (float)(thumbSize-2),
							   (float)(height-2),
							   2.0f);
	}
	else
	{
		juce::ColourGradient grad(colours[L"Window Background"].darker(0.25f),
							(float)x,
							(float)0.0f,
							colours[L"Window Background"],
							(float)width,
							0.0f,
							false);

		g.setGradientFill(grad);
		g.fillRect((float)x, (float)y, (float)width, (float)height);

		g.setColour(juce::Colour(0x30000000));
		g.fillRect((float)x, (float)y, 1.0f, (float)height);
		g.fillRect((float)width-1.0f, (float)y, 1.0f, (float)height);

		g.setColour(juce::Colour(0x80000000));
		g.drawRoundedRectangle((float)(x+1),
							   (float)(thumbStartPosition+1),
							   (float)(width-2),
							   (float)(thumbSize-2),
							   2.0f,
							   1.0f);
		g.setColour(colours[L"Button Colour"]);
		g.fillRoundedRectangle((float)(x+1),
							   (float)(thumbStartPosition+1),
							   (float)(width-2),
							   (float)(thumbSize-2),
							   2.0f);

		g.setColour(juce::Colour(0x08000000));
		g.fillRoundedRectangle((float)(width/2),
							   (float)(thumbStartPosition+1),
							   (float)(width-2),
							   (float)(thumbSize-2),
							   2.0f);
	}
}

//------------------------------------------------------------------------------
void BranchesLAF::drawMenuBarBackground(juce::Graphics &g,
							   int width,
							   int height,
							   bool isMouseOverBar,
							   juce::MenuBarComponent &menuBar)
{
	juce::Colour col = CustomColourScheme::getInstance().colours[L"Window Background"];

	juce::ColourGradient grad(col.brighter(0.8f),
					    0.0f,
					    0.0f,
					    col.darker(0.05f),
					    0.0f,
					    (float)height,
					    false);

	g.setGradientFill(grad);
	g.fillRect(0.0f, 0.0f, (float)width, (float)height);

	g.setColour(juce::Colour(0x30000000));
	g.fillRect(0.0f, (float)height-1.0f, (float)width, 1.0f);
}

//------------------------------------------------------------------------------
juce::Font BranchesLAF::getMenuBarFont(juce::MenuBarComponent &menuBar,
								 int itemIndex,
								 const juce::String &itemText)
{
	return juce::Font(15.0f);
}

//------------------------------------------------------------------------------
void BranchesLAF::drawMenuBarItem(juce::Graphics &g,
								  int width,
								  int height,
								  int itemIndex,
								  const juce::String &itemText,
								  bool isMouseOverItem,
								  bool isMenuOpen,
								  bool isMouseOverBar,
								  juce::MenuBarComponent &menuBar)
{
	std::map<juce::String, juce::Colour>& colours = CustomColourScheme::getInstance().colours;

	if (! menuBar.isEnabled())
    {
        g.setColour (colours[L"Text Colour"].withMultipliedAlpha (0.5f));
    }
    else if (isMenuOpen || isMouseOverItem)
    {
        g.fillAll (colours[L"Menu Selection Colour"]);
		g.setColour (colours[L"Menu Selection Colour"].contrasting());
    }
    else
    {
        g.setColour (colours[L"Text Colour"]);
    }

    g.setFont (getMenuBarFont (menuBar, itemIndex, itemText));
    g.drawFittedText (itemText, 0, 0, width, height, juce::Justification::centred, 1);
}

//------------------------------------------------------------------------------
int BranchesLAF::getMenuBarItemWidth(juce::MenuBarComponent &menuBar,
                                    int itemIndex,
                                    const juce::String &itemText)
{
    return getMenuBarFont(menuBar, itemIndex, itemText).getStringWidth(itemText) + 20;
}

//------------------------------------------------------------------------------
void BranchesLAF::drawPopupMenuBackground(juce::Graphics &g, int width, int height)
{
    juce::Path highlight, shadow;

    g.fillAll(CustomColourScheme::getInstance().colours[L"Window Background"]);

    highlight.startNewSubPath(2.0f, (float)(height-3));
    highlight.lineTo(2.0f, 2.0f);
    highlight.lineTo((float)(width-3), 2.0f);

    shadow.startNewSubPath((float)(width-3), 2.0f);
    shadow.lineTo((float)(width-3), (float)(height-3));
    shadow.lineTo(2.0f, (float)(height-3));

    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.strokePath(highlight, juce::PathStrokeType(1.0f));
    g.setColour(juce::Colours::black.withAlpha(0.9f));
    g.strokePath(shadow, juce::PathStrokeType(1.0f));
}

//------------------------------------------------------------------------------
const juce::Drawable *BranchesLAF::getDefaultFolderImage()
{
    static juce::DrawableImage im;

    if(im.getImage().isNull())
        im.setImage(juce::ImageCache::getFromMemory(LookAndFeelImages::lookandfeelfolder_32_png, 
                                                   LookAndFeelImages::lookandfeelfolder_32_pngSize));

    return &im;
}

//------------------------------------------------------------------------------
void BranchesLAF::drawComboBox(juce::Graphics& g,
                              int width,
                              int height,
                              bool isButtonDown,
                              int buttonX,
                              int buttonY,
                              int buttonW,
                              int buttonH,
                              juce::ComboBox& box)
{
    float inc;
    std::map<juce::String, juce::Colour>& colours = CustomColourScheme::getInstance().colours;

    g.setColour(colours[L"Text Editor Colour"]);
    g.fillRect(0, 0, width-buttonW+3, height);

    if (box.isEnabled() && box.hasKeyboardFocus(false))
    {
        g.setColour(colours[L"Button Colour"]);
        g.drawRect(0, 0, width, height, 2);
    }
    else
    {
        g.setColour(box.findColour(juce::ComboBox::outlineColourId));
        g.drawRect(0, 0, width-buttonW+3, height);
    }

    g.setColour(juce::Colour(0x80000000));
	g.drawRoundedRectangle(buttonX+1.0f,
						   buttonY+1.0f,
						   (float)(buttonW-2),
						   (float)(buttonH-2),
						   2.0f,
						   1.0f);
	g.setColour(colours[L"Button Colour"]);
	g.fillRoundedRectangle(buttonX+1.0f,
						   buttonY+1.0f,
						   (float)(buttonW-2),
						   (float)(buttonH-2),
						   2.0f);

	g.setColour(juce::Colour(0x08000000));
	g.fillRoundedRectangle(buttonX+1.0f,
						   buttonY+(float)(buttonH/2),
						   (float)(buttonW-2),
						   (float)(buttonH-2),
						   2.0f);

	if(isButtonDown)
		inc = 1.0f;
	else
		inc = 0.0f;

    if(box.isEnabled())
    {
        juce::Path tri;

        tri.startNewSubPath(buttonX+(float)(buttonW/2)-(float)(buttonW/4)+inc,
							buttonY+(float)(buttonH/2)-(float)(buttonH/8)+inc);
		tri.lineTo(buttonX+(float)(buttonW/2)+inc,
				   buttonY+(float)(buttonH/2)+(float)(buttonH/8)+inc);
		tri.lineTo(buttonX+(float)(buttonW/2)+(float)(buttonW/4)+inc,
				   buttonY+(float)(buttonH/2)-(float)(buttonH/8)+inc);

        g.setColour(colours[L"Vector Colour"]);
        g.strokePath(tri, juce::PathStrokeType(2.0f));
    }
}

//------------------------------------------------------------------------------
void BranchesLAF::drawProgressBar(juce::Graphics& g,
                                 juce::ProgressBar& progressBar,
                                 int width,
                                 int height,
                                 double progress,
                                 const juce::String& textToShow)
{
    std::map<juce::String, juce::Colour>& colours = CustomColourScheme::getInstance().colours;
    juce::ColourGradient grad(colours[L"Window Background"].darker(0.2f),
                        0.0f,
                        0.0f,
                        colours[L"Window Background"],
                        0.0f,
                        (float)height,
                        false);

    //Draw the background.
    g.setGradientFill(grad);
    g.fillRect(0.0f, 0.0f, (float)width, (float)height);
    g.setColour(juce::Colour(0x80000000));
    g.drawRect(0, 0, width, height, 1);

    //Draw the foreground.
    g.setColour(juce::Colour(0x80000000));
    g.drawRoundedRectangle(1.0f,
                           1.0f,
                           (float)(width-2)*(float)progress,
                           (float)(height-2),
                           2.0f,
                           1.0f);
    g.setColour(colours[L"CPU Meter Colour"]);
    g.fillRoundedRectangle(1.0f,
                           1.0f,
                           (float)(width-2)*(float)progress,
                           (float)(height-2),
                           2.0f);

    g.setColour(juce::Colour(0x08000000));
    g.fillRoundedRectangle(1.0f,
                           (float)(height/2),
                           (float)(width-2)*(float)progress,
                           (float)(height-2),
                           2.0f);

    //Draw the text.
    g.setColour(juce::Colour(0x80000000));
    g.drawText(textToShow,
               2,
               1,
               width-4,
               height-2,
               juce::Justification::horizontallyCentred,
               true);
}

//------------------------------------------------------------------------------
void BranchesLAF::drawKeymapChangeButton(juce::Graphics &g,
                                        int width,
                                        int height,
                                        juce::Button &button,
                                        const juce::String &keyDescription)
{
    std::map<juce::String, juce::Colour>& colours = CustomColourScheme::getInstance().colours;

    if(keyDescription.isNotEmpty())
    {
        drawButtonBackground(g,
                             button,
                             colours[L"Button Colour"],
                             button.isOver(),
                             button.isDown());

        g.setColour(colours[L"Text Colour"]);
        g.setFont(height * 0.6f);
        g.drawFittedText(keyDescription,
                         3,
                         0,
                         width - 6,
                         height,
                         juce::Justification::centred,
                         1);
    }
    else
    {
        const float thickness = 7.0f;
        const float indent = 22.0f;

        juce::Path p;
        p.addEllipse(0.0f, 0.0f, 100.0f, 100.0f);
        p.addRectangle(indent,
                       50.0f - thickness,
                       100.0f - indent * 2.0f,
                       thickness * 2.0f);
        p.addRectangle(50.0f - thickness,
                       indent,
                       thickness * 2.0f,
                       50.0f - indent - thickness);
        p.addRectangle(50.0f - thickness,
                       50.0f + thickness,
                       thickness * 2.0f,
                       50.0f - indent - thickness);
        p.setUsingNonZeroWinding(false);

        g.setColour(colours[L"Text Colour"].withAlpha(button.isDown() ? 0.7f : (button.isOver() ? 0.5f : 0.3f)));
        g.fillPath(p,
                   p.getTransformToScaleToFit(2.0f,
                                              2.0f,
                                              width - 4.0f,
                                              height - 4.0f,
                                              true));
    }
}

//------------------------------------------------------------------------------
void BranchesLAF::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.fillAll(label.findColour(juce::Label::backgroundColourId));

    if(!label.isBeingEdited())
    {
        const float alpha = label.isEnabled() ? 1.0f : 0.5f;
        const int borderSize = 4; // Default border size since the methods don't exist

        g.setColour(CustomColourScheme::getInstance().colours[L"Text Colour"]);
        g.setFont(label.getFont());
        g.drawFittedText(label.getText(),
                         borderSize,
                         borderSize,
                         label.getWidth() - 2 * borderSize,
                         label.getHeight() - 2 * borderSize,
                         label.getJustificationType(),
                         juce::jmax(1, (int)(label.getHeight()/label.getFont().getHeight())),
                         label.getMinimumHorizontalScale());

        g.setColour(label.findColour(juce::Label::outlineColourId).withMultipliedAlpha(alpha));
        g.drawRect(0, 0, label.getWidth(), label.getHeight());
    }
    else if (label.isEnabled())
    {
        g.setColour(label.findColour(juce::Label::outlineColourId));
        g.drawRect(0, 0, label.getWidth(), label.getHeight());
    }
}

//------------------------------------------------------------------------------
void BranchesLAF::drawToggleButton(juce::Graphics& g,
                                   juce::ToggleButton& button,
                                   bool isMouseOverButton,
                                   bool isButtonDown)
{
    if(button.hasKeyboardFocus (true))
    {
        g.setColour(CustomColourScheme::getInstance().colours[L"List Selected Colour"]);
        g.drawRect(0, 0, button.getWidth(), button.getHeight());
    }

    float fontSize = juce::jmin(15.0f, button.getHeight() * 0.75f);
    const float tickWidth = fontSize * 1.1f;

    drawTickBox(g, button, 4.0f, (button.getHeight() - tickWidth) * 0.5f,
                tickWidth, tickWidth,
                button.getToggleState(),
                button.isEnabled(),
                isMouseOverButton,
                isButtonDown);

    g.setColour(CustomColourScheme::getInstance().colours[L"Text Colour"]);
    g.setFont(fontSize);

    if (! button.isEnabled())
        g.setOpacity (0.5f);

    const int textX = (int) tickWidth + 5;

    g.drawFittedText(button.getButtonText(),
                     textX, 0,
                     button.getWidth() - textX - 2, button.getHeight(),
                     juce::Justification::centredLeft, 10);
}

//------------------------------------------------------------------------------
void BranchesLAF::drawTickBox(juce::Graphics& g,
                              juce::Component& component,
                              float x, float y, float w, float h,
                              bool ticked,
                              bool isEnabled,
                              bool isMouseOverButton,
                              bool isButtonDown)
{
    const float boxSize = w * 0.7f;
    const float sphereX = x;
    const float sphereY = y + (h - boxSize) * 0.5f;
    
    // Draw a simple sphere instead of calling drawGlassSphere
    juce::Colour sphereColour = CustomColourScheme::getInstance().colours[L"Tick Box Colour"].withMultipliedAlpha(isEnabled ? 1.0f : 0.5f);
    
    g.setColour(sphereColour);
    g.fillEllipse(sphereX, sphereY, boxSize, boxSize);
    
    // Add highlight
    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.fillEllipse(sphereX + boxSize * 0.2f, sphereY + boxSize * 0.2f, boxSize * 0.6f, boxSize * 0.6f);
    
    // Add outline
    float outlineThickness = isEnabled ? ((isButtonDown || isMouseOverButton) ? 1.1f : 0.5f) : 0.3f;
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawEllipse(sphereX, sphereY, boxSize, boxSize, outlineThickness);

    if(ticked)
    {
        juce::Path tick;
        juce::Colour tempCol = CustomColourScheme::getInstance().colours[L"Vector Colour"];
        tick.startNewSubPath(1.5f, 3.0f);
        tick.lineTo(3.0f, 6.0f);
        tick.lineTo(6.0f, 0.0f);

        g.setColour(isEnabled ? tempCol : tempCol.withAlpha(0.25f));

        const juce::AffineTransform trans(juce::AffineTransform::scale(w / 9.0f, h / 9.0f)
                                         .translated(x, y));

        g.strokePath(tick, juce::PathStrokeType(2.5f), trans);
    }
}

//------------------------------------------------------------------------------
void BranchesLAF::fillTextEditorBackground(juce::Graphics& g, int /*width*/, int /*height*/,
                                           juce::TextEditor& textEditor)
{
    g.fillAll(CustomColourScheme::getInstance().colours[L"Text Editor Colour"]);
}

//------------------------------------------------------------------------------
void BranchesLAF::drawCallOutBoxBackground(juce::CallOutBox &box,
                                          juce::Graphics &g,
                                          const juce::Path &path,
                                          juce::Image &cachedImage)
{
    juce::Image content(juce::Image::ARGB, box.getWidth(), box.getHeight(), true);

    {
        juce::Graphics g2(content);

        g2.setColour(CustomColourScheme::getInstance().colours[L"Window Background"].withAlpha(0.9f));
        g2.fillPath(path);

        g2.setColour(juce::Colours::black.withAlpha(0.8f));
        g2.strokePath(path, juce::PathStrokeType(2.0f));
    }

    juce::DropShadowEffect shadow;
    juce::DropShadow shad(juce::Colours::black.withAlpha(0.5f), 5, juce::Point<int>(2, 2));
    shadow.setShadowProperties(shad);
    shadow.applyEffect(content, g, 1.0f, 1.0f);
}