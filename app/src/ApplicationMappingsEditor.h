//	ApplicationMappingsEditor.h - Component used to edit the application
//								  mappings.

#ifndef APPLICATIONMAPPINGSEDITOR_H_
#define APPLICATIONMAPPINGSEDITOR_H_

#include <JuceHeader.h>
#include "MidiMappingManager.h"
#include "MidiCcAlertWindow.h"
#include "OscMappingManager.h"
#include "JuceHelperStuff.h"
#include "ColourScheme.h"
#include "Vectors.h"

class OscMappingManager;
class MidiMappingManager;

///	Component used to edit the application mappings.
class ApplicationMappingsEditor : public Component,
								  public Button::Listener
{
  public:
	///	Constructor.
	ApplicationMappingsEditor(ApplicationCommandManager *app,
							  MidiMappingManager *midi,
							  OscMappingManager *osc);
	///	Destructor.
	~ApplicationMappingsEditor();

	///	Fills the background the correct colour.
	void paint(Graphics& g);

	///	So we can resize the child components accordingly.
	void resized();

	///	So we can reset the mappings to their defaults.
	void buttonClicked(Button *button);

	juce_UseDebuggingNewOperator
  private:
	///	The app's ApplicationCommandManager.
	ApplicationCommandManager *appManager;
	///	The app's MidiMappingManager.
	MidiMappingManager *midiManager;
	///	The app's OscMappingManager.
	OscMappingManager *oscManager;

	///	The main tree view.
	TreeView *mappingsTree;
	///	Button to reset all the mappings.
	TextButton *resetButton;

	///	The actual mapping item in the mappingsTree.
	class MappingItem : public TreeViewItem
	{
	  private:
		class MappingItemButtons;
		CommandID id;
	  public:
		///	Constructor.
		MappingItem(const String& name, CommandID commandId)
			: name(name), id(commandId)
		{
			
		};
		///	Destructor.
		~MappingItem() {};

		///	Returns false, obviously.
		bool mightContainSubItems() {return false;};

		///	Draws the item.
		void paintItem(Graphics &g, int width, int height)
		{
			g.setColour(ColourScheme::getInstance().colours[L"Text Colour"]);
			g.setFont(14.0f);
			g.drawText(name, 0, 0, width, height, Justification::centredLeft, false);
		};

		///	Adds the buttons used to edit the command's mappings.
		std::unique_ptr<Component> createItemComponent() override
		{
			ApplicationMappingsEditor* editor = dynamic_cast<ApplicationMappingsEditor*>(getOwnerView()->getParentComponent());
			jassert(editor != nullptr);
			return std::make_unique<MappingItemButtons>(id, editor->appManager, editor->midiManager, editor->oscManager);
		}

	  private:
		///	The command's name.
		String name;

		///	Component holding the buttons to edit the command's mappings.
		class MappingItemButtons : public Component,
								   public Button::Listener,
								   public KeyListener
		{
		  public:
			///	Constructor.
			MappingItemButtons(CommandID commandId,
							   ApplicationCommandManager *app,
							   MidiMappingManager *midi,
							   OscMappingManager *osc)
				: id(commandId),
				  appManager(app),
				  midiManager(midi),
				  oscManager(osc),
				  addMapping(nullptr)
			{
				int i;

				//Setup the addMapping button.
				auto addImage = std::unique_ptr<Drawable>(JuceHelperStuff::loadSVGFromMemory(Vectors::addmappingbutton_svg,
																					Vectors::addmappingbutton_svgSize));
				auto addImageOver = std::unique_ptr<Drawable>(JuceHelperStuff::loadSVGFromMemory(Vectors::addmappingbuttonover_svg,
																					Vectors::addmappingbuttonover_svgSize));
				addMapping = new DrawableButton(L"Add Mapping Buttton",
												DrawableButton::ImageFitted);
				addMapping->setImages(addImage.get(), addImageOver.get());
				addMapping->addListener(this);
				addAndMakeVisible(addMapping);

				//Setup all the mapping buttons.
				const Array<KeyPress> keyMappings = appManager->getKeyMappings()->getKeyPressesAssignedToCommand(id);

				//KeyPresses.
				for(i=0;i<keyMappings.size();++i)
				{
					TextButton *tempB = new TextButton(keyMappings[i].getTextDescription());
					tempB->getProperties().set("type", L"KeyPress");
					tempB->addListener(this);
					mappingButtons.add(tempB);
					addAndMakeVisible(tempB);
				}

				//MIDI CCs.
				for(i=0;i<midiManager->getNumAppMappings();++i)
				{
					if(midiManager->getAppMapping(i)->getId() == id)
					{
						String tempstr;

						tempstr << "MIDI CC: " << midiManager->getAppMapping(i)->getCc();
						TextButton *tempB = new TextButton(tempstr);
						tempB->getProperties().set("type", L"MIDI CC");
						tempB->addListener(this);
						mappingButtons.add(tempB);
						addAndMakeVisible(tempB);
					}
				}

				//OSC.
				for(i=0;i<oscManager->getNumAppMappings();++i)
				{
					if(oscManager->getAppMapping(i)->getId() == id)
					{
						TextButton *tempB = new TextButton(oscManager->getAppMapping(i)->getAddress());
						tempB->getProperties().set("type", L"OSC");
						tempB->addListener(this);
						mappingButtons.add(tempB);
						addAndMakeVisible(tempB);
					}
				}
			};
			///	Destructor.
			~MappingItemButtons()
			{
				deleteAllChildren();
			};

			void buttonClicked(Button *button) override
			{
				if(button == addMapping)
				{
					PopupMenu popeye;

					popeye.addItem(1, "Add Key Mapping");
					popeye.addItem(2, "Add MIDI CC Mapping");
					popeye.addItem(3, "Add Open Sound Control Mapping");

					popeye.showMenuAsync(PopupMenu::Options().withTargetComponent(button), [this](int result){
						switch(result)
						{
							case 1:
							{
								auto* win = new AlertWindow("Keypress mapping",
												"Enter the key combination to map this command to:\n\n",
												AlertWindow::NoIcon);
								win->addButton("OK", 1);
								win->addButton("Cancel", 0);
								win->setWantsKeyboardFocus(true);
								win->grabKeyboardFocus();
								win->addKeyListener(this);
								win->enterModalState(true, juce::ModalCallbackFunction::create([this, win](int result) {
									if (result == 1) {
										KeyPressMappingSet *keyMappings = appManager->getKeyMappings();
										keyMappings->addKeyPress(id, tempKeyPress);
										TextButton *tempB = new TextButton(tempKeyPress.getTextDescription());
										tempB->getProperties().set("type", "KeyPress");
										tempB->addListener(this);
										mappingButtons.add(tempB);
										addAndMakeVisible(tempB);
										resized();
									}
									delete win;
								}));
							}
							break;
							case 2:
							{
								auto* win = new MidiCcAlertWindow(midiManager);
								win->enterModalState(true, juce::ModalCallbackFunction::create([this, win](int result) {
									if (result == 1) {
										int index = win->getComboBoxComponent("midiCc")->getSelectedId();
										if(index > 1) {
											String tempstr;
											MidiAppMapping *mapping = new MidiAppMapping(midiManager,
																					index-2,
																					id);
											midiManager->registerAppMapping(mapping);
											tempstr << "MIDI CC: " << index-2;
											TextButton *tempB = new TextButton(tempstr);
											tempB->getProperties().set("type", "MIDI CC");
											tempB->addListener(this);
											mappingButtons.add(tempB);
											addAndMakeVisible(tempB);
											resized();
										}
									}
									delete win;
								}));
							}
							break;
							case 3:
							{
								auto* win = new AlertWindow("Open Sound Control mapping", 
												"Enter OSC address to map this command to:",
												AlertWindow::NoIcon);
								win->addComboBox("oscAddress", oscManager->getReceivedAddresses());
								win->getComboBoxComponent("oscAddress")->setEditableText(true);
								win->addTextEditor("oscParam", "0", "Parameter:");
								win->getTextEditor("oscParam")->setInputRestrictions(3, "0123456789");
								win->addButton("OK", 1, KeyPress(KeyPress::returnKey));
								win->addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));
								win->enterModalState(true, juce::ModalCallbackFunction::create([this, win](int result) {
									if (result == 1) {
										int param;
										String tempstr, tempstr2;
										tempstr = win->getComboBoxComponent("oscAddress")->getText();
										tempstr2 = win->getTextEditorContents("oscParam");
										param = tempstr2.getIntValue();
										if(!tempstr.isEmpty()) {
											OscAppMapping *mapping = new OscAppMapping(oscManager,
																					tempstr,
																					param,
																					id);
											oscManager->registerAppMapping(mapping);
											TextButton *tempB = new TextButton(tempstr);
											tempB->getProperties().set("type", "OSC");
											tempB->addListener(this);
											mappingButtons.add(tempB);
											addAndMakeVisible(tempB);
											resized();
										}
									}
									delete win;
								}));
							}
							break;
						}
					});
				}
				else
				{
					PopupMenu popeye;

					popeye.addItem(1, "Remove Mapping");

					popeye.showMenuAsync(PopupMenu::Options().withTargetComponent(button), [this, button](int result){
						if(result)
						{
							String typeString = button->getProperties()["type"];

							if(typeString == "KeyPress")
							{
								int i;
								String buttonText = button->getName();
								KeyPressMappingSet *mappings = appManager->getKeyMappings();
								const Array<KeyPress> keys = mappings->getKeyPressesAssignedToCommand(id);

								for(i=0;i<keys.size();++i)
								{
									if(keys[i].getTextDescription() == buttonText)
									{
										mappings->removeKeyPress(keys[i]);
										removeChildComponent(button);
										mappingButtons.removeFirstMatchingValue(dynamic_cast<TextButton *>(button));
										delete button;

										break;
									}
								}
							}
							else if(typeString == "MIDI CC")
							{
								int i;
								String tempstr = button->getName().substring(9);
								int cc = tempstr.getIntValue();

								for(i=0;i<midiManager->getNumAppMappings();++i)
								{
									MidiAppMapping *mapping = midiManager->getAppMapping(i);
									if(mapping->getCc() == cc)
									{
										delete mapping;
										removeChildComponent(button);
										mappingButtons.removeFirstMatchingValue(dynamic_cast<TextButton *>(button));
										delete button;

										break;
									}
								}
							}
							else if(typeString == "OSC")
							{
								int i;

								for(i=0;i<oscManager->getNumAppMappings();++i)
								{
									OscAppMapping *mapping = oscManager->getAppMapping(i);
									if(mapping->getAddress() == button->getName())
									{
										delete mapping;
										removeChildComponent(button);
										mappingButtons.removeFirstMatchingValue(dynamic_cast<TextButton *>(button));
										delete button;

										break;
									}
								}
							}
							resized();
						}
					});
				}
			};

			void resized() override
			{
				int i;
				const int buttonWidth = 90;
				const int addMappingX = getWidth()-getHeight();

				addMapping->setBounds(addMappingX,
									  0,
									  getHeight(),
									  getHeight());

				for(i=0;i<mappingButtons.size();++i)
				{
					mappingButtons[i]->setBounds(addMappingX-((i+1)*(buttonWidth+2)),
												 0,
												 buttonWidth,
												 getHeight());
				}
			};

			bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override
			{
				juce::AlertWindow* win = dynamic_cast<juce::AlertWindow*>(originatingComponent);
				if (win)
				{
					juce::String tempstr;
					tempKeyPress = key;
					tempstr << L"Enter the key combination to map this command to:\n\n" << key.getTextDescription();
					win->setMessage(tempstr);
					win->repaint();
				}
				return true;
			}

		  private:
			CommandID id;
			ApplicationCommandManager* appManager;
			MidiMappingManager* midiManager;
			OscMappingManager* oscManager;
			Array<TextButton*> mappingButtons;
			DrawableButton* addMapping;
			KeyPress tempKeyPress;
		};
	};

	///	The mapping category item in the mappingsTree.
	class CategoryItem : public TreeViewItem
	{
	  public:
		///	Constructor.
		CategoryItem(const String& category,
					 ApplicationCommandManager *app,
					 MidiMappingManager *midi,
					 OscMappingManager *osc):
		name(category),
		appManager(app),
		midiManager(midi),
		oscManager(osc)
		{
			setLinesDrawnForSubItems(true);
			setOpen(true);
		};
		///	Destructor.
		~CategoryItem() {};

		///	Returns true, obviously.
		bool mightContainSubItems() {return true;};

		///	Adds all the sub-items (mappings in this category).
		void itemOpennessChanged(bool isNowOpen)
		{
			clearSubItems();
			//We only need to add the mappings once.
			//if(getNumSubItems() == 0)
			{
				int i;
				const Array<CommandID> commands = appManager->getCommandsInCategory(name);

				for(i=0;i<commands.size();++i)
				{
					addSubItem(new MappingItem(appManager->getNameOfCommand(commands[i]), commands[i]));
				}
			}
		};

		///	Draws the item.
		void paintItem(Graphics &g, int width, int height)
		{
			g.setColour(ColourScheme::getInstance().colours[L"Text Colour"]);
			g.setFont(Font(16.0f, Font::bold));
			g.drawText(name, 0, 0, width, height, Justification::centredLeft, false);
		};
	  private:
		///	The name of this category.
		String name;

		///	The app's ApplicationCommandManager.
		ApplicationCommandManager *appManager;
		///	The app's MidiMappingManager.
		MidiMappingManager *midiManager;
		///	The app's OscMappingManager.
		OscMappingManager *oscManager;
	};
	///	The root item in the mappingsTree.
	class RootItem : public TreeViewItem
	{
	  public:
		///	Constructor.
		RootItem(ApplicationCommandManager *app,
				 MidiMappingManager *midi,
				 OscMappingManager *osc):
		appManager(app),
		midiManager(midi),
		oscManager(osc)
		{
			setLinesDrawnForSubItems(false);
		};
		///	Destructor.
		~RootItem() {};

		///	Returns true, obviously.
		bool mightContainSubItems() {return true;};

		///	Adds all the sub-items (mapping categories).
		void itemOpennessChanged(bool isNowOpen)
		{
			//We only need to add the categories once.
			if(getNumSubItems() == 0)
			{
				int i;
				StringArray categories = appManager->getCommandCategories();

				for(i=0;i<categories.size();++i)
				{
					addSubItem(new CategoryItem(categories[i],
												appManager,
												midiManager,
												oscManager));
				}
			}
		};
	  private:
		///	The app's ApplicationCommandManager.
		ApplicationCommandManager *appManager;
		///	The app's MidiMappingManager.
		MidiMappingManager *midiManager;
		///	The app's OscMappingManager.
		OscMappingManager *oscManager;
	};
};

#endif