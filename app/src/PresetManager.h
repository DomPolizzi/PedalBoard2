//	PresetManager.h - A class used to keep track of user-saved plugin presets.
#ifndef PRESETMANAGER_H_
#define PRESETMANAGER_H_

#include <JuceHeader.h>

class PresetManager
{
  public:
	///	Constructor.
	PresetManager();
	///	Destructor.
	~PresetManager();

	///	Loads a preset from an .fxp file.
	void importPreset(const File& inFile, AudioProcessor *plugin);
	///	Attempts to load a named preset from the user-saved presets dir for this plugin.
	/*!
		This is essentially a wrapper for the above importPreset(); it just
		calculates a file path from the presetName passed in, like so:
		<Pedalboard2 user data dir>/presets/<plugin name>/<presetName>.fxp
	 */
	void importPreset(const String& presetName, AudioProcessor *plugin);
	///	Saves the contents of a MemoryBlock to a .fxp file.
	void savePreset(const MemoryBlock& block,
					const String& presetName,
					const String& pluginName);

	///	Returns a StringArray of the user-saved presets for the named plugin.
	static void getListOfUserPresets(const String& pluginName, StringArray& presetList);
  private:
	///	Helper method. Returns the location of a plugin's presets.
	static File getPluginPresetDir(const String& pluginName);
};

#endif
