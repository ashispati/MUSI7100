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
        setMinFreqInHz(110);
        setMaxFreqInHz(1000);
    }
    

    
    
private:
    float findACFPitchInHZ(AudioSampleRingFrame* window) override
    {
        int readPosition = window->getReadPosition();
        float currentFrame[windowSize];
        float rmsValue = window->rmsOfWindow();
        float energyWindow = rmsValue*rmsValue*windowSize;
        if (rmsValue < 0.005)
        {
            pitchArrayInHz.push_back(0);
            return 0;
        }
        else
        {
            for (int i = 0; i < windowSize; i++)
            {
                currentFrame[i] = window->getSample(0, (readPosition+i)%windowSize);
            }
            vector<float> autoCorrArray = autoCorrelation(currentFrame, energyWindow);
            int peakLocation = findPeak(autoCorrArray);
            float pitchInHz = float(sampleRate)/float(peakLocation);
            pitchArrayInHz.push_back(pitchInHz);
            return pitchInHz;
        }
    }
    
    vector<float> smoothAutoCorr(vector<float> autoCorrArray)
    {
        vector<float> dummy(windowSize,0);
        for (int i = 1; i < windowSize; i++ )
        {
            if (i==1 || i==windowSize-1)
            {
                dummy[i] = autoCorrArray[i];
            }
            else if (i == 2 || i==windowSize-2)
            {
                dummy[i] = (autoCorrArray[i-1] + autoCorrArray[i] + autoCorrArray[i+1])/3;
            }
            else
            {
                dummy[i] = (autoCorrArray[i-2] + autoCorrArray[i-1] + autoCorrArray[i] + autoCorrArray[i+1] + autoCorrArray[i+2])/5;
            }
        }
       
        return dummy;
    }
    
    vector<float> autoCorrelation(float currFrame[], float energyWindow)
    {
        vector<float> autoCorrArray(windowSize,0);
        for (int delay = 0; delay < windowSize; delay++)
        {
            float dummy(0);
            for (int i = 0; i < windowSize - delay; i++)
            {
                dummy = dummy + currFrame[i]*currFrame[delay+i];
            }
            autoCorrArray[delay] = dummy/energyWindow;
        }
        vector<float> smoothedAutoCorrArray = smoothAutoCorr(autoCorrArray);
        return smoothedAutoCorrArray;
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
        int maxSearchInHz = 0;
        int minSearchInHz = 0;
        int size = pitchArrayInHz.size();
        
        if (size == 0)
        {
            maxSearchInHz = maxFreqInHz;
            minSearchInHz = minFreqInHz;
        }
        else
        {
            if (pitchArrayInHz[size-1] == 0)
            {
                maxSearchInHz = maxFreqInHz;
                minSearchInHz = minFreqInHz;
            }
            else
            {
                maxSearchInHz = pitchArrayInHz[size-1]*2;
                minSearchInHz = pitchArrayInHz[size-1]/2;
            }
        }
        
        int minOffsetSample = sampleRate/maxFreqInHz;
        int maxOffsetSample = sampleRate/minFreqInHz;
        if (maxFreqInHz > maxSearchInHz)
        {
            minOffsetSample = (int)sampleRate/maxSearchInHz;
        }
        if (minFreqInHz < minSearchInHz)
        {
            maxOffsetSample = (int)sampleRate/minSearchInHz;
        }

        
        int numPeaks(0);
        for (int i = minOffsetSample+1; i < maxOffsetSample-1; i++)
        {
            /*
            if (autoCorrArray[i] >= autoCorrArray[i-1] && autoCorrArray[i] > autoCorrArray[i+1])
            {
                peaksLocation.push_back(i);
                peakValue.push_back(autoCorrArray[i]);
                numPeaks++;
            }
            */
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
    
    void writeFrameToFile(vector<float>autoCorrArray, String fileName)
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(fileName + ".txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        String autoCorrData;
        int i;
        for (i=0; i < windowSize; ++i)
        {
            autoCorrData  = autoCorrData + String(autoCorrArray[i]) + " ";
        }
        autoCorrData = autoCorrData + '\n';
        stream.setPosition(stream.getPosition());
        stream.writeText(autoCorrData, false, false);
    }
};





#endif  // ACFPITCHTRACKER_H_INCLUDED
