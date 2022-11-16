/*
  ==============================================================================

    RAStreamAnalysis.h
    Created: 15 Nov 2022 3:54:53am
    Author:  dombr

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <map>
class RAStreanAnalysis
{
public:
    template <typename ProcessContext>
    void process(const ProcessContext& context) {

        auto&& inBlock = context.getInputBlock();
        auto&& outBlock = context.getOutputBlock();

        int numChannels = inBlock.getNumChannels();
        int numSamples = inBlock.getNumSamples();
        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* dst = outBlock.getChannelPointer(channel);
            for (int sampleIndex = 0; sampleIndex < numSamples; sampleIndex++)
            {
                auto currentSample = inBlock.getSample(channel, sampleIndex);
                // This should not really write anything to the out block
                dst[sampleIndex] = currentSample;
                if (bufferIndex[channel] >= bufferLength)
                {
                    bufferIndex[channel] = 0;
                    bufferFilled = true;
                }
                buffer[channel][bufferIndex[channel]] = currentSample;
                bufferIndex[channel]++;
                
            }
            lastSample[channel] = dst[numSamples - 1];
        }
    }
    double getLastSample(int channel)
    {
        return lastSample[channel];
    }
    double getLastSampleAvg(int channel)
    {
        if (bufferFilled == false)
        {
            return 0;
        }
        double total = 0;
        for (int i = 0; i < bufferLength; i++)
        {
            jassert(buffer[channel][i] > -10.0 && buffer[channel][i] < 10.0);
            total += buffer[channel][i];
        }
        double avg = total / bufferLength;
        jassert(avg>-10.0 && avg <10.0);
        return avg;
    }
    double getLastSamplePeak(int channel, bool abs = false)
    {
        if (bufferFilled == false)
        {
            return 0;
        }
        double peak = 0;
        for (int i = 0; i < bufferLength; i++)
        {
            if (std::abs(buffer[channel][i]) > std::abs(peak))//added second abs
            {
                if (abs)
                {
                    peak = std::abs(buffer[channel][i]);
                }
                else
                {
                    peak = buffer[channel][i];
                }
            }
        }
         return peak;
    }
    bool isReady()
    {
        return bufferFilled;
    }
private:
    double lastSample[2] = {0 , 0};
    double buffer[2][96000];
    int bufferLength = 480;
    int bufferIndex[2] = { 0,0 };
    bool bufferFilled = false;
};