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

class AudioSampleRingFrame : public AudioSampleBuffer
{
public:
    AudioSampleRingFrame() : windowSize(1024), hopSize(512), writePosition(0), readPosition(512)
    {
        setSize(1, windowSize);
        clear();
    }
    
    AudioSampleRingFrame(int wSize) : windowSize(wSize), hopSize(512), writePosition(0), readPosition(512)
    {
        setSize(1, windowSize);
        clear();
    }

    void addNextBufferToFrame(float channelDataAvg[], int bufferSize)
    {
        if(bufferSize == hopSize)
        {
            //Logger::getCurrentLogger()->writeToLog (String(writePosition));
            for (int i = 0; i < bufferSize; i++)
            {
                setSample(0, i+writePosition, channelDataAvg[i]);
            }
            writePosition = (writePosition+bufferSize) % windowSize;
        }
        else if (bufferSize > hopSize)
        {
            Logger::getCurrentLogger()->writeToLog ("Hop Size and Buffer Size don't match");
        }
        else if(bufferSize < hopSize)
        {
            Logger::getCurrentLogger()->writeToLog ("Hop Size and Buffer Size don't match");
        }
        
    }
    
    int getNextReadPosition()
    {
        readPosition = (readPosition + hopSize) % windowSize;
        //Logger::getCurrentLogger()->writeToLog (String(readPosition));
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
    
private:
    const int windowSize;
    const int hopSize;
    int writePosition;
    int readPosition;
    
};



#endif  // AUDIOSAMPLERINGFRAME_H_INCLUDED
