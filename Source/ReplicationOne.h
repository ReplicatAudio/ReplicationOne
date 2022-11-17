/*
  ==============================================================================

    ReplicationOne.h
    v0.8.0
    ---
    No memory should be allocated at runtime
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
class ReplicationOne : public juce::Component, private juce::Timer
{
public:
    ReplicationOne(ReplicationOneAudioProcessor& p) : audioProcessor(p) {
        startTimerHz(fps*samplesPerPaint);
        // Ensure the entire plugin window is running in the OpenGL context
        openGLContext.attachTo(*getTopLevelComponent());
    };
    ~ReplicationOne() {};
    void timerCallback()
    {
        // If the analysis module is not ready dont even try
        if (audioProcessor.streamAnalysis.isReady() == false) return;
        // Cache samples for each stereo channel
        cachedSample[0] = audioProcessor.streamAnalysis.getLastSamplePeak(0);
        cachedSample[1] = audioProcessor.streamAnalysis.getLastSamplePeak(1);
        // Calculate the difference between the stereo channels
        diff = std::abs(std::abs(cachedSample[0]) - std::abs(cachedSample[1]));
        // Point the difference value in the right direction
        diff = std::abs(cachedSample[0]) > std::abs(cachedSample[1]) ? -diff : diff;
        // Define the peak as the higher abs value of the 2 channels
        peak = juce::jmax(std::abs(cachedSample[0]), std::abs(cachedSample[1]));
        // Assign the X and Y coords of this point
        y[pointIndex] = (getHeight() - 20) - (peak * getHeight());
        x[pointIndex] = (getWidth() / 2) + (diff * (getWidth() / 2));

        pointIndex++;
        // Check if we are over the point buffer size
        if (pointIndex >= pbSize)
        {
            // Reset the point index
            pointIndex = 0;
        }

        timerCount++;
        // Check if we should actually paint this time
        if (timerCount == samplesPerPaint)
        {
            repaint();
            timerCount = 0;
        }
        
    }
    void paint(juce::Graphics& g) override
    {
        // Draw the main UI elements
        g.setGradientFill(juce::ColourGradient(juce::Colour(11, 9, 12), getWidth() / 2, getHeight(), juce::Colour(33, 26, 35), getWidth() / 2, peak * (getHeight()/2)-(getHeight()/2), false));
        g.fillAll();
        g.setGradientFill(juce::ColourGradient(juce::Colour(33/2, 26/2, 35/2), getWidth() / 2, getHeight(), juce::Colour(11, 9, 12), getWidth() / 2, peak * (getHeight()) - (getHeight()), false));
        g.drawLine(getWidth()/2, 0, getWidth() / 2, getHeight(), 3.0f);
        
        g.fillPath(t1);
        
        g.fillPath(t2);

        // Draw the points and lines
        for (pitr = 0; pitr < pbSize; pitr++)
        {
            // Actual sample index
            pitrRel = pitr > pointIndex ? pbSize - pitr + pointIndex : pointIndex - pitr;
            if (pitrRel == pointIndex)
            {
                // Drawing the point index itself causes bugs
                continue;
            }
            maxSize = (juce::jmax(12 - (pitr / 2), 2));
            // Set the dot/line colour
            switch (pitrRel % 3)
            {
            case 0:
                g.setColour(juce::Colour(219, 90, 66));
                break;
            case 1:
                g.setColour(juce::Colour(84, 134, 135));
                break;
            case 2:
                g.setColour(juce::Colour(246, 174, 45));
                break;
            }
            // Draw point
            g.setOpacity(0.8f - (pitr * (0.8f / pbSize)));
            switch (mode)
            {
            case 0:
                g.fillEllipse(x[pitrRel] - (maxSize / 2), y[pitrRel] - (maxSize / pbSize), maxSize, maxSize);
                break;
            case 1:
                g.fillEllipse(x[pitrRel], y[pitrRel], 2, 2);
                break;
            }
            // Draw the connecting lines between this dot and the last dot in the point buffer
            g.setOpacity(juce::jmax(0.2f - ((pitr *2) * (0.2f / pbSize)),0.0f));
            g.drawLine(x[pitrRel], y[pitrRel], x[pitrRel > 1 ? pitrRel - 1 : pbSize - 1], y[pitrRel > 1 ? pitrRel - 1 : pbSize - 1], 3.0f);
        }
    }
    void resized() override
    {
        // We only need to setup the UI triangle paths once 
        t1.clear();
        t1.addTriangle(0, 0, 0, getHeight(), getWidth() / 2, getHeight());
        t2.clear();
        t2.addTriangle(getWidth(), 0, getWidth(), getHeight(), getWidth() / 2, getHeight());
    }
private:
    // Graphical Settings 
    int mode = 0;
    // Analysis Settings
    int samplesPerPaint = 8;
    int fps = 60;
    const int pbSize = 1024;// Point buffer size
    // Analysis Cache
    int x[1024] = { 0 };// Point ring buffer X
    int y[1024] = { 0 };// Point ring buffer Y
    int pointIndex = 0;// Current position in the point buffer
    double peak = 0;
    float diff = 0;
    // Drawing
    bool isClipping = false;
    double cachedSample[2] = { 0,0 };// Cache the last sample in stereo (L,R)
    // Drawing Cache
    int pitr = 0;// Main paint iterator
    int pitrRel = 0;// Relative points iterator
    int maxSize = 0;// The max/starting size of the dots
    juce::Path t1;// UI path for left triangle
    juce::Path t2;// UI path for right triangle
    // Processing 
    int timerCount = 0;
    juce::OpenGLContext openGLContext;
    ReplicationOneAudioProcessor& audioProcessor;
};
