#pragma once

#include <glad/glad.h> 

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>

#include <iostream>

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
    glm::mat4 preModel;
    meshNode();
    void Draw(Shader& shader, glm::mat4 parentModel = glm::mat4(1.0f));
};