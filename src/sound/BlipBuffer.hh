// $Id:$

// Heavily based on:
//
// Band-limited sound synthesis and buffering
// Blip_Buffer 0.4.0
// http://www.slack.net/~ant/

#ifndef BLIPBUFFER_HH
#define BLIPBUFFER_HH

#include "FixedPoint.hh"

namespace openmsx {

class BlipBuffer
{
public:
	// Number bits in phase offset. Fewer than 6 bits (64 phase offsets) results in
	// noticeable broadband noise when synthesizing high frequency square waves.
	static const int BLIP_PHASE_BITS = 6;
	typedef FixedPoint<BLIP_PHASE_BITS> TimeIndex;

	BlipBuffer();

	// Update amplitude of waveform at given time. Time is in output sample
	// units and since the last time readSamples() was called.
	void update(TimeIndex time, int amplitude);

	// Read the given amount of samples into destination buffer.
	bool readSamples(int* dest, unsigned samples, unsigned pitch = 1);

private:
	static const unsigned BUFFER_SIZE = 1 << 14;
	static const unsigned BUFFER_MASK = BUFFER_SIZE - 1;
	int buffer[BUFFER_SIZE];
	unsigned offset;
	int accum;
	int lastAmp;
	int availSamp;
};

} // namespace openmsx

#endif
