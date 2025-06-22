
#include <JuceHeader.h>
using namespace juce;

#include "MainPanel.h"
#include "App.h"
#include "Images.h"
#include "Vectors.h"
#include "LogFile.h"
#include "AboutPage.h"
#include "LogDisplay.h"
#include "TapTempoBox.h"
#include "PluginField.h"
#include "MainTransport.h"
#include "AudioSingletons.h"
#include "ColourSchemeEditor.h"
#include "PropertiesSingleton.h"
#include "PedalboardProcessors.h"
#include "ApplicationMappingsEditor.h"
#include "PatchOrganiser.h"
#include "UserPresetWindow.h"
#include "PreferencesDialog.h"
#include "ColourSchemeEditor.h"
#include "PropertiesSingleton.h"
#include "PedalboardProcessors.h"
#include "ApplicationMappingsEditor.h"
#include <sstream>
#include <iostream>

using namespace std;

File MainPanel::lastDocument = File();

class PluginListWindow  : public DocumentWindow
{
public:
    PluginListWindow(KnownPluginList& knownPluginList, MainPanel *p):
    DocumentWindow("Available Plugins",
				   Colour(0xffeeece1),
				   DocumentWindow::minimiseButton | DocumentWindow::closeButton),
	panel(p)
    {
        const File deadMansPedalFile(PropertiesSingleton::getInstance().getUserSettings()->getFile().getSiblingFile("RecentlyCrashedPluginsList"));

        setContentOwned(new PluginListComponent(AudioPluginFormatManagerSingleton::getInstance(),
												knownPluginList,
                                                deadMansPedalFile,
                                                PropertiesSingleton::getInstance().getUserSettings()),
							true);

        setResizable(true, false);
        //setResizeLimits(300, 400, 800, 1500);
        //setTopLeftPosition(60, 60);
		centreWithSize(300, 400);
		setUsingNativeTitleBar(true);
		//setDropShadowEnabled(false);
		getPeer()->setIcon(ImageCache::getFromMemory(Images::icon512_png,
													 Images::icon512_pngSize));

        restoreWindowStateFromString(PropertiesSingleton::getInstance().getUserSettings()->getValue("listWindowPos"));
        setVisible(true);
    }

    ~PluginListWindow()
    {
		panel->setListWindow(0);

        PropertiesSingleton::getInstance().getUserSettings()->setValue("listWindowPos", getWindowStateAsString());
    }

    void closeButtonPressed()
    {
        delete this;
    }
  private:
	///	The 'parent' main panel.
	MainPanel *panel;
};

