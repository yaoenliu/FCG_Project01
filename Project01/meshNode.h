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
    glm::vec3 scale, translation;
    glm::fquat rotation;
    glm::mat4 modelMatrix;

    meshNode()
    {
        name = "";
        isJoint = 0;
        modelMatrix = glm::mat4(1.0f);
        translation = glm::vec3(0.0f);
        scale = glm::vec3(1.0f);
        rotation = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f);
        center = glm::vec3(0.0f);
    }

    void Draw(Shader& shader, glm::mat4 parentModel = glm::mat4(1.0f))
    {
        if (isJoint)
        {
            modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(center.x * scale.x, center.y * scale.y, center.z * scale.z)) *
                glm::translate(glm::mat4(1.0f), glm::vec3(translation.x * scale.x, translation.y * scale.y, translation.z * scale.z))*
                glm::mat4_cast(rotation)*
                glm::scale(glm::mat4(1.0f), scale)*
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

    void loadModelState(modelState& state)
    {
        // check is joint map to joint
        if (state.isJoint && isJoint)
        {
            this->translation = state.translation;
            this->scale = state.scale;
            this->rotation = state.rotation;
        }

        if (state.children.size() != children.size())
        {
            std::cout << "modelState problem";
            return;
        }

        for (int i = 0; i < this->children.size(); i++)
        {
            this->children[i]->loadModelState(state.children[i]);
        }
    }

    modelState getModelState()
    {
        modelState state;

        if (isJoint)
        {
            state.isJoint = 1;
            state.translation = this->translation;
            state.scale = this->scale;
            state.rotation = this->rotation;
        }

        state.children.resize(this->children.size());
        for (int i = 0; i < this->children.size(); i++)
            state.children[i] = this->children[i]->getModelState();

        return state;
    }
};