
#include "powerAmp.h"

PowerAmp::PowerAmp()
{

}

std::shared_ptr<PowerAmp::powerData> PowerAmp::setup(double _sampleRate)
{
	sampleRate = _sampleRate;

	pentodes->setup(sampleRate);

	update();

	return params;
}

void PowerAmp::update()
{
	int channel = params->activeChannel;
	pentodes->update(params->sag[channel], params->saturation[channel]);
}

void PowerAmp::processBlock(float* buffer, size_t size)
{
	pentodes->processBlock(buffer, size);
}
