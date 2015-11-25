/*
  ==============================================================================

    HomeScreen.h
    Created: 24 Nov 2015 12:02:14am
    Author:  Som

  ==============================================================================
*/

#ifndef HOMESCREEN_H_INCLUDED
#define HOMESCREEN_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "LessonManager.h"
#include <vector>
#include <stdlib.h>

//==============================================================================
// Class to display the various Lessons and Buttons to select them
/*
*/
class HomeScreen    : public Component, public Button::Listener
{
public:
    HomeScreen()
    {
        //initialize buttons 
    }

    ~HomeScreen()
    {
        //remove button listeners
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);
        g.setColour (Colours::red);
        g.setFont (14.0f);
        g.drawText ("Choose a Lesson to Practise", getLocalBounds(), Justification::centredTop, true);
    }

    void resized() override
    {
        //code for resizing the window goes here
        //set bounds for the lesson Buttons
    }

private:
    TextButton lesson1;
    TextButton lesson2;
    TextButton lesson3;
    TextButton lesson4;
    TextButton lesson5;
    LessonManager lesson;
    
    void buttonClicked (Button* button) override
    {
        //Based on the selected button assign the particulat MIDI file to the LessonManager object
        //Enter the main component screen when any of the buttons are clicked
    }
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HomeScreen)
};


#endif  // HOMESCREEN_H_INCLUDED
