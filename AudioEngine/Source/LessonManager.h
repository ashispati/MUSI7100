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
#include "PitchContour.h"
#include <vector>
#include <stdlib.h>

class LessonManager : public MidiFile
{
public:
    LessonManager(AudioDeviceManager& deviceManager, PitchContour& pitchContour):
    _deviceManager(deviceManager),
    _pitchContour(pitchContour),
    _lessonSelected(0),
    _numMidiEvents(0),
    _nextEventID(0),
    _samplePos(0),
    _sampleOfNextMidi(0),
    _tempo(0),
    _hopSize(512),
    _sizeOfRefPitch(0),
    _refPitchCounter(0)
    {
        _file = File("../../../../Midi Files/");
        FileInputStream stream(_file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to find the right directory");
        }
        _numLessons = _file.getNumberOfChildFiles(2,"*.mid");
        if (_numLessons == 0)
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to any Lessons");
        }
        Logger::getCurrentLogger()->writeToLog("Number of Lessons = " + String(_numLessons));
        
    }
    
    ~LessonManager()
    {
    }
    
    void createRefPitchContour()
    {
        // read the current selected lesson and compute the reference pitch Contour
    }
    
    float getTempoOfLesson()
    {
        return _tempo;
    }
    
    int getNumLessons()
    {
        return _numLessons;
    }
    
    void setLesson(int lessonToSelect)
    {
        deselectLesson();
        _lessonSelected = lessonToSelect;
        Logger::getCurrentLogger()->writeToLog("Lesson Selected : " + String(_lessonSelected));
        String fileName = "Lesson "+String(_lessonSelected)+".mid";
        File lessonFile = _file.getChildFile(fileName);
        FileInputStream midiFileStream(lessonFile);
        if (!midiFileStream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream to read Midi File");
        }
        else
        {
            
            _midiFile.readFrom(midiFileStream);
            //Logger::getCurrentLogger()->writeToLog(String(_midiFile.getNumTracks()));
            readMidiFile();
            getReferencePitch(_numMidiEvents);

        }
    }
    
    void handleMidiFile(MidiBuffer& midiBuffer, int numSamples)
    {
        int waitSamples;
        while (_nextEventID < _numMidiEvents && (waitSamples = (int)_sampleOfNextMidi - _samplePos) < numSamples)
        {
            MidiMessageSequence::MidiEventHolder *midiEvent = _midiSequence.getEventPointer(_nextEventID++);
            midiBuffer.addEvent(midiEvent->message, waitSamples);
            _sampleOfNextMidi = (int)(_midiSequence.getEventTime(_nextEventID)*_sampleRate);
        }
        _samplePos = _samplePos + numSamples;
    }
    
    void deselectLesson()
    {
        _numMidiEvents = 0;
        _nextEventID = 0;
        _samplePos = 0;
        _sampleOfNextMidi = 0;
    }
    
    void resetLesson()
    {
        String fileName = "Lesson "+String(_lessonSelected)+".mid";
        File lessonFile = _file.getChildFile(fileName);
        FileInputStream midiFileStream(lessonFile);
        if (!midiFileStream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream to read Midi File");
        }
        else
        {
            _midiFile.readFrom(midiFileStream);
            readMidiFile();
            getReferencePitch(_numMidiEvents);
        }
    }
    
    float getNextRefPitch(int index)
    {
        if (index < _sizeOfRefPitch)
        {
            return _refPitch[index];
        }
        else
        {
            return 0;
        }
    }
    
