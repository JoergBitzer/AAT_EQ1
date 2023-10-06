#define _USE_MATH_DEFINES
#include <math.h>
#include "PeakEQ.h"


PeakEQAudio::PeakEQAudio()
:SynchronBlockProcessor()
{
    designCoeffs();
}

void PeakEQAudio::prepareToPlay(double sampleRate, int max_samplesPerBlock, int max_channels)
{
    juce::ignoreUnused(max_samplesPerBlock,max_channels);
    int synchronblocksize;
    synchronblocksize = (g_desired_blocksize_ms*sampleRate*0.001);
    if (g_forcePowerOf2)
    {
        int nextpowerof2 = int(log2(synchronblocksize))+1;
        synchronblocksize = int(pow(2,nextpowerof2));
    }
    prepareSynchronProcessing(max_channels,synchronblocksize);
    m_Latency += synchronblocksize;
    // here your code
    m_left.reset();
    m_right.reset();
    m_smoothedGain.reset(sampleRate/synchronblocksize, 0.05);
    m_smoothedFrequency.reset(sampleRate/synchronblocksize, 0.05);
    m_smoothedQ.reset(sampleRate/synchronblocksize, 0.05);

}

int PeakEQAudio::processSynchronBlock(juce::AudioBuffer<float> & buffer, juce::MidiBuffer &midiMessages)
{
    // update parameter

    m_smoothedGain.setTargetValue(m_gainparameter.update());
    m_gain = m_smoothedGain.getNextValue();
    m_smoothedFrequency.setTargetValue(m_frequencyparameter.update());
    m_f0 = m_smoothedFrequency.getNextValue();
    m_smoothedQ.setTargetValue( m_Qparameter.update());
    m_Q = m_smoothedQ.getNextValue();
    designCoeffs();

    juce::ignoreUnused(midiMessages);

    int nrofchns = buffer.getNumChannels();
    int nrofsamples = buffer.getNumSamples();
    auto datapointers = buffer.getArrayOfWritePointers();

    if (nrofchns > 0)
        m_left.processSamples(datapointers[0],datapointers[0], nrofsamples);
    
    if (nrofchns > 1)
        m_right.processSamples(datapointers[1],datapointers[1], nrofsamples);

    return 0;
}

void PeakEQAudio::addParameter(std::vector<std::unique_ptr<juce::RangedAudioParameter>> &paramVector)
{
    // this is just a placeholder (necessary for compiling/testing the template)
    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramGain.ID,
         g_paramGain.name,
         NormalisableRange<float>(g_paramGain.minValue, g_paramGain.maxValue),
         g_paramGain.defaultValue,
         g_paramGain.unitName,
         AudioProcessorParameter::genericParameter,
// these are to additional lines wit lambdas to convert data
         [](float value, int MaxLen) { value = int(value * 10) * 0.1;  return (String(value, MaxLen) + " dB"); },
         [](const String& text) {return text.getFloatValue(); }));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramFrequency.ID,
         g_paramFrequency.name,
         NormalisableRange<float>(g_paramFrequency.minValue, g_paramFrequency.maxValue),
         g_paramFrequency.defaultValue,
         g_paramFrequency.unitName,
         AudioProcessorParameter::genericParameter,
// these are to additional lines wit lambdas to convert data
         [](float value, int MaxLen) { value = int(exp(value) * 10) * 0.1;  return (String(value, MaxLen) + " Hz"); },
         [](const String& text) {return text.getFloatValue(); }));

    paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramQ.ID,
         g_paramQ.name,
         NormalisableRange<float>(g_paramQ.minValue, g_paramQ.maxValue),
         g_paramQ.defaultValue,
         g_paramQ.unitName,
         AudioProcessorParameter::genericParameter,
// these are to additional lines wit lambdas to convert data
         [](float value, int MaxLen) { value = int(value * 10) * 0.1;  return (String(value, MaxLen) + ""); },
         [](const String& text) {return text.getFloatValue(); }));


}

void PeakEQAudio::prepareParameter(std::unique_ptr<juce::AudioProcessorValueTreeState> &vts)
{
    m_gainparameter.prepareParameter(vts->getRawParameterValue(g_paramGain.ID));
    m_frequencyparameter.prepareParameter(vts->getRawParameterValue(g_paramFrequency.ID));
    m_frequencyparameter.changeTransformer(AudioProcessParameter<float>::transformerFunc::exptransform);
    m_Qparameter.prepareParameter(vts->getRawParameterValue(g_paramQ.ID));
}

void PeakEQAudio::designCoeffs()
{
    double A  =  pow(10.0,m_gain/40.0);
    double w0 = 2.0*M_PI*m_f0/m_fs;
    double alpha = sin(w0)/(2.0*m_Q);

    double b0 =   1.0 + alpha*A;
    double b1 =  -2.0*cos(w0);
    double b2 =   1.0 - alpha*A;
    double a0 =   1.0 + alpha/A;
    double a1 =  -2.0*cos(w0);
    double a2 =   1.0 - alpha/A;

    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
    a0 /= a0;

    m_left.setCoeffs(b0,b1,b2,a1,a2);
    m_right.setCoeffs(b0,b1,b2,a1,a2);

}

PeakEQGUI::PeakEQGUI(juce::AudioProcessorValueTreeState& apvts)
:m_apvts(apvts)
{
    m_GainSlider.setRange (g_paramGain.minValue, g_paramGain.maxValue);         
    m_GainSlider.setTextValueSuffix (g_paramGain.unitName);    
    m_GainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    m_GainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, true, 60, 20);
	m_GainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramGain.ID, m_GainSlider);
    auto val = m_apvts.getRawParameterValue(g_paramGain.ID);
    m_GainSlider.setValue(*val);
	addAndMakeVisible(m_GainSlider);

    m_FrequencySlider.setRange (g_paramFrequency.minValue, g_paramFrequency.maxValue);         
    m_FrequencySlider.setTextValueSuffix (g_paramFrequency.unitName);    
    m_FrequencySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    m_FrequencySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, true, 60, 20);
	m_FrequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramFrequency.ID, m_FrequencySlider);
    m_FrequencySlider.setValue(*m_apvts.getRawParameterValue(g_paramFrequency.ID));
	addAndMakeVisible(m_FrequencySlider);

    m_QSlider.setRange (g_paramQ.minValue, g_paramQ.maxValue);         
    m_QSlider.setTextValueSuffix (g_paramQ.unitName);    
    m_QSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    m_QSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, true, 60, 20);
	m_QAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(m_apvts, g_paramQ.ID, m_QSlider);
    m_QSlider.setValue(*m_apvts.getRawParameterValue(g_paramQ.ID));
	addAndMakeVisible(m_QSlider);

}

void PeakEQGUI::paint(juce::Graphics &g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId).brighter(0.3));

}

void PeakEQGUI::resized()
{
	auto r = getLocalBounds();
    
    // if you have to place several components, use scaleFactor
    //int width = r.getWidth();
	//float scaleFactor = float(width)/g_minGuiSize_x;

    // use the given canvas in r
    juce::ignoreUnused(r);
	m_GainSlider.setBounds(0,0,r.getWidth(), r.getHeight()/3);
	m_FrequencySlider.setBounds(0,r.getHeight()/3+1,r.getWidth(), r.getHeight()/3);
	m_QSlider.setBounds(0,2*r.getHeight()/3+1,r.getWidth(), r.getHeight()/3);

}
