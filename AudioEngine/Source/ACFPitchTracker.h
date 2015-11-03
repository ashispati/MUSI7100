/*
  ==============================================================================

    ACFPitchTrack.h
    Created: 2 Nov 2015 1:55:21pm
    Author:  Som

  ==============================================================================
*/

#ifndef ACFPITCHTRACKER_H_INCLUDED
#define ACFPITCHTRACKER_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioSampleRingFrame.h"
#include "PitchTracker.h"
#include <vector>

using namespace std;

class ACFPitchTracker: public PitchTracker
{
public:
    ACFPitchTracker()
    {
        setWindowSize(1024);
        setSampleRate(44100);
        setMinFreq(100);
        setMaxFreq(1000);
        setHammingCoeff();
    }
    
private:
    float findACFPitchInHZ(AudioSampleRingFrame* window) override
    {
        int readPosition = window->getReadPosition();
        float currentFrame[windowSize];
        float rmsValue = window->rmsOfWindow();
        if (rmsValue < 0.01)
        {
            return 0;
        }
        else
        {
            for (int i = 0; i < windowSize; i++)
            {
                currentFrame[i] = window->getSample(0, (readPosition+i)%windowSize);
            }
            hammingWeighting(currentFrame);
            float* autoCorrArray = autoCorrelation(currentFrame);
            int peakLocation = findPeak(autoCorrArray);
            //Logger::getCurrentLogger()->writeToLog (String(peakLocation));
            return float(sampleRate)/float(peakLocation);
        }
    }
    
};





#endif  // ACFPITCHTRACKER_H_INCLUDED
