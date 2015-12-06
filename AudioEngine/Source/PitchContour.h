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
    PitchContour():
            lengthOfPitchArray(1000),
            pitchesToPlot(500)
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
        float prevYActPitch, prevYRefPitch = getHeight();
        float prevXActPitch, prevXRefPitch = 0;
        float currYActPitch, currYRefPitch = getHeight();
        float currXActPitch, currXRefPitch = 0;
        
        for(int i = 0; i < pitchesToPlot; i++)
        {
            g.setColour (Colours::red);
            currYActPitch = getHeight()*(1 - (pitchArray[lengthOfPitchArray-pitchesToPlot+i] - 45)/36);
            currXActPitch = (float)i;
            if (prevYActPitch <= getHeight() && currYActPitch <= getHeight())
            {
                g.drawLine(prevXActPitch, prevYActPitch, currXActPitch, currYActPitch);
            }
            prevXActPitch = currXActPitch;
            prevYActPitch = currYActPitch;
            
        }
        
        for(int i = 0; i < 2*pitchesToPlot; i++)
        {
            g.setColour (Colours::green);
            currYRefPitch = getHeight()*(1 - (refPitchArray[lengthOfPitchArray-2*pitchesToPlot+i] - 45)/36);
            currXRefPitch = (float)i;
            if (prevYRefPitch <= getHeight() && currYRefPitch <= getHeight())
            {
                g.drawLine(prevXRefPitch, prevYRefPitch, currXRefPitch, currYRefPitch,2);
            }
            prevXRefPitch = currXRefPitch;
            prevYRefPitch = currYRefPitch;
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
    
    void addNextRefPitchInitial(vector<float> refPitch)
    {
        for (int i = 0; i < pitchesToPlot; i++)
        {
            refPitchArray.erase(refPitchArray.begin());
            refPitchArray.push_back(refPitch[i]);
        }
    }
    
    int getPitchesToPlot()
    {
        return pitchesToPlot;
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
