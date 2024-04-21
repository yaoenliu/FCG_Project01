#include "meshNode.h"

void meshNode::Draw(Shader& shader, glm::mat4 parentModel)
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

meshNode::meshNode()
{
    name = "";
    isJoint = 0;
    modelMatrix = glm::mat4(1.0f);
    center = glm::vec3(0.0f);
}
