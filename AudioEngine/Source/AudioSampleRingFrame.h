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
        readPosition = getNextReadPosition();
    }
    
    int getReadPosition()
    {
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
            sampleValue = getSample(0, (readPosition+i)%windowSize);
            rms = rms + sampleValue*sampleValue;
        }
        rms = sqrt(rms/windowSize);
        return rms;
    }
    
    void writeFrameToFile()
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Frame.txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        String windowData;
        int i;
        for (i=0; i < windowSize; ++i)
        {
            windowData  = windowData + String(getSample(0, (readPosition+i)%windowSize)) + " ";
        }
        windowData = windowData + ";" + '\n';
        stream.setPosition(stream.getPosition());
        stream.writeText(windowData, false, false);
        //Logger::getCurrentLogger()->writeToLog ("Complete one write operation frame" + String(i));
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
    
    int getNextReadPosition()
    {
        readPosition = (readPosition + hopSize) % windowSize;
        return readPosition;
        
    }
    
};



#endif  // AUDIOSAMPLERINGFRAME_H_INCLUDED
