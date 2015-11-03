/*
  ==============================================================================

    PitchTracker.h
    Created: 2 Nov 2015 1:33:23pm
    Author:  Som

  ==============================================================================
*/

#ifndef PITCHTRACKER_H_INCLUDED
#define PITCHTRACKER_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioSampleRingFrame.h"
#include <vector>
using namespace std;

class PitchTracker
{
public:
    PitchTracker() : sampleRate(44100), windowSize(1024), minFreq(100), maxFreq(1000)
    {
        setHammingCoeff();
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
    
    int getMinFreq()
    {
        return minFreq;
    }
    
    void setMinFreq(int minPitchtoSet)
    {
        minFreq = minPitchtoSet;
    }
    
    int getMaxFreq()
    {
        return maxFreq;
    }
    
    void setMaxFreq(int maxPitchtoSet)
    {
        maxFreq = maxPitchtoSet;
    }
    
    virtual float findACFPitchInHZ(AudioSampleRingFrame* window) = 0;
    
    float findACFPitchMidi(AudioSampleRingFrame* window)
    {
        float pitchInHz = findACFPitchInHZ(window);
        float midiPitch = 0;
        if (pitchInHz == 0)
        {
            midiPitch = 0;
        }
        else
        {
            midiPitch = 69 + 12*log(pitchInHz/440)/log(2);
        }
        
        return midiPitch;
    }
    
    
protected:
    int sampleRate;
    int windowSize;
    int minFreq; //upper frequency limit in Hz
    int maxFreq; //lower frequency limit in Hz
    float hammingWindowCoeff[2048];
    
    void hammingWeighting(float currFrame[])
    {
        for(int i = 0; i < windowSize; i++)
        {
            currFrame[i] = currFrame[i]*hammingWindowCoeff[i];
        }
    }
    
    void setHammingCoeff(void)
    {
        for (int i = 0; i < windowSize; i++)
        {
            hammingWindowCoeff[i] = 0.54 - 0.46*cos(2*3.14159*i/(windowSize-1));
        }
    }
    
    float* autoCorrelation(float currFrame[])
    {
        float* autoCorrArray = new float[windowSize];
        for (int delay = 0; delay < windowSize; delay++)
        {
            float dummy(0);
            for (int i = 0; i < windowSize - delay; i++)
            {
                dummy = dummy + currFrame[i]*currFrame[delay+i];
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
        int minOffsetSample = sampleRate/maxFreq;
        int maxOffsetSample = sampleRate/minFreq;
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


#endif  // PITCHTRACKER_H_INCLUDED
