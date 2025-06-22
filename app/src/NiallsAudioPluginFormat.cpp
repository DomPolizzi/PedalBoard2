//	NiallsAudioPluginFormat.cpp - Describes the NiallsAudioPlugin format.
//	----------------------------------------------------------------------------
//	This file is part of Pedalboard2, an audio plugin host.
//	Copyright (c) 2009 Niall Moody.
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

#include "NiallsAudioPluginFormat.h"
#include "NiallsAudioPlugin.h"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
NiallsAudioPluginFormat::NiallsAudioPluginFormat():
AudioPluginFormat()
{
	
}

//------------------------------------------------------------------------------
NiallsAudioPluginFormat::~NiallsAudioPluginFormat()
{
	
}

//------------------------------------------------------------------------------
void NiallsAudioPluginFormat::findAllTypesForFile(OwnedArray<PluginDescription>& results,
												  const String& fileOrIdentifier)
{
	if(!fileMightContainThisPluginType(fileOrIdentifier))
		return;

	PluginDescription desc;
	desc.fileOrIdentifier = fileOrIdentifier;
	// desc.uid = 0; // uid member removed in JUCE 7

	NAPInstance* instance = dynamic_cast<NAPInstance*>(createInstanceFromDescription(desc));

	if(instance == 0)
		return;

	try
	{
		instance->fillInPluginDescription(desc);

		if(instance->getModule()->pluginCreator != 0)
			results.add(new PluginDescription(desc));
	}
	catch (...)
	{
		cout << "NiallsAudioPluginFormat: Caught exception interrogating plugin." << endl;
	}

	deleteAndZero(instance);
}

//------------------------------------------------------------------------------
AudioPluginInstance *NiallsAudioPluginFormat::createInstanceFromDescription(const PluginDescription& desc)
{
	NAPInstance *result = 0;

	if(fileMightContainThisPluginType(desc.fileOrIdentifier))
	{
		//NAPModuleHandle *m = 0;
		File file(desc.fileOrIdentifier);

		const File previousWorkingDirectory(File::getCurrentWorkingDirectory());
		file.getParentDirectory().setAsCurrentWorkingDirectory();

		const ReferenceCountedObjectPtr<NAPModuleHandle> module(NAPModuleHandle::findOrCreateModule(file));

		if(module != nullptr)
		{
			result = new NAPInstance(module);

			if(result)
			{
				if(!result->getPluginLoadedOkay())
				{
					cout << "NiallsAudioPluginFormat: Could not load plugin." << endl;
					deleteAndZero(result);
				}
			}
		}

		previousWorkingDirectory.setAsCurrentWorkingDirectory();
	}

	return result;
}

//------------------------------------------------------------------------------
bool NiallsAudioPluginFormat::fileMightContainThisPluginType(const String& fileOrIdentifier)
{
	const File f (fileOrIdentifier);

	return f.existsAsFile() && f.hasFileExtension(".so");
}

//------------------------------------------------------------------------------
String NiallsAudioPluginFormat::getNameOfPluginFromIdentifier(const String& fileOrIdentifier)
{
	return fileOrIdentifier;
}

//------------------------------------------------------------------------------
bool NiallsAudioPluginFormat::doesPluginStillExist(const PluginDescription& desc)
{
	return File(desc.fileOrIdentifier).exists();
}

//------------------------------------------------------------------------------
StringArray NiallsAudioPluginFormat::searchPathsForPlugins(const FileSearchPath& directoriesToSearch,
																 const bool recursive)
{
	int j;
	StringArray results;

    for(j=0;j<directoriesToSearch.getNumPaths();++j)
        recursiveFileSearch(results, directoriesToSearch[j], recursive);

    return results;
}

//------------------------------------------------------------------------------
FileSearchPath NiallsAudioPluginFormat::getDefaultLocationsToSearch()
{
	return FileSearchPath ("/usr/lib/nap;/usr/local/lib/nap");
}

