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
            _audioEngine(_deviceManager, _audioSourcePlayer, _pitchTracker,_keyboardState, _pitchContour, _pianoRoll, _lessonManager),
            _homeScreen(_lessonManager),
            _lessonManager(_deviceManager,_pitchContour),
            _vocalEvaluation(_pitchTracker,_lessonManager),
            _windowSize(1024), _accuracy(0)
    {
        addAndMakeVisible(_pitchContour);
        addAndMakeVisible(_pianoRoll);
        
        _recordButton.setColour (TextButton::buttonColourId, Colours::lightgreen );
        _recordButton.setColour (TextButton::textColourOnId, Colours::black);
        _recordButton.setButtonText ("Record");
        _recordButton.addListener(this);
        addAndMakeVisible(_recordButton);
        
        _backButton.setColour (TextButton::buttonColourId, Colours::lightgreen );
        _backButton.setColour (TextButton::textColourOnId, Colours::black);
        _backButton.setButtonText ("Back");
        _backButton.addListener(this);
        addAndMakeVisible(_backButton);
        
        _playButton.setColour (TextButton::buttonColourId, Colours::lightgreen );
        _playButton.setColour (TextButton::textColourOnId, Colours::black);
        _playButton.setButtonText ("Play");
        _playButton.addListener(this);
        addAndMakeVisible(_playButton);
        
        _metronomeButton.setColour (TextButton::buttonColourId, Colours::lightgreen );
        _metronomeButton.setColour (TextButton::textColourOnId, Colours::black);
        _metronomeButton.setButtonText ("Metro");
        _metronomeButton.addListener(this);
        addAndMakeVisible(_metronomeButton);
        
        setAudioChannels(2, 2);
        
        _homeScreen.setNumLessons();
        addAndMakeVisible(_homeScreen);
        
        setOpaque(true);
        setSize (800, 400);
    }

    ~MainContentComponent()
    {
        _recordButton.removeListener(this);
        _backButton.removeListener(this);
        _playButton.removeListener(this);
        _metronomeButton.removeListener(this);
        _audioSourcePlayer.setSource (nullptr);
        _deviceManager.removeAudioCallback (&_audioSourcePlayer);
        _deviceManager.closeAudioDevice();
    }

    //=======================================================================
    void setAudioChannels(int numInputChannels, int numOutputChannels)
    {
        String audioError = _deviceManager.initialise (numInputChannels, numOutputChannels, nullptr, true);
        Logger::getCurrentLogger()->writeToLog("Input Latency: "+String(_deviceManager.getCurrentAudioDevice()->getInputLatencyInSamples()));
        Logger::getCurrentLogger()->writeToLog("Output Latency: "+String(_deviceManager.getCurrentAudioDevice()->getOutputLatencyInSamples()));
        jassert (audioError.isEmpty());
        _audioSourcePlayer.setSource (&_audioEngine);
        _deviceManager.addAudioCallback (&_audioSourcePlayer);
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::white);
        g.setColour(Colours::red);
        g.drawText("Overall Score: "+String((int)(_accuracy*100))+"%", getWidth()-200, getHeight()-2*getHeight()/30, 200, 2*getHeight()/30, Justification::centred, true );   // draw some placeholder text

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
        _recordButton.setBounds(posx-offSetX1, posy-offSetY1, 1.5*buttonWidth, buttonHeight);
        _backButton.setBounds(offSetX1, posy-offSetY1, buttonWidth, buttonHeight);
        _playButton.setBounds(posx - 4*offSetX1, posy-offSetY1, buttonWidth, buttonHeight);
        _metronomeButton.setBounds(posx + 3*offSetX1, posy-offSetY1, buttonWidth, buttonHeight);
        _pianoRoll.setBounds(offSetX1, offSetY1, widthPianoRoll, posy-offSetY2);
        _pitchContour.setBounds(2*offSetX1, offSetY1, posx*2-4*offSetX1, posy-offSetY2);
        _homeScreen.setBounds(getLocalBounds());
    }
    
    void setHomeScreenStatus(bool status)
    {
        
    }

