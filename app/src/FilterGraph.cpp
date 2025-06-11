/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#include "FilterGraph.h"
#include "InternalFilters.h"
#include "BypassableInstance.h"
#include "AudioDeviceManagerSingleton.h"
#include "OscMappingManager.h"
#include "MidiMappingManager.h"
#include "PropertiesSingleton.h"
#include "AudioSingletons.h"

#include <iostream>

using namespace std;


//==============================================================================
FilterConnection::FilterConnection (FilterGraph& owner_)
    : owner (owner_)
{
}

FilterConnection::FilterConnection (const FilterConnection& other)
    : sourceFilterID (other.sourceFilterID),
      sourceChannel (other.sourceChannel),
      destFilterID (other.destFilterID),
      destChannel (other.destChannel),
      owner (other.owner)
{
}

FilterConnection::~FilterConnection()
{
}


//==============================================================================
const int FilterGraph::midiChannelNumber = 0x1000;

FilterGraph::FilterGraph()
    : FileBasedDocument (filenameSuffix,
                         filenameWildcard,
                         "Load a filter graph",
                         "Save a filter graph"),
      lastUID (0)
{
    InternalPluginFormat internalFormat;
	bool audioInput = PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("AudioInput", true);
	bool midiInput = PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("MidiInput", true);

	if(audioInput)
	{
		addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::audioInputFilter),
				   10.0f, 10.0f);
	}

	if(midiInput)
	{
		addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::midiInputFilter),
				   10.0f, 120.0f);
	}

	addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::audioOutputFilter),
				892.0f, 10.0f);

    setChangedFlag (false);
}

FilterGraph::~FilterGraph()
{
    graph.clear();
}

uint32 FilterGraph::getNextUID() throw()
{
    return ++lastUID;
}

//==============================================================================
int FilterGraph::getNumFilters() const throw()
{
    return graph.getNumNodes();
}

const AudioProcessorGraph::Node::Ptr FilterGraph::getNode (const int index) const throw()
{
    return graph.getNode (index);
}

const AudioProcessorGraph::Node::Ptr FilterGraph::getNodeForId (const juce::AudioProcessorGraph::NodeID uid) const throw()
{
    return graph.getNodeForId (uid);
}

void FilterGraph::addFilter (const PluginDescription* desc, double x, double y)
{
    if (desc != nullptr)
    {
        String errorMessage;
        double sampleRate = AudioDeviceManagerSingleton::getInstance().getCurrentAudioDevice()->getCurrentSampleRate();
        int bufferSize = AudioDeviceManagerSingleton::getInstance().getCurrentAudioDevice()->getCurrentBufferSizeSamples();
        
        std::unique_ptr<AudioPluginInstance> tempInstance = 
            AudioPluginFormatManagerSingleton::getInstance().createPluginInstance(
                *desc, sampleRate, bufferSize, errorMessage);
                
        if (tempInstance != nullptr) {
            std::unique_ptr<AudioProcessor> instance;
            
            if (dynamic_cast<AudioProcessorGraph::AudioGraphIOProcessor*>(tempInstance.get()) ||
                dynamic_cast<MidiInterceptor*>(tempInstance.get()) ||
                dynamic_cast<OscInput*>(tempInstance.get())) {
                instance = std::move(tempInstance);
            } else {
                instance = std::make_unique<BypassableInstance>(tempInstance.release());
            }
            
            AudioProcessorGraph::Node::Ptr node = graph.addNode(std::move(instance));
            
            if (node != nullptr) {
                node->properties.set("x", x);
                node->properties.set("y", y);
                changed();
            }
        } else {
            juce::NativeMessageBox::showMessageBoxAsync(
                juce::MessageBoxIconType::WarningIcon,
                TRANS("Couldn't create filter"),
                errorMessage);
        }
    }
}

