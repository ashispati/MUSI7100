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
    HomeScreen(): isHomeScreenActive(true)
    {
        addAndMakeVisible(groupComponent);
        groupComponent.setColour(GroupComponent::textColourId, Colour (0xffff5c5c));
        groupComponent.setText("Vocal Performance Evaluator");
        groupComponent.setTextLabelPosition(Justification::centredTop);
        
        addAndMakeVisible(comboBox);
        comboBox.setEditableText(false);
        comboBox.setText("Choose Lesson");
        comboBox.setJustificationType(Justification::centred);
        comboBox.setTextWhenNothingSelected(String::empty);
        comboBox.setTextWhenNoChoicesAvailable("no choices");
        for (int i = 1; i <= 3; i++)
        {
            comboBox.addItem("Item", i);
        }
        comboBox.addListener(this);
        
        goButton.setColour (TextButton::buttonColourId, Colours::lightgreen);
        goButton.setColour (TextButton::textColourOnId, Colours::black);
        goButton.setButtonText ("Go");
        goButton.addListener(this);
        addAndMakeVisible(goButton);
    }

    ~HomeScreen()
    {
        comboBox.removeListener(this);
        goButton.removeListener(this);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::white);
    }

    void resized() override
    {
        int width = getWidth();
        int height = getHeight();
        int offsetX = width/40;
        int offsetY = height/20;
        int widthOfComboBox = width/5;
        int heightOfComboBox = height/20;
        comboBox.setBounds(width/2 - widthOfComboBox/2, height/2-offsetY, widthOfComboBox, heightOfComboBox);
        
        
        int widthOfGroupComponent = width/3;
        int heightOfGroupComponent = height/20;
        groupComponent.setBounds(width/2 - widthOfGroupComponent/2, offsetY, widthOfGroupComponent, heightOfGroupComponent );
        
        int buttonWidth = width/20;
        int buttonHeight = height/20;
        int offSetX1 = getWidth()/40;
        int offSetY1 = getHeight()/10;
        int posx = width/2;
        int posy = height/2;
        goButton.setBounds(posx-offSetX1, posy+offSetY1, buttonWidth, buttonHeight);
    }

    void buttonClicked (Button* button) override
    {
        if (button == &goButton)
        {
            if (comboBox.getSelectedId() > 0)
            {
                setVisible(false);
                setHomeScreenStatus(false);
            }
        }
        
    }
    
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override
    {
        if (comboBoxThatHasChanged == &comboBox)
        {
            for (int i = 0; i < comboBox.getNumItems(); i++)
            if (comboBox.getSelectedItemIndex() == i  )
            {
                
                Logger::getCurrentLogger()->writeToLog(String(i));
            }
        }
    }
    
    void setHomeScreenStatus(bool status)
    {
        isHomeScreenActive = status;
    }
    
private:
    GroupComponent groupComponent;
    ComboBox comboBox;
    TextButton goButton;
    bool isHomeScreenActive;

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HomeScreen)
};


#endif  // HOMESCREEN_H_INCLUDED
