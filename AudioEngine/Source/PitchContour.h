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
#include "PianoRoll.h"
#include <vector>
using namespace std;

class PitchContour    : public Component, private Timer
{
public:
    PitchContour(): lengthOfPitchArray(1000), pitchesToPlot(500)
    {
        for(int i = 0; i < lengthOfPitchArray; i++)
        {
            pitchArray.push_back(0);
            refPitchArray.push_back(0);
        }
        pitchesToPlot = getWidth()/2;
        startTimerHz (30);
    }
    
    ~PitchContour()
    {
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::white);   // clear the background
        g.setColour (Colours::grey);
        g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
        g.setColour (Colours::red);
        float prevY = getHeight();
        float prevX = 0;
        float currY = getHeight();
        float currX = 0;
        
        for(int i = 0; i < pitchesToPlot; i++)
        {
            currY = getHeight()*(1 - (pitchArray[lengthOfPitchArray-pitchesToPlot+i] - 45)/36);
            currX = (float)i;
            if (prevY <= getHeight() && currY <= getHeight())
            {
                g.drawLine(prevX, prevY, currX, currY);
            }
            prevX = currX;
            prevY = currY;
        }
        
        g.setColour (Colours::green);
        prevY = getHeight();
        prevX = 0;
        currY = getHeight();
        currX = 0;
        for(int i = 0; i < pitchesToPlot; i++)
        {
            currY = getHeight()*(1 - (refPitchArray[lengthOfPitchArray-pitchesToPlot+i] - 45)/36);
            currX = (float)i;
            if (prevY <= getHeight() && currY <= getHeight())
            {
                g.drawLine(prevX, prevY, currX, currY);
            }
            prevX = currX;
            prevY = currY;
        }
        g.setColour (Colours::red);
        g.setFont (14.0f);
        g.drawText (String(pitchArray[lengthOfPitchArray-1]), getLocalBounds(),Justification::centred, true);   // draw some placeholder text
    }
    
    void addNextPitch(float pitchOfFrame)
    {
        pitchArray.erase(pitchArray.begin());
        pitchArray.push_back(pitchOfFrame);
        //Logger::getCurrentLogger()->writeToLog (String(pitchOfFrame));
    }
    
    void addNextRefPitch(int refPitchOfFrame)
    {
        refPitchArray.erase(refPitchArray.begin());
        refPitchArray.push_back(refPitchOfFrame);
    }
    
    void timerCallback() override
    {
        repaint();
    }
    
    void resized() override
    {
        pitchesToPlot = getWidth()/2;
        // This method is where you should set the bounds of any child
        // components that your component contains..
    }
    
    void clear()
    {
        for(int i = 0; i < lengthOfPitchArray; i++)
        {
            pitchArray[i] = 0;
            refPitchArray[i] = 0;
        }
    }
    

private:
    vector<float> pitchArray;
    vector<float> refPitchArray;
    int lengthOfPitchArray;
    int pitchesToPlot;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchContour)
};


#endif  // PITCHCONTOUR_H_INCLUDED
