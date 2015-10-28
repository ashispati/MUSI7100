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
                const float* channelData1 = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
                const float* channelData2 = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample);
                float channelDataAvg[bufferSize];
                for(int i=0; i < bufferSize; i++)
                {
                    channelDataAvg[i] = (channelData1[i] + channelData2[i])/2;
                }
                window->addNextBufferToFrame(channelDataAvg, bufferSize);
                //writeToFile(channelDataAvg, bufferSize);
                int readPosition = window->getNextReadPosition();
                //writeFrameToFile(window, readPosition ,windowSize);
                //float rmsValue = window->rmsOfWindow();
                //Logger::getCurrentLogger()->writeToLog (String(rmsValue));
                float pitchOfFrame = pitchTracker->findACFPitchMidi(window, readPosition);
                Logger::getCurrentLogger()->writeToLog (String(pitchOfFrame));
            }
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
    
    void writeToFile(float channelDataAvg[], int bufferSize)
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
            bufferData  = String(channelDataAvg[i]) + " " + bufferData;
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
            windowData  = String(window->getSample(0, (readPosition+i)%windowSize)) + " " + windowData;
        }
        windowData = windowData + ";" + '\n';
        stream.setPosition(stream.getPosition());
        stream.writeText(windowData, false, false);
        //Logger::getCurrentLogger()->writeToLog ("Complete one write operation frame" + String(i));
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
