/*
  ==============================================================================

    PitchContour.h
    Created: 3 Nov 2015 9:33:09am
    Author:  Som

  ==============================================================================
*/

#ifndef PITCHCONTOUR_H_INCLUDED
#define PITCHCONTOUR_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioSampleRingFrame.h"
#include "ACFPitchTracker.h"
#include "PitchTracker.h"
#include <vector>
using namespace std;

class PitchContour    : public Component, private Timer
{
public:
    PitchContour(): lengthOfPitchArray(390)
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        for(int i = 0; i < lengthOfPitchArray; i++)
        {
            pitchArray.push_back(0);
        }
        startTimerHz (60);
    }

    ~PitchContour()
    {
    }

    void paint (Graphics& g)
    {
        g.fillAll (Colours::white);   // clear the background

        g.setColour (Colours::grey);
        g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

        g.setColour (Colours::red);
        float prevY = 300;
        float prevX = 0;
        float currY = 300;
        float currX = 0;
        for(int i = 1; i < lengthOfPitchArray; i++)
        {
            currY = 300 - 300*(pitchArray[i] - 45)/36;
            currX = (float)i;
            g.drawLine(prevX, prevY, currX, currY);
            prevX = currX;
            prevY = currY;
        }
        g.setFont (14.0f);
        g.drawText (String(pitchArray[lengthOfPitchArray-1]), getLocalBounds(),Justification::centred, true);   // draw some placeholder text
    }
    
    void addNextPitch(float pitchOfFrame)
    {
        //const MessageManagerLock mmLock;
        pitchArray.erase(pitchArray.begin());
        /*
        for (int i = 0; i < 2; i++)
        {
            pitchOfFrame = pitchOfFrame + pitchArray[lengthOfPitchArray-i-2];
        }
        pitchOfFrame = pitchOfFrame/3;
         */
        pitchArray.push_back(pitchOfFrame);
        //Logger::getCurrentLogger()->writeToLog (String(pitchOfFrame));
        //repaint();
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
    
    vector<float> pitchArray;

private:
    
    int lengthOfPitchArray;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchContour)
};


#endif  // PITCHCONTOUR_H_INCLUDED
