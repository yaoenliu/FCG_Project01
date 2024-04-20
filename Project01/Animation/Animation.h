#pragma once
#include <iostream>
#include "keyFrame.h"
class Animation
{
private:
    modelState interpolate(modelState& start, modelState& end, float progression)
    {
        modelState outPutState;

        for (auto& [name, startJoint] : start.jointMap)
        {
            jointState &endJoint =  end.jointMap[name];

            outPutState.jointMap[name] = jointState(glm::mix(startJoint.translation, endJoint.translation, progression),
                glm::degrees(glm::eulerAngles(glm::slerp(glm::quat(glm::radians(startJoint.rotation)), glm::quat(glm::radians(endJoint.rotation)), progression))),
                glm::mix(startJoint.scale, endJoint.scale, progression));
        }
        return outPutState;
    }

public:
    std::vector<keyFrame> keyFrames;
    float duration;

    void reset()
    {
        keyFrames.clear();
        duration = 0.0f;
    }

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

    void endWithLastFrame()
    {
        duration = keyFrames.back().time;
    }
};