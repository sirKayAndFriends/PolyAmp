#pragma once
#include <vector>
#include <memory>

class Output
{
public:

    struct outputData
    {
        int activeChannel = 0;
        float volume = 0.0f;
    };

    std::shared_ptr<outputData> setup();

    void update();

    void processBlock(float* buffer, size_t size);
private:

    std::shared_ptr<outputData> params = std::make_shared<outputData>();

    std::vector<float> channelOutputMax
    {
        0.3f, 0.2f, 0.2f
    };

    std::vector<float> channelOutputGain
    {
        0.0f, 0.0f, 0.0f
    };

    inline float scalePin(float min, float max, float value)
    {
        return value * (max - min) + min;
    }
};