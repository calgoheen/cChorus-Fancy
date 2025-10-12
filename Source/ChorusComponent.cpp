#include "ChorusComponent.h"

static juce::Font labelFont (juce::FontOptions (juce::Typeface::createSystemTypefaceFor (BinaryData::Digital7_ttf, BinaryData::Digital7_ttfSize)));

class KnobLabel : public juce::Component, public juce::Slider::Listener
{
public:
    KnobLabel (juce::Slider& s, std::function<juce::String (double)> f)
        : slider (s),
        stringFromValue (std::move (f))
    {
        jassert (stringFromValue != nullptr);

        slider.addListener (this);
        setInterceptsMouseClicks (false, false);
    }

    ~KnobLabel() override
    {
        slider.removeListener (this);
    }

    void paint (juce::Graphics& g) override
    {
        g.setFont (labelFont.withHeight (getHeight() * 0.9f));
        g.setColour (juce::Colours::red);
        g.drawText (stringFromValue (slider.getValue()), getLocalBounds().toFloat(), juce::Justification::centred);
    }

private:
    void sliderValueChanged (juce::Slider*) override
    {
        repaint();
    }

    juce::Slider& slider;
    const std::function<juce::String (double)> stringFromValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KnobLabel)
};

ChorusComponent::ChorusComponent (ChorusProcessor& proc)
    : parameters (proc.parameters),
    bgImage (juce::ImageCache::getFromMemory (BinaryData::bg_png, BinaryData::bg_pngSize)),
    bypassButton (juce::ImageCache::getFromMemory (BinaryData::switch_bypass_png, BinaryData::switch_bypass_pngSize)),
    typeButton (juce::ImageCache::getFromMemory (BinaryData::switch_type_png, BinaryData::switch_type_pngSize)),
    flipButton (juce::ImageCache::getFromMemory (BinaryData::switch_flip_png, BinaryData::switch_flip_pngSize)),
    amountSlider (juce::ImageCache::getFromMemory (BinaryData::knob_png, BinaryData::knob_pngSize)),
    rateSlider (juce::ImageCache::getFromMemory (BinaryData::knob_png, BinaryData::knob_pngSize)),
    feedbackSlider (juce::ImageCache::getFromMemory (BinaryData::knob_png, BinaryData::knob_pngSize)),
    warmthSlider (juce::ImageCache::getFromMemory (BinaryData::knob_png, BinaryData::knob_pngSize)),
    mixSlider (juce::ImageCache::getFromMemory (BinaryData::knob_png, BinaryData::knob_pngSize)),
    widthSlider (juce::ImageCache::getFromMemory (BinaryData::width_thumb_png, BinaryData::width_thumb_pngSize), true)
{
    static constexpr int labelWidth = 140;
    static constexpr int labelHeight = 90;

    addAndMakeVisible (bypassButton);
    bypassButton.setBounds ({ 268, 100, bypassButton.imageWidth, bypassButton.imageHeight });
    bypassAttachment = std::make_unique<juce::ButtonParameterAttachment> (*proc.bypass, bypassButton);

    addAndMakeVisible (typeButton);
    typeButton.setBounds ({ 399, 319, typeButton.imageWidth, typeButton.imageHeight });
    typeAttachment = std::make_unique<juce::ButtonParameterAttachment> (*proc.parameters.type, typeButton);
    typeButton.onClick = [this] { repaint(); };

    addAndMakeVisible (flipButton);
    flipButton.setBounds ({ 493, 879, flipButton.imageWidth, flipButton.imageHeight });
    flipAttachment = std::make_unique<juce::ButtonParameterAttachment> (*proc.parameters.flipFeedback, flipButton);
    
    addAndMakeVisible (amountSlider);
    amountSlider.setBounds ({ 716, 365, amountSlider.imageWidth, amountSlider.imageHeight });
    amountAttachment = std::make_unique<juce::SliderParameterAttachment> (*proc.parameters.amount, amountSlider);

    auto amountStrFromVal = [] (double val) 
    { 
        return juce::String (static_cast<int> (val * 100.0f)) + "%"; 
    };
    amountLabel = std::make_unique<KnobLabel> (amountSlider, std::move (amountStrFromVal));
    addAndMakeVisible (*amountLabel);
    amountLabel->setBounds (amountSlider.getBounds().withSizeKeepingCentre (labelWidth, labelHeight));

    addAndMakeVisible (rateSlider);
    const float rateSliderScale = 0.86f;
    rateSlider.setBounds ({ 1260, 381, static_cast<int> (rateSlider.imageWidth * rateSliderScale), static_cast<int> (rateSlider.imageHeight * rateSliderScale) });
    rateAttachment = std::make_unique<juce::SliderParameterAttachment> (*proc.parameters.rate, rateSlider);

    auto rateStrFromVal = [] (double val) 
    { 
        return val < 0.1 ? juce::String (val, 2) : juce::String (val, 1);
    };
    rateLabel = std::make_unique<KnobLabel> (rateSlider, std::move (rateStrFromVal));
    addAndMakeVisible (*rateLabel);
    rateLabel->setBounds (rateSlider.getBounds().withSizeKeepingCentre (labelWidth * rateSliderScale, labelHeight * rateSliderScale).translated (0, -15));
    
    addAndMakeVisible (feedbackSlider);
    const float feedbackSliderScale = 0.7f;
    feedbackSlider.setBounds ({ 170, 891, static_cast<int> (feedbackSlider.imageWidth * feedbackSliderScale), static_cast<int> (feedbackSlider.imageHeight * feedbackSliderScale) });
    feedbackAttachment = std::make_unique<juce::SliderParameterAttachment> (*proc.parameters.feedback, feedbackSlider);

    auto feedbackStrFromVal = [] (double val) 
    { 
        return juce::String (static_cast<int> (val * 100.0f)) + "%"; 
    };
    feedbackLabel = std::make_unique<KnobLabel> (feedbackSlider, std::move (feedbackStrFromVal));
    addAndMakeVisible (*feedbackLabel);
    feedbackLabel->setBounds (feedbackSlider.getBounds().withSizeKeepingCentre (labelWidth * feedbackSliderScale, labelHeight * feedbackSliderScale));
    
    addAndMakeVisible (warmthSlider);
    const float warmthSliderScale = 0.7f;
    warmthSlider.setBounds ({ 603, 891, static_cast<int> (warmthSlider.imageWidth * warmthSliderScale), static_cast<int> (warmthSlider.imageHeight * warmthSliderScale) });
    warmthAttachment = std::make_unique<juce::SliderParameterAttachment> (*proc.parameters.warmth, warmthSlider);

    auto warmthStrFromVal = [] (double val) 
    { 
        return juce::String (static_cast<int> (val * 100.0f)) + "%"; 
    };
    warmthLabel = std::make_unique<KnobLabel> (warmthSlider, std::move (warmthStrFromVal));
    addAndMakeVisible (*warmthLabel);
    warmthLabel->setBounds (warmthSlider.getBounds().withSizeKeepingCentre (labelWidth * warmthSliderScale, labelHeight * warmthSliderScale));
    
    addAndMakeVisible (mixSlider);
    const float mixSliderScale = 0.8f;
    mixSlider.setBounds ({ 1332, 882, static_cast<int> (mixSlider.imageWidth * mixSliderScale), static_cast<int> (mixSlider.imageHeight * mixSliderScale) });
    mixAttachment = std::make_unique<juce::SliderParameterAttachment> (*proc.parameters.mix, mixSlider);

    auto mixStrFromVal = [] (double val) 
    { 
        return juce::String (static_cast<int> (val * 100.0f)) + "%"; 
    };
    mixLabel = std::make_unique<KnobLabel> (mixSlider, std::move (mixStrFromVal));
    addAndMakeVisible (*mixLabel);
    mixLabel->setBounds (mixSlider.getBounds().withSizeKeepingCentre (labelWidth * mixSliderScale, labelHeight * mixSliderScale));

    addAndMakeVisible (widthSlider);
    widthSlider.setBounds ({ 1079, 926, widthSlider.thumbWidth, 246 });
    widthAttachment = std::make_unique<juce::SliderParameterAttachment> (*proc.parameters.width, widthSlider);

    setSize (bgImage.getWidth(), bgImage.getHeight());
}

void ChorusComponent::resized()
{
}

void ChorusComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
    g.drawImage (bgImage, getLocalBounds().toFloat());

    static const auto typeImg = juce::ImageCache::getFromMemory (BinaryData::type_png, BinaryData::type_pngSize);
    static const auto chorusImg = typeImg.getClippedImage ({ 0, 0, typeImg.getWidth(), typeImg.getHeight() / 2 });
    static const auto ensembleImg = typeImg.getClippedImage ({ 0, typeImg.getHeight() / 2, typeImg.getWidth(), typeImg.getHeight() / 2 });

    g.drawImage (typeButton.getToggleState() ? ensembleImg : chorusImg, juce::Rectangle<int> (142, 239, chorusImg.getWidth(), chorusImg.getHeight()).toFloat());
}

void ChorusComponent::paintOverChildren (juce::Graphics& g)
{
    g.setFont (labelFont.withHeight (rateLabel->getHeight() * 0.6f));
    g.setColour (juce::Colours::red);
    g.drawText ("HZ", rateLabel->getBounds().translated (0, 55).toFloat(), juce::Justification::centred);
}
