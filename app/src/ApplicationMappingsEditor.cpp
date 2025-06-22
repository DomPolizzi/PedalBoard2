//	ApplicationMappingsEditor.cpp - Component used to edit the application
//								  mappings.


#include "ApplicationMappingsEditor.h"
#include "MidiMappingManager.h"
#include "OscMappingManager.h"

//------------------------------------------------------------------------------
ApplicationMappingsEditor::ApplicationMappingsEditor(ApplicationCommandManager *app,
													 MidiMappingManager *midi,
													 OscMappingManager *osc):
appManager(app),
midiManager(midi),
oscManager(osc)
{
	mappingsTree = new TreeView();
	mappingsTree->setRootItem(new RootItem(app, midi, osc));
	mappingsTree->setRootItemVisible(false);
	mappingsTree->setSize(400, 430);
	addAndMakeVisible(mappingsTree);

	resetButton = new TextButton("Reset to defaults");
	resetButton->setBounds(298, 444, 110, 24);
	resetButton->addListener(this);
	addAndMakeVisible(resetButton);

	setSize(400, 524);
}

//------------------------------------------------------------------------------
ApplicationMappingsEditor::~ApplicationMappingsEditor()
{
	mappingsTree->deleteRootItem();
	deleteAllChildren();
}

//------------------------------------------------------------------------------
void ApplicationMappingsEditor::paint(Graphics& g)
{
	g.fillAll(ColourScheme::getInstance().colours["Window Background"]);
}

//------------------------------------------------------------------------------
void ApplicationMappingsEditor::resized()
{
	mappingsTree->setSize(getWidth(), getHeight()-28);

	resetButton->setTopLeftPosition((getWidth()-112), (getHeight()-26));
}

//------------------------------------------------------------------------------
void ApplicationMappingsEditor::buttonClicked(Button *button)
{
	int i;

	if(button == resetButton)
	{
		appManager->getKeyMappings()->resetToDefaultMappings();

		for(i=(midiManager->getNumAppMappings()-1);i>=0;--i)
			delete midiManager->getAppMapping(i);

		for(i=(oscManager->getNumAppMappings()-1);i>=0;--i)
			delete oscManager->getAppMapping(i);

		for(i=0;i<mappingsTree->getRootItem()->getNumSubItems();++i)
		{
			mappingsTree->getRootItem()->getSubItem(i)->setOpen(false);
			mappingsTree->getRootItem()->getSubItem(i)->setOpen(true);
		}
	}
}

