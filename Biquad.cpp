#include "Biquad.h"

Biquad::Biquad(float b0, float b1, float b2, float a1, float a2)
:m_b0(b0), m_b1(b1), m_b2(b2),m_a1(a1), m_a2(a2)
{
    reset();
}

void Biquad::reset()
{
    m_state_a1 = 0.f;
    m_state_a2 = 0.f;
    m_state_b1 = 0.f;
    m_state_b2 = 0.f;
}

int Biquad::processSamples(float *In, float *Out, int nrofSamples)
{
    for (auto kk = 0; kk < nrofSamples; ++kk)
    {
        float Temp = In[kk];
        Out[kk] = Temp*m_b0 + m_state_b1*m_b1 + m_state_b2*m_b2 - m_state_a1*m_a1 - m_state_a2*m_a2;
        m_state_b2 = m_state_b1;
        m_state_b1 = Temp;
        m_state_a2 = m_state_a1;
        m_state_a1 = Out[kk];
    }
    return 0;
}

void Biquad::setCoeffs(float b0, float b1, float b2, float a1, float a2)
{
    m_b0 = b0;
    m_b1 = b1;
    m_b2 = b2;
    m_a1 = a1;
    m_a2 = a2;
}
