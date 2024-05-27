﻿#ifndef MODEL_H
#define MODEL_H
#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "mesh.hpp"
#include "shader.hpp"
#include "stb_image.h"
#include "meshNode.h"
#include "Animation/animation.h"

using namespace std;

enum playMode
{
	once = 0,
	loop = 1,
	loopAll = 2,
	stop = 3,
	dev = 4
};

vector<string>playModeStr = { "once", "loop", "loopAll", "stop", "dev" };

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
	// model data 
	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	string directory;
	bool gammaCorrection;
	meshNode* rootMesh;
	vector<string> joints;
	unordered_map<string, meshNode*> jointMesh;

	// constructor, expects a filepath to a 3D model.
	Model(string const& path, bool gamma = false);
	void setShader(Shader* shader) { this->shader = shader; }
	void Draw();// draws the model, and thus all its meshes
	void setScale(glm::vec3 scale);// set Scale with different value for each axis
	void setScale(float scale);	// set Scale for all axis

	// animation data
	vector<Animation> animations;
	void setMode(int mode) { playMode = mode; }
	int playMode = stop;
	int animationIndex;

	void addKeyFrame(const int& animationIndex, const float& time);
	void addNewAnimeation(const float& duration);
	void saveAnimation(fstream& fout, const int& animationIndex);
	void loadAnimation(fstream& fin, const int& animationIndex);
	void addAnimation(fstream& fin);

	// model state operation
	void loadModelState(modelState& state);
	modelState getModelState();
	void reposition();
	float playTime;
	int curIndex = 0;
private:
	unordered_map<string, meshNode*> nodeMap;
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const& path);
	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene);
	Material loadMaterialWithoutTextures(aiMaterial* mat);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	Shader* shader;
	// animation data

	float lastUpdate;
	void AnimatorUpdate();
};

// constructor, expects a filepath to a 3D model.
Model::Model(string const& path, bool gamma) : gammaCorrection(gamma)
{
	loadModel(path);
}

void Model::loadModel(string const& path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);

	// make tree
	for (auto& [name, mnode] : nodeMap)
	{
		string temp = "";
		int len = name.find_last_of(' ');

		// is root
		if (len == string::npos)
		{
			rootMesh = mnode;
			len = -1;
			temp = "";
		}
		else
		{
			// let parent get this kid
			temp.assign(name.begin(), name.begin() + len);  // get parent name
			nodeMap[temp]->children.push_back(mnode);
		}

		// get some data if this obj is joint
		temp = "";
		temp.assign(name, len + 1);   // get obj name
		if (temp.length() >= 5 && temp.substr(0, 5) == "joint")
		{
			temp = temp.substr(5);
			joints.push_back(temp);
			jointMesh[temp] = mnode;
			mnode->isJoint = 1;

			// get the point that hightest between lowest point become center (for ball joint)
			vector<Vertex>& meshVertex = mnode->meshes[0].vertices;
			glm::vec3 hightest, lowest = hightest = meshVertex[0].Position;
			for (int i = 1; i < meshVertex.size(); i++)
			{
				if (meshVertex[i].Position.y > hightest.y)
					hightest = meshVertex[i].Position;
				if (meshVertex[i].Position.y < lowest.y)
					lowest = meshVertex[i].Position;
			}

			mnode->center = (hightest + lowest) * 0.5f;
			cout<<temp<<": " << mnode->center.x << " " << mnode->center.y << " " << mnode->center.z << endl;
		}
	}
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode* node, const aiScene* scene)
{
	string name(node->mName.C_Str());

	nodeMap[name] = new meshNode();
	rootMesh = nodeMap[name];
	nodeMap[name]->name = name;
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		nodeMap[name]->meshes.push_back(processMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

}

Material Model::loadMaterialWithoutTextures(aiMaterial* mat) {
	Material result;
	aiColor3D color;
	mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
	result.Ka = glm::vec3(color.r, color.g, color.b);
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	result.Kd = glm::vec3(color.r, color.g, color.b);
	mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
	result.Ks = glm::vec3(color.r, color.g, color.b);
	return result;
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	if (material->GetTextureCount(aiTextureType_DIFFUSE) == 0 && material->GetTextureCount(aiTextureType_SPECULAR) == 0) {
		Material colorMaterial = loadMaterialWithoutTextures(material);
		return Mesh(vertices, indices, colorMaterial);
	}

	// 1. diffuse maps
	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
		}
	}
	return textures;
}

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


