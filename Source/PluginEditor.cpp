/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReplicationOneAudioProcessorEditor::ReplicationOneAudioProcessorEditor(ReplicationOneAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), replication(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800,600);//1280
    addAndMakeVisible(replication);
}

ReplicationOneAudioProcessorEditor::~ReplicationOneAudioProcessorEditor()
{
}

//==============================================================================
void ReplicationOneAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colour(20,20,20));
}

void ReplicationOneAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    replication.setBounds(0, 0, getWidth(), getHeight());
}