MainPanel::MainPanel (ApplicationCommandManager *appManager)
    : FileBasedDocument(".pd", "*.pd", "Choose a set of patches to open...", "Choose a set of patches to save as..."),
      Thread("OSC Thread"), commandManager(appManager), currentPatch(0),
      patchLabel (0),
      prevPatch (0),
      nextPatch (0),
      patchComboBox (0),
      viewport (0),
      cpuSlider (0),
      cpuLabel (0),
      playButton (0),
      rtzButton (0),
      tempoLabel (0),
      tempoEditor (0),
      tapTempoButton (0)
{
    addAndMakeVisible (patchLabel = new Label ("patchLabel",
                                               "Patch:"));
    patchLabel->setFont (Font (15.0000f, Font::plain));
    patchLabel->setJustificationType (Justification::centredLeft);
    patchLabel->setEditable (false, false, false);
    patchLabel->setColour (juce::Label::textColourId, Colours::black);
    patchLabel->setColour (juce::Label::backgroundColourId, Colour (0x0));

    addAndMakeVisible (prevPatch = new TextButton ("prevPatch"));
    prevPatch->setButtonText ("-");
    prevPatch->setConnectedEdges (Button::ConnectedOnRight);
    prevPatch->addListener (this);

    addAndMakeVisible (nextPatch = new TextButton ("nextPatch"));
    nextPatch->setButtonText ("+");
    nextPatch->setConnectedEdges (Button::ConnectedOnLeft);
    nextPatch->addListener (this);

    addAndMakeVisible (patchComboBox = new ComboBox ("patchComboBox"));
    patchComboBox->setEditableText (true);
    patchComboBox->setJustificationType (Justification::centredLeft);
    patchComboBox->setTextWhenNothingSelected (juce::String());
    patchComboBox->setTextWhenNoChoicesAvailable ("(no choices)");
    patchComboBox->addItem ("1 - <untitled>", 1);
    patchComboBox->addItem ("<new patch>", 2);
    patchComboBox->addListener (this);

    addAndMakeVisible (viewport = new Viewport ("new viewport"));

    addAndMakeVisible (cpuSlider = new Slider ("cpuSlider"));
    cpuSlider->setRange (0, 1, 0);
    cpuSlider->setSliderStyle (Slider::LinearBar);
    cpuSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    cpuSlider->addListener (this);

    addAndMakeVisible (cpuLabel = new Label ("cpuLabel",
                                             "CPU Usage:"));
    cpuLabel->setFont (Font (15.0000f, Font::plain));
    cpuLabel->setJustificationType (Justification::centredLeft);
    cpuLabel->setEditable (false, false, false);
    cpuLabel->setColour (juce::Label::textColourId, Colours::black);
    cpuLabel->setColour (juce::Label::backgroundColourId, Colour (0x0));

    addAndMakeVisible (playButton = new DrawableButton ("playButton", DrawableButton::ImageOnButtonBackground));
    playButton->setName ("playButton");

    addAndMakeVisible (rtzButton = new DrawableButton ("rtzButton", DrawableButton::ImageOnButtonBackground));
    rtzButton->setName ("rtzButton");

    addAndMakeVisible (tempoLabel = new Label ("tempoLabel",
                                               "Tempo:"));
    tempoLabel->setFont (Font (15.0000f, Font::plain));
    tempoLabel->setJustificationType (Justification::centredLeft);
    tempoLabel->setEditable (false, false, false);
    tempoLabel->setColour (juce::Label::textColourId, Colours::black);
    tempoLabel->setColour (juce::Label::backgroundColourId, Colour (0x0));

    addAndMakeVisible (tempoEditor = new TextEditor ("tempoEditor"));
    tempoEditor->setMultiLine (false);
    tempoEditor->setReturnKeyStartsNewLine (false);
    tempoEditor->setReadOnly (false);
    tempoEditor->setScrollbarsShown (true);
    tempoEditor->setCaretVisible (true);
    tempoEditor->setPopupMenuEnabled (true);
    tempoEditor->setText ("120.00");

    addAndMakeVisible (tapTempoButton = new ArrowButton ("tapTempoButton", 0.0, Colour(0x40000000)));
    tapTempoButton->setName ("tapTempoButton");

	Colour buttonCol = ColourScheme::getInstance().colours["Button Colour"];

	patches.add(0);

	programChangePatch = currentPatch;
	lastCombo = 1;

	listWindow = 0;

	doNotSaveNextPatch = false;

	lastTempoTicks = 0;

	prevPatch->setTooltip("Previous patch");
	nextPatch->setTooltip("Next patch");
	playButton->setTooltip("Play (main transport)");
	rtzButton->setTooltip("Return to zero (main transport)");
	tapTempoButton->setTooltip("Tap tempo");

	//So the user can't drag the cpu meter.
	cpuSlider->setInterceptsMouseClicks(false, true);
	cpuSlider->setColour(Slider::thumbColourId, ColourScheme::getInstance().colours["CPU Meter Colour"]);

	//Setup the DrawableButton images.
	playImage.reset(loadSVGFromMemory(Vectors::playbutton_svg,
								  Vectors::playbutton_svgSize));
	pauseImage.reset(loadSVGFromMemory(Vectors::pausebutton_svg,
							   Vectors::pausebutton_svgSize));
	playButton->setImages(playImage.get());
	playButton->setColour(DrawableButton::backgroundColourId,
						  buttonCol);
	playButton->setColour(DrawableButton::backgroundOnColourId,
						  buttonCol);
	playButton->addListener(this);

	std::unique_ptr<Drawable> rtzImage(loadSVGFromMemory(Vectors::rtzbutton_svg,
											   Vectors::rtzbutton_svgSize));
	rtzButton->setImages(rtzImage.get());
	rtzButton->setColour(DrawableButton::backgroundColourId,
						 buttonCol);
	rtzButton->setColour(DrawableButton::backgroundOnColourId,
						 buttonCol);
	rtzButton->addListener(this);

	MainTransport::getInstance()->registerTransport(this);

	tempoEditor->addListener(this);
	tempoEditor->setInputRestrictions(0, "0123456789.");

	tapTempoButton->addListener(this);

	//Setup the soundcard.
	String tempstr;
	std::unique_ptr<XmlElement> savedAudioState = PropertiesSingleton::getInstance().getUserSettings()->getXmlValue("audioDeviceState");
    tempstr = deviceManager.initialise(2, 2, savedAudioState.get(), true);
    // savedAudioState will be automatically deleted by unique_ptr

	//Load the plugin list.
	std::unique_ptr<XmlElement> savedPluginList = PropertiesSingleton::getInstance().getUserSettings()->getXmlValue("pluginList");
    if (savedPluginList)
    {
        pluginList.recreateFromXml(*savedPluginList);
    }
	{
		LevelProcessor lev;
		FilePlayerProcessor fPlay;
		OutputToggleProcessor toggle;
		VuMeterProcessor vuMeter;
		RecorderProcessor recorder;
		MetronomeProcessor metronome;
		LooperProcessor looper;
		PluginDescription desc;

		lev.fillInPluginDescription(desc);
		pluginList.addType(desc);

		fPlay.fillInPluginDescription(desc);
		pluginList.addType(desc);

		toggle.fillInPluginDescription(desc);
		pluginList.addType(desc);

		vuMeter.fillInPluginDescription(desc);
		pluginList.addType(desc);

		recorder.fillInPluginDescription(desc);
		pluginList.addType(desc);

		metronome.fillInPluginDescription(desc);
		pluginList.addType(desc);

		looper.fillInPluginDescription(desc);
		pluginList.addType(desc);
	}
	pluginList.addChangeListener(this);

	pluginList.sort(KnownPluginList::sortAlphabetically, true);

	//Setup the signal path to connect it to the soundcard.
	graphPlayer.setProcessor(&signalPath.getGraph());
	deviceManager.addAudioCallback(&graphPlayer);

	//Setup midi.
	{
		auto midiInputs = juce::MidiInput::getAvailableDevices();
		for (const auto& midiInput : midiInputs)
			deviceManager.setMidiInputDeviceEnabled(midiInput.identifier, true);

		deviceManager.addMidiInputDeviceCallback("", &graphPlayer); // Listen to all enabled MIDI inputs
	}

	//Setup the PluginField.
	PluginField *field = new PluginField(&signalPath,
										 &pluginList,
										 commandManager);
	field->addChangeListener(this);
	viewport->setViewedComponent(field);
	viewport->setWantsKeyboardFocus(false);

	patchComboBox->setSelectedId(1);

	//Setup the socket.
	{
		String port, address;

		port = PropertiesSingleton::getInstance().getUserSettings()->getValue("OSCPort", "5678");
		if(port == "")
			port = "5678";
		address = PropertiesSingleton::getInstance().getUserSettings()->getValue("OSCMulticastAddress");

		if(PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("OscInput", "1"))
		{
			sock.setPort((int16_t)port.getIntValue());
			sock.setMulticastGroup(std::string(address.toUTF8()));
			sock.bindSocket();
			startThread();
		}
	}

	savePatch();

	//Necessary?
    Process::setPriority(Process::HighPriority);

	//Used to ensure we get MidiAppMapping events even when the window's not
	//focused.
	appManager->setFirstCommandTarget(this);

    //[/UserPreSize]

    setSize (1024, 570);


    //[Constructor] You can add your own custom stuff here..

	//Setup the program change warning.
	warningBox.reset(new CallOutBox(warningText, patchComboBox->getBounds(), this));
	warningBox->setVisible(false);

	//Start timers.
	startTimer(CpuTimer, 100);
	startTimer(MidiAppTimer, 30);

	//To load the default patch.
				File defaultFile = JuceHelperStuff::getAppDataFolder().getChildFile("default.pd");

		if(defaultFile.existsAsFile())
			commandManager->invokeDirectly(FileNew, true);
	}
    //[/Constructor]

