//	TrayIcon.cpp - System tray icon.
//	----------------------------------------------------------------------------
//	This file is part of Pedalboard2, an audio plugin host.
//	Copyright (c) 2011 Niall Moody.
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

#include "TrayIcon.h"
#include "Images.h"

//------------------------------------------------------------------------------
TrayIcon::TrayIcon(DocumentWindow *win):
window(win)
{
	// Note: setIconImage() is now private in JUCE 7
	// The icon should be set through other means or overridden method
	setIconTooltip("Pedalboard 2");
}

//------------------------------------------------------------------------------
TrayIcon::~TrayIcon()
{
	
}

//------------------------------------------------------------------------------
void TrayIcon::mouseDown(const MouseEvent& e)
{
	if(e.mods.isRightButtonDown())
	{
		PopupMenu m;
		
		if(window->isVisible())
			m.addItem(1, "Hide Pedalboard");
		else
			m.addItem(1, "Show Pedalboard");
		m.addSeparator();
		m.addItem(2, "Quit");

		m.showMenuAsync(juce::PopupMenu::Options(),
			[this](int result)
			{
				if(result == 1)
				{
					if(window->isVisible())
						window->setVisible(false);
					else
						window->setVisible(true);
				}
				else if(result == 2)
				{
					JUCEApplication::getInstance()->systemRequestedQuit();
				}
			});
	}
}

//------------------------------------------------------------------------------
void TrayIcon::mouseDoubleClick(const MouseEvent& e)
{
	if(e.mods.isLeftButtonDown())
	{
		if(window->isVisible())
			window->setVisible(false);
		else
			window->setVisible(true);
	}
}
