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
#include "PitchTracker.h"
#include "PitchContour.h"
#include "PianoRoll.h"
#include <vector>
#include <iomanip>
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
        setSize (800, 400);
        setAudioChannels (2, 0);
        addAndMakeVisible(pitchContour);
        addAndMakeVisible(pianoRoll);
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
                    float value = 0.5*sin(2*3.14159*441*(i+bufferSize*numBuffers)/44100);
                    if (i == 0)
                    {
                        Logger::getCurrentLogger()->writeToLog(String(value));
                    }
                    //float value = i + numBuffers*bufferSize;
                    channelDataAvg.push_back(value);
                }
                */
                //writeDataToFile(channelDataAvg, bufferSize);
                //writeDataToFileFromPointer(channelData1, bufferSize);
                
                while (channelDataAvg.size() >= hopSize)
                {
                    window->addNextBufferToFrame(channelDataAvg);
                    //window->writeFrameToFile();
                    float midiPitchOfFrame = pitchTracker->findACFPitchMidi(window);
                    //writePitchToFile(midiPitchOfFrame);
                    pitchContour.addNextPitch(midiPitchOfFrame);
                    channelDataAvg.erase(channelDataAvg.begin(), channelDataAvg.begin()+hopSize);
                }
            }
            else
            {
                Logger::getCurrentLogger()->writeToLog("Error");
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

    //=======================================================================
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);

        // You can add your drawing code here!
    }
    
    void resized() override
    {
        int buttonWidth = getWidth()/20;
        int buttonHeight = getHeight()/30;
        int posx = getWidth()/2;
        int posy = getHeight()-10;
        int offSetX1 = getWidth()/40;
        int offSetY1 = getHeight()/30;
        int widthPianoRoll = getWidth()/16;
        int offSetY2 = getHeight()/12;
        recordButton.setBounds(posx-offSetX1, posy-offSetY1, buttonWidth, buttonHeight);
        pianoRoll.setBounds(offSetX1, offSetY1, widthPianoRoll, posy-offSetY2);
        pitchContour.setBounds(2*offSetX1, offSetY1, posx*2-4*offSetX1, posy-offSetY2);
    }


private:
    TextButton recordButton;
    const int windowSize;
    bool recordingStatus;
    AudioSampleRingFrame* window;
    ACFPitchTracker* pitchTracker;
    PitchContour pitchContour;
    double sampleRateInputAudio;
    int numBuffers;
    vector<float> channelDataAvg;
    PianoRoll pianoRoll;
   

    
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
