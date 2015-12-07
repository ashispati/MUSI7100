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
    VocalEvaluation(ACFPitchTracker& pitchTracker, LessonManager& lessonManager):
            _pitchTracker(pitchTracker),
            _lessonManager(lessonManager)
    {
       
    }
    
    ~VocalEvaluation()
    {
    }
    
    float overallPerformanceMeasure(int lengthOfArray)
    {
        return evaluatePitchAccuray(lengthOfArray);
    }
    

private:
    ACFPitchTracker& _pitchTracker;
    LessonManager& _lessonManager;
    
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
    
    float evaluatePitchAccuray(int lengthOfArray)
    {
        if (lengthOfArray == 0)
        {
            Logger::getCurrentLogger()->writeToLog("Not enough length of recording to evaluate");
            return 0.0f;
        }
        
        else if (lengthOfArray > _lessonManager.getSizeOfRefPitch())
        {
            int numNotesEvaluated = 0;
            int numCorrectNotes = 0;
            for (int i = 0; i < _lessonManager.getSizeOfRefPitch(); i++)
            {
                if ((int)_lessonManager.getPitchAtIndex(i) != 0)
                {
                    float sungPitch = _pitchTracker.getPitchAtIndex(i);
                    float refPitch = _lessonManager.getPitchAtIndex(i);
                    if ( abs(sungPitch-refPitch)<0.5)
                    {
                        numCorrectNotes++;
                    }
                    numNotesEvaluated++;
                }
                
            }
            if(numNotesEvaluated == 0)
            {
                Logger::getCurrentLogger()->writeToLog("Not enough length of recording to evaluate");
                return 0;
            }
            else
            {
                float accuracy = (float)numCorrectNotes/(float)numNotesEvaluated;
                return accuracy;
            }
        }
        else
        {
            int numNotesEvaluated = 0;
            int numCorrectNotes = 0;
            for (int i = 0; i < lengthOfArray; i++)
            {
                if ((int)_lessonManager.getPitchAtIndex(i) != 0)
                {
                    float sungPitch = _pitchTracker.getPitchAtIndex(i);
                    float refPitch = _lessonManager.getPitchAtIndex(i);
                    if ( abs(sungPitch-refPitch)<0.50)
                    {
                        numCorrectNotes++;
                    }
                    numNotesEvaluated++;
                }

            }
            if(numNotesEvaluated == 0)
            {
                Logger::getCurrentLogger()->writeToLog("Not enough length of recording to evaluate");
                return 0;
            }
            else
            {
                float accuracy = (float)numCorrectNotes/(float)numNotesEvaluated;
                return accuracy;
            }
        }
        
    }

    float evaluateNoteTiming(int lengthOfArray)
    {
        //evaluates the timing accuracy of the notes measures
    }
    
};



#endif  // VOCALEVALUATION_H_INCLUDED
