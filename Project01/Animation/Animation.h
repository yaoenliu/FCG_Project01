#pragma once
#include <iostream>
#include "keyFrame.h"
class Animation
{
private:
    modelState interpolate(modelState& start, modelState& end, float progression)
    {
        modelState state;
        if (start.isJoint && end.isJoint)
        {
            state.isJoint = 1;
            state.translation = glm::mix(start.translation, end.translation, progression);
            state.rotation = glm::slerp(start.rotation, end.rotation, progression);
            state.scale = glm::mix(start.scale, end.scale, progression);
        }

        if (start.children.size() != end.children.size() || start.isJoint xor end.isJoint)
        {
            std::cout << "modelState problem";
            return state;
        }

        for (int i = 0; i < start.children.size(); i++)
        {
            state.children.push_back(interpolate(start.children[i], end.children[i], progression));
        }
        return state;
    }

public:
    std::vector<keyFrame> keyFrames;
    float duration;

    // set the duration of the animation
    Animation(float duration) : duration(duration)
    {

    }

    modelState update(float aniTime)
    {
        keyFrame startFrame = keyFrames.back();
        for (auto& frame : keyFrames) {
            if (frame.time > aniTime) {
                keyFrame endFrame = frame;

                float progression = (aniTime - startFrame.time) / (endFrame.time - startFrame.time);

                return interpolate(startFrame.state, endFrame.state, progression);
            }
            startFrame = frame;
        }
        return modelState();
    }
};