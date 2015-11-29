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
#include "LessonManager.h"
#include "HomeScreen.h"
#include "VocalEvaluation.h"
#include "MetronomeTimer.h"
#include "AudioEngine.h"
#include <vector>
#include <iomanip>
using namespace std;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component, public Button::Listener
{
public:
    //==============================================================================
    MainContentComponent() :
            audioEngine(deviceManager, audioSourcePlayer, keyboardState, pitchContour, pianoRoll, lessonManager),
            homeScreen(lessonManager),
            lessonManager(deviceManager),
            windowSize(1024)
    {
        addAndMakeVisible(pitchContour);
        addAndMakeVisible(pianoRoll);
        
        recordButton.setColour (TextButton::buttonColourId, Colours::lightgreen );
        recordButton.setColour (TextButton::textColourOnId, Colours::black);
        recordButton.setButtonText ("Record");
        recordButton.addListener(this);
        addAndMakeVisible(recordButton);
        
        backButton.setColour (TextButton::buttonColourId, Colours::lightgreen );
        backButton.setColour (TextButton::textColourOnId, Colours::black);
        backButton.setButtonText ("Back");
        backButton.addListener(this);
        addAndMakeVisible(backButton);
        
        playButton.setColour (TextButton::buttonColourId, Colours::lightgreen );
        playButton.setColour (TextButton::textColourOnId, Colours::black);
        playButton.setButtonText ("Play");
        playButton.addListener(this);
        addAndMakeVisible(playButton);
        
        metronomeButton.setColour (TextButton::buttonColourId, Colours::lightgreen );
        metronomeButton.setColour (TextButton::textColourOnId, Colours::black);
        metronomeButton.setButtonText ("Metro");
        metronomeButton.addListener(this);
        addAndMakeVisible(metronomeButton);
        
        setAudioChannels(2, 2);
        
        homeScreen.setNumLessons();
        addAndMakeVisible(homeScreen);
        
        setOpaque(true);
        setSize (800, 400);
    }

    ~MainContentComponent()
    {
        recordButton.removeListener(this);
        backButton.removeListener(this);
        playButton.removeListener(this);
        metronomeButton.removeListener(this);
        audioSourcePlayer.setSource (nullptr);
        deviceManager.removeAudioCallback (&audioSourcePlayer);
        deviceManager.closeAudioDevice();
    }

    //=======================================================================
    void setAudioChannels(int numInputChannels, int numOutputChannels)
    {
        String audioError = deviceManager.initialise (numInputChannels, numOutputChannels, nullptr, true);
        jassert (audioError.isEmpty());
        audioSourcePlayer.setSource (&audioEngine);
        deviceManager.addAudioCallback (&audioSourcePlayer);
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::white);

        // You can add your drawing code here!
    }
    
    void resized() override
    {
        int buttonWidth = getWidth()/20;
        int buttonHeight = getHeight()/20;
        int offSetX1 = getWidth()/40;
        int offSetY1 = getHeight()/30;
        int posx = getWidth()/2;
        int posy = getHeight()-offSetY1;
        int widthPianoRoll = getWidth()/16;
        int offSetY2 = getHeight()/12;
        recordButton.setBounds(posx-offSetX1, posy-offSetY1, 1.5*buttonWidth, buttonHeight);
        backButton.setBounds(offSetX1, posy-offSetY1, buttonWidth, buttonHeight);
        playButton.setBounds(posx - 4*offSetX1, posy-offSetY1, buttonWidth, buttonHeight);
        metronomeButton.setBounds(posx + 3*offSetX1, posy-offSetY1, buttonWidth, buttonHeight);
        pianoRoll.setBounds(offSetX1, offSetY1, widthPianoRoll, posy-offSetY2);
        pitchContour.setBounds(2*offSetX1, offSetY1, posx*2-4*offSetX1, posy-offSetY2);
        homeScreen.setBounds(getLocalBounds());
    }
    
    void setHomeScreenStatus(bool status)
    {
        
    }

private:
    TextButton recordButton;
    TextButton backButton;
    TextButton playButton;
    TextButton metronomeButton;
    AudioEngine audioEngine;
    PitchContour pitchContour;
    PianoRoll pianoRoll;
    HomeScreen homeScreen;
    LessonManager lessonManager;
    MidiKeyboardState keyboardState;
    AudioDeviceManager deviceManager;
    AudioSourcePlayer audioSourcePlayer;
    const int windowSize;
    
    
    void startRecording()
    {
        recordButton.setColour (TextButton::buttonColourId, Colour (0xffff5c5c));
        recordButton.setButtonText ("Stop");
    }
    
    void stopRecording()
    {
        recordButton.setColour (TextButton::buttonColourId, Colours::lightgreen);
        recordButton.setButtonText ("Record");
    }
    
    void startPlayback()
    {
        playButton.setColour(TextButton::buttonColourId, Colour(0xffff5c5c));
        playButton.setButtonText ("Stop");
    }
    
    void stopPlayback()
    {
        audioEngine.playheadReset();
        playButton.setColour(TextButton::buttonColourId, Colours::lightgreen);
        playButton.setButtonText ("Play");
    }
    
    void disablePlayButton()
    {
        playButton.setColour(TextButton::buttonColourId, Colours::lightgrey);
        playButton.setEnabled(false);
    }
    
    void enablePlayButton()
    {
        playButton.setColour(TextButton::buttonColourId, Colours::lightgreen);
        playButton.setEnabled(true);
    }
    
    void disableRecordButton()
    {
        recordButton.setColour(TextButton::buttonColourId, Colours::lightgrey);
        recordButton.setEnabled(false);
    }
    
    void enableRecordButton()
    {
        recordButton.setColour(TextButton::buttonColourId, Colours::lightgreen);
        recordButton.setEnabled(true);
    }
    
    void disableBackButton()
    {
        backButton.setColour(TextButton::buttonColourId, Colours::lightgrey);
        backButton.setEnabled(false);
    }
    
    void enableBackButton()
    {
        backButton.setColour(TextButton::buttonColourId, Colours::lightgreen);
        backButton.setEnabled(true);
    }
    
    void buttonClicked (Button* button) override
    {
        if (button == &recordButton)
        {
            if (audioEngine.getRecordingStatus())
            {
                stopRecording();
                enablePlayButton();
                enableBackButton();
                audioEngine.setRecordingStatus(false);
            }
            else
            {
                startRecording();
                disablePlayButton();
                disableBackButton();
                audioEngine.setRecordingStatus(true);
            }
        }
        if (button == &backButton)
        {
            homeScreen.setVisible(true);
            homeScreen.setHomeScreenStatus(true);
            pitchContour.clear();
            pianoRoll.clear();
            audioEngine.clear();
            audioEngine.playheadReset();
            
        }
        
        if (button == &playButton)
        {
            if(audioEngine.getPlaybackStatus())
            {
                stopPlayback();
                enableRecordButton();
                enableBackButton();
                audioEngine.setPlaybackStatus(false);
            }
            else
            {
                startPlayback();
                disableRecordButton();
                disableBackButton();
                audioEngine.setPlaybackStatus(true);
            }
        }
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
