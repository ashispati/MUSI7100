/*
  ==============================================================================

    MetronomeTimer.h
    Created: 24 Nov 2015 1:35:25am
    Author:  Som

  ==============================================================================
*/

#ifndef METRONOMETIMER_H_INCLUDED
#define METRONOMETIMER_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
#include <stdlib.h>

class MetronomeTimer: public Timer
{

public:
    MetronomeTimer()
    {
    }
    
    ~MetronomeTimer()
    {
    }
    
    void setMetronomeTempo(int tempo)
    {
        metronomeTempo = tempo;
    }
    
    
    
    
private:
    int metronomeTempo;
    
    void setMetronomeTimer()
    {
        //sets the Timer Hz
    }
    
    void timerCallback() override
    {
        //implement timer callback here
    }
    
};



#endif  // METRONOMETIMER_H_INCLUDED
