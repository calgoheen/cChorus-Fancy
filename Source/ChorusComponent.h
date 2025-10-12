#pragma once

#include <JuceHeader.h>
#include "ChorusProcessor.h"

class ChorusComponent : public juce::Component
{
public:
    ChorusComponent (ChorusProcessor& processor);
    ~ChorusComponent() override = default;

    void resized() override;
    void paint (juce::Graphics& g) override;
    void paintOverChildren (juce::Graphics& g) override;

private:
    static constexpr int maxWidth = 1812;
    static constexpr int maxHeight = 1352;

    cgo::Chorus::Params& parameters;

    const juce::Image bgImage;

    cgo::ImageToggleButton bypassButton;
    std::unique_ptr<juce::ButtonParameterAttachment> bypassAttachment;

    cgo::ImageToggleButton typeButton;
    std::unique_ptr<juce::ButtonParameterAttachment> typeAttachment;

    cgo::ImageToggleButton flipButton;
    std::unique_ptr<juce::ButtonParameterAttachment> flipAttachment;

    cgo::ImageRotarySlider amountSlider;
    std::unique_ptr<juce::SliderParameterAttachment> amountAttachment;
    std::unique_ptr<juce::Component> amountLabel;

    cgo::ImageRotarySlider rateSlider;
    std::unique_ptr<juce::SliderParameterAttachment> rateAttachment;
    std::unique_ptr<juce::Component> rateLabel;

    cgo::ImageRotarySlider feedbackSlider;
    std::unique_ptr<juce::SliderParameterAttachment> feedbackAttachment;
    std::unique_ptr<juce::Component> feedbackLabel;

    cgo::ImageRotarySlider warmthSlider;
    std::unique_ptr<juce::SliderParameterAttachment> warmthAttachment;
    std::unique_ptr<juce::Component> warmthLabel;

    cgo::ImageRotarySlider mixSlider;
    std::unique_ptr<juce::SliderParameterAttachment> mixAttachment;
    std::unique_ptr<juce::Component> mixLabel;

    cgo::ImageThumbSlider widthSlider;
    std::unique_ptr<juce::SliderParameterAttachment> widthAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusComponent)
};