// draws the model, and thus all its meshes
void Model::Draw()
{
	AnimatorUpdate();
	rootMesh->Draw(*shader);
}
// set Scale with different value for each axis
void Model::setScale(glm::vec3 scale)
{
	rootMesh->joint.scale = scale;
}
// set Scale for all axis
void Model::setScale(float scale)
{
	rootMesh->joint.scale = glm::vec3(scale);
}


void Model::loadModelState(modelState& state)
{
	for (auto& [name, joint] : state.jointMap)
	{
		jointMesh[name]->joint = joint;
	}
}

modelState Model::getModelState()
{
	modelState outputState;

	for (auto& name : joints)
		outputState.jointMap[name] = jointMesh[name]->joint;

	return outputState;
}

void Model::addKeyFrame(const int& animationIndex, const float& time)
{
	animations[animationIndex].keyFrames.push_back(keyFrame(getModelState(), time));
	sort(animations[animationIndex].keyFrames.begin(), animations[animationIndex].keyFrames.end(),
		[](keyFrame a, keyFrame b)
		{
			return a.time < b.time;
		}
	);
}


void Model::AnimatorUpdate()
{
	float curTime = glfwGetTime();
	if (playMode == stop)
	{
		lastUpdate = curTime;
		return;
	}
	if (animations.empty()) // no animation[0] or no keyFrame[0
	{
		playMode = stop;
		return;
	}
	float deltaTime = curTime - lastUpdate;
	if (deltaTime < 0) // open windows too lDong
	{
		lastUpdate = curTime;
		return;
	}
	if (playMode != dev)
	{
		playTime += deltaTime;
	}	
	Animation& curAnimation = animations[curIndex];
	if (curAnimation.keyFrames.empty())     // no keyFrame[0]
	{
		curIndex++;
	}
	if (playTime > curAnimation.duration)   // next animation
	{
		playTime = 0;
		if (playMode == once)
		{
			playMode = stop;
		}
		if (playMode == loopAll)
		{
			curIndex++;
		}
	}
	if (curIndex >= animations.size())      // out of stack
	{
		playTime = 0;
		if (playMode == loopAll)
		{
			curIndex = 0;
			return;
		}
		else
		{
			curIndex = 0;
			playMode = stop;
			return;
		}
	}
	modelState state = curAnimation.update(playTime);
	loadModelState(state);
	lastUpdate = curTime;
}

void Model::addNewAnimeation(const float& duration)
{
	animations.push_back(Animation(duration));
}

void Model::saveAnimation(fstream& fout, const int& animationIndex)
{
	Animation& animation = animations[animationIndex];
	fout << animation.name << "\n";
	fout << animation.duration << "\n";
	for (const auto& frame : animation.keyFrames)
	{
		fout << frame.time << "\n";

		for (const auto& [name, joint] : frame.state.jointMap)
		{
			fout << name << '\n';
			fout << joint;
		}
		fout << "endFrame\n";
	}
}

void Model::loadAnimation(fstream& fin, const int& animationIndex)
{

	Animation& animation = animations[animationIndex];
	string strIn = "";

	animation.reset();
	std::getline(fin, strIn);
	animation.name = strIn;
	std::getline(fin, strIn);
	animation.duration = std::stof(strIn);
	while (std::getline(fin, strIn))
	{
		animation.keyFrames.push_back(keyFrame(modelState(), std::stof(strIn)));
		modelState& state = animation.keyFrames.back().state;
		while (std::getline(fin, strIn) && strIn != "endFrame")
		{
			fin >> state[strIn];
		}
	}
}

void Model::addAnimation(fstream& fin)
{
	Animation animation(0.0f);
	string strIn = "";

	animation.reset();
	std::getline(fin, strIn);
	animation.name = strIn;
	std::getline(fin, strIn);
	animation.duration = std::stof(strIn);
	while (std::getline(fin, strIn))
	{
		animation.keyFrames.push_back(keyFrame(modelState(), std::stof(strIn)));
		modelState& state = animation.keyFrames.back().state;
		while (std::getline(fin, strIn) && strIn != "endFrame")
		{
			fin >> state[strIn];
		}
	}
	animations.push_back(animation);
}

void Model::reposition()
{
	for (const auto& name : joints)
	{
		jointMesh[name]->joint = jointState();
	}
}

#endif