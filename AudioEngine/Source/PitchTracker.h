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
    PitchTracker() : sampleRate(44100), windowSize(1024), minFreqInHz(100), maxFreqInHz(600)
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
    
    int getMinFreqInHz()
    {
        return minFreqInHz;
    }
    
    void setMinFreqInHz(int minPitchtoSet)
    {
        minFreqInHz = minPitchtoSet;
    }
    
    int getMaxFreqInHz()
    {
        return maxFreqInHz;
    }
    
    void setMaxFreqInHz(int maxPitchtoSet)
    {
        maxFreqInHz = maxPitchtoSet;
    }
    
    int getLengthOfPitchArray()
    {
        return pitchArray.size();
    }
    
    float movingAverageFilter(float midiPitch)
    {
        int filterLen = 10;
        if (pitchArray.size() >= filterLen-1)
        {
            for (int i = 0; i < filterLen-1; i++)
            {
                midiPitch = midiPitch + pitchArray[pitchArray.size()-i];
            }
            midiPitch = midiPitch/filterLen;
            return midiPitch;
        }
        else
        {
            pitchArray.push_back(midiPitch);
            return midiPitch;
        }
    }
    
    
    float findACFPitchMidi(AudioSampleRingFrame* window)
    {
        float pitchInHz = findACFPitchInHZ(window);
        //Logger::getCurrentLogger()->writeToLog (String(pitchInHz));
        float midiPitch = 0;
        if (pitchInHz == 0)
        {
            midiPitch = 0;
        }
        else
        {
            midiPitch = 69 + 12*log(pitchInHz/440)/log(2);
        }
        pitchArray.push_back(midiPitch);
        //midiPitch = movingAverageFilter(midiPitch);
        Logger::getCurrentLogger()->writeToLog (String(midiPitch));
        return midiPitch;
    }
    
    
protected:
    int sampleRate;
    int windowSize;
    int minFreqInHz; //upper frequency limit in Hz
    int maxFreqInHz; //lower frequency limit in Hz
    float hammingWindowCoeff[2048];
    vector<float> pitchArray;
    
    virtual float findACFPitchInHZ(AudioSampleRingFrame* window) = 0;
    
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

    
};


#endif  // PITCHTRACKER_H_INCLUDED
