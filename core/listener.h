#pragma once
#include <vector>
#include <memory>

class Listener
{
public:

    //the amount of parameters to listen to
    void setup(int numBroadcasters);

    //sets the +/- threshold size
    //~0.025f works for most ADC pins
    void setSize(float size);

    //sets inital value for int / bool checking
    //i.e. not a hysteresis check
    void setInitial(int initialValue);

    //check if values have changed beyond their threshold
    //input = values to be checked
    //updateFlags = vector of results: false = unchanged, true = marked as changed
    void query(std::shared_ptr<std::vector<float>> input, std::shared_ptr<std::vector<bool>> updateFlags);

    //check if values have changed beyond their threshold
    //input = values to be checked
    //updateFlags = vector of results: false = unchanged, true = marked as changed
    void queryInt(std::shared_ptr<std::vector<int>> input, std::shared_ptr<std::vector<bool>> updateFlags);
    
    //registers all parameter slots as true
    //i.e. for the hardware values to be transferred over to read values
    void markAsUpdate(std::shared_ptr<std::vector<bool>> updateFlags);

    //registers all parameter slots as false
    //i.e. for the hardware values to be ignored
    void markAsIgnore(std::shared_ptr<std::vector<bool>> updateFlags);

private:

    int numSlots;
    float windowSize;

    std::vector<float> windowUpper;
    std::vector<float> windowLower;
    std::vector<int> lastInt;
};