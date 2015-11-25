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
    MainContentComponent() : windowSize(1024)
    {
        setSize (800, 400);
        addAndMakeVisible(pitchContour);
        addAndMakeVisible(pianoRoll);
        audioEngine.setRefereces(&pitchContour, &pianoRoll);
        recordButton.setColour (TextButton::buttonColourId, Colour (0xffff5c5c));
        recordButton.setColour (TextButton::textColourOnId, Colours::black);
        recordButton.setButtonText ("Start");
        recordButton.addListener(this);
        addAndMakeVisible(recordButton);
    }

    ~MainContentComponent()
    {
        recordButton.removeListener(this);
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
        int buttonHeight = getHeight()/20;
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
    AudioEngine audioEngine;
    PitchContour pitchContour;
    PianoRoll pianoRoll;
    
    
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
            if (audioEngine.getRecordingStatus() == true)
            {
                stopRecording();
                audioEngine.setRecordingStatus(false);
            }
            else
            {
                startRecording();
                audioEngine.setRecordingStatus(true);
            }
        }
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
