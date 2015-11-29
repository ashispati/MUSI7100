/*
  ==============================================================================

    PlaybackSynth.h
    Created: 28 Nov 2015 5:25:00pm
    Author:  Som

  ==============================================================================
*/

#ifndef PLAYBACKSYNTH_H_INCLUDED
#define PLAYBACKSYNTH_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"

struct SynthSound : public SynthesiserSound
{
    SynthSound()
    {
    }
    
    bool appliesToNote(const int /*midiNoteNumber*/)
    {
        return true;
    }
    
    bool appliesToChannel(const int /*midiChannel*/)
    {
        return true;
    }
};

struct SynthVoice : public SynthesiserVoice
{
    SynthVoice(): currentAngle(0), angleDelta(0), level(0), tailOff(0)
    {
    }
    
    bool canPlaySound(SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*>(sound) != nullptr;
    }
    
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0;
        level = velocity*0.15;
        tailOff = 0.0;
        double cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        double cyclesPerSample = cyclesPerSecond/getSampleRate();
        angleDelta = cyclesPerSample*2.0*double_Pi;
    }
    
    void stopNote(float velocity, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            if (tailOff == 0.0)
                tailOff = 1.0;
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }
    
    void pitchWheelMoved(int /*newValue*/) override
    {
    }
    
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override
    {
    }
    
    void renderNextBlock(AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0)
        {
            if(tailOff >0)
            {
                while(--numSamples >=0)
                {
                    const float currentSample = (float)(sin(currentAngle)*level*tailOff);
                    for(int i = outputBuffer.getNumChannels(); --i >= 0;)
                    {
                        outputBuffer.addSample(i,startSample,currentSample);
                    }
                    currentAngle = currentAngle + angleDelta;
                    ++startSample;
                    tailOff = tailOff*0.99;
                    if(tailOff <= 0.005)
                    {
                        clearCurrentNote();
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while(--numSamples >= 0)
                {
                    const float currentSample = (float) (sin (currentAngle) * level);
                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                    {
                        outputBuffer.addSample (i, startSample, currentSample);
                    }
                    currentAngle = currentAngle + angleDelta;
                    startSample = startSample + 1;
                }
            }
        }
    }
    
private:
    double currentAngle, angleDelta, level, tailOff;
};



#endif  // PLAYBACKSYNTH_H_INCLUDED
