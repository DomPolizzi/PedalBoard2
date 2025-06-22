//	PresetManager.cpp - A class used to keep track of user-saved plugin presets.
#include "PresetManager.h"

PresetManager::PresetManager()
{
	File presetDir = File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile("Pedalboard2").getChildFile("presets");

	if(!presetDir.exists())
		presetDir.createDirectory();
}

//------------------------------------------------------------------------------
PresetManager::~PresetManager()
{

}

//------------------------------------------------------------------------------
void PresetManager::importPreset(const File& inFile, AudioProcessor *plugin)
{
	FileInputStream inStream(inFile);
	MemoryBlock memBlock((const size_t)inStream.getTotalLength());

	inStream.read(memBlock.getData(), (const size_t)inStream.getTotalLength());

	plugin->setCurrentProgramStateInformation(memBlock.getData(),
											  memBlock.getSize());
}

//------------------------------------------------------------------------------
void PresetManager::importPreset(const String& presetName,
								 AudioProcessor *plugin)
{
	File presetPath;

	presetPath = getPluginPresetDir(plugin->getName()).getChildFile(presetName).withFileExtension("fxp");
	importPreset(presetPath, plugin);
}

//------------------------------------------------------------------------------
void PresetManager::savePreset(const MemoryBlock& block,
							   const String& presetName,
							   const String& pluginName)
{
	File presetDir = getPluginPresetDir(pluginName);

	if(!presetDir.exists())
		presetDir.createDirectory();

	FileOutputStream outStream(presetDir.getChildFile(presetName).withFileExtension(".fxp"));

	outStream.write(block.getData(), block.getSize());
}

//------------------------------------------------------------------------------
void PresetManager::getListOfUserPresets(const String& pluginName,
										 StringArray& presetList)
{
	int i;
	File presetDir = getPluginPresetDir(pluginName);
	Array<File> presets;

	if(presetDir.exists())
	{
		presetDir.findChildFiles(presets, File::findFiles, false, "*.fxp");
		for(i=0;i<presets.size();++i)
			presetList.add(presets[i].getFileNameWithoutExtension());
	}
}

//------------------------------------------------------------------------------
File PresetManager::getPluginPresetDir(const String& pluginName)
{
	return File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile("Pedalboard2").getChildFile("presets").getChildFile(pluginName);
}
