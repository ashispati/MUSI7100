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
#include <stdlib.h>
using namespace std;

//==============================================================================
/*
*/
class PianoRoll    : public Component, private Timer
{
public:
    PianoRoll(): numWhiteKeys(21), numBlackKeys(15), currentQuantizedPitch(0)
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
        double start = Time::getMillisecondCounterHiRes();
        g.fillAll (Colours::white);   // clear the background
        int heightOfWhiteKeys = (int)(getHeight()/numWhiteKeys);
        int widthOfWhiteKeys = getWidth();
        int yCoorWhiteKeys[numWhiteKeys];
        int xCoorWhiteKeys[numWhiteKeys];
        
        int heightOfBlackKeys = (int)heightOfWhiteKeys*4/5;
        int widthOfBlackKeys = (int)widthOfWhiteKeys*4/5;
        int yCoorBlackKeys[numBlackKeys];
        int xCoorBlackKeys[numBlackKeys];
        
        int blackKeyIndex = 0;
        for (int i = 0; i <= numWhiteKeys; i++)
        {
            g.setColour (Colours::grey);
            yCoorWhiteKeys[i] = i*heightOfWhiteKeys;
            xCoorWhiteKeys[i] = 0;
            g.drawRect(xCoorWhiteKeys[i], yCoorWhiteKeys[i], widthOfWhiteKeys ,heightOfWhiteKeys);
            int j = i%7;
            if ((j != 2) && (j !=5))
            {
                g.setColour (Colours::black);
                yCoorBlackKeys[blackKeyIndex] = yCoorWhiteKeys[i] - heightOfBlackKeys/2;
                xCoorBlackKeys[blackKeyIndex] = 0;
                g.fillRect(xCoorBlackKeys[blackKeyIndex], yCoorBlackKeys[blackKeyIndex], widthOfBlackKeys ,heightOfBlackKeys);
                blackKeyIndex = blackKeyIndex + 1;
            }
            
        }
        
        if (currentQuantizedPitch >= 45 && currentQuantizedPitch < 81)
        {
            Logger::getCurrentLogger()->writeToLog (String(currentQuantizedPitch));
            int scaledPitch = currentQuantizedPitch - 45;
            div_t divresult = div(scaledPitch,12);
            int octaveNumber = divresult.quot;
            int pitchClass = divresult.rem;
            if (pitchClass == 1 || pitchClass == 4 || pitchClass == 6 || pitchClass == 9 || pitchClass == 11)
            {
                int keyIndex;
                switch (pitchClass)
                {
                    case 1:
                        keyIndex = 1;
                        break;
                    case 4:
                        keyIndex = 2;
                        break;
                    case 6:
                        keyIndex = 3;
                        break;
                    case 9:
                        keyIndex = 4;
                        break;
                    case 11:
                        keyIndex = 5;
                        break;
                    default:
                        break;
                }
                keyIndex = 5*octaveNumber + keyIndex;
                g.setColour (Colours::red);
                g.drawRect(xCoorBlackKeys[15-keyIndex], yCoorBlackKeys[15-keyIndex], widthOfBlackKeys ,heightOfBlackKeys, 3);
            }
            else
            {
                int keyIndex;
                switch (pitchClass)
                {
                    case 0:
                        keyIndex = 0;
                        break;
                    case 2:
                        keyIndex = 1;
                        break;
                    case 3:
                        keyIndex = 2;
                        break;
                    case 5:
                        keyIndex = 3;
                        break;
                    case 7:
                        keyIndex = 4;
                        break;
                    case 8:
                        keyIndex = 5;
                        break;
                    case 10:
                        keyIndex = 6;
                        break;
                    default:
                        break;
                }
                keyIndex = 7*octaveNumber + keyIndex;
                g.setColour (Colours::red);
                g.drawRect(xCoorWhiteKeys[20-keyIndex], yCoorWhiteKeys[20-keyIndex], widthOfWhiteKeys ,heightOfWhiteKeys, 3);
            }
            
        }
        
        
        double duration = Time::getMillisecondCounterHiRes() - start;
        if (duration > 30)
        {
            Logger::getCurrentLogger()->writeToLog ("Time Exceeded");
        }
    }
    
    void setCurrentQuantizedPitch(int quantizedPitchToSet)
    {
        currentQuantizedPitch = quantizedPitchToSet;
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
    int numWhiteKeys;
    int numBlackKeys;
    int currentQuantizedPitch;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRoll)
};


#endif  // PIANOROLL_H_INCLUDED
