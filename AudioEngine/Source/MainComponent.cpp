/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioSampleRingFrame.h"
#include "ACFPitchTracker.h"
#include <vector>
using namespace std;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public AudioAppComponent, public Button::Listener
{
public:
    //==============================================================================
    MainContentComponent() : windowSize(1024),recordingStatus(false)
    {
        setSize (800, 600);
        setAudioChannels (2, 0);
        recordButton.setColour (TextButton::buttonColourId, Colour (0xffff5c5c));
        recordButton.setColour (TextButton::textColourOnId, Colours::black);
        recordButton.setButtonText ("Start");
        recordButton.addListener(this);
        addAndMakeVisible(recordButton);
        window = new AudioSampleRingFrame(windowSize);
        pitchTracker = new ACFPitchTracker();
        pitchTracker->setSampleRate(sampleRateInputAudio);
        pitchTracker->setWindowSize(windowSize);
        numBuffers = 0;
        channelDataAvg.begin();
        
        //Logger::getCurrentLogger()->writeToLog (String(window->getNumSamples()));
        //writeFrameToFile(window, windowSize);
    }

    ~MainContentComponent()
    {
        recordButton.removeListener(this);
        delete window;
        delete pitchTracker;
        shutdownAudio();
    }

    //=======================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        String message;
        message << "Preparing to play audio...\n";
        message << " samplesPerBlockExpected = " << samplesPerBlockExpected << "\n";
        message << " sampleRate = " << sampleRate;
        sampleRateInputAudio = sampleRate;
        Logger::getCurrentLogger()->writeToLog (message);
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        if(recordingStatus)
        {
            if (bufferToFill.buffer->getNumChannels() > 0)
            {
                int bufferSize = bufferToFill.numSamples;
                int hopSize = window->getHopSize();
                
                const float* channelData1 = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
                const float* channelData2 = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample); //check for num of channels
                for(int i=0; i < bufferSize; i++)
                {
                    channelDataAvg.push_back((channelData1[i] + channelData2[i])/2);
                }
                
                // code for testing the buffer and pitch tracker
                /*
                for(int i = 0; i < bufferSize; i++)
                {
                    float value = 0.5*sin(2*3.14159*261.23*(i+bufferSize*numBuffers)/44100);
                    //float value = i + numBuffers*bufferSize;
                    channelDataAvg.push_back(value);
                }
                */
                    
                while (channelDataAvg.size() >= hopSize)
                {
                    window->addNextBufferToFrame(channelDataAvg);
                    //writeToFile(channelDataAvg, hopSize);
                    int readPosition = window->getNextReadPosition();
                    //writeFrameToFile(window, readPosition ,windowSize);
                    float pitchOfFrame = pitchTracker->findACFPitchMidi(window, readPosition);
                    Logger::getCurrentLogger()->writeToLog (String(pitchOfFrame));
                    channelDataAvg.erase(channelDataAvg.begin(), channelDataAvg.begin()+hopSize);
                }
                
            }
            numBuffers = numBuffers + 1;
        }
        else
        {
            numBuffers = 0;
        }
        
        bufferToFill.clearActiveBufferRegion();
    }

    void releaseResources() override
    {
        Logger::getCurrentLogger()->writeToLog ("Releasing audio resources");
    }

    //=======================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colours::black);


        // You can add your drawing code here!
    }

    void resized() override
    {
        int width = 40;
        int height = 20;
        int posx = getWidth()/2;
        int posy = getHeight()/2;
        recordButton.setBounds(posx-20, posy-20, width, height);
    }


private:
    TextButton recordButton;
    const int windowSize;
    bool recordingStatus;
    AudioSampleRingFrame* window;
    ACFPitchTracker* pitchTracker;
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
        bufferData = bufferData + ";" + '\n';
        stream.writeText(bufferData, false, false);
        //Logger::getCurrentLogger()->writeToLog ("Complete one write operation buffer" + String(i));
    }
    
    void writeToFile(vector<float> channelDataAvg, int hopSize)
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
        bufferData = bufferData + ";" + '\n';
        stream.writeText(bufferData, false, false);
        //Logger::getCurrentLogger()->writeToLog ("Complete one write operation buffer" + String(i));
    }
    
    void writeFrameToFile(AudioSampleRingFrame* window, int readPosition, int frameSize)
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Frame.txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        String windowData;
        int i;
        for (i=0; i < frameSize; ++i)
        {
            windowData  = windowData + String(window->getSample(0, (readPosition+i)%windowSize)) + " ";
        }
        windowData = windowData + ";" + '\n';
        stream.setPosition(stream.getPosition());
        stream.writeText(windowData, false, false);
        //Logger::getCurrentLogger()->writeToLog ("Complete one write operation frame" + String(i));
    }
    
    void removeFromChannelData(vector<float> channelDataAvg)
    {
        int length = channelDataAvg.size();
        
    }
    
    void startRecording()
    {
        recordButton.setButtonText ("Stop");
    }
    
    void stopRecording()
    {
        recordButton.setButtonText ("Start");
    }
    
    void buttonClicked (Button* button) override
    {
        if (button == &recordButton)
        {
            if (recordingStatus == true)
            {
                stopRecording();
                recordingStatus = false;
            }
            else
            {
                startRecording();
                recordingStatus = true;
            }
        }
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