MainPanel::~MainPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	//Logger::setCurrentLogger(0);

	signalThreadShouldExit();

	//deviceManager.setAudioCallback(0);
	deviceManager.removeAudioCallback(&graphPlayer);
	deviceManager.removeMidiInputDeviceCallback("", &graphPlayer); // Remove callback for all MIDI inputs
    graphPlayer.setProcessor(0);
    signalPath.clear(false, false, false);

	if(listWindow)
		delete listWindow;

	for(int i=0;i<patches.size();++i)
		delete patches[i];
    //[/Destructor_pre]

    patchLabel = nullptr;
    prevPatch = nullptr;
    nextPatch = nullptr;
    patchComboBox = nullptr;
    viewport = nullptr;
    cpuSlider = nullptr;
    cpuLabel = nullptr;
    playButton = nullptr;
    rtzButton = nullptr;
    tempoLabel = nullptr;
    tempoEditor = nullptr;
    tapTempoButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..

	MainTransport::getInstance()->unregisterTransport(this);
	if(LogFile::getInstance().getIsLogging())
		LogFile::getInstance().stop();

    //[/Destructor]
}

//==============================================================================
void MainPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..

	Colour tempCol = ColourScheme::getInstance().colours["Button Colour"];

	playButton->setColour(DrawableButton::backgroundColourId,
						  tempCol);
	playButton->setColour(DrawableButton::backgroundOnColourId,
						  tempCol);
	rtzButton->setColour(DrawableButton::backgroundColourId,
						 tempCol);
	rtzButton->setColour(DrawableButton::backgroundOnColourId,
						 tempCol);

    //[/UserPrePaint]

    g.fillAll (Colour (0xffeeece1));

    //[UserPaint] Add your own custom painting code here..

	g.fillAll(ColourScheme::getInstance().colours["Window Background"]);

    //[/UserPaint]
}

