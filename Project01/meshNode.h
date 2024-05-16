#pragma once

#include "pch.h"

#include "LoadShaders.h"
#include "mesh.hpp"
#include "shader.hpp"
#include "modelState.h"

class meshNode
{
public:
    std::vector<Mesh> meshes;    // mesh data
    std::vector<meshNode*> children; // link
    std::string name;        // name
    bool isJoint;       // true if it is joint
    glm::vec3 center;   // center data for some action

    // model action (use when this is joint
    jointState joint;
    glm::mat4 modelMatrix;

    meshNode();
    void Draw(Shader& shader, glm::mat4 parentModel = glm::mat4(1.0f));
};