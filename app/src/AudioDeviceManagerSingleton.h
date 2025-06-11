//	AudioDeviceManagerSingleton.h - Singleton wrapper for AudioDeviceManager
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

#ifndef AUDIODEVICEMANAGERSINGLETON_H_
#define AUDIODEVICEMANAGERSINGLETON_H_

#include <JuceHeader.h>

//------------------------------------------------------------------------------
///	Singleton wrapper for AudioDeviceManager.
class AudioDeviceManagerSingleton
{
  public:
	///	Returns the sole instance of AudioDeviceManager.
	static juce::AudioDeviceManager& getInstance();
	///	Kills the AudioDeviceManager instance.
	/*!
		****ONLY CALL THIS ONCE, WHEN YOU'RE CLOSING THE PROGRAM.****
	 */
	static void killInstance();

	///	Returns the AudioDeviceManager instance.
	juce::AudioDeviceManager& getAudioDeviceManager() { return *instance; }

  private:
	///	Constructor.
	AudioDeviceManagerSingleton();
	///	Destructor.
	~AudioDeviceManagerSingleton();

	///	The global AudioDeviceManagerSingleton instance.
	static AudioDeviceManagerSingleton singletonInstance;
	///	The global AudioDeviceManager instance.
	juce::AudioDeviceManager* instance;
};

#endif // AUDIODEVICEMANAGERSINGLETON_H_
