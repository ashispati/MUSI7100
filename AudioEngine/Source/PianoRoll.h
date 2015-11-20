/*
  ==============================================================================

    PianoRoll.h
    Created: 20 Nov 2015 12:45:04pm
    Author:  Som

  ==============================================================================
*/

#ifndef PIANOROLL_H_INCLUDED
#define PIANOROLL_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioSampleRingFrame.h"
#include "ACFPitchTracker.h"
#include "PitchTracker.h"
#include <vector>
using namespace std;

//==============================================================================
/*
*/
class PianoRoll    : public Component, private Timer
{
public:
    PianoRoll()
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        startTimerHz (30);
    }

    ~PianoRoll()
    {
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::white);   // clear the background

        g.setColour (Colours::grey);
        g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

        g.setColour (Colours::lightblue);
        g.setFont (14.0f);
        g.drawText ("PianoRoll", getLocalBounds(),
                    Justification::centred, true);   // draw some placeholder text
    }
    
    
    void timerCallback() override
    {
        repaint();
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..

    }
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRoll)
};


#endif  // PIANOROLL_H_INCLUDED
