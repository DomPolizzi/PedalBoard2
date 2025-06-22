#include "ColourScheme.h"
#include "AboutPage.h"

AboutPage::AboutPage (const String& ip)
    : ipAddress(ip),
      titleLabel (0),
      descriptionLabel (0),
      creditsLabel (0),
      authorLabel (0),
      niallmoodyLink (0),
      juceLink (0),
      versionLabel (0),
      juceVersionLabel (0),
      ipAddressLabel (0)
{
    addAndMakeVisible (titleLabel = new Label ("titleLabel",
                                               "Pedalboard 2"));
    titleLabel->setFont (Font (32.0000f, Font::bold));
    titleLabel->setJustificationType (Justification::centredLeft);
    titleLabel->setEditable (false, false, false);
    titleLabel->setColour (Label::textColourId, Colour (0x80000000));
    titleLabel->setColour (juce::Label::textColourId, Colours::black);
    titleLabel->setColour (juce::Label::backgroundColourId, Colour (0x0));

    addAndMakeVisible (descriptionLabel = new Label ("descriptionLabel",
                                                     "A simple plugin host intended for live use, with plugin\nparameters easily mapped to MIDI or Open Sound Control inputs."));
    descriptionLabel->setFont (Font (15.0000f, Font::plain));
    descriptionLabel->setJustificationType (Justification::topLeft);
    descriptionLabel->setEditable (false, false, false);
    descriptionLabel->setColour (Label::textColourId, Colour (0x80000000));
    descriptionLabel->setColour (juce::Label::textColourId, Colours::black);
    descriptionLabel->setColour (juce::Label::backgroundColourId, Colour (0x0));

    addAndMakeVisible (creditsLabel = new Label ("creditsLabel",
                                                 "Written using the JUCE library, with sections taken from\nthe \'audio plugin host\' example code."));
    creditsLabel->setFont (Font (15.0000f, Font::plain));
    creditsLabel->setJustificationType (Justification::topLeft);
    creditsLabel->setEditable (false, false, false);
    creditsLabel->setColour (Label::textColourId, Colour (0x80000000));
    creditsLabel->setColour (juce::Label::textColourId, Colours::black);
    creditsLabel->setColour (juce::Label::backgroundColourId, Colour (0x0));

    addAndMakeVisible (authorLabel = new Label ("authorLabel",
                                                "Author: Niall Moody\nLicense: GPL v3"));
    authorLabel->setFont (Font (15.0000f, Font::plain));
    authorLabel->setJustificationType (Justification::topLeft);
    authorLabel->setEditable (false, false, false);
    authorLabel->setColour (Label::textColourId, Colour (0x80000000));
    authorLabel->setColour (juce::Label::textColourId, Colours::black);
    authorLabel->setColour (juce::Label::backgroundColourId, Colour (0x0));

    addAndMakeVisible (niallmoodyLink = new HyperlinkButton ("niallmoody.com",
                                                             URL ("http://www.niallmoody.com")));
    niallmoodyLink->setTooltip ("http://www.niallmoody.com");
    niallmoodyLink->setButtonText ("niallmoody.com");

    addAndMakeVisible (juceLink = new HyperlinkButton ("rawmaterialsoftware.com/juce",
                                                       URL ("http://www.rawmaterialsoftware.com/juce")));
    juceLink->setTooltip ("http://www.rawmaterialsoftware.com/juce");
    juceLink->setButtonText ("rawmaterialsoftware.com/juce");

    addAndMakeVisible (versionLabel = new Label ("versionLabel",
                                                 "Version: 2.00"));
    versionLabel->setFont (Font (12.0000f, Font::plain));
    versionLabel->setJustificationType (Justification::centredRight);
    versionLabel->setEditable (false, false, false);
    versionLabel->setColour (Label::textColourId, Colour (0x80000000));
    versionLabel->setColour (juce::Label::textColourId, Colours::black);
    versionLabel->setColour (juce::Label::backgroundColourId, Colour (0x0));

    addAndMakeVisible (juceVersionLabel = new Label ("juceVersionLabel",
                                                     "JUCE Version: 1.5.36"));
    juceVersionLabel->setFont (Font (12.0000f, Font::plain));
    juceVersionLabel->setJustificationType (Justification::centredRight);
    juceVersionLabel->setEditable (false, false, false);
    juceVersionLabel->setColour (Label::textColourId, Colour (0x80000000));
    juceVersionLabel->setColour (juce::Label::textColourId, Colours::black);
    juceVersionLabel->setColour (juce::Label::backgroundColourId, Colour (0x0));

    addAndMakeVisible (ipAddressLabel = new Label ("ipAddressLabel",
                                                   "Current IP Address: 192.168.1.68"));
    ipAddressLabel->setFont (Font (15.0000f, Font::plain));
    ipAddressLabel->setJustificationType (Justification::centredLeft);
    ipAddressLabel->setEditable (false, false, false);
    ipAddressLabel->setColour (Label::textColourId, Colour (0x80000000));
    ipAddressLabel->setColour (juce::Label::textColourId, Colours::black);
    ipAddressLabel->setColour (juce::Label::backgroundColourId, Colour (0x0));

	String tempstr;
	Colour textCol = ColourScheme::getInstance().colours["Text Colour"].withAlpha(0.5f);

	tempstr << "Version: " << JUCEApplication::getInstance()->getApplicationVersion();
	versionLabel->setText(tempstr, dontSendNotification);

	tempstr = "";
	tempstr << SystemStats::getJUCEVersion();
	juceVersionLabel->setText(tempstr, dontSendNotification);

	tempstr = "";
	tempstr << "Current IP Address: " << ipAddress;
	ipAddressLabel->setText(tempstr, dontSendNotification);

	titleLabel->setColour(Label::textColourId, textCol);
	descriptionLabel->setColour(Label::textColourId, textCol);
	creditsLabel->setColour(Label::textColourId, textCol);
	authorLabel->setColour(Label::textColourId, textCol);
	versionLabel->setColour(Label::textColourId, textCol);
	juceVersionLabel->setColour(Label::textColourId, textCol);
	ipAddressLabel->setColour(Label::textColourId, textCol);

    setSize (400, 280);

}

AboutPage::~AboutPage()
{
    deleteAndZero (titleLabel);
    deleteAndZero (descriptionLabel);
    deleteAndZero (creditsLabel);
    deleteAndZero (authorLabel);
    deleteAndZero (niallmoodyLink);
    deleteAndZero (juceLink);
    deleteAndZero (versionLabel);
    deleteAndZero (juceVersionLabel);
    deleteAndZero (ipAddressLabel);

}

void AboutPage::paint (Graphics& g)
{
    g.fillAll (Colour (0xffeeece1));

	g.fillAll(ColourScheme::getInstance().colours["Window Background"]);

}

void AboutPage::resized()
{
    titleLabel->setBounds (8, 8, 208, 32);
    descriptionLabel->setBounds (16, 48, getWidth() - 16, 56);
    creditsLabel->setBounds (16, 104, getWidth() - 16, 56);
    authorLabel->setBounds (16, 152, getWidth() - 16, 40);
    niallmoodyLink->setBounds (proportionOfWidth (0.5000f) - ((150) / 2), 224, 150, 24);
    juceLink->setBounds (proportionOfWidth (0.5000f) - ((252) / 2), 248, 252, 24);
    versionLabel->setBounds (getWidth() - 154, 0, 150, 24);
    juceVersionLabel->setBounds (getWidth() - 154, 16, 150, 24);
    ipAddressLabel->setBounds (16, 192, getWidth() - 16, 24);
}

#if 0

#endif

