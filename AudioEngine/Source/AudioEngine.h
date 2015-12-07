/*
 ==============================================================================
 
 AudioEngine.h
 Created: 24 Nov 2015 7:21:42pm
 Author:  Som
 
 ==============================================================================
 */

#ifndef AUDIOENGINE_H_INCLUDED
#define AUDIOENGINE_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioSampleRingFrame.h"
#include "ACFPitchTracker.h"
#include "PitchTracker.h"
#include "PitchContour.h"
#include "PianoRoll.h"
#include "PlaybackSynth.h"
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
using namespace std;


class AudioEngine: public AudioSource
{
public:
    AudioEngine(AudioDeviceManager& deviceManager, AudioSourcePlayer& audioSourcePlayer, ACFPitchTracker& pitchTracker, MidiKeyboardState& keyState, PitchContour& contour, PianoRoll& roll, LessonManager& lessonManager) :
            _deviceManager(deviceManager),
            _audioSourcePlayer(audioSourcePlayer),
            _windowSize(1024),
            _recordingStatus(false),
            _playbackStatus(false),
            _metronomeStatus(false),
            _pitchTracker(pitchTracker),
            _pitchContour(contour),
            _pianoRoll(roll),
            _lessonManager(lessonManager),
            _keyboardState(keyState),
            _tickMarker(0), _tickCounter(0), _cumulativeSamples(0)
    {
        _window = new AudioSampleRingFrame(_windowSize);
        _pitchTracker.setWindowSize(_windowSize);
        _numRecordingBuffers = _numPlaybackBuffers = 0;
        _channelDataAvg.begin();
        
        for (int i = 0; i < 5; i++)
        {
            _synth.addVoice(new SynthVoice());
        }
        
        _synth.clearSounds();
        _synth.addSound(new SynthSound());
    }
    
    ~AudioEngine()
    {
        delete _window;
        //delete _pitchTracker;
    }
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        Logger::getCurrentLogger()->writeToLog("Preparing to Play Audio...");
        Logger::getCurrentLogger()->writeToLog("Samples per block = " + String(samplesPerBlockExpected));
        Logger::getCurrentLogger()->writeToLog("Sample Rate = " + String(sampleRate));
        _sampleRateInputAudio = sampleRate;
        _midiCollector.reset(sampleRate);
        _synth.setCurrentPlaybackSampleRate(sampleRate);
        _pitchTracker.setSampleRate(_sampleRateInputAudio);
        Logger::getCurrentLogger()->writeToLog("Sample Rate Input to Pitch Tracker = "+String(_pitchTracker.getSampleRate()));
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        double start = Time::getMillisecondCounterHiRes();
        float numSamplesPerTick = _sampleRateInputAudio*60.0f/_lessonManager.getTempoOfLesson();
        
        if(_recordingStatus)
        {
            if (bufferToFill.buffer->getNumChannels() > 0)
            {
                int bufferSize = bufferToFill.numSamples;
                int hopSize = _window->getHopSize();
                
                const float* channelData1 = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
                const float* channelData2 = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample); //check for num of channels
                
                for(int i = 0; i < bufferSize; i++)
                {
                    _channelDataAvg.push_back((channelData1[i]+channelData2[i])/2);
                }
                /*
                 // code for testing the buffer and pitch tracker
                 for(int i = 0; i < bufferSize; i++)
                 {
                 float value = 0.5*sin(2*float_Pi*440*(i+bufferSize*numBuffers)/44100);
                 //float value = i + numBuffers*bufferSize;
                 channelDataAvg.push_back(value);
                 }
                 */
                //writeDataToFile(channelDataAvg, bufferSize);
                
                while (_channelDataAvg.size() >= hopSize)
                {
                    _window->addNextBufferToFrame(_channelDataAvg);
                    //window->writeFrameToFile();
                    float midiPitchOfFrame = _pitchTracker.findACFPitchMidi(_window);
                    //writePitchToFile(midiPitchOfFrame);
                    _pitchContour.addNextPitch(midiPitchOfFrame);
                    float refPitchOfBlock = _lessonManager.getNextRefPitch(_numRecordingBuffers+_pitchContour.getPitchesToPlot());
                    _pitchContour.addNextRefPitch(refPitchOfBlock);
                    _pianoRoll.setCurrentQuantizedPitch(_pitchTracker.quantizeMidiPitch(midiPitchOfFrame));
                    _channelDataAvg.erase(_channelDataAvg.begin(), _channelDataAvg.begin()+hopSize);
                }
                
                bufferToFill.clearActiveBufferRegion();
                
                
                if (_cumulativeSamples + bufferSize > numSamplesPerTick || _cumulativeSamples == 0)
                {
                    if (_cumulativeSamples != 0)
                    {
                        _tickMarker = numSamplesPerTick - _cumulativeSamples;
                        _cumulativeSamples = _cumulativeSamples + bufferSize - numSamplesPerTick;
                    }
                    if(_metronomeStatus)
                    {
                        for(int i = bufferToFill.buffer->getNumChannels(); --i >= 0;)
                        {
                            bufferToFill.buffer->addSample(i, _tickMarker, 1);
                        }
                    }
                }
                
                _cumulativeSamples = _cumulativeSamples + bufferSize;
                
                
                
            }
            else
            {
                Logger::getCurrentLogger()->writeToLog("Error: No Input Audio Channel");
            }
            _numRecordingBuffers = _numRecordingBuffers + 1;
        }
        else
        {
            _cumulativeSamples = 0;
            bufferToFill.clearActiveBufferRegion();
            _numRecordingBuffers = 0;
        }
        
        
        if (_playbackStatus)
        {
            MidiBuffer midiBuffer;
            _lessonManager.handleMidiFile(midiBuffer, bufferToFill.numSamples);
            _midiCollector.removeNextBlockOfMessages(midiBuffer, bufferToFill.numSamples);
            //_keyboardState.processNextMidiBuffer(midiBuffer, 0, bufferToFill.numSamples, false);
            _synth.renderNextBlock(*bufferToFill.buffer, midiBuffer, 0, bufferToFill.numSamples);
            float refPitchOfBlock = _lessonManager.getNextRefPitch(_numPlaybackBuffers+_pitchContour.getPitchesToPlot());
            _pitchContour.addNextRefPitch(refPitchOfBlock);
            _numPlaybackBuffers = _numPlaybackBuffers + 1;
            
        }
        else
        {
            _numPlaybackBuffers = 0;
        }
        
        
        
        double duration = Time::getMillisecondCounterHiRes() - start;
        //cout << duration << endl;
        if (duration > 11.6099773)
        {
            Logger::getCurrentLogger()->writeToLog ("Time Exceeded in Audio Callback");
        }
    }
    
    void releaseResources() override
    {
        Logger::getCurrentLogger()->writeToLog ("Releasing audio resources");
    }
    
    void setRecordingStatus(bool status)
    {
        _recordingStatus = status;
    }
    
    bool getRecordingStatus()
    {
        return _recordingStatus;
    }
    
    void setPlaybackStatus(bool status)
    {
        _playbackStatus = status;
    }
    
    bool getPlaybackStatus()
    {
        return _playbackStatus;
    }
    
    void setMetronomeStatus(bool status)
    {
        _metronomeStatus = status;
    }
    
    bool getMetronomeStatus()
    {
        return _metronomeStatus;
    }
    
    void clear()
    {
        _pitchTracker.clear();
    }
    
    void playheadReset()
    {
        clear();
        _lessonManager.resetLesson();
        _midiCollector.reset(_sampleRateInputAudio);
        _synth.setCurrentPlaybackSampleRate(_sampleRateInputAudio);
        _synth.clearVoices();
        for (int i = 0; i < 5; i++)
        {
            _synth.addVoice(new SynthVoice());
        }
        _synth.clearSounds();
        _synth.addSound(new SynthSound());
        
    }
    
    int getNumRecordingBuffers()
    {
        return _numRecordingBuffers;
    }
    
