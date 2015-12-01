//+=============================================================================
// WowWee Light Strike laser tag protocol implementation
// by Corom Thompson based on the implementation
// http://www.meatandnetworking.com/projects/hacking-at-the-light-strike-laser-tag-game/
//+=============================================================================

#include "IRremote.h"
#include "IRremoteInt.h"

//+=============================================================================

// timing is in microseconds
#define LS_HDR_MARK		6700
#define LS_BIT_MARK		845
#define LS_ZERO_SPACE	838
#define LS_ONE_SPACE	3360
#define LS_BITS			32

//+=============================================================================

#if SEND_LIGHTSTRIKE
void IRsend::sendLightStrike(unsigned long data, int nbits)
{
	// Set IR carrier frequency
	enableIROut(38);

	// Header
	mark(LS_HDR_MARK);

	// Data
	for (unsigned long mask = 1UL << (nbits - 1); mask; mask >>= 1) {
		if (data & mask) {
			space(LS_ONE_SPACE);
			mark(LS_BIT_MARK);
		}
		else {
			space(LS_ZERO_SPACE);
			mark(LS_BIT_MARK);
		}
	}

	// Footer
	space(0);  // Always end with the LED off
}
#endif

#if DECODE_LIGHTSTRIKE
bool IRrecv::decodeLightStrike(decode_results *results) {
	long  data = 0;  // We decode in to here; Start with nothing
	int   offset = 1;  // Index in to results; Skip first entry!?

	// Check header "mark"
	if (!MATCH_MARK(results->rawbuf[offset], LS_HDR_MARK))  return false;

	// Check we have enough data
	if (irparams.rawlen < (2 * LS_BITS) + 2) 
		return false;

	// Build the data
	for (int i = 0; i < LS_BITS; i++) {
		// Determine if the bit is a 1 or 0
		offset++;
		if (MATCH_SPACE(results->rawbuf[offset], LS_ONE_SPACE))
			data = (data << 1) | 1;
		else if (MATCH_SPACE(results->rawbuf[offset], LS_ZERO_SPACE))
			data = (data << 1) | 0;
		else
			return false;

		// Check data "mark"
		offset++;
		if (!MATCH_MARK(results->rawbuf[offset], LS_BIT_MARK))
			return false;
	}

	// Success
	results->bits = LS_BITS;
	results->value = data;
	results->decode_type = LIGHTSTRIKE;

	return true;
}
#endif