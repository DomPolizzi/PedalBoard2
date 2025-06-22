//	ColourScheme.cpp - Singleton struct handling colour schemes.
#include "ColourScheme.h"
#include "JuceHelperStuff.h"

using namespace std;


ColourScheme& ColourScheme::getInstance()
{
	static ColourScheme retval;

	return retval;
}

const StringArray ColourScheme::getPresets() const
{
	int i;
	Array<File> files;
	StringArray retval;
	File settingsDir = JuceHelperStuff::getAppDataFolder();

	settingsDir.findChildFiles(files, File::findFiles, false, "*.colourscheme");
	for(i=0;i<files.size();++i)
		retval.add(files[i].getFileNameWithoutExtension());

	return retval;
}

void ColourScheme::loadPreset(const String& name)
{
	String filename;
	File settingsDir = JuceHelperStuff::getAppDataFolder();

	filename << name << ".colourscheme";

	File presetFile = settingsDir.getChildFile(filename);

	if(presetFile.existsAsFile())
	{
		std::unique_ptr<XmlElement> rootXml = XmlDocument::parse(presetFile);

		if(rootXml)
		{
			if(rootXml->hasTagName("Pedalboard2ColourScheme"))
			{
				for (auto* colour : rootXml->getChildIterator())
				{
					if(colour->hasTagName("Colour"))
					{
						String colName;
						String tempstr;

						colName = colour->getStringAttribute("name", "NoName");
						tempstr = colour->getStringAttribute("value", "FFFFFFFF");
						if(colName != "NoName")
							colours[colName] = Colour(tempstr.getHexValue32());
					}
				}
				presetName = name;
			}
		}
	}
}

void ColourScheme::savePreset(const String& name)
{
	String filename;
	map<String, Colour>::iterator it;
	XmlElement rootXml("Pedalboard2ColourScheme");
	File settingsDir = JuceHelperStuff::getAppDataFolder();

	filename << name << ".colourscheme";

	File presetFile = settingsDir.getChildFile(filename);

	for(it=colours.begin();it!=colours.end();++it)
	{
		XmlElement *colour = new XmlElement("Colour");

		colour->setAttribute("name", it->first);
		colour->setAttribute("value", it->second.toString());

		rootXml.addChildElement(colour);
	}

	presetName = name;
	rootXml.writeTo(presetFile);
}

bool ColourScheme::doesColoursMatchPreset(const String& name)
{
	String tempstr;
	File presetFile;
	bool retval = true;
	File settingsDir = JuceHelperStuff::getAppDataFolder();

	tempstr << name << ".colourscheme";
	presetFile = settingsDir.getChildFile(tempstr);

	if(presetFile.existsAsFile())
	{
		std::unique_ptr<XmlElement> rootXml = XmlDocument::parse(presetFile);

		if(rootXml)
		{
			if(rootXml->hasTagName("Pedalboard2ColourScheme"))
			{
				for (auto* colour : rootXml->getChildIterator())
				{
					if(colour->hasTagName("Colour"))
					{
						String colName;
						String value;

						colName = colour->getStringAttribute("name", "NoName");
						value = colour->getStringAttribute("value", "FFFFFFFF");

						if(colours[colName] != Colour(value.getHexValue32()))
						{
							retval = false;
							break;
						}
					}
				}
				presetName = name;
			}
		}
	}
	else
		retval = false;

	return retval;
}

ColourScheme::ColourScheme()
{
	File defaultFile = JuceHelperStuff::getAppDataFolder().getChildFile("default.colourscheme");

	if(defaultFile.existsAsFile())
		loadPreset("default");
	else
	{
		presetName = "default";

		colours["Window Background"] = Colour(0xFFEEECE1);
		colours["Field Background"] = Colour(Colour(0xFFEEECE1).brighter(0.5f));
		colours["Text Colour"] = Colour(0xFF000000);
		colours["Plugin Border"] = Colour(0xFFB0B0FF);
		colours["Plugin Background"] = Colour(0xFFFFFFFF);
		colours["Audio Connection"] = Colour(0xFFB0B0FF).darker(0.25f);
		colours["Parameter Connection"] = Colour(0xFFFFD3B3).darker(0.25f);
		colours["Button Colour"] = Colour(0xFFEEECE1);
		colours["Button Highlight"] = Colour(0xB0B0B0FF);
		colours["Text Editor Colour"] = Colour(0xFFFFFFFF);
		colours["Dialog Inner Background"] = Colour(0xFFFFFFFF);
		colours["CPU Meter Colour"] = Colour(0xB0B0B0FF);
		colours["Slider Colour"] = Colour(0xFF9A9181);
		colours["List Selected Colour"] = Colour(0xFFB0B0FF);
		colours["VU Meter Lower Colour"] = Colour(0x7F00BF00);
		colours["VU Meter Upper Colour"] = Colour(0x7FFFFF00);
		colours["VU Meter Over Colour"] = Colour(0x7FFF0000);
		colours["Vector Colour"] = Colour(0x80000000);
		colours["Menu Selection Colour"] = Colour(0x40000000);
		colours["Waveform Colour"] = Colour(0xFFB0B0FF);
		colours["Level Dial Colour"] = Colour(0xFFB0B0FF).darker(0.25f);
		colours["Tick Box Colour"] = Colour(0x809A9181);

		savePreset("default");
	}
}

ColourScheme::~ColourScheme()
{

}
