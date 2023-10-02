#pragma once

class Biquad
{
public:    
    Biquad(){reset();};
    Biquad(float b0, float b1, float b2, float a1, float a2);
    void reset();
    int processSamples(float *In, float *Out, int nrofSamples);
    void setCoeffs(float b0, float b1, float b2, float a1, float a2);

private:
    float m_b0 = 1.f, m_b1 = 0.f, m_b2 = 0.f, m_a1 = 0.f, m_a2 = 0.f;
    float m_state_b1 = 0.f, m_state_b2 = 0.f, m_state_a1 = 0.f, m_state_a2 = 0.f;

};
