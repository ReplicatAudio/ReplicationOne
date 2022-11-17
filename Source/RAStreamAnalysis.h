/*
  ==============================================================================

    RAStreamAnalysis.h
    Created: 15 Nov 2022 3:54:53am
    Author:  dombr
    ---

    This module implements a ring buffer and provides some analysis tools for said buffer.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <map>
class RAStreamAnalysis
{
public:
    // Use this to prepare the processor
    // Higher sample rate divisions mean more averaging but at the cost of perfromance
    void prepareToPlay(int newSampleRate, int sampleRateDivision = 20)
    {
        jassert(newSampleRate > 0);
        jassert(sampleRateDivision > 0);
        sampleRate = newSampleRate;
        srd = sampleRateDivision;
        prepareBuffer();
    }
    template <typename ProcessContext>
    void process(const ProcessContext& context) {
        if (sampleRate <= 0 || bufferLength > 96000)
        {
            jassertfalse;
            return;
        }
        // Feed the audio steam into the buffer
        auto&& inBlock = context.getInputBlock();

        int numChannels = inBlock.getNumChannels();
        int numSamples = inBlock.getNumSamples();
        for (int channel = 0; channel < numChannels; ++channel)
        {
            for (int sampleIndex = 0; sampleIndex < numSamples; sampleIndex++)
            {
                auto currentSample = inBlock.getSample(channel, sampleIndex);
                if (bufferIndex[channel] >= bufferLength)
                {
                    // We have reached the "end" of the buffer and need to return to the beginning
                    bufferIndex[channel] = 0;
                    bufferFilled = true;
                }
                buffer[channel][bufferIndex[channel]] = currentSample;
                bufferIndex[channel]++;
            }
        }
    }
    // Returns the last sample from the current buffer
    // This is NOT sample rate agnostic
    // You probablly dont want this
    double getLastSample(int channel)
    {
        return buffer[channel][bufferIndex[channel]];
    }
    // Returns an average sample from the current buffer
    double getLastSampleAvg(int channel)
    {
        if (!bufferFilled) return 0;
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
    // Returns the highest peak from the current buffer
    double getLastSamplePeak(int channel, bool abs = false)
    {
        if (!bufferFilled) return 0;
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
    // Returns the current sample rate
    int getSampleRate()
    {
        return sampleRate;
    }
    // Returns true if the buffer is has filled at least once
    bool isReady()
    {
        return bufferFilled;
    }
private:
    void prepareBuffer()
    {
        // Our buffer length needs to go up as
        // the sample rate increases
        bufferLength = sampleRate / srd;
    }

    // Max buffer size is set to 1 second of audio at 96k. 
    // For lower sample rates this buffer can cover a longer period of time
    double buffer[2][96000];

    // Set the actual size/length of the buffer
    // Higher values will lead to more averaging
    // But higher sample rates mean less averaging with the same length
    // This is set in prepareBuffer() by calling prepareToPlay()
    int bufferLength = 0;

    // Sample rate division controls the buffer length in a way that 
    // is sample rate agnostic
    int srd = 0;

    int bufferIndex[2] = { 0,0 };
    bool bufferFilled = false;
    int sampleRate = 0;// Must be set with prepareToPlay()
};