void FilterGraph::addFilter(AudioPluginInstance *plugin, double x, double y)
{
	if(plugin != nullptr)
    {
        String errorMessage;

		std::unique_ptr<BypassableInstance> instance = std::make_unique<BypassableInstance>(plugin);

        AudioProcessorGraph::Node::Ptr node = graph.addNode(std::move(instance));

        if(node != nullptr)
        {
            node->properties.set("x", x);
            node->properties.set("y", y);
            changed();
        }
        else
        {
            juce::NativeMessageBox::showMessageBoxAsync(
                juce::MessageBoxIconType::WarningIcon,
                TRANS("Couldn't create filter"),
                errorMessage);
        }
    }
}

void FilterGraph::removeFilter (const juce::AudioProcessorGraph::NodeID id)
{
    //PluginWindow::closeCurrentlyOpenWindowsFor (id);

    if (graph.removeNode (id))
        changed();
}

void FilterGraph::disconnectFilter (const juce::AudioProcessorGraph::NodeID id)
{
    if (graph.disconnectNode (id))
        changed();
}

void FilterGraph::removeIllegalConnections()
{
    if (graph.removeIllegalConnections())
        changed();
}

void FilterGraph::setNodePosition (const int nodeId, double x, double y)
{
    const AudioProcessorGraph::Node::Ptr n (graph.getNodeForId (juce::AudioProcessorGraph::NodeID(nodeId)));

    if (n != nullptr)
    {
        n->properties.set ("x", jlimit (0.0, 1.0, x));
        n->properties.set ("y", jlimit (0.0, 1.0, y));
    }
}

void FilterGraph::getNodePosition (const int nodeId, double& x, double& y) const
{
    x = y = 0;

    const AudioProcessorGraph::Node::Ptr n (graph.getNodeForId (juce::AudioProcessorGraph::NodeID(nodeId)));

    if (n != nullptr)
    {
        //x = n->properties.getDoubleValue ("x");
        x = n->properties.getWithDefault ("x", 0.0);
        //y = n->properties.getDoubleValue ("y");
        y = n->properties.getWithDefault ("y", 0.0);
    }
}

//==============================================================================
int FilterGraph::getNumConnections() const throw()
{
    return graph.getConnections().size();
}

const AudioProcessorGraph::Connection* FilterGraph::getConnection (const int index) const throw()
{
    auto connections = graph.getConnections();
    return index < connections.size() ? &connections[index] : nullptr;
}

const AudioProcessorGraph::Connection* FilterGraph::getConnectionBetween (juce::AudioProcessorGraph::NodeID sourceFilterUID, int sourceFilterChannel,
                                                                          juce::AudioProcessorGraph::NodeID destFilterUID, int destFilterChannel) const throw()
{
    AudioProcessorGraph::Connection connection = {{sourceFilterUID, static_cast<uint32>(sourceFilterChannel)},
                                                  {destFilterUID, static_cast<uint32>(destFilterChannel)}};
    
    auto connections = graph.getConnections();
    for (const auto& conn : connections)
    {
        if (conn.source == connection.source && conn.destination == connection.destination)
            return &conn;
    }
    
    return nullptr;
}

bool FilterGraph::canConnect (juce::AudioProcessorGraph::NodeID sourceFilterUID, int sourceFilterChannel,
                              juce::AudioProcessorGraph::NodeID destFilterUID, int destFilterChannel) const throw()
{
    AudioProcessorGraph::Connection connection = {{sourceFilterUID, static_cast<int>(sourceFilterChannel)},
                                                 {destFilterUID, static_cast<int>(destFilterChannel)}};
    return graph.canConnect(connection);
}

bool FilterGraph::addConnection (juce::AudioProcessorGraph::NodeID sourceFilterUID, int sourceFilterChannel,
                                 juce::AudioProcessorGraph::NodeID destFilterUID, int destFilterChannel)
{
    const bool result = graph.addConnection ({{sourceFilterUID, static_cast<int>(sourceFilterChannel)},
                                              {destFilterUID, static_cast<int>(destFilterChannel)}});

    if (result)
        changed();

    return result;
}

void FilterGraph::removeConnection (const int index)
{
    auto connections = graph.getConnections();
    if (index >= 0 && index < connections.size())
    {
        if (graph.removeConnection(connections[index]))
            changed();
    }
}

