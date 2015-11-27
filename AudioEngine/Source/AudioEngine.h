/*
 ==============================================================================
 
 AudioEngine.h
 Created: 24 Nov 2015 7:21:42pm
 Author:  Som
 
 ==============================================================================
 */

#ifndef AUDIOENGINE_H_INCLUDED
#define AUDIOENGINE_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioSampleRingFrame.h"
#include "ACFPitchTracker.h"
#include "PitchTracker.h"
#include "PitchContour.h"
#include "PianoRoll.h"
#include <vector>
#include <iomanip>
using namespace std;


class AudioEngine: public AudioSource
{
public:
    AudioEngine() : windowSize(1024), recordingStatus(false)
    {
        setAudioChannels(2, 0);
        window = new AudioSampleRingFrame(windowSize);
        pitchTracker = new ACFPitchTracker();
        pitchTracker->setSampleRate(sampleRateInputAudio);
        pitchTracker->setWindowSize(windowSize);
        numBuffers = 0;
        channelDataAvg.begin();
    }
    
    ~AudioEngine()
    {
        delete window;
        delete pitchTracker;
        shutDownAudio();
    }
    
    void setAudioChannels(int numInputChannels, int numOutputChannels)
    {
        String audioError = deviceManager.initialise (numInputChannels, numOutputChannels, nullptr, true);
        jassert (audioError.isEmpty());
        deviceManager.addAudioCallback (&audioSourcePlayer);
        audioSourcePlayer.setSource (this);
    }
    
    void shutDownAudio()
    {
        audioSourcePlayer.setSource (nullptr);
        deviceManager.removeAudioCallback (&audioSourcePlayer);
        deviceManager.closeAudioDevice();
    }
    
    void setRefereces(PitchContour* contour, PianoRoll* roll)
    {
        pitchContour = contour;
        pianoRoll = roll;
    }
    
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        String message;
        message << "Preparing to Play Audio... \n";
        message << "Samples per Block Expected = " << samplesPerBlockExpected << "\n";
        message << "Sample Rate = " << sampleRate;
        sampleRateInputAudio = sampleRate;
        Logger::getCurrentLogger()->writeToLog(message);
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        double start = Time::getMillisecondCounterHiRes();
        if(recordingStatus)
        {
            if (bufferToFill.buffer->getNumChannels() > 0)
            {
                int bufferSize = bufferToFill.numSamples;
                int hopSize = window->getHopSize();
                //Logger::getCurrentLogger()->writeToLog("Recording");
                
                const float* channelData1 = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
                //const float* channelData2 = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample); //check for num of channels
                
                for(int i = 0; i < bufferSize; i++)
                {
                    channelDataAvg.push_back(channelData1[i]);
                }
                /*
                // code for testing the buffer and pitch tracker
                for(int i = 0; i < bufferSize; i++)
                {
                    float value = 0.5*sin(2*3.14159*440*(i+bufferSize*numBuffers)/44100);
                    //float value = i + numBuffers*bufferSize;
                    channelDataAvg.push_back(value);
                }
                */
                //writeDataToFile(channelDataAvg, bufferSize);
                
                while (channelDataAvg.size() >= hopSize)
                {
                    window->addNextBufferToFrame(channelDataAvg);
                    //window->writeFrameToFile();
                    float midiPitchOfFrame = pitchTracker->findACFPitchMidi(window);
                    //writePitchToFile(midiPitchOfFrame);
                    pitchContour->addNextPitch(midiPitchOfFrame);
                    pianoRoll->setCurrentQuantizedPitch(pitchTracker->quantizeMidiPitch(midiPitchOfFrame));
                    channelDataAvg.erase(channelDataAvg.begin(), channelDataAvg.begin()+hopSize);
                }
            }
            else
            {
                Logger::getCurrentLogger()->writeToLog("Error: No Input Audio Channel");
            }
            numBuffers = numBuffers + 1;
        }
        else
        {
            numBuffers = 0;
        }
        
        bufferToFill.clearActiveBufferRegion();
        double duration = Time::getMillisecondCounterHiRes() - start;
        //cout << duration << endl;
        if (duration > 11.6099773)
        {
            Logger::getCurrentLogger()->writeToLog ("Time Exceeded");
        }
    }
    
    void releaseResources() override
    {
        Logger::getCurrentLogger()->writeToLog ("Releasing audio resources");
    }
    
    void setRecordingStatus(bool status)
    {
        recordingStatus = status;
    }
    
    bool getRecordingStatus()
    {
        return recordingStatus;
    }
        
    void clear()
    {
        pitchTracker->clear();
    }
    
private:
    AudioDeviceManager deviceManager;
    AudioSourcePlayer audioSourcePlayer;
    const int windowSize;
    bool recordingStatus;
    AudioSampleRingFrame* window;
    ACFPitchTracker* pitchTracker;
    PitchContour* pitchContour;
    PianoRoll* pianoRoll;
    double sampleRateInputAudio;
    int numBuffers;
    vector<float> channelDataAvg;
    
    
    void writeToFile(float channelDataAvg[], int hopSize)
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Buffer.txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        stream.setPosition(stream.getPosition());
        String bufferData;
        int i;
        for (i=0; i < hopSize; ++i)
        {
            bufferData  = bufferData + String(channelDataAvg[i]) + " ";
        }
        bufferData = bufferData + '\n';
        stream.writeText(bufferData, false, false);
        //Logger::getCurrentLogger()->writeToLog ("Complete one write operation buffer" + String(i));
    }
    
    void writeDataToFile(vector<float> channelDataAvg, int bufferSize)
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Audio.txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        stream.setPosition(stream.getPosition());
        String bufferData;
        int i;
        for (i = 0; i < bufferSize; i++)
        {
            bufferData  = String(channelDataAvg[i]) + '\n';
            stream.writeText(bufferData, false, false);
        }
        
    }
    
    void writeDataToFileFromPointer(const float* channelData, int bufferSize)
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("AudioPointer.txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        stream.setPosition(stream.getPosition());
        String bufferData;
        int i;
        for (i = 0; i < bufferSize; i++)
        {
            bufferData  = String(channelData[i]) + '\n';
            stream.writeText(bufferData, false, false);
        }
        
    }
    
    void writePitchToFile(float pitchOfFrame)
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Pitch.txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        stream.setPosition(stream.getPosition());
        String pitchToWrite = String(pitchOfFrame) + '\n';
        stream.writeText(pitchToWrite, false, false);
    }
    
    void writeToFile(vector<float> channelDataAvg, int bufferSize)
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Buffer.txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        stream.setPosition(stream.getPosition());
        String bufferData;
        int i;
        for (i=0; i < bufferSize; ++i)
        {
            bufferData  = bufferData + String(channelDataAvg[i]) + " ";
        }
        bufferData = bufferData + '\n';
        stream.writeText(bufferData, false, false);
        //Logger::getCurrentLogger()->writeToLog ("Complete one write operation buffer" + String(i));
    }
    
    
};



#endif  // AUDIOENGINE_H_INCLUDED
