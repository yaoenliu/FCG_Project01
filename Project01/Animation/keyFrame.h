#pragma once

#include "modelState.h"

class keyFrame
{
public:

    modelState state;
    float time;

    keyFrame(modelState state, float time) :state(state), time(time) { }
};