#ifndef MODEL_H
#define MODEL_H

#include "pch.h"

#include "mesh.hpp"
#include "shader.hpp"
#include "meshNode.h"
#include "Animation/animation.h"

enum playMode
{
	once = 0,
	loop = 1,
	loopAll = 2,
	stop = 3,
	dev = 4
};

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

	Shader* shader;

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
	// animation data

	float lastUpdate;
	void AnimatorUpdate();
};

#endif