void FilterGraph::removeConnection (juce::AudioProcessorGraph::NodeID sourceFilterUID, int sourceFilterChannel,
                                    juce::AudioProcessorGraph::NodeID destFilterUID, int destFilterChannel)
{
    if (graph.removeConnection ({{sourceFilterUID, static_cast<int>(sourceFilterChannel)},
                                 {destFilterUID, static_cast<int>(destFilterChannel)}}))
        changed();
}

void FilterGraph::clear(bool addAudioIn, bool addMidiIn, bool addAudioOut)
{
    graph.clear();
    setChangedFlag (true);

    if(addAudioIn || addMidiIn || addAudioOut)
    {
        InternalPluginFormat internalFormat;
        
        if(addAudioIn)
        {
            addFilter(internalFormat.getDescriptionFor(InternalPluginFormat::audioInputFilter),
                      10.0f, 10.0f);
        }

        if(addMidiIn)
        {
            addFilter(internalFormat.getDescriptionFor(InternalPluginFormat::midiInputFilter),
                      10.0f, 120.0f);
        }

        if(addAudioOut)
        {
            addFilter(internalFormat.getDescriptionFor(InternalPluginFormat::audioOutputFilter),
                      892.0f, 10.0f);
        }
    }
}

//==============================================================================
String FilterGraph::getDocumentTitle()
{
    if (! getFile().exists())
        return "Unnamed";

    return getFile().getFileNameWithoutExtension();
}

Result FilterGraph::loadDocument (const File& file)
{
    /*XmlDocument doc (file);
    XmlElement* xml = doc.getDocumentElement();

    if (xml == 0 || ! xml->hasTagName (L"FILTERGRAPH"))
    {
        delete xml;
		return Result::fail("Not a valid filter graph file");
    }

    restoreFromXml (*xml);
    delete xml;*/

	jassert(false);
	return Result::ok();
}

Result FilterGraph::saveDocument (const File& file)
{
    /*XmlElement* xml = createXml();

    String error;

    if (! xml->writeTo (file))
		return Result::fail("Couldn't write to the file");

    delete xml;*/
	jassert(false); //Pretty sure this is never used, but just to be sure...
	return Result::ok();
}

File FilterGraph::getLastDocumentOpened()
{
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (PropertiesSingleton::getInstance().getUserSettings()->getValue ("recentFilterGraphFiles"));

    return recentFiles.getFile (0);
}

void FilterGraph::setLastDocumentOpened (const File& file)
{
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (PropertiesSingleton::getInstance().getUserSettings()->getValue ("recentFilterGraphFiles"));

    recentFiles.addFile (file);

    PropertiesSingleton::getInstance().getUserSettings()->setValue ("recentFilterGraphFiles", recentFiles.toString());
}

//==============================================================================
void FilterGraph::createNodeFromXml(const XmlElement& xml,
									OscMappingManager& oscManager)
{
    PluginDescription pd;
    
    forEachXmlChildElement (xml, e)
    {
        if (e->hasTagName ("PLUGIN"))
        {
            pd.loadFromXml (*e);
            break;
        }
    }
    
    String errorMessage;

    if (pd.fileOrIdentifier.isNotEmpty())
    {
        double sampleRate = AudioDeviceManagerSingleton::getInstance().getCurrentAudioDevice()->getCurrentSampleRate();
        int bufferSize = AudioDeviceManagerSingleton::getInstance().getCurrentAudioDevice()->getCurrentBufferSizeSamples();
        
        std::unique_ptr<AudioPluginInstance> instance = 
            AudioPluginFormatManagerSingleton::getInstance().createPluginInstance(
                pd, sampleRate, bufferSize, errorMessage);
                
        if (instance != nullptr)
        {
            AudioProcessorGraph::Node::Ptr node = graph.addNode(
                std::move(instance), 
                juce::AudioProcessorGraph::NodeID(xml.getIntAttribute("uid")));

            if (node != nullptr)
            {
                node->properties.set("x", xml.getDoubleAttribute("x"));
                node->properties.set("y", xml.getDoubleAttribute("y"));

                for (auto* e : xml.getChildWithTagNameIterator("OSCMAPPING"))
                {
                    // Create OscMapping with proper constructor parameters
                    // Note: We're adapting to the new OscMappingManager API
                    // OscMapping* mapping = new OscMapping(&oscManager, this, node->nodeID.uid, 0, "", 0);
                    // Load mapping parameters from XML
                    // XmlElement* copy = new XmlElement(*e);
                    // delete mapping;
                    // mapping = new OscMapping(&oscManager, this, copy);
                    // delete copy;
                    
                    // TODO: Update this section when OscMapping class is updated
                }
            }
        }
        else if (!errorMessage.isEmpty())
        {
            juce::NativeMessageBox::showMessageBoxAsync(
                juce::MessageBoxIconType::WarningIcon,
                TRANS("Couldn't create filter"),
                errorMessage);
        }
    }
}

