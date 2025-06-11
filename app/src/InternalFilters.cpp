#include "PedalboardProcessors.h"
#include "MidiMappingManager.h"
#include "OscMappingManager.h"
#include "InternalFilters.h"
#include "FilterGraph.h"

//==============================================================================
InternalPluginFormat::InternalPluginFormat()
{
    {
        AudioProcessorGraph::AudioGraphIOProcessor p (AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
        p.fillInPluginDescription (audioOutDesc);
    }

    {
        AudioProcessorGraph::AudioGraphIOProcessor p (AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);
        p.fillInPluginDescription (audioInDesc);
    }

    {
        AudioProcessorGraph::AudioGraphIOProcessor p (AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode);
        p.fillInPluginDescription (midiInDesc);
    }

    {
        MidiInterceptor p;
        p.fillInPluginDescription (midiInterceptorDesc);
    }

    {
        OscInput p;
        p.fillInPluginDescription (oscInputDesc);
    }

    {
        LevelProcessor p;
        p.fillInPluginDescription (levelProcDesc);
    }

    {
        FilePlayerProcessor p;
        p.fillInPluginDescription (filePlayerProcDesc);
    }

    {
        OutputToggleProcessor p;
        p.fillInPluginDescription (outputToggleProcDesc);
    }

    {
        VuMeterProcessor p;
        p.fillInPluginDescription (vuMeterProcDesc);
    }

    {
        RecorderProcessor p;
        p.fillInPluginDescription (recorderProcDesc);
    }

    {
        MetronomeProcessor p;
        p.fillInPluginDescription (metronomeProcDesc);
    }

    {
        LooperProcessor p;
        p.fillInPluginDescription (looperProcDesc);
    }
}

void InternalPluginFormat::createPluginInstance(const PluginDescription& desc, double initialSampleRate,
                                           int initialBufferSize, PluginCreationCallback callback)
{
    AudioPluginInstance* instance = nullptr;

    if (desc.name == audioOutDesc.name)
    {
        instance = new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
    }
    else if (desc.name == audioInDesc.name)
    {
        instance = new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);
    }
    else if (desc.name == midiInDesc.name)
    {
        instance = new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode);
    }
    else if (desc.name == midiInterceptorDesc.name)
    {
        instance = new MidiInterceptor();
    }
    else if (desc.name == oscInputDesc.name)
    {
        instance = new OscInput();
    }
    else if (desc.name == levelProcDesc.name)
    {
        instance = new LevelProcessor();
    }
    else if (desc.name == filePlayerProcDesc.name)
    {
        instance = new FilePlayerProcessor();
    }
    else if (desc.name == outputToggleProcDesc.name)
    {
        instance = new OutputToggleProcessor();
    }
    else if (desc.name == vuMeterProcDesc.name)
    {
        instance = new VuMeterProcessor();
    }
    else if (desc.name == recorderProcDesc.name)
    {
        instance = new RecorderProcessor();
    }
    else if (desc.name == metronomeProcDesc.name)
    {
        instance = new MetronomeProcessor();
    }
    else if (desc.name == looperProcDesc.name)
    {
        instance = new LooperProcessor();
    }

    if (instance != nullptr)
    {
        // In JUCE 7.0.12, AudioPluginInstance no longer has an initialise method
        // The plugin is already initialized during construction
        callback(std::unique_ptr<AudioPluginInstance>(instance), "");
    }
    else
    {
        callback(nullptr, "Failed to create plugin instance");
    }
}

AudioPluginInstance* InternalPluginFormat::createInstanceFromDescription(const PluginDescription& desc)
{
    if (desc.name == audioOutDesc.name)
    {
        return new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
    }
    else if (desc.name == audioInDesc.name)
    {
        return new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);
    }
    else if (desc.name == midiInDesc.name)
    {
        return new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode);
    }
    else if (desc.name == midiInterceptorDesc.name)
    {
        return new MidiInterceptor();
    }
    else if (desc.name == oscInputDesc.name)
    {
        return new OscInput();
    }
    else if (desc.name == levelProcDesc.name)
    {
        return new LevelProcessor();
    }
    else if (desc.name == filePlayerProcDesc.name)
    {
        return new FilePlayerProcessor();
    }
    else if (desc.name == outputToggleProcDesc.name)
    {
        return new OutputToggleProcessor();
    }
    else if (desc.name == vuMeterProcDesc.name)
    {
        return new VuMeterProcessor();
    }
    else if (desc.name == recorderProcDesc.name)
    {
        return new RecorderProcessor();
    }
    else if (desc.name == metronomeProcDesc.name)
    {
        return new MetronomeProcessor();
    }
    else if (desc.name == looperProcDesc.name)
    {
        return new LooperProcessor();
    }

    return nullptr;
}

const PluginDescription* InternalPluginFormat::getDescriptionFor (const InternalFilterType type)
{
    switch (type)
    {
    case audioInputFilter:
        return &audioInDesc;
    case audioOutputFilter:
        return &audioOutDesc;
    case midiInputFilter:
        return &midiInDesc;
	case midiInterceptorFilter:
		return &midiInterceptorDesc;
	case oscInputFilter:
		return &oscInputDesc;
	case levelProcFilter:
		return &levelProcDesc;
	case filePlayerProcFilter:
		return &filePlayerProcDesc;
	case outputToggleProcFilter:
		return &outputToggleProcDesc;
	case vuMeterProcFilter:
		return &vuMeterProcDesc;
	case recorderProcFilter:
		return &recorderProcDesc;
	case metronomeProcFilter:
		return &metronomeProcDesc;
	case looperProcFilter:
		return &looperProcDesc;
	default:
		return 0;
    }
}

void InternalPluginFormat::getAllTypes (OwnedArray <PluginDescription>& results)
{
    for (int i = 0; i < (int) endOfFilterTypes; ++i)
        results.add (new PluginDescription (*getDescriptionFor ((InternalFilterType) i)));
}
