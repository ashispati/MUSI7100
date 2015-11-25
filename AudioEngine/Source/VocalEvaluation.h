/*
  ==============================================================================

    VocalEvaluation.h
    Created: 24 Nov 2015 12:01:27am
    Author:  Som

  ==============================================================================
*/

#ifndef VOCALEVALUATION_H_INCLUDED
#define VOCALEVALUATION_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "LessonManager.h"
#include "ACFPitchTracker.h"
#include "PitchTracker.h"
#include <vector>
#include <stdlib.h>
using namespace std;

class VocalEvaluation
{
public:
    VocalEvaluation()
    {
        sungPitch = new ACFPitchTracker();
    }
    
    VocalEvaluation(ACFPitchTracker* pitchTracker, LessonManager* lessonToEvaluate)
    {
        sungPitch = pitchTracker;
        lesson = lessonToEvaluate;
    }
    
    ~VocalEvaluation()
    {
    }
    
    float overallPerformanceMeasure()
    {
        //gives the overall vocal performance to display on screen finally at the end of the performance
    }
    

private:
    ACFPitchTracker* sungPitch;
    LessonManager* lesson;
    
    float evaluatePitchSteadiness()
    {
        //evaluates the Pitch Steadiness measure based on the Note Segments
    }
    
    vector<float> evaluateNoteOnsets()
    {
        //finds the note Onsets of the sungPitch
    }
    
    vector<float> segmentNotes()
    {
        //finds the note Segments of the sungPitch
    }
    
    float evaluatePitchAccuray()
    {
        //evaluates the accuracy of the sungPitch measures against the lesson
    }

    float evaluateNoteTiming()
    {
        //evalutes the accuracy in timing of the notes
    }
    
};



#endif  // VOCALEVALUATION_H_INCLUDED