XmlElement* FilterGraph::createNodeXml (AudioProcessorGraph::Node* const node,
                                  const OscMappingManager& oscManager)
{
    AudioProcessor* const filter = node->getProcessor();

    if (filter == nullptr)
        return nullptr;

    XmlElement* e = new XmlElement ("FILTER");
    e->setAttribute (juce::Identifier("uid"), (int) node->nodeID.uid);
    e->setAttribute (juce::Identifier("x"), node->properties ["x"].toString());
    e->setAttribute (juce::Identifier("y"), node->properties ["y"].toString());

    // OscMapping methods need to be fixed - temporarily comment out this section
    // as we need to update the OscMapping class separately
    /*
    for (int i = 0; i < oscManager.getNumAppMappings(); ++i)
    {
        const OscMapping* mapping = oscManager.getAppMapping(i);

        // Update with the correct method names when OscMapping class is updated
        // if (mapping->getNodeId() == node->nodeID.uid)
        // {
        //     XmlElement* oscXml = mapping->createXml();
        //     e->addChildElement(oscXml);
        // }
    }
    */

    PluginDescription pd;

    AudioPluginInstance* plugin = dynamic_cast <AudioPluginInstance*> (filter);

    if (plugin != nullptr)
        plugin->fillInPluginDescription (pd);

    e->addChildElement (pd.createXml().release());

    return e;
}

XmlElement* FilterGraph::createXml(const OscMappingManager& oscManager) const
{
    XmlElement* xml = new XmlElement ("FILTERGRAPH");

    for (int i = 0; i < graph.getNumNodes(); ++i)
    {
        // Need to make createNodeXml const-compatible
        FilterGraph* mutableThis = const_cast<FilterGraph*>(this);
        xml->addChildElement (mutableThis->createNodeXml (graph.getNode(i), oscManager));
    }

    auto connections = graph.getConnections();
    for (const auto& connection : connections)
    {
        XmlElement* e = new XmlElement ("CONNECTION");

        e->setAttribute (juce::Identifier("srcFilter"), (int) connection.source.nodeID.uid);
        e->setAttribute (juce::Identifier("srcChannel"), (int) connection.source.channelIndex);
        e->setAttribute (juce::Identifier("dstFilter"), (int) connection.destination.nodeID.uid);
        e->setAttribute (juce::Identifier("dstChannel"), (int) connection.destination.channelIndex);

        xml->addChildElement (e);
    }

    return xml;
}

void FilterGraph::restoreFromXml(const XmlElement& xml,
                                OscMappingManager& oscManager)
{
    clear(true, true, true);

    for (auto* e : xml.getChildWithTagNameIterator("FILTER"))
    {
        createNodeFromXml(*e, oscManager);
    }

    for (auto* e : xml.getChildWithTagNameIterator("CONNECTION"))
    {
        addConnection (juce::AudioProcessorGraph::NodeID(e->getIntAttribute("srcFilter")),
                       e->getIntAttribute("srcChannel"),
                       juce::AudioProcessorGraph::NodeID(e->getIntAttribute("dstFilter")),
                       e->getIntAttribute("dstChannel"));
    }

    graph.removeIllegalConnections();
}