//------------------------------------------------------------------------------
void NiallsAudioPluginFormat::recursiveFileSearch(StringArray& results,
												  const File& dir, 
												  const bool recursive)
{
	for (juce::RangedDirectoryIterator iter(dir, false, "*", juce::File::findFilesAndDirectories); iter != juce::RangedDirectoryIterator{}; ++iter)
	{
		File f = iter->getFile();
		bool isPlugin = false;

		if(fileMightContainThisPluginType(f.getFullPathName()))
		{
			isPlugin = true;
			results.add(f.getFullPathName());
		}

		if(recursive && (!isPlugin) && f.isDirectory())
			recursiveFileSearch(results, f, true);
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
NAPInstance::NAPInstance(const ReferenceCountedObjectPtr<NAPModuleHandle>& mod):
AudioPluginInstance(),
plugin(0),
module(mod)
{
	try
	{
		if(module->pluginCreator != 0)
		{
			plugin = module->pluginCreator();
			if(plugin == 0)
			{
				cout << "NAPInstance: Could not create plugin." << endl;
				return;
			}
			else
			{
				name = plugin->getName().c_str();

				//So the app knows how many channels the plugin has.
				setPlayConfigDetails(plugin->getNumAudioInputs()+plugin->getNumParameters(),
									 plugin->getNumAudioOutputs(),
									 44100.0,
									 4096);
			}
		}
		else
		{
			cout << "NAPInstance: No createPlugin() function in this library." << endl;
			return;
		}

		//uint32 sampleRate = (uint32)(getSampleRate() > 0 ? getSampleRate() : 44100.0f);
    }
    catch (...)
    {
        cout << "NAPInstance: Caught exception when creating plugin." << endl;
    }
}

//------------------------------------------------------------------------------
NAPInstance::~NAPInstance()
{
	try
	{
		if(plugin)
			delete plugin;
	}
	catch(...)
	{
		cout << "NAPInstance: Exception deleting plugin." << endl;
	}
}

//------------------------------------------------------------------------------
void NAPInstance::fillInPluginDescription(PluginDescription& description) const
{
	if(module != nullptr)
	{
		description.name = module->pluginName;
		description.fileOrIdentifier = module->pluginFile.getFullPathName();
		description.lastFileModTime = module->pluginFile.getLastModificationTime();
		description.pluginFormatName = "NAP";
		description.category = "Effect";
		description.manufacturerName = "Niall Moody";
		description.version = "1.0";
		description.numInputChannels = getTotalNumInputChannels();
		description.numOutputChannels = getTotalNumOutputChannels();
		description.isInstrument = false;
	}
}

//------------------------------------------------------------------------------
void NAPInstance::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
	if(!plugin)
		return;

	setPlayConfigDetails(plugin->getNumAudioInputs()+plugin->getNumParameters(),
						 plugin->getNumAudioOutputs(),
						 sampleRate,
						 estimatedSamplesPerBlock);
}

//------------------------------------------------------------------------------
void NAPInstance::releaseResources()
{
	
}

//------------------------------------------------------------------------------
void NAPInstance::processBlock(AudioSampleBuffer& buffer,
							   MidiBuffer& midiMessages)
{
	if(plugin)
	{
		// Cast away const to match the plugin interface
		plugin->processAudio(const_cast<float**>(buffer.getArrayOfWritePointers()),
							 const_cast<float**>(buffer.getArrayOfReadPointers()),
							 buffer.getNumSamples());
	}
}

//------------------------------------------------------------------------------
const String NAPInstance::getInputChannelName(int channelIndex) const
{
	return "Input " + String(channelIndex + 1);
}

//------------------------------------------------------------------------------
const String NAPInstance::getOutputChannelName(int channelIndex) const
{
	return "Output " + String(channelIndex + 1);
}

//------------------------------------------------------------------------------
bool NAPInstance::hasEditor() const
{
	return false;
}

//------------------------------------------------------------------------------
AudioProcessorEditor* NAPInstance::createEditor()
{
	return nullptr;
}

//------------------------------------------------------------------------------
int NAPInstance::getNumParameters()
{
	if(plugin)
		return plugin->getNumParameters();
	return 0;
}

//------------------------------------------------------------------------------
const String NAPInstance::getParameterName(int parameterIndex)
{
	if(plugin)
		return plugin->getParameterName(parameterIndex);
	return "";
}

//------------------------------------------------------------------------------
float NAPInstance::getParameter(int parameterIndex)
{
	// NiallsAudioPlugin doesn't have getParameter method
	// Return default value
	return 0.0f;
}

//------------------------------------------------------------------------------
const String NAPInstance::getParameterText(int parameterIndex)
{
	// NiallsAudioPlugin doesn't have getParameterText method
	// Return parameter name if available
	if(plugin && parameterIndex < plugin->getNumParameters())
		return plugin->getParameterName(parameterIndex).c_str();
	return "";
}

//------------------------------------------------------------------------------
void NAPInstance::setParameter(int parameterIndex, float newValue)
{
	// NiallsAudioPlugin doesn't have setParameter method
	// This is a no-op for now
}

//------------------------------------------------------------------------------
void NAPInstance::getStateInformation(MemoryBlock &destData)
{
	// Implementation for saving plugin state
}

//------------------------------------------------------------------------------
void NAPInstance::setStateInformation(const void *data, int sizeInBytes)
{
	// Implementation for loading plugin state
}

//------------------------------------------------------------------------------
double NAPInstance::getTailLengthSeconds() const
{
	return 0.0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static Array<NAPModuleHandle *> activeNAPModules;

//------------------------------------------------------------------------------
NAPModuleHandle::NAPModuleHandle(const File& file) :
	pluginFile(file),
	pluginCreator(nullptr),
	pluginName(""),
	hModule()  // Default construct DynamicLibrary
{
	activeNAPModules.add(this);

	fullParentDirectoryPathName = file.getParentDirectory().getFullPathName();
}

//------------------------------------------------------------------------------
NAPModuleHandle::~NAPModuleHandle()
{
	activeNAPModules.removeFirstMatchingValue(this);

	close();
}

//------------------------------------------------------------------------------
bool NAPModuleHandle::open()
{
	if(hModule.open(pluginFile.getFullPathName()))
	{
		pluginCreator = (NAPCreatePlugin)hModule.getFunction("createPlugin");
		if(pluginCreator != nullptr)
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
void NAPModuleHandle::close()
{
	if(hModule.getNativeHandle() != nullptr)
	{
		hModule.close();
	}
}

//------------------------------------------------------------------------------
NAPModuleHandle *NAPModuleHandle::findOrCreateModule(const File& file)
{
	int i;

	for(i=(activeNAPModules.size()-1);i>=0;--i)
	{
		NAPModuleHandle* const module = (NAPModuleHandle *)activeNAPModules.getUnchecked(i);

		if(module->pluginFile == file)
			return module;
	}

	Logger::writeToLog("Attempting to load NAP: " + file.getFullPathName());

	NAPModuleHandle* m = 0;
	m = new NAPModuleHandle(file);

	if(m)
	{
		if(!m->open())
		{
			/*delete m;
			m = 0;*/
			deleteAndZero(m);
		}
	}

	return m;
}