void MainPanel::resized()
{
    patchLabel->setBounds (8, getHeight() - 33, 48, 24);
    prevPatch->setBounds (264, getHeight() - 33, 24, 24);
    nextPatch->setBounds (288, getHeight() - 33, 24, 24);
    patchComboBox->setBounds (56, getHeight() - 33, 200, 24);
    viewport->setBounds (0, 0, getWidth() - 0, getHeight() - 40);
    cpuSlider->setBounds (getWidth() - 156, getHeight() - 33, 150, 24);
    cpuLabel->setBounds (getWidth() - 236, getHeight() - 33, 78, 24);
    playButton->setBounds (proportionOfWidth (0.5000f) - ((36) / 2), getHeight() - 38, 36, 36);
    rtzButton->setBounds ((proportionOfWidth (0.5000f) - ((36) / 2)) + 38, getHeight() - 32, 24, 24);
    tempoLabel->setBounds ((proportionOfWidth (0.5000f) - ((36) / 2)) + -151, getHeight() - 33, 64, 24);
    tempoEditor->setBounds ((proportionOfWidth (0.5000f) - ((36) / 2)) + -87, getHeight() - 33, 52, 24);
    tapTempoButton->setBounds ((proportionOfWidth (0.5000f) - ((36) / 2)) + -31, getHeight() - 27, 10, 16);
    //[UserResized] Add your own custom resize handling here..

	int x, y;
	Component *field = viewport->getViewedComponent();

	x = field->getWidth();
	y = field->getHeight();
	if(field->getWidth() < getWidth())
		x = getWidth();
	if(field->getHeight() < (getHeight()-40))
		y = getHeight()-40;
	field->setSize(x, y);

    //[/UserResized]
}

void MainPanel::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == prevPatch)
    {
        //[UserButtonCode_prevPatch] -- add your button handler code here..
		commandManager->invokeDirectly(PatchPrevPatch, true);
        //[/UserButtonCode_prevPatch]
    }
    else if (buttonThatWasClicked == nextPatch)
    {
        //[UserButtonCode_nextPatch] -- add your button handler code here..
        /*if(patchComboBox->getSelectedItemIndex() < (patchComboBox->getNumItems()-2))
        	patchComboBox->setSelectedItemIndex(patchComboBox->getSelectedItemIndex()+1);*/
		commandManager->invokeDirectly(PatchNextPatch, true);
        //[/UserButtonCode_nextPatch]
    }

    //[UserbuttonClicked_Post]

	else if(buttonThatWasClicked == playButton)
		commandManager->invokeDirectly(TransportPlay, true);
	else if(buttonThatWasClicked == rtzButton)
		commandManager->invokeDirectly(TransportRtz, true);
	else if(buttonThatWasClicked == tapTempoButton)
	{
		PluginField *pluginField = dynamic_cast<PluginField *>(viewport->getViewedComponent());
		TapTempoBox tempoBox(pluginField, tempoEditor);

		auto& content = tempoBox;
		auto* callout = &CallOutBox::launchAsynchronously(std::make_unique<TapTempoBox>(pluginField, tempoEditor),
											tapTempoButton->getBounds(), this);
	}

    //[/UserbuttonClicked_Post]
}

