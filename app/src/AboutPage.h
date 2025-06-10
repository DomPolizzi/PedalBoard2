#ifndef __JUCER_HEADER_ABOUTPAGE_ABOUTPAGE_B72A047D__
#define __JUCER_HEADER_ABOUTPAGE_ABOUTPAGE_B72A047D__

#include <JuceHeader.h>

class AboutPage  : public Component
{
public:

    AboutPage (const String& ip);
    ~AboutPage();

    void paint (Graphics& g);
    void resized();

    juce_UseDebuggingNewOperator

private:
	String ipAddress;

    Label* titleLabel;
    Label* descriptionLabel;
    Label* creditsLabel;
    Label* authorLabel;
    HyperlinkButton* niallmoodyLink;
    HyperlinkButton* juceLink;
    Label* versionLabel;
    Label* juceVersionLabel;
    Label* ipAddressLabel;

    AboutPage (const AboutPage&);
    const AboutPage& operator= (const AboutPage&);
};


#endif
