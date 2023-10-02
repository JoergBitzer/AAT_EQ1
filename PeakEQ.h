#pragma once

#include <vector>
#include <juce_audio_processors/juce_audio_processors.h>

#include "tools/SynchronBlockProcessor.h"
#include "tools/AudioProcessParameter.h"
#include "PluginSettings.h"
#include "Biquad.h"


// This is how we define our parameter as globals to use it in the audio processor as well as in the editor
const struct
{
	const std::string ID = "gainID";
	const std::string name = "gain";
	const std::string unitName = "";
	const float minValue = -24.f;
	const float maxValue = 24.f;
	const float defaultValue = 0.f;
}g_paramGain;
const struct
{
	const std::string ID = "FrequencyID";
	const std::string name = "Frequency";
	const std::string unitName = "";
	const float minValue = log(20.f);
	const float maxValue = log(18000.f);
	const float defaultValue = log(1000.f);
}g_paramFrequency;
const struct
{
	const std::string ID = "QID";
	const std::string name = "Q";
	const std::string unitName = "";
	const float minValue = 0.1f;
	const float maxValue = 10.f;
	const float defaultValue = 1.0f;
}g_paramQ;


class PeakEQAudio : public SynchronBlockProcessor
{
public:
    PeakEQAudio();
    void prepareToPlay(double sampleRate, int max_samplesPerBlock, int max_channels);
    virtual int processSynchronBlock(juce::AudioBuffer<float>&, juce::MidiBuffer& midiMessages);

    // parameter handling
  	void addParameter(std::vector < std::unique_ptr<juce::RangedAudioParameter>>& paramVector);
    void prepareParameter(std::unique_ptr<juce::AudioProcessorValueTreeState>&  vts);
    
    // some necessary info for the host
    int getLatency(){return m_Latency;};

private:
    int m_Latency = 0;
	float m_fs = 44100.f, m_f0 = 2000.f, m_gain = 20.f, m_Q = 2.f;
	void designCoeffs();
	Biquad m_left;
	Biquad m_right;
	AudioProcessParameter <float> m_gainparameter;
	AudioProcessParameter <float> m_frequencyparameter;
	AudioProcessParameter <float> m_Qparameter;

    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Linear> m_smoothedGain;
    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Multiplicative> m_smoothedFrequency;
    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Multiplicative> m_smoothedQ;

};

class PeakEQGUI : public Component
{
public:
	PeakEQGUI(juce::AudioProcessorValueTreeState& apvts);

	void paint(juce::Graphics& g) override;
	void resized() override;
private:
    AudioProcessorValueTreeState& m_apvts; 
    juce::Slider m_GainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_GainAttachment;
    juce::Slider m_FrequencySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_FrequencyAttachment;
    juce::Slider m_QSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_QAttachment;


};
