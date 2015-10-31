/*
  ==============================================================================

    AudioSampleRingFrame.h
    Created: 25 Oct 2015 10:49:33am
    Author:  Som

  ==============================================================================
*/

#ifndef AUDIOSAMPLERINGFRAME_H_INCLUDED
#define AUDIOSAMPLERINGFRAME_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
using namespace std;

class AudioSampleRingFrame : public AudioSampleBuffer
{
public:
    AudioSampleRingFrame() : windowSize(1024), hopSize(512), writePosition(windowSize - hopSize), readPosition(windowSize - hopSize)
    {
        setSize(1, windowSize);
        clear();
    }
    
    AudioSampleRingFrame(int wSize) : windowSize(wSize), hopSize(512), writePosition(windowSize - hopSize), readPosition(windowSize - hopSize)
    {
        setSize(1, windowSize);
        clear();
    }

    void addNextBufferToFrame(vector<float> channelDataAvg)
    {
        //Logger::getCurrentLogger()->writeToLog (String(writePosition));
        for (int i = 0; i < hopSize; i++)
        {
            setSample(0, i+writePosition, channelDataAvg[i]);
        }
        writePosition = (writePosition+hopSize) % windowSize;
    }
    
    int getNextReadPosition()
    {
        readPosition = (readPosition + hopSize) % windowSize;
        return readPosition;
        
    }
    
    int getWindowSize()
    {
        return windowSize;
    }
    
    int getHopSize()
    {
        return hopSize;
    }
    
    float rmsOfWindow()
    {
        float rms = 0;
        float sampleValue = 0;
        for (int i = 0; i < windowSize; i++)
        {
            sampleValue = getSample(0, i);
            rms = rms + sampleValue*sampleValue;
        }
        rms = sqrt(rms/windowSize);
        return rms;
    }
    
    /*
    float getMean()
    {
        float mean(0);
        for (int i = 0; i < windowSize; i++)
        {
            mean = mean + getSample(0, i);
        }
        mean = mean/windowSize;
        return mean;
    }
    
    void removeDCOffset()
    {
        float mean = getMean();
        for (int i = 0; i < windowSize; i++)
        {
            setSample(0, i, getSample(0, i) - mean);
        }
        
    }
    */
    
private:
    const int windowSize;
    const int hopSize;
    int writePosition;
    int readPosition;
    
};



#endif  // AUDIOSAMPLERINGFRAME_H_INCLUDED
