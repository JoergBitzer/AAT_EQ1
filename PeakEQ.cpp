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
}

int PeakEQAudio::processSynchronBlock(juce::AudioBuffer<float> & buffer, juce::MidiBuffer &midiMessages)
{
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
    // paramVector.push_back(std::make_unique<AudioParameterFloat>(g_paramExample.ID,
    //     g_paramExample.name,
    //     NormalisableRange<float>(g_paramExample.minValue, g_paramExample.maxValue),
    //     g_paramExample.defaultValue,
    //     g_paramExample.unitName,
    //     AudioProcessorParameter::genericParameter));
// these are to additional lines wit lambdas to convert data
        // [](float value, int MaxLen) { value = int(exp(value) * 10) * 0.1;  return (String(value, MaxLen) + " Hz"); },
        // [](const String& text) {return text.getFloatValue(); }));




}

void PeakEQAudio::prepareParameter(std::unique_ptr<juce::AudioProcessorValueTreeState> &vts)
{
    juce::ignoreUnused(vts);
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

}

PeakEQGUI::PeakEQGUI(juce::AudioProcessorValueTreeState& apvts)
:m_apvts(apvts)
{
    
}

void PeakEQGUI::paint(juce::Graphics &g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId).brighter(0.3));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("PeakEQ", getLocalBounds(), juce::Justification::centred, 1);

}

void PeakEQGUI::resized()
{
	auto r = getLocalBounds();
    
    // if you have to place several components, use scaleFactor
    //int width = r.getWidth();
	//float scaleFactor = float(width)/g_minGuiSize_x;

    // use the given canvas in r
    juce::ignoreUnused(r);


}
