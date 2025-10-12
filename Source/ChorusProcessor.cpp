#include "ChorusProcessor.h"
#include "ChorusComponent.h"

ChorusProcessor::ChorusProcessor()
    : juce::AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                             .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
    bypass (cgo::ParamUtils::createBoolParameter ("bypass", "Bypass", false)),
    vts (*this, nullptr, "PARAMS", { bypass.release(),
                                     parameters.type.release(),
                                     parameters.rate.release(),
                                     parameters.amount.release(),
                                     parameters.feedback.release(),
                                     parameters.flipFeedback.release(),
                                     parameters.width.release(),
                                     parameters.warmth.release(),
                                     parameters.output.release(),
                                     parameters.mix.release() }),
    chorus (parameters)
{
}

bool ChorusProcessor::isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void ChorusProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    bypassMixer.prepare ({ sampleRate, static_cast<juce::uint32> (samplesPerBlock), static_cast<juce::uint32> (getMainBusNumOutputChannels()) });
    chorus.prepareToPlay (sampleRate, samplesPerBlock, getMainBusNumOutputChannels());
}

void ChorusProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{    
    juce::ScopedNoDenormals noDenormals;

    const bool isBypassed = bypass->getValue() > 0.5f;
    bypassMixer.setWetMixProportion (isBypassed ? 0.0f : 1.0f);
    
    bypassMixer.pushDrySamples ({ buffer });

    if (getMainBusNumInputChannels() == 1 && getMainBusNumOutputChannels() == 2)
        buffer.copyFrom (1, 0, buffer.getReadPointer (0), buffer.getNumSamples());
    
    chorus.processBlock (buffer, midi, getPlayHead());

    bypassMixer.mixWetSamples ({ buffer });
}

void ChorusProcessor::releaseResources() 
{
}

void ChorusProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    const auto xml = vts.copyState().createXml();
    copyXmlToBinary (*xml, destData);
}

void ChorusProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xml = getXmlFromBinary (data, sizeInBytes);
    vts.replaceState (juce::ValueTree::fromXml (*xml));
}

const juce::String ChorusProcessor::getName() const
{
    return JucePlugin_Name;
}

double ChorusProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

bool ChorusProcessor::acceptsMidi() const
{
    return false;
}

bool ChorusProcessor::producesMidi() const
{
    return false;
}

int ChorusProcessor::getNumPrograms()
{
    return 1;
}

int ChorusProcessor::getCurrentProgram()
{
    return 0;
}

void ChorusProcessor::setCurrentProgram (int) 
{
}

const juce::String ChorusProcessor::getProgramName (int)
{
    return "default";
}

void ChorusProcessor::changeProgramName (int, const juce::String&)
{
}

bool ChorusProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* ChorusProcessor::createEditor()
{
    return new cgo::ResizableEditor (*this, std::make_unique<ChorusComponent> (*this));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChorusProcessor();
}