private:
    AudioDeviceManager& _deviceManager;
    AudioSourcePlayer& _audioSourcePlayer;
    const int _windowSize;
    bool _recordingStatus, _playbackStatus, _metronomeStatus;
    AudioSampleRingFrame* _window;
    ACFPitchTracker& _pitchTracker;
    PitchContour& _pitchContour;
    PianoRoll& _pianoRoll;
    LessonManager& _lessonManager;
    MidiKeyboardState& _keyboardState;
    MidiMessageCollector _midiCollector;
    Synthesiser _synth;
    double _sampleRateInputAudio;
    int _numRecordingBuffers, _numPlaybackBuffers, _tickMarker, _tickCounter, _cumulativeSamples;
    vector<float> _channelDataAvg;
    
    
    void writeToFile(float channelDataAvg[], int hopSize)
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Buffer.txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        stream.setPosition(stream.getPosition());
        String bufferData;
        int i;
        for (i=0; i < hopSize; ++i)
        {
            bufferData  = bufferData + String(channelDataAvg[i]) + " ";
        }
        bufferData = bufferData + '\n';
        stream.writeText(bufferData, false, false);
        //Logger::getCurrentLogger()->writeToLog ("Complete one write operation buffer" + String(i));
    }
    
    void writeDataToFile(vector<float> channelDataAvg, int bufferSize)
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Audio.txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        stream.setPosition(stream.getPosition());
        String bufferData;
        int i;
        for (i = 0; i < bufferSize; i++)
        {
            bufferData  = String(channelDataAvg[i]) + '\n';
            stream.writeText(bufferData, false, false);
        }
        
    }
    
    void writeDataToFileFromPointer(const float* channelData, int bufferSize)
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("AudioPointer.txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        stream.setPosition(stream.getPosition());
        String bufferData;
        int i;
        for (i = 0; i < bufferSize; i++)
        {
            bufferData  = String(channelData[i]) + '\n';
            stream.writeText(bufferData, false, false);
        }
        
    }
    
    void writePitchToFile(float pitchOfFrame)
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Pitch.txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        stream.setPosition(stream.getPosition());
        String pitchToWrite = String(pitchOfFrame) + '\n';
        stream.writeText(pitchToWrite, false, false);
    }
    
    void writeToFile(vector<float> channelDataAvg, int bufferSize)
    {
        const File file(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Buffer.txt"));
        FileOutputStream stream(file);
        if (!stream.openedOk())
        {
            Logger::getCurrentLogger()->writeToLog ("Failed to open stream");
        }
        stream.setPosition(stream.getPosition());
        String bufferData;
        int i;
        for (i=0; i < bufferSize; ++i)
        {
            bufferData  = bufferData + String(channelDataAvg[i]) + " ";
        }
        bufferData = bufferData + '\n';
        stream.writeText(bufferData, false, false);
        //Logger::getCurrentLogger()->writeToLog ("Complete one write operation buffer" + String(i));
    }
    
    
};



#endif  // AUDIOENGINE_H_INCLUDED
