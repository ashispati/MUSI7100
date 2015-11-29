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
class HomeScreen    : public Component, public Button::Listener, public ComboBox::Listener
{
public:
    HomeScreen(LessonManager& lessonManager):
        _lessonManager(lessonManager),
        _isHomeScreenActive(true)
    {
        addAndMakeVisible(_groupComponent);
        _groupComponent.setColour(GroupComponent::textColourId, Colour (0xffff5c5c));
        _groupComponent.setText("Vocal Performance Evaluator");
        _groupComponent.setTextLabelPosition(Justification::centredTop);
        
        addAndMakeVisible(_comboBox);
        _comboBox.setEditableText(false);
        _comboBox.setText("Choose Lesson");
        _comboBox.setJustificationType(Justification::centred);
        _comboBox.setTextWhenNothingSelected(String::empty);
        _comboBox.setTextWhenNoChoicesAvailable("no choices");
        _comboBox.addListener(this);
        
        _goButton.setColour (TextButton::buttonColourId, Colours::lightgreen);
        _goButton.setColour (TextButton::textColourOnId, Colours::black);
        _goButton.setButtonText ("Go");
        _goButton.addListener(this);
        addAndMakeVisible(_goButton);
    }

    ~HomeScreen()
    {
        _comboBox.removeListener(this);
        _goButton.removeListener(this);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::white);
    }

    void resized() override
    {
        int width = getWidth();
        int height = getHeight();
        //int offsetX = width/40;
        int offsetY = height/20;
        int widthOfComboBox = width/5;
        int heightOfComboBox = height/20;
        _comboBox.setBounds(width/2 - widthOfComboBox/2, height/2-offsetY, widthOfComboBox, heightOfComboBox);
        
        
        int widthOfGroupComponent = width/3;
        int heightOfGroupComponent = height/20;
        _groupComponent.setBounds(width/2 - widthOfGroupComponent/2, offsetY, widthOfGroupComponent, heightOfGroupComponent );
        
        int buttonWidth = width/20;
        int buttonHeight = height/20;
        int offSetX1 = getWidth()/40;
        int offSetY1 = getHeight()/10;
        int posx = width/2;
        int posy = height/2;
        _goButton.setBounds(posx-offSetX1, posy+offSetY1, buttonWidth, buttonHeight);
    }

    void buttonClicked (Button* button) override
    {
        if (button == &_goButton)
        {
            if (_comboBox.getSelectedId() > 0)
            {
                setVisible(false);
                setHomeScreenStatus(false);
            }
        }
        
    }
    
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override
    {
        if (comboBoxThatHasChanged == &_comboBox)
        {
            for (int i = 0; i < _comboBox.getNumItems(); i++)
            if (_comboBox.getSelectedItemIndex() == i  )
            {
                _lessonManager.setLesson(i+1);
            }
        }
    }
    
    void setHomeScreenStatus(bool status)
    {
        _isHomeScreenActive = status;
    }
    
    void setNumLessons()
    {
        int numLessons = _lessonManager.getNumLessons();
        _comboBox.clear();
        for (int i = 1; i <= numLessons; i++)
        {
            String LessonTitle = "Lesson " + String(i);
            _comboBox.addItem(LessonTitle, i);
        }
    }
    
private:
    GroupComponent _groupComponent;
    ComboBox _comboBox;
    TextButton _goButton;
    LessonManager& _lessonManager;
    bool _isHomeScreenActive;

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HomeScreen)
};


#endif  // HOMESCREEN_H_INCLUDED
