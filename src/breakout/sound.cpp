#include "../platform/toms-lane-platform.hpp"
#include "../win32/win32-sound.hpp"
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


static Tone activeTones[2];

static const int samplesPerSecond = 44100;
static const int samplesPerCallback = 512; // TODO: work out sound card latency and optimize
static double pi = 3.14159;
static double max16BitValue = 32767;

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
	Tone& activeTone = activeTones[0];

	if (activeTone.sampleCounter > 0 && activeTone.sampleCounter < activeTone.envelope.totalDuration)
	{
		return;
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
}

int UpdateSound(const tl::SoundBuffer& soundBuffer)
{
	int16_t* sampleOutput = soundBuffer.samples;
	Tone& activeTone = activeTones[0];

	for (int i = 0; i < soundBuffer.sampleCount; i += 1)
	{
		double soundValueAs16Bit = 0;
		if (activeTone.sampleCounter < activeTone.envelope.totalDuration)
		{
			double soundValue = activeTone.volume * sin(activeTone.sampleCounter * activeTone.toneHz * 2.0 * pi / (double)samplesPerSecond);
			double envelopeFactor = getEnvelopeAmplitude(activeTone);
			soundValueAs16Bit = max16BitValue * soundValue * envelopeFactor;
		}

		*sampleOutput = (int16_t)soundValueAs16Bit;
		sampleOutput++;
		activeTone.sampleCounter += 1;
	}

	return 0;
}

int startSound()
{
	return tl::win32_sound_interface_initialize(
		0,
		&UpdateSound,
		samplesPerCallback,
		samplesPerSecond,
		1
	);
}

