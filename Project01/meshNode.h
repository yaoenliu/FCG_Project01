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

    meshNode()
    {
        name = "";
        isJoint = 0;
        modelMatrix = glm::mat4(1.0f);
        center = glm::vec3(0.0f);
    }

    void Draw(Shader& shader, glm::mat4 parentModel = glm::mat4(1.0f))
    {
        if (isJoint)
        {
            modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(center.x * joint.scale.x, center.y * joint.scale.y, center.z * joint.scale.z)) *
                joint.translationMatrix() *
                joint.rotationMatrix() *
                joint.scaleMatrix() *
                glm::translate(glm::mat4(1.0f), -center);
        }
        else
            modelMatrix = glm::mat4(1.0f);

        shader.setMat4("model", parentModel * modelMatrix);
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
        for (int i = 0; i < children.size(); i++)
            children[i]->Draw(shader, parentModel * modelMatrix);
    }
};