private:
    TextButton _recordButton;
    TextButton _backButton;
    TextButton _playButton;
    TextButton _metronomeButton;
    ACFPitchTracker _pitchTracker;
    PitchContour _pitchContour;
    PianoRoll _pianoRoll;
    AudioEngine _audioEngine;
    HomeScreen _homeScreen;
    LessonManager _lessonManager;
    MidiKeyboardState _keyboardState;
    AudioDeviceManager _deviceManager;
    AudioSourcePlayer _audioSourcePlayer;
    VocalEvaluation _vocalEvaluation;
    const int _windowSize;
    float _accuracy;
    
    
    void startRecording()
    {
        _audioEngine.playheadReset();
        _recordButton.setColour (TextButton::buttonColourId, Colour (0xffff5c5c));
        _recordButton.setButtonText ("Stop");
    }
    
    void stopRecording()
    {
        _recordButton.setColour (TextButton::buttonColourId, Colours::lightgreen);
        _recordButton.setButtonText ("Record");
    }
    
    void startPlayback()
    {
        _audioEngine.playheadReset();
        _playButton.setColour(TextButton::buttonColourId, Colour(0xffff5c5c));
        _playButton.setButtonText ("Stop");
    }
    
    void stopPlayback()
    {
        _playButton.setColour(TextButton::buttonColourId, Colours::lightgreen);
        _playButton.setButtonText ("Play");
    }
    
    void startMetronome()
    {
        _metronomeButton.setColour(TextButton::buttonColourId, Colours::orange);
        _metronomeButton.setButtonText ("Stop");
    }
    
    void stopMetronome()
    {
        _metronomeButton.setColour(TextButton::buttonColourId, Colours::lightgreen);
        _metronomeButton.setButtonText ("Metro");
    }
    
    void disablePlayButton()
    {
        _playButton.setColour(TextButton::buttonColourId, Colours::lightgrey);
        _playButton.setEnabled(false);
    }
    
    void enablePlayButton()
    {
        _playButton.setColour(TextButton::buttonColourId, Colours::lightgreen);
        _playButton.setEnabled(true);
    }
    
    void disableRecordButton()
    {
        _recordButton.setColour(TextButton::buttonColourId, Colours::lightgrey);
        _recordButton.setEnabled(false);
    }
    
    void enableRecordButton()
    {
        _recordButton.setColour(TextButton::buttonColourId, Colours::lightgreen);
        _recordButton.setEnabled(true);
    }
    
    void disableBackButton()
    {
        _backButton.setColour(TextButton::buttonColourId, Colours::lightgrey);
        _backButton.setEnabled(false);
    }
    
    void enableBackButton()
    {
        _backButton.setColour(TextButton::buttonColourId, Colours::lightgreen);
        _backButton.setEnabled(true);
    }
    
    void buttonClicked (Button* button) override
    {
        if (button == &_recordButton)
        {
            if (_audioEngine.getRecordingStatus())
            {
                stopRecording();
                enablePlayButton();
                enableBackButton();
                _accuracy = _vocalEvaluation.overallPerformanceMeasure(_audioEngine.getNumRecordingBuffers());
                repaint();
                Logger::getCurrentLogger()->writeToLog("The overall performance accuracy is: "+String(_accuracy*100)+"%");
                _audioEngine.setRecordingStatus(false);
            }
            else
            {
                startRecording();
                disablePlayButton();
                disableBackButton();
                _audioEngine.setRecordingStatus(true);
            }
        }
        if (button == &_backButton)
        {
            _homeScreen.setVisible(true);
            _homeScreen.setHomeScreenStatus(true);
            _pitchContour.clear();
            _pianoRoll.clear();
            _audioEngine.clear();
            _audioEngine.playheadReset();
            
        }
        
        if (button == &_metronomeButton)
        {
            if(_audioEngine.getMetronomeStatus())
            {
                stopMetronome();
                _audioEngine.setMetronomeStatus(false);
            }
            else
            {
                startMetronome();
                _audioEngine.setMetronomeStatus(true);
            }
        }
        
        if (button == &_playButton)
        {
            if(_audioEngine.getPlaybackStatus())
            {
                stopPlayback();
                enableRecordButton();
                enableBackButton();
                _audioEngine.setPlaybackStatus(false);
            }
            else
            {
                startPlayback();
                disableRecordButton();
                disableBackButton();
                _audioEngine.setPlaybackStatus(true);
            }
        }
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
