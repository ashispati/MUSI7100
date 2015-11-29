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
    LessonManager(AudioDeviceManager& deviceManager):
    _deviceManager(deviceManager),
    lessonSelected(0),
    numMidiEvents(0),
    nextEventID(0),
    samplePos(0),
    sampleOfNextMidi(0)
    {
        file = File("../../../../Midi Files/");
        FileInputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to find the right directory");
        }
        numLessons = file.getNumberOfChildFiles(2,"*.mid");
        if (numLessons == 0)
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to any Lessons");
        }
        Logger::getCurrentLogger()->writeToLog("Number of Lessons = " + String(numLessons));
        
    }
    
    ~LessonManager()
    {
    }
    
    void createRefPitchContour()
    {
        // read the current selected lesson and compute the reference pitch Contour
    }
    
    int getTempoOfLesson()
    {
        //returns the tempo in BPM of the input MIDI file
        //will be used by the metronome class to play the metronome
        int tempo;
        return tempo;
    }
    
    int getNumLessons()
    {
        return numLessons;
    }
    
    void setLesson(int lessonToSelect)
    {
        deselectLesson();
        lessonSelected = lessonToSelect;
        Logger::getCurrentLogger()->writeToLog("Lesson Selected : " + String(lessonSelected));
        String fileName = "Lesson "+String(lessonSelected)+".mid";
        File lessonFile = file.getChildFile(fileName);
        FileInputStream midiFileStream(lessonFile);
        if (!midiFileStream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream to read Midi File");
        }
        else
        {
            midiFile.readFrom(midiFileStream);
            //int numTracks = midiFile.getNumTracks();
            Logger::getCurrentLogger()->writeToLog(String(midiFile.getNumTracks()));
            midiFile.convertTimestampTicksToSeconds();
            numMidiEvents = -1; // stop playing previous input
            midiSequence = *midiFile.getTrack(1); //select track 1
            
            AudioIODevice *aio = _deviceManager.getCurrentAudioDevice();
            if (aio)
            {
                sampleRate = aio->getCurrentSampleRate();
            }
            
            samplePos = sampleOfNextMidi = 0;
            numMidiEvents = midiSequence.getNumEvents();
            Logger::getCurrentLogger()->writeToLog(String(numMidiEvents));
            
        }
    }
    
    void handleMidiFile(MidiBuffer& midiBuffer, int numSamples)
    {
        int waitSamples;
        while (nextEventID < numMidiEvents && (waitSamples = (int)sampleOfNextMidi - samplePos) < numSamples)
        {
            MidiMessageSequence::MidiEventHolder *midiEvent = midiSequence.getEventPointer(nextEventID++);
            midiBuffer.addEvent(midiEvent->message, waitSamples);
            sampleOfNextMidi = (int)(midiSequence.getEventTime(nextEventID)*sampleRate);
        }
        samplePos = samplePos + numSamples;
    }
    
    void deselectLesson()
    {
        numMidiEvents = 0;
        nextEventID = 0;
        samplePos = 0;
        sampleOfNextMidi = 0;
    }
    
    void resetLesson()
    {
        String fileName = "Lesson "+String(lessonSelected)+".mid";
        File lessonFile = file.getChildFile(fileName);
        FileInputStream midiFileStream(lessonFile);
        if (!midiFileStream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream to read Midi File");
        }
        else
        {
            midiFile.readFrom(midiFileStream);
            midiFile.convertTimestampTicksToSeconds();
            numMidiEvents = -1; // stop playing previous input
            midiSequence = *midiFile.getTrack(1); //select track 1
            
            AudioIODevice *aio = _deviceManager.getCurrentAudioDevice();
            if (aio)
            {
                sampleRate = aio->getCurrentSampleRate();
            }
            
            samplePos = sampleOfNextMidi = nextEventID = 0;
            numMidiEvents = midiSequence.getNumEvents();
        }
    }
    
private:
    AudioDeviceManager& _deviceManager;
    MidiFile midiFile;
    MidiMessageSequence midiSequence;
    vector<float> refPitch;
    File file;
    int numLessons;
    int lessonSelected;
    int numMidiEvents;
    int nextEventID;
    int samplePos, sampleOfNextMidi;
    double sampleRate;
    
};



#endif  // LESSONMANAGER_H_INCLUDED
