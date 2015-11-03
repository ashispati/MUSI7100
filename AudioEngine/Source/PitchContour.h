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

class PitchContour    : public Component
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
        Logger::getCurrentLogger()->writeToLog (String(pitchArray[lengthOfPitchArray-1]));
    }

    ~PitchContour()
    {
    }

    void paint (Graphics& g)
    {
        /* This demo code just fills the component's background and
           draws some placeholder text to get you started.

           You should replace everything in this method with your own
           drawing code..
        */

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
            currY = 300 - 300*(pitchArray[i] - 0)/100;
            currX = (float)i;
            g.drawLine(prevX, prevY, currX, currY);
            prevX = currX;
            prevY = currY;
            //g.fillRect(i, yCoordinate ,1,1);
        }
        g.fillRect(1,1,2,2);
        g.setFont (14.0f);
        g.drawText (String(pitchArray[lengthOfPitchArray-1]), getLocalBounds(),Justification::centred, true);   // draw some placeholder text
    }
    
    void drawPitchTrack(float pitchOfFrame)
    {
        const MessageManagerLock mmLock;
        pitchArray.erase(pitchArray.begin());
        pitchArray.push_back(pitchOfFrame);
        repaint();
    }
    
    void resized()
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..

    }

private:
    vector<float> pitchArray;
    int lengthOfPitchArray;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchContour)
};


#endif  // PITCHCONTOUR_H_INCLUDED
