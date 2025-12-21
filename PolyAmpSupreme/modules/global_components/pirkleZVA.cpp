#include "pirkleZVA.h"

ZVAFilter::ZVAFilter() {}		/* C-TOR */
ZVAFilter::~ZVAFilter() {}		/* D-TOR */

/** reset members to initialized state */
bool ZVAFilter::setup(double _sampleRate, float frequency)
{
	sampleRate = _sampleRate;
	fc = frequency;
	integrator_z[0] = 0.0;
	integrator_z[1] = 0.0;

	return true;
}

/** process input x(n) through the VA filter to produce return value y(n) */
/**
\param xn input
\return the processed sample
*/
float ZVAFilter::processAudioSample(float xn)
{
	// --- create vn node
	float vn = (xn - integrator_z[0]) * alpha;

	// --- form LP output
	float lpf = ((xn - integrator_z[0]) * alpha) + integrator_z[0];

	// --- update memory
	integrator_z[0] = vn + lpf;

	return lpf;
}

/** recalculate the filter coefficients*/
void ZVAFilter::calculateFilterCoeffs()
{
	// --- normal Zavalishin SVF calculations here
	//     prewarp the cutoff- these are bilinear-transform filters
	float wd = (PI * 2.0f) * fc;
	float T = 1.0 / sampleRate;
	float wa = (2.0 / T) * tan(wd * T / 2.0);
	float g = wa * T / 2.0;
	
	// --- calculate alpha
	alpha = g / (1.0 + g);
}