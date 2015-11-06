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
        setMinFreqInHz(100);
        setMaxFreqInHz(800);
        setHammingCoeff();
    }
    

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
            vector<float> autoCorrArray = autoCorrelation(currentFrame);
            int peakLocation = findPeak(autoCorrArray);
            //Logger::getCurrentLogger()->writeToLog (String(peakLocation));
            return float(sampleRate)/float(peakLocation);
        }
    }
    
private:
    vector<float> smoothAutoCorr(vector<float> autoCorrArray)
    {
        int filterLen = 2;
        vector<float> dummy(windowSize,0);
        for (int i = filterLen; i < windowSize-filterLen; i++ )
        {
            if ((i <filterLen) || (i >= windowSize - filterLen))
            {
                dummy[i] = autoCorrArray[i];
            }
            else
            {
                dummy[i] = dummy[i-1] + (autoCorrArray[i+2] - autoCorrArray[i-1-2])/(2*filterLen+1);
            }
        }
       
        return dummy;
    }
    
    vector<float> autoCorrelation(float currFrame[])
    {
        vector<float> autoCorrArray(windowSize,0);
        for (int delay = 0; delay < windowSize; delay++)
        {
            float dummy(0);
            for (int i = 0; i < windowSize - delay; i++)
            {
                dummy = dummy + currFrame[i]*currFrame[delay+i];
            }
            autoCorrArray[delay] = dummy;
        }
        vector<float> smoothedAutoCorrArray = smoothAutoCorr(autoCorrArray);
        return autoCorrArray;
    }
    
    int maxIndexAutoCorr(vector<float> autoCorrArray, int startIndex, int endIndex)
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
    
    int findPeak(vector<float> autoCorrArray)
    {
        vector<float> peaksLocation;
        vector<float> peakValue;
        int minOffsetSample = sampleRate/maxFreqInHz;
        int maxOffsetSample = sampleRate/minFreqInHz;
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
