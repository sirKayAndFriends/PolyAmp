#include "listener.h"

void Listener::setup(int _numSlots)
{
    numSlots = _numSlots;

    for (int i = 0; i < numSlots; i++)
    {
        windowUpper.push_back(0.025f);
        windowLower.push_back(0.0f);
    }
}

void Listener::setSize(float size)
{
    windowSize = size;
}

void Listener::setInitial(int initialValue)
{
    for (int i = 0; i < numSlots; i++)
    {
        lastInt[i] = initialValue;
    }
}

void Listener::markAsUpdate(std::shared_ptr<std::vector<bool>> updateFlags)
{
    for(int i = 0; i < numSlots; i++)
    {
        updateFlags->at(i) = true;
    }
}

void Listener::markAsIgnore(std::shared_ptr<std::vector<bool>> updateFlags)
{
    for(int i = 0; i < numSlots; i++)
    {
        updateFlags->at(i) = false;
    }
}

void Listener::query(std::shared_ptr<std::vector<float>> input, std::shared_ptr<std::vector<bool>> updateFlags)
{
    for(int i = 0; i < numSlots; i++)
    {
        if(input->at(i) > windowUpper[i])
        {
            windowUpper[i] = input->at(i);
            windowLower[i] = input->at(i) - windowSize;
            updateFlags->at(i) = true;
        }

        else if (input->at(i) < windowLower[i])
        {
            windowLower[i] = input->at(i);
            windowUpper[i] = input->at(i) + windowSize;
            updateFlags->at(i) = true;
        }

        else
        {
            updateFlags->at(i) = false;
        }
    }   
}

void Listener::queryInt(std::shared_ptr<std::vector<int>> input, std::shared_ptr<std::vector<bool>> updateFlags)
{
    for(int i = 0; i < numSlots; i++)
    {
        if(input->at(i) != lastInt[i])
        {
            updateFlags->at(i) = true;
        }

        else
        {
            updateFlags->at(i) = false;
        }

        lastInt[i] = input->at(i);
    } 
}