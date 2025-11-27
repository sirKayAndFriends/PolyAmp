#pragma once
#include "math.h"
#define PI 3.14159265358979311600

class ZVAFilter
{
public:
	ZVAFilter();	/* C-TOR */
	~ZVAFilter();		/* D-TOR */

	/** reset members to initialized state */
	virtual bool setup(double _sampleRate, float frequency);

	/** process input x(n) through the VA filter to produce return value y(n) */
	/**
	\param xn input
	\return the processed sample
	*/
	virtual float processAudioSample(float xn);

	/** recalculate the filter coefficients*/
	void calculateFilterCoeffs();

protected:

	float fc = 1000.0;	
	float sampleRate = 44100.0;

	// --- state storage
	float integrator_z[2];

	// --- filter coefficients
	float alpha = 0.0;			///< alpha is (wcT/2)
};