void MainPanel::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == patchComboBox)
    {
        //[UserComboBoxCode_patchComboBox] -- add your combo box handling code here..

		//Add a new patch.
		if(patchComboBox->getSelectedItemIndex() == (patchComboBox->getNumItems()-1))
		{
			String tempstr;

			//Save current patch.
			savePatch();

			//Setup the new ComboBox stuff.
			tempstr << patchComboBox->getNumItems() << " - <untitled>";
			patchComboBox->changeItemText(patchComboBox->getNumItems(),
										  tempstr);
			patchComboBox->addItem("<new patch>", patchComboBox->getNumItems()+1);
			patches.add(0);

			//Make the new patch the current patch, clear it to the default
			//state.
			patchComboBox->setSelectedItemIndex(patchComboBox->getNumItems()-2, juce::NotificationType::dontSendNotification);
			switchPatch(patchComboBox->getNumItems()-2);
			savePatch();

			changed();

			int temp;

			temp = patches.size();

			PluginField* pluginField = dynamic_cast<PluginField*>(viewport->getViewedComponent());
			if (pluginField)
				pluginField->clearDoubleClickMessage();
		}
		//Update the patch text if the user's changed it.
		else if(patchComboBox->getSelectedItemIndex() == -1)
		{
		    patchComboBox->changeItemText(lastCombo,
		    							  patchComboBox->getText());
			patches[currentPatch]->setAttribute("name", patchComboBox->getText());

			changed();
		}
		//Switch to the new patch.
		else
		{
			switchPatch(patchComboBox->getSelectedItemIndex());
		}

		lastCombo = patchComboBox->getSelectedId();

        //[/UserComboBoxCode_patchComboBox]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void MainPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == cpuSlider)
    {
        //[UserSliderCode_cpuSlider] -- add your slider handling code here..
        //[/UserSliderCode_cpuSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
StringArray MainPanel::getMenuBarNames()
{
	StringArray retval;

	retval.add("File");
	retval.add("Edit");
	retval.add("Options");
	retval.add("Help");

	return retval;
}

//------------------------------------------------------------------------------
PopupMenu MainPanel::getMenuForIndex(int topLevelMenuIndex,
									 const String &menuName)
{
	PopupMenu retval;

	if(menuName == "File")
	{
		retval.addCommandItem(commandManager, FileNew);
		retval.addCommandItem(commandManager, FileOpen);
		retval.addSeparator();
		retval.addCommandItem(commandManager, FileSave);
		retval.addCommandItem(commandManager, FileSaveAs);
		retval.addSeparator();
		retval.addCommandItem(commandManager, FileSaveAsDefault);
		retval.addCommandItem(commandManager, FileResetDefault);
		retval.addSeparator();
		retval.addCommandItem(commandManager, FileExit);
	}
	else if(menuName == "Edit")
	{
		retval.addCommandItem(commandManager, EditDeleteConnection);
		retval.addSeparator();
		retval.addCommandItem(commandManager, EditOrganisePatches);
		retval.addCommandItem(commandManager, EditUserPresetManagement);
	}
	else if(menuName == "Options")
	{
		retval.addCommandItem(commandManager, OptionsAudio);
		retval.addCommandItem(commandManager, OptionsPluginList);
		retval.addCommandItem(commandManager, OptionsPreferences);
		retval.addCommandItem(commandManager, OptionsColourSchemes);
		retval.addSeparator();
		retval.addCommandItem(commandManager, OptionsKeyMappings);
	}
	else if(menuName == "Help")
	{
		retval.addCommandItem(commandManager, HelpDocumentation);
		retval.addCommandItem(commandManager, HelpLog);
		retval.addSeparator();
		retval.addCommandItem(commandManager, HelpAbout);
	}

	return retval;
}

//------------------------------------------------------------------------------
void MainPanel::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{

}

//------------------------------------------------------------------------------
ApplicationCommandTarget *MainPanel::getNextCommandTarget()
{
	return findFirstTargetParentComponent();
}

//------------------------------------------------------------------------------
void MainPanel::getAllCommands(Array<CommandID> &commands)
{
	const CommandID ids[] = { FileNew,
							  FileOpen,
							  FileSave,
							  FileSaveAs,
							  FileSaveAsDefault,
							  FileResetDefault,
							  FileExit,
							  EditDeleteConnection,
							  EditOrganisePatches,
							  EditUserPresetManagement,
							  OptionsPreferences,
							  OptionsAudio,
							  OptionsPluginList,
							  OptionsColourSchemes,
							  OptionsKeyMappings,
							  HelpAbout,
							  HelpDocumentation,
							  HelpLog,
							  PatchNextPatch,
							  PatchPrevPatch,
							  TransportPlay,
							  TransportRtz,
							  TransportTapTempo
							};
	commands.addArray(ids, numElementsInArray(ids));
}

//------------------------------------------------------------------------------
void MainPanel::getCommandInfo(const CommandID commandID,
							   ApplicationCommandInfo &result)
{
	const String fileCategory("File");
	const String editCategory("Edit");
	const String optionsCategory("Options");
	const String helpCategory("Help");
	const String patchCategory("Patch");
	const String transportCategory("Main Transport");

	switch(commandID)
	{
		case FileNew:
			result.setInfo("New",
					   "Creates a new pedalboard file to work from.",
					   fileCategory,
					   0);
			result.addDefaultKeypress('n', ModifierKeys::commandModifier);
			break;
		case FileOpen:
			result.setInfo("Open...",
					   "Opens an existing pedalboard file from disk.",
					   fileCategory,
					   0);
			result.addDefaultKeypress('o', ModifierKeys::commandModifier);
			break;
		case FileSave:
			result.setInfo("Save",
					   "Saves the current pedalboard file to disk.",
					   fileCategory,
					   0);
			result.addDefaultKeypress('s', ModifierKeys::commandModifier);
			break;
		case FileSaveAs:
			result.setInfo("Save As...",
					   "Saves the current pedalboard file to a new file on disk.",
					   fileCategory,
					   0);
			result.addDefaultKeypress('s', ModifierKeys::commandModifier|ModifierKeys::shiftModifier);
			break;
		case FileSaveAsDefault:
			result.setInfo("Save As Default",
					   "Saves the current pedalboard file as the default file to load.",
					   fileCategory,
					   0);
			break;
		case FileResetDefault:
			result.setInfo("Reset Default",
					   "Resets the default pedalboard file to its original state.",
					   fileCategory,
					   0);
			break;
		case FileExit:
			result.setInfo("Exit",
					   "Quits the program.",
					   fileCategory,
					   0);
			break;
		case EditDeleteConnection:
			result.setInfo("Delete selected connection(s)",
					   "Deletes the selected connection(s).",
					   editCategory,
					   0);
			result.addDefaultKeypress(KeyPress::deleteKey, ModifierKeys());
			result.addDefaultKeypress(KeyPress::backspaceKey, ModifierKeys());
			break;
		case EditOrganisePatches:
			result.setInfo("Organise patches",
					   "Opens the patch organiser.",
					   editCategory,
					   0);
			break;
		case EditUserPresetManagement:
			result.setInfo("User Preset Management",
					   "Opens the user preset management window.",
					   editCategory,
					   0);
			break;
		case OptionsPreferences:
			result.setInfo("Misc Settings",
					   "Displays miscellaneous settings.",
					   optionsCategory,
					   0);
			break;
		case OptionsAudio:
			result.setInfo("Audio Settings",
					   "Displays soundcard settings.",
					   optionsCategory,
					   0);
			break;
		case OptionsPluginList:
			result.setInfo("Plugin List",
					   "Options to scan and remove plugins.",
					   optionsCategory,
					   0);
			break;
		case OptionsColourSchemes:
			result.setInfo("Colour Schemes",
					   "Load and edit alternate colour schemes.",
					   optionsCategory,
					   0);
			break;
		case OptionsKeyMappings:
			result.setInfo("Application Mappings",
					   "Change the application mappings.",
					   optionsCategory,
					   0);
			break;
		case HelpDocumentation:
			result.setInfo("Documentation",
					   "Loads the documentation in your default browser.",
					   helpCategory,
					   0);
			result.addDefaultKeypress(KeyPress::F1Key, ModifierKeys());
			break;
		case HelpLog:
			result.setInfo("Event Log",
					   "Displays an event log for the program.",
					   helpCategory,
					   0);
			break;
		case HelpAbout:
			result.setInfo("About",
					   "Shows some details about the program.",
					   helpCategory,
					   0);
			break;
		case PatchNextPatch:
			result.setInfo("Next Patch",
					   "Switches to the next patch.",
					   patchCategory,
					   0);
			break;
		case PatchPrevPatch:
			result.setInfo("Previous Patch",
					   "Switches to the previous patch.",
					   patchCategory,
					   0);
			break;
		case TransportPlay:
			result.setInfo("Play/Pause",
					   "Plays/pauses the main transport.",
					   transportCategory,
					   0);
			result.addDefaultKeypress(KeyPress::spaceKey, ModifierKeys());
			break;
		case TransportRtz:
			result.setInfo("Return to Zero",
					   "Returns the main transport to the beginning.",
					   transportCategory,
					   0);
			break;
		case TransportTapTempo:
			result.setInfo("Tap Tempo",
					   "Tap to set the tempo.",
					   transportCategory,
					   0);
			break;
	}
}

//------------------------------------------------------------------------------
void MainPanel::setSocketPort(const String& port)
{
	int16_t tempVal;
	ScopedLock lock(sockCritSec);

	tempVal = (int16_t)port.getIntValue();
	sock.setPort(tempVal);
	sock.bindSocket();

	PropertiesSingleton::getInstance().getUserSettings()->setValue("OSCPort", port);
}

//------------------------------------------------------------------------------
void MainPanel::setSocketMulticast(const String& address)
{
	ScopedLock lock(sockCritSec);

	sock.setMulticastGroup(std::string(address.toUTF8()));
	sock.bindSocket();

	PropertiesSingleton::getInstance().getUserSettings()->setValue("OSCMulticastAddress", address);
}

//------------------------------------------------------------------------------
void MainPanel::enableAudioInput(bool val)
{
	PluginField *field = dynamic_cast<PluginField *>(viewport->getViewedComponent());

	field->enableAudioInput(val);

	PropertiesSingleton::getInstance().getUserSettings()->setValue("AudioInput", val);
}

//------------------------------------------------------------------------------
void MainPanel::enableMidiInput(bool val)
{
	PluginField *field = dynamic_cast<PluginField *>(viewport->getViewedComponent());

	field->enableMidiInput(val);

	PropertiesSingleton::getInstance().getUserSettings()->setValue("MidiInput", val);
}

//------------------------------------------------------------------------------
void MainPanel::enableOscInput(bool val)
{
	PluginField *field = dynamic_cast<PluginField *>(viewport->getViewedComponent());

	field->enableOscInput(val);

	//If there's no OSC input, we don't need to run the OSC thread.
	if(!val && isThreadRunning())
		signalThreadShouldExit();
	else if(val && !isThreadRunning())
	{
		String port, address;

		port = PropertiesSingleton::getInstance().getUserSettings()->getValue("OSCPort");
		if(port == "")
			port = "5678";
		address = PropertiesSingleton::getInstance().getUserSettings()->getValue("OSCMulticastAddress");

		sock.setPort((int16_t)port.getIntValue());
		sock.setMulticastGroup(std::string(address.toUTF8()));
		sock.bindSocket();
		startThread();
	}

	PropertiesSingleton::getInstance().getUserSettings()->setValue("OscInput", val);
}

//------------------------------------------------------------------------------
void MainPanel::setAutoMappingsWindow(bool val)
{
	PluginField *field = dynamic_cast<PluginField *>(viewport->getViewedComponent());

	field->setAutoMappingsWindow(val);

	PropertiesSingleton::getInstance().getUserSettings()->setValue("AutoMappingsWindow", val);
}

//------------------------------------------------------------------------------
void MainPanel::run()
{
	char *data;
	int32_t dataSize;
	PluginField *field = dynamic_cast<PluginField *>(viewport->getViewedComponent());

	while(!threadShouldExit())
	{
		{
			ScopedLock lock(sockCritSec);

			data = sock.getData(dataSize);
		}

		if(field && (dataSize > 0))
			field->socketDataArrived(data, dataSize);
	}
}

//------------------------------------------------------------------------------
String MainPanel::getDocumentTitle()
{
	return "Pedalboard2 Patch File";
}

//------------------------------------------------------------------------------
Result MainPanel::loadDocument (const File& file)
{
	int i;
	XmlDocument doc(file);
	std::unique_ptr<XmlElement> root = doc.getDocumentElement();
	//XmlElement *patch;

	if(root)
	{
		if(root->hasTagName("Pedalboard2PatchFile"))
		{
			//Clear existing patches.
			for(int i=0;i<patches.size();++i)
				delete patches[i];
			patches.clear();

			//Clear patchComboBox.
			patchComboBox->clear();
			/*patchComboBox->addItem("1 - <untitled>", 1);
			patchComboBox->addItem("<new patch>", 2);
			patchComboBox->setSelectedId(1);*/

			//If there are audio settings saved in this file and
			//pdlAudioSettings is set, load them.
			if(PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("pdlAudioSettings"))
			{
				auto* deviceXml = root->getChildByName("DEVICESETUP");

				if(deviceXml)
				{
					String err;

					err = deviceManager.initialise(2, 2, deviceXml, true);

					if(err != "")
					{
						AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
														 "Audio Device Error",
														 "Could not initialise audio settings loaded from .pdl file");
					}
				}
			}

			//Load any xml patches into patches.
			for (auto* patch : root->getChildWithTagNameIterator("*"))
			{
				if(patch->getTagName() == "Patch")
					patches.add(patch);
			}

			//Remove xml patches from root, so they don't get deleted.
			for(i=(root->getNumChildElements()-1);i>=0;--i)
			{
				if(root->getChildElement(i)->getTagName() == "Patch")
					root->removeChildElement(root->getChildElement(i), false);
			}

			// No need to delete root as it's a std::unique_ptr that will clean up automatically

			//Load the current patch.
			switchPatch(0, false);

			//Fill out patchComboBox.
			for(int i=0;i<patches.size();++i)
				patchComboBox->addItem(patches[i]->getStringAttribute("name"), i+1);
			patchComboBox->addItem("<new patch>", patches.size()+1);
			patchComboBox->setSelectedItemIndex(0, juce::NotificationType::sendNotificationSync);
		}
	}

	return Result::ok();
}

//------------------------------------------------------------------------------
Result MainPanel::saveDocument (const File& file)
{
	int i;
	PluginField *field = ((PluginField *)viewport->getViewedComponent());
	std::unique_ptr<XmlElement> main = std::make_unique<XmlElement>("Pedalboard2PatchFile");
	XmlElement *patch = field->getXml();

	//Save the current patch.
	{
		patch->setAttribute("name", patchComboBox->getText());

		delete patches[currentPatch];
		patches.set(currentPatch, patch);
	}

	for(int i=0;i<patches.size();++i)
		main->addChildElement(patches[i]);

	if(PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("pdlAudioSettings"))
		main->addChildElement(deviceManager.createStateXml().release());

	main->writeTo(file);

	//Remove the child "Patch" elements so they don't get deleted.
	for((i=main->getNumChildElements()-1);i>=0;--i)
		main->removeChildElement(main->getChildElement(i), false);

	return Result::ok();
}

//------------------------------------------------------------------------------
File MainPanel::getLastDocumentOpened()
{
	return lastDocument;
}

//------------------------------------------------------------------------------
void MainPanel::setLastDocumentOpened (const File& file)
{
	lastDocument = file;
}

//------------------------------------------------------------------------------
void MainPanel::addPatch(XmlElement *patch)
{
	patches.add(patch);

	patchComboBox->changeItemText(patchComboBox->getNumItems(),
								  patch->getStringAttribute("name"));
	patchComboBox->addItem("<new patch>", patchComboBox->getNumItems()+1);

	changed();
}

//------------------------------------------------------------------------------
void MainPanel::savePatch()
{
	XmlElement *patch = 0;
	PluginField *field = ((PluginField *)viewport->getViewedComponent());

	//Save current patch.
	patch = field->getXml();
	patch->setAttribute("name", patchComboBox->getItemText(lastCombo-1));
	delete patches[currentPatch];
	patches.set(currentPatch, patch);
}

//------------------------------------------------------------------------------
void MainPanel::duplicatePatch(int index)
{
	String tempstr;
	XmlElement *patch;
	//PluginField *field = ((PluginField *)viewport->getViewedComponent());

	jassert((index > -1) && (index < patches.size()));

	//Save current patch.
	savePatch();

	//Setup the new ComboBox stuff.
	tempstr << patches[index]->getStringAttribute("name") << " (copy)";
	patchComboBox->changeItemText(patchComboBox->getNumItems(), tempstr);
	patchComboBox->addItem("<new patch>", patchComboBox->getNumItems()+1);

	//Copy the current patch to the new one.
	patch = new XmlElement(*patches[index]);
	patch->setAttribute("name", tempstr);
	patches.set(patches.size(), patch);

	changed();
}

//------------------------------------------------------------------------------
void MainPanel::nextSwitchDoNotSavePrev()
{
	doNotSaveNextPatch = true;
}

//------------------------------------------------------------------------------
void MainPanel::switchPatchFromProgramChange(int newPatch)
{
	//programChangePatch = newPatch;
	/*if(newPatch > currentPatch)
		midiAppFifo.writeID(PatchNextPatch);
	else if(newPatch < currentPatch)
		midiAppFifo.writeID(PatchPrevPatch);*/
	midiAppFifo.writePatchChange(newPatch);
}

//------------------------------------------------------------------------------
/*void MainPanel::logMessage(const String &message)
{
	CharPointer_UTF8 temp = message.toUTF8();
	String endline = "\n";

	outFile.write(temp.getAddress(), temp.length());
	outFile.write(endline.toUTF8().getAddress(), endline.toUTF8().length());
}*/

//------------------------------------------------------------------------------
Drawable *MainPanel::loadSVGFromMemory(const void *dataToInitialiseFrom,
									   size_t sizeInBytes)
{
	Drawable *retval = 0;

	MemoryBlock memBlock(dataToInitialiseFrom, sizeInBytes);
	XmlDocument doc(memBlock.toString());
	auto svgData = doc.getDocumentElement();

	retval = Drawable::createFromSVG(*svgData).release();

	return retval;
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MainPane" componentName=""
                 parentClasses="public Component, public MenuBarModel, public ApplicationCommandTarget, public MultiTimer, public ChangeListener, public FileBasedDocument, public Thread, public FileDragAndDropTarget, public TextEditor::Listener"
                 constructorParams="ApplicationCommandManager *appManager" variableInitialisers="FileBasedDocument(&quot;.pdl&quot;, &quot;*.pdl&quot;, &quot;Choose a set of patches to open...&quot;, &quot;Choose a set of patches to save as...&quot;),&#10;Thread(&quot;OSC Thread&quot;), commandManager(appManager), currentPatch(0)&#10;"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="0" initialWidth="1024" initialHeight="570">
  <BACKGROUND backgroundColour="ffeeece1"/>
  <LABEL name="patchLabe" id="7487f2115f5ed988" memberName="patchLabe"
         virtualName="" explicitFocusOrder="0" pos="8 33R 48 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Patch:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="prevPatch" id="342ff0e9a494b29" memberName="prevPatch"
              virtualName="" explicitFocusOrder="0" pos="264 33R 24 24" buttonText="-"
              connectedEdges="2" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="nextPatch" id="6247f6fccfbcb165" memberName="nextPatch"
              virtualName="" explicitFocusOrder="0" pos="288 33R 24 24" buttonText="+"
              connectedEdges="1" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="patchComboBox" id="20bec1c831d2831c" memberName="patchComboBox"
            virtualName="" explicitFocusOrder="0" pos="56 33R 200 24" editable="1"
            layout="33" items="1 - &lt;untitled&gt;&#10;&lt;new patch&gt;"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <VIEWPORT name="new viewport" id="17841313120b1834" memberName="viewport"
            virtualName="" explicitFocusOrder="0" pos="0 0 0M 40M" vscroll="1"
            hscroll="1" scrollbarThickness="18" contentType="0" jucerFile=""
            contentClass="" constructorParams=""/>
  <SLIDER name="cpuSlider" id="49855b028c510925" memberName="cpuSlider"
          virtualName="" explicitFocusOrder="0" pos="156R 33R 150 24" min="0"
          max="1" int="0" style="LinearBar" textBoxPos="NoTextBox" textBoxEditable="0"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <LABEL name="cpuLabe" id="896921bd35cf3005" memberName="cpuLabe" virtualName=""
         explicitFocusOrder="0" pos="236R 33R 78 24" edTextCol="ff000000"
         edBkgCol="0" labelText="CPU Usage:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="playButton" id="382190f9abb24dc2" memberName="playButton"
                    virtualName="" explicitFocusOrder="0" pos="50%c 38R 36 36" class="DrawableButton"
                    params="&quot;playButton&quot;, DrawableButton::ImageOnButtonBackground"/>
  <GENERICCOMPONENT name="rtzButton" id="22f2164788c3f1be" memberName="rtzButton"
                    virtualName="" explicitFocusOrder="0" pos="38 32R 24 24" posRelativeX="382190f9abb24dc2"
                    class="DrawableButton" params="&quot;rtzButton&quot;, DrawableButton::ImageOnButtonBackground"/>
  <LABEL name="tempoLabe" id="b6ca8b83aba988fd" memberName="tempoLabe"
         virtualName="" explicitFocusOrder="0" pos="-151 33R 64 24" posRelativeX="382190f9abb24dc2"
         edTextCol="ff000000" edBkgCol="0" labelText="Tempo:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="tempoEditor" id="17103462ab2d58d1" memberName="tempoEditor"
              virtualName="" explicitFocusOrder="0" pos="-87 33R 52 24" posRelativeX="382190f9abb24dc2"
              initialText="120.00" multiline="0" retKeyStartsLine="0" readonly="0"
              scrollbars="1" caret="1" popupmenu="1"/>
  <GENERICCOMPONENT name="tapTempoButton" id="2fec60b9d3555246" memberName="tapTempoButton"
                    virtualName="" explicitFocusOrder="0" pos="-31 27R 10 16" posRelativeX="382190f9abb24dc2"
                    class="ArrowButton" params="\"tapTempoButton\", 0.0, Colour(0x40000000)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