private:
    AudioDeviceManager& _deviceManager;
    MidiFile _midiFile;
    MidiMessageSequence _midiSequence;
    PitchContour& _pitchContour;
    vector<float> _refPitch;
    File _file;
    int _numLessons;
    int _lessonSelected;
    int _numMidiEvents;
    int _nextEventID;
    int _samplePos, _sampleOfNextMidi;
    double _sampleRate;
    float _tempo;
    int _hopSize;
    int _sizeOfRefPitch;
    int _refPitchCounter;
    
    void getReferencePitch(int numMidiEvents)
    {
        double startTime = _midiSequence.getStartTime();
        double endTime = _midiSequence.getEndTime();
        double duration = endTime - startTime;
        double timeResolution;
        if (_hopSize == 0 || _sampleRate == 0)
        {
            Logger::getCurrentLogger()->writeToLog("Error in Sample Rate of Hop Size Specification");
        }
        else
        {
            timeResolution = _hopSize/_sampleRate;
        }
        _sizeOfRefPitch = (int)duration/timeResolution;
        Logger::getCurrentLogger()->writeToLog("Time Resolution"+String(timeResolution));
        for (int i = 0; i < _sizeOfRefPitch; i++)
        {
            _refPitch.push_back(0);
        }
        Logger::getCurrentLogger()->writeToLog("File Start Time: "+String(startTime)+" File End Time: "+String(endTime)+" Array Length: "+String(_sizeOfRefPitch));
        for (int i = 0; i < numMidiEvents; i++)
        {
            MidiMessageSequence::MidiEventHolder *midiEvent = _midiSequence.getEventPointer(i);
            if (midiEvent->message.isNoteOnOrOff())
            {
                if (midiEvent->message.isNoteOn())
                {
                    int note = midiEvent->message.getNoteNumber();
                    double noteStartTime = midiEvent->message.getTimeStamp();
                    int noteOff = _midiSequence.getIndexOfMatchingKeyUp(i);
                    MidiMessageSequence::MidiEventHolder *midiEvent2 = _midiSequence.getEventPointer(noteOff);
                    double noteStopTime = midiEvent2->message.getTimeStamp();
                    int noteStartIndex = (int)(noteStartTime/timeResolution);
                    int noteStopIndex = (int)(noteStopTime/timeResolution);
                    for(int j = noteStartIndex; j < noteStopIndex; j++)
                    {
                        _refPitch[j] = note;
                    }
                    //Logger::getCurrentLogger()->writeToLog("Midi Note: "+String(note)+" Start Time: "+String(noteStartTime)+" Stop Time: "+String(noteStopTime));
                    Logger::getCurrentLogger()->writeToLog("Midi Note: "+String(note)+" Start Index: "+String(noteStartIndex)+" Stop Index: "+String(noteStopIndex));
                }
            }
        }
        _pitchContour.clear();
        _pitchContour.addNextRefPitchInitial(_refPitch);
        
    }
    
    void readMidiFile()
    {
        short timeFormat = _midiFile.getTimeFormat();
        _midiFile.convertTimestampTicksToSeconds();
        _numMidiEvents = -1; // stop playing previous input
        _midiSequence = *_midiFile.getTrack(0); //select track 1
        _samplePos = _sampleOfNextMidi = _nextEventID = 0;
        _numMidiEvents = _midiSequence.getNumEvents();
        for (int i = 0; i < _numMidiEvents; i++)
        {
            MidiMessageSequence::MidiEventHolder *midiEvent = _midiSequence.getEventPointer(i);
            if (midiEvent->message.isTempoMetaEvent())
            {
                _tempo = 60/(midiEvent->message.getTempoMetaEventTickLength(timeFormat)*timeFormat);
                Logger::getCurrentLogger()->writeToLog("Tempo Meta Event Length is: "+String(_tempo)+" BPM");
            }
        }
        AudioIODevice *aio = _deviceManager.getCurrentAudioDevice();
        if (aio)
        {
            _sampleRate = aio->getCurrentSampleRate();
        }
    }
    
    void writeDataToFile()
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("RefPitch.txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        stream.setPosition(stream.getPosition());
        String bufferData;
        for (int i = 0; i < _sizeOfRefPitch; i++)
        {
            bufferData  = String(_refPitch[i]) + '\n';
            stream.writeText(bufferData, false, false);
        }
        
    }
    
};



#endif  // LESSONMANAGER_H_INCLUDED
