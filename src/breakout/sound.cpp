#include "../platform/toms-lane-platform.hpp"
#include "../tl-sound.hpp"
#include <math.h>

struct ADSREnvelope
{
	int attackDuration;
	double attackAmplitude;
	int decayDuration;
	double sustainAmplitude;
	int releaseDuration;
	int totalDuration;
};

struct Tone
{
	int sampleCounter = 0;
	int toneHz;
	double volume;
	ADSREnvelope envelope;
};

static const int activeToneCount = 8;
static Tone activeTones[activeToneCount];

static const int samplesPerSecond = 44100;
static const int samplesPerCallback = 512; // TODO: work out sound card latency and optimize


static int samplesPerMillisecond = samplesPerSecond / 1000;

double getEnvelopeAmplitude(const Tone& tone)
{
	ADSREnvelope envelope = tone.envelope;
	int lastAttackSample = envelope.attackDuration;
	int lastDecaySample = lastAttackSample + envelope.decayDuration;
	int lastSustainSample = tone.envelope.totalDuration - envelope.releaseDuration;
	int lastSample = tone.envelope.totalDuration;

	if (tone.sampleCounter > lastSample)
	{
		return 0.0;
	}

	if (tone.sampleCounter > lastSustainSample)
	{
		double releaseGradient = envelope.sustainAmplitude / (double)envelope.releaseDuration;
		double releaseTime = (double)(tone.sampleCounter - lastSustainSample);
		return envelope.sustainAmplitude - (releaseGradient * releaseTime);
	}

	if (tone.sampleCounter > lastDecaySample)
	{
		return envelope.sustainAmplitude;
	}

	if (tone.sampleCounter > lastAttackSample)
	{
		double decayGradient = (envelope.attackAmplitude - envelope.sustainAmplitude) / (double)envelope.decayDuration;
		double decayTime = (double)(tone.sampleCounter - lastAttackSample);
		return envelope.attackAmplitude - (decayGradient * decayTime);
	}

	double attackGradient = envelope.attackAmplitude / (double)envelope.attackDuration;
	return attackGradient * (double)tone.sampleCounter;
}

void playTone(int toneHz, int durationInMilliseconds)
{

	for (int i = 0; i < activeToneCount; i += 1)
	{
		Tone& activeTone = activeTones[i];

		if (activeTone.sampleCounter > 0 && activeTone.sampleCounter < activeTone.envelope.totalDuration)
		{
			continue;
		}

		ADSREnvelope& activeEnvelope = activeTone.envelope;
		activeEnvelope.attackDuration = 50 * samplesPerMillisecond;
		activeEnvelope.attackAmplitude = 1.0;
		activeEnvelope.sustainAmplitude = 0.8;
		activeEnvelope.releaseDuration = 200 * samplesPerMillisecond;
		activeEnvelope.decayDuration = 50 * samplesPerMillisecond;

		activeTone.envelope.totalDuration = (samplesPerMillisecond * durationInMilliseconds) + activeEnvelope.attackDuration + + activeEnvelope.decayDuration + activeEnvelope.releaseDuration;
		activeTone.sampleCounter = 0;
		activeTone.toneHz = toneHz;
		activeTone.volume = 0.15;

		return;
	}

}

int UpdateSound(const tl::SoundBuffer& soundBuffer)
{
	int16_t* sampleOutput = soundBuffer.samples;
	const double pi = 3.14159;
	const double max16BitValue = 32767;
	for (int i = 0; i < soundBuffer.sampleCount; i += 1)
	{
		double soundValue = 0; // max value 1.0

		for (int j = 0; j < activeToneCount; j += 1)
		{
			Tone& activeTone = activeTones[j];
			if (activeTone.sampleCounter < activeTone.envelope.totalDuration)
			{
				double toneValue = activeTone.volume * sin(activeTone.sampleCounter * activeTone.toneHz * 2.0 * pi / (double)samplesPerSecond);
				double envelopeFactor = getEnvelopeAmplitude(activeTone);
				soundValue += toneValue * envelopeFactor;

				if (soundValue >= 1.0)
				{
					soundValue = 1.0;
					continue;
				}
				if (soundValue <= -1.0)
				{
					soundValue = -1.0;
					continue;
				}
			}

			activeTone.sampleCounter += 1;
		}
		double soundValueAs16Bit = soundValue * max16BitValue;
		*sampleOutput = (int16_t)soundValueAs16Bit;
		sampleOutput++;
	}

	return 0;
}

int startSound()
{
	return tl::sound_interface_initialize(
		&UpdateSound,
		samplesPerCallback,
		samplesPerSecond,
		1
	);
}

