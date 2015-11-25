/*
  ==============================================================================

    LessonManager.h
    Created: 24 Nov 2015 12:05:18am
    Author:  Som

  ==============================================================================
*/

#ifndef LESSONMANAGER_H_INCLUDED
#define LESSONMANAGER_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "ACFPitchTracker.h"
#include "PitchTracker.h"
#include <vector>
#include <stdlib.h>

class LessonManager : public MidiFile
{
public:
    LessonManager()
    {
    }
    
    ~LessonManager()
    {
    }
    
    void createRefPitchContour()
    {
        // read the MIDI file and compute the reference pitch Contour
        // add the pitch value to the refPitch member variable
    }
    
    int getTempoOfLesson()
    {
        //returns the tempo in BPM of the input MIDI file
        //will be used by the metronome class to play the metronome
        int tempo;
        return tempo;
    }
    
    
    vector<float> refPitch;

private:
    
    
    
};



#endif  // LESSONMANAGER_H_INCLUDED
