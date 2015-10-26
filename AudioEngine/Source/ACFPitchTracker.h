/*
  ==============================================================================

    ACFPitchTracker.h
    Created: 25 Oct 2015 1:54:09pm
    Author:  Som

  ==============================================================================
*/

#ifndef ACFPITCHTRACKER_H_INCLUDED
#define ACFPITCHTRACKER_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioSampleRingFrame.h"
#include <vector>

using namespace std;

class ACFPitchTracker
{

public:
    
    ACFPitchTracker() : sampleRate(44100), windowSize(1024), minPitch(100), maxPitch(1000)
    {
        
    }
    
    
    int getSampleRate()
    {
        return sampleRate;
    }
    
    
    void setSampleRate(double sampleRatetoSet)
    {
        sampleRate = sampleRatetoSet;
    }
    
    int getwindowSize()
    {
        return windowSize;
    }
    
    void setWindowSize(int windowSizetoSet)
    {
        windowSize = windowSizetoSet;
    }
    
    int getMinPitch()
    {
        return minPitch;
    }
    
    void setMinPitch(int minPitchtoSet)
    {
        minPitch = minPitchtoSet;
    }
    
    int getMaxPitch()
    {
        return maxPitch;
    }
    
    void setMaxPitch(int maxPitchtoSet)
    {
        maxPitch = maxPitchtoSet;
    }
    
    float findACFPitchInHZ(AudioSampleRingFrame* window, int readPosition)
    {
        float rmsValue = window->rmsOfWindow();
        if (rmsValue < 0.01)
        {
            return 0;
        }
        else
        {
            float* autoCorrArray = autoCorrelation(window,readPosition);
            int peakLocation = findPeak(autoCorrArray);
            return float(sampleRate)/float(peakLocation);
        }
    }
    
private:
    int sampleRate;
    int windowSize;
    int minPitch; //upper frequency limit in Hz
    int maxPitch; //lower frequency limit in Hz
    //float pitchInHz;
    //float pitchInMidi;
    
    float* autoCorrelation(AudioSampleRingFrame* window, int readPosition)
    {
        float* autoCorrArray = new float[windowSize];
        for (int delay = 0; delay < windowSize; delay++)
        {
            float dummy(0);
            for (int i = 0; i < windowSize - delay; i++)
            {
                dummy = dummy + window->getSample(0, i)*window->getSample(0, delay+i);
            }
            autoCorrArray[delay] = dummy;
        }
        return autoCorrArray;
    }
    
    int maxIndexAutoCorr(float* autoCorrArray, int startIndex, int endIndex)
    {
        int maxIndex = startIndex;
        for (int i = startIndex+1; i < endIndex; i++)
        {
            if (autoCorrArray[i] > autoCorrArray[maxIndex])
            {
                maxIndex = i;
            }
        }
        return maxIndex;
    }
    
    int findPeak(float* autoCorrArray)
    {
        vector<float> peaksLocation;
        vector<float> peakValue;
        int minOffsetSample = sampleRate/maxPitch;
        int maxOffsetSample = sampleRate/minPitch;
        int numPeaks(0);
        for (int i = minOffsetSample+1; i < maxOffsetSample; i++)
        {
            float backDiff = autoCorrArray[i] - autoCorrArray[i-1];
            float forDiff = autoCorrArray[i+1] - autoCorrArray[i];
            if (backDiff*forDiff <= 0)
            {
                peaksLocation.push_back(i);
                peakValue.push_back(autoCorrArray[i]);
                numPeaks++;
            }
        }
        if (peaksLocation.size() == 0)
        {
            return maxIndexAutoCorr(autoCorrArray, minOffsetSample, maxOffsetSample);
        }
        else
        {
            int maxIndex = 0;
            float max = peakValue[maxIndex];
            for (int i = 1; i < numPeaks; i++)
            {
                if (peakValue[i] > max)
                {
                    max = peakValue[i];
                    maxIndex = i;
                }
            }
            return peaksLocation[maxIndex];
        }
        
    }

};

#endif  // ACFPITCHTRACKER_H_INCLUDED
