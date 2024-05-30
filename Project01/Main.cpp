#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <LoadShaders.h>
#include "Object.h"
#include <fstream>
#include <string>
#include "shader.hpp"
#include "model.h"
#include "stb_image.h"
#include "Animation\Animator.h"
#include "ParticleEffect.hpp"
#include "waterFrameBuffers.hpp"

// Function prototypes here
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLuint loadCubemap(vector<const GLchar*> faces);
void renderQuad();
void loadParticleEffect(Shader* shader , Model* robot , const int& animationIndex);
void saveParticleEffect(Model* robot,const int& animationIndex);

bool isInversPitch = false;
// camera and color variables
glm::mat4 proj; // projection matrix
glm::mat4 view; // view matrix
glm::vec4 color; // color 
glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // model position
glm::vec3 cameraPos = glm::vec3(0.0f, 0.3f, 3.0f); // camera position
glm::vec3 lightPos = glm::vec3(-2, 4, -1); // light position
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // light color
float speed = 0.1f; // camera speed
float horizontal_angle = 0; // camera horizontal angle
float vertical_angle = glm::degrees(asin(0.1)); // camera vertical angle
float dist = 3.0f; // camera distance to the target

bool keyS[4] = { false, false, false, false }; // W, A, S, D keys status
bool mouseS[2] = { false, false }; // left and right mouse buttons status
void cameaMove(); // camera movement function

GLuint sceneShaderProgram; // scene shader program

int selectedJoint = 0;
int selectedFrame = 0;

bool isMosaic = false;
bool isFloor = false;

float WAVE_SPEED = 0.001f;
float moveFactor = 0.0f;

std::map<float , vector<ParticleEffect*> > particleEffects;


enum Environment
{
	normal , 
	reflection,
	reflectionMap,
	refraction,
	toonShader,
}environment;


ImGuiIO io;


float planeVertices[] = {
	// positions            // normals         // texcoords
	 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
	-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
	-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

	 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
	-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
	 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
};

float skyboxVertices[] = {
	// positions          
-1.0f,  1.0f, -1.0f,
-1.0f, -1.0f, -1.0f,
 1.0f, -1.0f, -1.0f,
 1.0f, -1.0f, -1.0f,
 1.0f,  1.0f, -1.0f,
-1.0f,  1.0f, -1.0f,

-1.0f, -1.0f,  1.0f,
-1.0f, -1.0f, -1.0f,
-1.0f,  1.0f, -1.0f,
-1.0f,  1.0f, -1.0f,
-1.0f,  1.0f,  1.0f,
-1.0f, -1.0f,  1.0f,

 1.0f, -1.0f, -1.0f,
 1.0f, -1.0f,  1.0f,
 1.0f,  1.0f,  1.0f,
 1.0f,  1.0f,  1.0f,
 1.0f,  1.0f, -1.0f,
 1.0f, -1.0f, -1.0f,

-1.0f, -1.0f,  1.0f,
-1.0f,  1.0f,  1.0f,
 1.0f,  1.0f,  1.0f,
 1.0f,  1.0f,  1.0f,
 1.0f, -1.0f,  1.0f,
-1.0f, -1.0f,  1.0f,

-1.0f,  1.0f, -1.0f,
 1.0f,  1.0f, -1.0f,
 1.0f,  1.0f,  1.0f,
 1.0f,  1.0f,  1.0f,
-1.0f,  1.0f,  1.0f,
-1.0f,  1.0f, -1.0f,

-1.0f, -1.0f, -1.0f,
-1.0f, -1.0f,  1.0f,
 1.0f, -1.0f, -1.0f,
 1.0f, -1.0f, -1.0f,
-1.0f, -1.0f,  1.0f,
 1.0f, -1.0f,  1.0f
};

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};
float endTT = -1;

int main()
{

	// Initialize GLFW
	if (!glfwInit())
		return -1;

	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Computer Graphics Project 1", NULL, NULL);
	// set up the camera
	proj = glm::perspective(glm::radians(45.0f), 1920.f / 1080.f, 0.1f, 100.0f);
	view = glm::lookAt(
		cameraPos, // camera position
		glm::vec3(0.0f, 0.4f, 0.0f), // target position
		glm::vec3(0.0f, 1.0f, 0.0f)); // up vector

	// set the initial color
	color = glm::vec4(105.0 / 255, 72.0 / 255, 40.0 / 255, 1.0f);

	// detect if the window wasn't created
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Set callback function
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);

	// Make the window's context current
	glfwMakeContextCurrent(window);
	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		return -1;


	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// Setup skybox VAO
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);


	// screen quad VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	// plane VAO
	GLuint planeVAO , planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	// Cubemap (Skybox)
	vector<const GLchar*> faces;
	faces.push_back("skybox/right.jpg");
	faces.push_back("skybox/left.jpg");
	faces.push_back("skybox/top.jpg");
	faces.push_back("skybox/bottom.jpg");
	faces.push_back("skybox/back.jpg");
	faces.push_back("skybox/front.jpg");
	GLuint cubemapTexture = loadCubemap(faces);

	Shader skyboxShader("shader/SkyboxVertexShader.glsl", "shader/SkyboxFragmentShader.glsl");

	Shader ourShader("shader/RobotVertexShader.glsl", "shader/RobotFragmentShader.glsl");

	Shader screenShader("shader/ScreenFrameBufferVertexShader.glsl", "shader/ScreenFrameBufferFragmentShader.glsl");

	Shader depthShader("shader/ShadowVertexShader.glsl", "shader/ShadowFragmentShader.glsl");

	Shader simpleDepthShader("shader/ShadowMappingDepthVertexShader.glsl", "shader/ShadowMappingDepthFragmentShader.glsl");

	Shader debugDepthQuad("shader/DebugQuadVertexShader.glsl", "shader/DebugQuadFragmentShader.glsl");

	Shader particleShader("shader/ParticleVertexShader.glsl", "shader/ParticleFragmentShader.glsl");

	Shader waterShader("shader/WaterVertexShader.glsl", "shader/WaterFragmentShader.glsl");


	// load models
	Model androidBot("robot/robot.obj");
	androidBot.setShader(&ourShader);

	// make animation
	float frameTime = 0.0f;
	androidBot.setMode(playMode::stop);

	fstream saved;
	saved.open(".\\animationFile\\basicAnimation.txt", ios::in);
	androidBot.addAnimation(saved);
	saved.close();
	saved.open(".\\animationFile\\walkAnimation.txt", ios::in);
	androidBot.addAnimation(saved);
	saved.close();
	saved.open(".\\animationFile\\jumping_jacks_v2.txt", ios::in);
	androidBot.addAnimation(saved);
	saved.close();
	saved.open(".\\animationFile\\squatAnimation.txt", ios::in);
	androidBot.addAnimation(saved);
	saved.close();
	saved.open(".\\animationFile\\Kame_Hame_Ha.txt", ios::in);
	androidBot.addAnimation(saved);
	saved.close();
	saved.open(".\\animationFile\\puuAnimation.txt", ios::in);
	androidBot.addAnimation(saved);
	saved.close();

	// setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	// Setup Platform/Renderer bindings
	ImGui_ImplOpenGL3_Init("#version 330");

	// shader configuration
	// --------------------

	screenShader.use();
	screenShader.setInt("screenTexture", 0);

	// framebuffer configuration
	// -------------------------
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// configure depth map FBO
	// -----------------------
	const unsigned int SHADOW_WIDTH = 10000, SHADOW_HEIGHT = 10000;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	int wwidth, wheight;
	GLuint wdudvTexture;
	int wnrComponents;
	unsigned char* wdudv = stbi_load("texture/wdudv.png", &wwidth, &wheight, &wnrComponents, 0);
	glGenTextures(1, &wdudvTexture);
	glBindTexture(GL_TEXTURE_2D, wdudvTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wwidth, wheight, 0, GL_RGB, GL_UNSIGNED_BYTE, wdudv);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint normalMapTexture;
	unsigned char* normalMap = stbi_load("texture/normalMap.png", &wwidth, &wheight, &wnrComponents, 0);
	glGenTextures(1, &normalMapTexture);
	glBindTexture(GL_TEXTURE_2D, normalMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wwidth, wheight, 0, GL_RGB, GL_UNSIGNED_BYTE, normalMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(normalMap);

	depthShader.use();
	depthShader.setInt("shadowMap", 0);

	debugDepthQuad.use();
	debugDepthQuad.setInt("depthMap", 0);

	particleEffects.clear();

	ParticleEffect newParticle(&particleShader);
	WaterFrameBuffers waterFrameBuffers;

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.f, far_plane = 100.f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f,0.4f,0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);

		// switch to the model shader
		ourShader.use();
		ourShader.setBool("isDepth", true);
		ourShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		// render the loaded model
		androidBot.setScale(0.005f);

		androidBot.Draw();

		// floor part
		simpleDepthShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		simpleDepthShader.setMat4("model", model);
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		waterFrameBuffers.bindReflectionFrameBuffer();
		float distance = 2 * (cameraPos.y - 4.0f);
		cameraPos.y -= distance;
		cameraPos = glm::vec3(dist * cos(glm::radians(-vertical_angle)) * sin(glm::radians(horizontal_angle)), dist * sin(glm::radians(-vertical_angle)), dist * cos(glm::radians(-vertical_angle)) * cos(glm::radians(horizontal_angle)));
		view = glm::lookAt(cameraPos, // camera position
			glm::vec3(0.0f, 0.4f, 0.0f), // target position
			glm::vec3(0.0f, 1.0f, 0.0f)); // up vector
		// reset viewport
		glViewport(0, 0, 1920, 1080);
		// Render here
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDepthMask(GL_FALSE);// Remember to turn depth writing off
		skyboxShader.use();

		skyboxShader.setMat4("projection", proj);
		skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		skyboxShader.setInt("skybox", 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);

		// switch to the model shader
		ourShader.use();
		ourShader.setBool("isDepth", false);
		// binding the uniform matrix view and projection
		ourShader.setMat4("projection", proj);
		ourShader.setMat4("view", view);

		// render the loaded model
		androidBot.setScale(0.005f);


		if (environment == normal)
			ourShader.setInt("mapType", 0);
		if (environment == reflection)
			ourShader.setInt("mapType", 1);
		if (environment == reflectionMap)
			ourShader.setInt("mapType", 2);
		if (environment == refraction)
			ourShader.setInt("mapType", 3);
		if (environment == toonShader)
			ourShader.setInt("mapType", 4);


		if (environment != normal)
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		androidBot.Draw();

		ourShader.setVec3("viewPos", position);
		ourShader.setVec3("light.position", lightPos);
		ourShader.setVec3("light.color", lightColor);

		if (isFloor)
		{
			// floor part
			depthShader.use();
			glm::mat4 model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
			depthShader.setMat4("projection", proj);
			depthShader.setMat4("view", view);
			depthShader.setMat4("model", model);
			depthShader.setVec3("lightPos", lightPos);
			depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			glBindVertexArray(planeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

		};
		static int preAnimationIndex = -1;
		if (androidBot.playMode != stop && androidBot.playMode != dev && preAnimationIndex != androidBot.curIndex)
		{
			loadParticleEffect(&particleShader, &androidBot, androidBot.curIndex);
			preAnimationIndex = androidBot.curIndex;
		}
		if (!particleEffects.empty() && androidBot.playMode != stop)
		{
			for (auto& [startTime, particleEffect] : particleEffects)
			{
				if (androidBot.playTime < startTime)continue;
				for (auto& particle : particleEffect)
				{
					particleShader.use();
					particleShader.setMat4("projection", proj);
					particleShader.setMat4("view", view);
					jointState& joint = androidBot.jointMesh[particle->partName]->joint;
					glm::mat4 parentModel = joint.scaleMatrix() * joint.translationMatrix() * joint.rotationMatrix();
					particle->offset = androidBot.jointMesh[particle->partName]->center;
					particle->parentModel = parentModel;
					particle->currentTime = androidBot.playTime;
					particle->draw();
				}
			}
		}
		waterFrameBuffers.unbindCurrentFrameBuffer();
		cameraPos = glm::vec3(dist * cos(glm::radians(vertical_angle)) * sin(glm::radians(horizontal_angle)), dist * sin(glm::radians(vertical_angle)), dist * cos(glm::radians(vertical_angle)) * cos(glm::radians(horizontal_angle)));
		view = glm::lookAt(cameraPos, // camera position
			glm::vec3(0.0f, 0.4f, 0.0f), // target position
			glm::vec3(0.0f, 1.0f, 0.0f)); // up vector
		cameraPos.y += distance;

		waterFrameBuffers.bindRefractionFrameBuffer();
		// reset viewport
		glViewport(0, 0, 1920, 1080);
		// Render here
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDepthMask(GL_FALSE);// Remember to turn depth writing off
		skyboxShader.use();

		skyboxShader.setMat4("projection", proj);
		skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		skyboxShader.setInt("skybox", 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
		if (isFloor)
		{
			// floor part
			depthShader.use();
			glm::mat4 model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
			depthShader.setMat4("projection", proj);
			depthShader.setMat4("view", view);
			depthShader.setMat4("model", model);
			depthShader.setVec3("lightPos", lightPos);
			depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			glBindVertexArray(planeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

		};

		waterFrameBuffers.unbindCurrentFrameBuffer();
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
		
		// Render here
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDepthMask(GL_FALSE);// Remember to turn depth writing off
		skyboxShader.use();

		skyboxShader.setMat4("projection", proj);
		skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		skyboxShader.setInt("skybox", 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);

		// switch to the model shader
		ourShader.use();
		ourShader.setBool("isDepth", false);
		// binding the uniform matrix view and projection
		ourShader.setMat4("projection", proj);
		ourShader.setMat4("view", view);

		// render the loaded model
		androidBot.setScale(0.005f);


		if (environment == normal)
			ourShader.setInt("mapType", 0);
		if (environment == reflection)
			ourShader.setInt("mapType", 1);
		if (environment == reflectionMap)
			ourShader.setInt("mapType", 2);
		if (environment == refraction)
			ourShader.setInt("mapType", 3);
		if(environment == toonShader)
			ourShader.setInt("mapType", 4);


		if (environment != normal)
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		androidBot.Draw();

		ourShader.setVec3("viewPos", position);
		ourShader.setVec3("light.position", lightPos);
		ourShader.setVec3("light.color", lightColor);

		moveFactor += 0.004;
		moveFactor -= (int)moveFactor;

		waterShader.use();
		waterShader.setMat4("projection", proj);
		waterShader.setMat4("view", view);
		waterShader.setInt("reflectionTexture", 0);
		waterShader.setInt("refractionTexture", 1);
		waterShader.setInt("dudvMap", 2);
		waterShader.setInt("normalMap", 3);
		waterShader.setVec3("lightPosition", lightPos);
		waterShader.setVec3("lightColour", lightColor);
		glm::mat4 waterModel = glm::mat4(1.0f);
		waterModel = glm::scale(waterModel, glm::vec3(2.0,2.0,1.0));
		waterShader.setMat4("model", waterModel);
		waterShader.setVec3("cameraPos", glm::vec3(dist * cos(glm::radians(vertical_angle)) * sin(glm::radians(horizontal_angle)),
			dist * sin(glm::radians(vertical_angle)), dist * cos(glm::radians(vertical_angle)) * cos(glm::radians(horizontal_angle))));
		waterShader.setFloat("moveFactor", moveFactor);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, waterFrameBuffers.getReflectionTexture());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, waterFrameBuffers.getRefractionTexture());

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, wdudvTexture);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalMapTexture);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glActiveTexture(GL_TEXTURE0);


		if (isFloor)
		{
			// floor part
			depthShader.use();
			glm::mat4 model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
			depthShader.setMat4("projection", proj);
			depthShader.setMat4("view", view);
			depthShader.setMat4("model", model);
			depthShader.setVec3("lightPos", lightPos);
			depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			glBindVertexArray(planeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

		};
		if (androidBot.playMode != stop && androidBot.playMode != dev &&preAnimationIndex != androidBot.curIndex)
		{
			loadParticleEffect(&particleShader, &androidBot, androidBot.curIndex);
			preAnimationIndex = androidBot.curIndex;
		}
		if (!particleEffects.empty()&&androidBot.playMode!=stop)
		{
			for (auto& [startTime , particleEffect] : particleEffects)
			{
				if (androidBot.playTime < startTime)continue;
				for (auto& particle : particleEffect)
				{
					particleShader.use();
					particleShader.setMat4("projection", proj);
					particleShader.setMat4("view", view);
					jointState& joint = androidBot.jointMesh[particle->partName]->joint;
					glm::mat4 parentModel = joint.scaleMatrix() * joint.translationMatrix() * joint.rotationMatrix();
					particle->offset = androidBot.jointMesh[particle->partName]->center;
					particle->parentModel = parentModel;	
					particle->currentTime = androidBot.playTime;
					particle->draw();
				}
			}
		}
		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
		// clear all relevant buffers
		glClearColor(1.f, 1.f, 1.f, 1.0f);; // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, 1920, 1080);
		screenShader.use();
		screenShader.setInt("isMosaic", isMosaic);
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//glViewport(0, 480, 800, 600);
		//screenShader.setInt("isMosaic", 0);
		//glBindVertexArray(quadVAO);
		//glBindTexture(GL_TEXTURE_2D, waterFrameBuffers.getReflectionTexture());	// use the color attachment texture as the texture of the quad plane
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		//glViewport(1120, 480, 800, 600);
		//glBindVertexArray(quadVAO);
		//glBindTexture(GL_TEXTURE_2D, waterFrameBuffers.getRefractionTexture());	// use the color attachment texture as the texture of the quad plane
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		// render Depth map to quad for visual debugging
// ---------------------------------------------
	/*	debugDepthQuad.use();
		debugDepthQuad.setFloat("near_plane", near_plane);
		debugDepthQuad.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderQuad();*/

		// Scene part
		// switch back to the Scene shader
		//glUseProgram(sceneShaderProgram);
		// draw the scene
		// Poll for and process events
		glfwPollEvents();

		// imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Animation Control Panel");
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);½u®Ø¼Ò¦¡


		ImGui::SliderFloat("lightX", &lightPos.x, -100.0f, 100.0f);
		ImGui::SliderFloat("lightY", &lightPos.y, -100.0f, 100.0f);
		ImGui::SliderFloat("lightZ", &lightPos.z, -100.0f, 100.0f);
		ImGui::ColorEdit3("lightColor", (float*)&lightColor);
		if (ImGui::Button("Mosaic"))
		{
			isMosaic = !isMosaic;
		}
		if (ImGui::Button("Floor"))
		{
			isFloor = !isFloor;
		}
		
		// select map type
		const char* mapTypeItems[] = { "normal", "reflection", "mxRelfectionAndRefraction", "refraction" , "toonShader"};
		ImGui::Combo("Map Type", (int*)&environment, mapTypeItems, 5);

		// select model part
		const char** playModeItems = new const char* [playModeStr.size()];
		for (size_t i = 0; i < playModeStr.size(); i++) {
			playModeItems[i] = playModeStr[i].c_str();
		}
		ImGui::Combo("Play Mode", &androidBot.playMode, playModeItems, playModeStr.size());
		if (androidBot.playMode == stop)
		{
			ImGui::SameLine();
			if (ImGui::Button("Replay"))
			{
				androidBot.setMode(playMode::once);
			}
		}
		// select animation part
		const char** aniItems = new const char* [androidBot.animations.size()];
		for (size_t i = 0; i < androidBot.animations.size(); i++) {
			aniItems[i] = androidBot.animations[i].name.c_str();
		}
		int preIndex = androidBot.curIndex;
		ImGui::Combo("Animation", &androidBot.curIndex, aniItems, androidBot.animations.size());
		if (androidBot.curIndex != preIndex)
		{
			loadParticleEffect(&particleShader, &androidBot, androidBot.curIndex);
		}
		if (androidBot.playMode == dev)
		{
			// select joint part
			const char** jointItems = new const char* [androidBot.joints.size()];
			for (size_t i = 0; i < androidBot.joints.size(); i++) {
				jointItems[i] = androidBot.joints[i].c_str();
			}
			ImGui::Combo("Joint", &selectedJoint, jointItems, androidBot.joints.size());
			jointState& slectedJoint = androidBot.jointMesh[androidBot.joints[selectedJoint]]->joint;


			// select key frame part
			const char** frameItems = new const char* [androidBot.animations[androidBot.curIndex].keyFrames.size()];
			for (size_t i = 0; i < androidBot.animations[androidBot.curIndex].keyFrames.size(); i++) {
				std::string str = std::to_string(androidBot.animations[androidBot.curIndex].keyFrames[i].time);
				char* cstr = new char[str.length() + 1];
				strcpy_s(cstr, str.length() + 1, str.c_str());
				frameItems[i] = cstr;
			}
			ImGui::Combo("Frame", &selectedFrame, frameItems, androidBot.animations[androidBot.curIndex].keyFrames.size());
			if (androidBot.playMode == dev)
			{
				androidBot.playTime = androidBot.animations[androidBot.curIndex].keyFrames[selectedFrame].time;
			}
			// delete frame button
			if (ImGui::Button("Key Frame Delete"))
			{
				androidBot.animations[androidBot.curIndex].keyFrames.erase(androidBot.animations[androidBot.curIndex].keyFrames.begin() + selectedFrame);
				androidBot.animations[androidBot.curIndex].duration = androidBot.animations[androidBot.curIndex].keyFrames[androidBot.animations[androidBot.curIndex].keyFrames.size() - 1].time;
				selectedFrame = 0;
			}
			// joint control panel
			ImGui::Text("Translation");
			ImGui::SameLine();
			if (ImGui::Button("reset translation"))
				androidBot.animations[androidBot.curIndex].keyFrames[selectedFrame].state.jointMap[androidBot.joints[selectedJoint]].translation = glm::vec3(0.0f);
			ImGui::SliderFloat("posx", &androidBot.animations[androidBot.curIndex].keyFrames[selectedFrame].state.jointMap[androidBot.joints[selectedJoint]].translation.x, -50.0f, 50.0f);
			ImGui::SliderFloat("posy", &androidBot.animations[androidBot.curIndex].keyFrames[selectedFrame].state.jointMap[androidBot.joints[selectedJoint]].translation.y, -50.0f, 50.0f);
			ImGui::SliderFloat("posz", &androidBot.animations[androidBot.curIndex].keyFrames[selectedFrame].state.jointMap[androidBot.joints[selectedJoint]].translation.z, -50.0f, 50.0f);

			ImGui::Text("Scale");
			ImGui::SameLine();
			if (ImGui::Button("reset scale"))
				androidBot.animations[androidBot.curIndex].keyFrames[selectedFrame].state.jointMap[androidBot.joints[selectedJoint]].scale = glm::vec3(1.0f);
			ImGui::SliderFloat("sclx", &androidBot.animations[androidBot.curIndex].keyFrames[selectedFrame].state.jointMap[androidBot.joints[selectedJoint]].scale.x, 0.2f, 5.0f);
			ImGui::SliderFloat("scly", &androidBot.animations[androidBot.curIndex].keyFrames[selectedFrame].state.jointMap[androidBot.joints[selectedJoint]].scale.y, 0.2f, 5.0f);
			ImGui::SliderFloat("sclz", &androidBot.animations[androidBot.curIndex].keyFrames[selectedFrame].state.jointMap[androidBot.joints[selectedJoint]].scale.z, 0.2f, 5.0f);

			ImGui::Text("Rotation");
			ImGui::SameLine();
			if (ImGui::Button("reset rotation"))
				androidBot.animations[androidBot.curIndex].keyFrames[selectedFrame].state.jointMap[androidBot.joints[selectedJoint]].rotation = glm::vec3(0.0f);
			ImGui::SliderFloat("rotx", &androidBot.animations[androidBot.curIndex].keyFrames[selectedFrame].state.jointMap[androidBot.joints[selectedJoint]].rotation.x, -180.0f, 180.0f);
			ImGui::SliderFloat("roty", &androidBot.animations[androidBot.curIndex].keyFrames[selectedFrame].state.jointMap[androidBot.joints[selectedJoint]].rotation.y, -180.0f, 180.0f);
			ImGui::SliderFloat("rotz", &androidBot.animations[androidBot.curIndex].keyFrames[selectedFrame].state.jointMap[androidBot.joints[selectedJoint]].rotation.z, -180.0f, 180.0f);
		}

		ImGui::Text("Animation seek bar");
		ImGui::SliderFloat("time", &androidBot.playTime, 0, androidBot.animations[androidBot.curIndex].duration);
		if (androidBot.playMode == dev)
		{
			int closestFrame = 0;
			float closestTime = 100;
			for (int i = 0; i < androidBot.animations[androidBot.curIndex].keyFrames.size(); i++)
			{
				float temp = abs(androidBot.animations[androidBot.curIndex].keyFrames[i].time - androidBot.playTime);
				if (temp < closestTime)
				{
					closestTime = temp;
					closestFrame = i;
				}
			}
			selectedFrame = closestFrame;
			androidBot.playTime = androidBot.animations[androidBot.curIndex].keyFrames[closestFrame].time;

			// add key frame button
			static bool addKeyFramePopup = false;
			if (ImGui::Button("Key Frame Add"))
			{
				addKeyFramePopup = true;
			}
			if (addKeyFramePopup)
			{
				ImGui::OpenPopup("Add Animation");
				if (ImGui::BeginPopupModal("Add Animation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
					static float frameTime = 0.0f;
					ImGui::InputFloat("frameTime at:", &frameTime);
					if (ImGui::Button("Add"))
					{
						androidBot.addKeyFrame(androidBot.curIndex, frameTime);
						androidBot.animations[androidBot.curIndex].endWithLastFrame();
						addKeyFramePopup = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
			const char** particleEffectItem = new const char*[particleEffects[androidBot.playTime].size()];
			for (int i = 0; i < particleEffects[androidBot.playTime].size(); i++)
			{
				particleEffectItem[i] = particleEffects[androidBot.playTime][i]->partName.c_str();
			}
			static int selectedParticleEffect = 0;
			ImGui::Combo("Particle Effect",&selectedParticleEffect, particleEffectItem, particleEffects[androidBot.playTime].size());
			if (ImGui::Button("Add Particle Effect"))
			{

				particleEffects[androidBot.playTime].push_back(new ParticleEffect(&particleShader));
				particleEffects[androidBot.playTime].back()->partName = androidBot.joints[selectedJoint];
				particleEffects[androidBot.playTime].back()->startTime = androidBot.playTime;
			}
			if (!particleEffects[androidBot.playTime].empty()&&ImGui::Button("Delete Particle Effect"))
			{
				particleEffects[androidBot.playTime].erase(particleEffects[androidBot.playTime].begin() + selectedParticleEffect);
				selectedParticleEffect = 0;
			}
			if (!particleEffects[androidBot.playTime].empty())
			{
				ImGui::Text("ParticleEffect Translation");
				ImGui::SameLine();
				if (ImGui::Button("reset Particle translation"))
					particleEffects[androidBot.playTime][selectedParticleEffect]->translation = glm::vec3(0.0f);
				ImGui::SliderFloat("Particle posx", &particleEffects[androidBot.playTime][selectedParticleEffect]->translation.x, -100.0f, 100.0f);
				ImGui::SliderFloat("Particle posy", &particleEffects[androidBot.playTime][selectedParticleEffect]->translation.y, -100.0f, 100.0f);
				ImGui::SliderFloat("Particle posz", &particleEffects[androidBot.playTime][selectedParticleEffect]->translation.z, -100.0f, 100.0f);
				ImGui::Text("ParticleEffect Scale");
				ImGui::SameLine();
				if (ImGui::Button("reset Particle scale"))
					particleEffects[androidBot.playTime][selectedParticleEffect]->scale = glm::vec3(1.0f);
				ImGui::SliderFloat("Particle sclx", &particleEffects[androidBot.playTime][selectedParticleEffect]->scale.x, 0.2f, 5.0f);
				ImGui::SliderFloat("Particle scly", &particleEffects[androidBot.playTime][selectedParticleEffect]->scale.y, 0.2f, 5.0f);
				ImGui::SliderFloat("Particle sclz", &particleEffects[androidBot.playTime][selectedParticleEffect]->scale.z, 0.2f, 5.0f);
				ImGui::Text("ParticleEffect Rotation");
				ImGui::SameLine();
				if (ImGui::Button("reset Particle rotation"))
					particleEffects[androidBot.playTime][selectedParticleEffect]->rotation = glm::vec3(0.0f);
				ImGui::SliderFloat("Particle rotx", &particleEffects[androidBot.playTime][selectedParticleEffect]->rotation.x, -180.0f, 180.0f);
				ImGui::SliderFloat("Particle roty", &particleEffects[androidBot.playTime][selectedParticleEffect]->rotation.y, -180.0f, 180.0f);
				ImGui::SliderFloat("Particle rotz", &particleEffects[androidBot.playTime][selectedParticleEffect]->rotation.z, -180.0f, 180.0f);
				ImGui::ColorEdit3("Particle color", (float*)&particleEffects[androidBot.playTime][selectedParticleEffect]->color);
				ImGui::SliderFloat("Particle Effect lifeTime", &particleEffects[androidBot.playTime][selectedParticleEffect]->lifeTime, 0.0f, 10.0f);
				ImGui::SliderFloat("Particle radius", &particleEffects[androidBot.playTime][selectedParticleEffect]->radius, 0.0f, 10.0f);
				ImGui::SliderFloat("Particle lifeTime", &particleEffects[androidBot.playTime][selectedParticleEffect]->particleLifeTime, 0.0f, 2.0f);
			}

			// add animation button
			static bool addAnimationPopup = false;
			if (ImGui::Button("Add Animation"))
			{
				addAnimationPopup = true;
			}
			if (addAnimationPopup)
			{
				ImGui::OpenPopup("Add Animation");
				if (ImGui::BeginPopupModal("Add Animation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
					static char aniName[256] = "unnamed";
					static float aniDuration = 0.0f;
					ImGui::InputText("name", aniName, 256);
					ImGui::InputFloat("duration", &aniDuration);
					if (ImGui::Button("Add"))
					{
						androidBot.animations.push_back(Animation(aniName, aniDuration));
						androidBot.addKeyFrame(androidBot.animations.size()-1, 0);
						androidBot.animations[androidBot.animations.size() - 1].endWithLastFrame();
						addAnimationPopup = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
			ImGui::Text("Save and Load");
			static char buffer[256] = ".\\robotAnimation.txt";
			ImGui::InputText("path", buffer, 256);

			if (ImGui::Button("Load animation"))
			{
				saved.open(buffer, ios::in);
				androidBot.loadAnimation(saved, androidBot.curIndex);
				saved.close();
			}
			ImGui::SameLine();
			if (ImGui::Button("Save animation"))
			{
				saved.open(buffer, ios::out);
				androidBot.saveAnimation(saved, androidBot.curIndex);
				saved.close();;
			}
			if (ImGui::Button("Save particle effect"))
			{
				saveParticleEffect(&androidBot, androidBot.curIndex);
			}
			if (ImGui::Button("Load particle effect"))
			{
				loadParticleEffect(&particleShader, &androidBot, androidBot.curIndex);
			}
		}
		// Show FPS
		ImGui::Text(" frame generated in %.3f ms\n FPS: %.1f", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		// quit button
		if (ImGui::Button("Quit"))
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap front and back buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &framebuffer);
	waterFrameBuffers.cleanUp();

	// Terminate GLFW
	glfwTerminate();
	return 0;
}


// key callback function
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static int windowWidth, windowHeight, windowPosX, windowPosY;
	static bool isFullScreen = false;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		if (isFullScreen)
		{
			glfwSetWindowMonitor(window, NULL, windowPosX, windowPosY, windowWidth, windowHeight, GLFW_DONT_CARE);
			isFullScreen = false;
		}
		else
			glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		static int windowWidth, windowHeight, windowPosX, windowPosY;
		if (isFullScreen)
		{
			glfwSetWindowMonitor(window, NULL, windowPosX, windowPosY, windowWidth, windowHeight, GLFW_DONT_CARE);
			isFullScreen = false;
		}
		else
		{
			glfwGetWindowPos(window, &windowPosX, &windowPosY);
			glfwGetWindowSize(window, &windowWidth, &windowHeight);
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
			isFullScreen = true;
		}
	}

	if (key == GLFW_KEY_W)
		keyS[0] = action == GLFW_RELEASE ? false : true;
	if (key == GLFW_KEY_A)
		keyS[1] = action == GLFW_RELEASE ? false : true;
	if (key == GLFW_KEY_S)
		keyS[2] = action == GLFW_RELEASE ? false : true;
	if (key == GLFW_KEY_D)
		keyS[3] = action == GLFW_RELEASE ? false : true;
}

// resize callback function
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	if (width == 0 || height == 0)
		return;
	// resize the display
	glViewport(0, 0, width, height);
	proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
}

void cameaMove()
{
	if (keyS[0])
		cameraPos.z -= 0.1f;
	if (keyS[1])
		cameraPos.x -= 0.1f;
	if (keyS[2])
		cameraPos.z += 0.1f;
	if (keyS[3])
		cameraPos.x += 0.1f;
	view = glm::lookAt(cameraPos, // camera position
		glm::vec3(0.0f, 0.4f, 0.0f), // target position
		glm::vec3(0.0f, 1.0f, 0.0f)); // up vector
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	static double lastX = xpos;
	static double lastY = ypos;

	if (!mouseS[0])
	{
		lastX = xpos;
		lastY = ypos;
		return;
	}
	if (ImGui::GetIO().WantCaptureMouse)
	{
		lastX = xpos;
		lastY = ypos;
		return;
	}
	horizontal_angle += speed * (lastX - xpos);
	if(isInversPitch)
	vertical_angle -= speed * -(lastY - ypos);
	else
		vertical_angle -= speed * (lastY - ypos);
	if (sin(glm::radians(vertical_angle)) < 0.1)vertical_angle = glm::degrees(asin(0.1));
	if (horizontal_angle > 360)horizontal_angle -= 360;
	if (horizontal_angle < 0)horizontal_angle += 360;
	if (vertical_angle > 89)vertical_angle = 89;
	if (vertical_angle < -89)vertical_angle = -89;
	cameraPos = glm::vec3(dist * cos(glm::radians(vertical_angle)) * sin(glm::radians(horizontal_angle)), dist * sin(glm::radians(vertical_angle)), dist * cos(glm::radians(vertical_angle)) * cos(glm::radians(horizontal_angle)));
	view = glm::lookAt(cameraPos, // camera position
		glm::vec3(0.0f, 0.4f, 0.0f), // target position
		glm::vec3(0.0f, 1.0f, 0.0f)); // up vector

	lastX = xpos;
	lastY = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		mouseS[0] = action == GLFW_RELEASE ? false : true;
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
		mouseS[1] = action == GLFW_RELEASE ? false : true;
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;
	dist -= 0.1 * yoffset;
	if (dist < 1.0f)dist = 1.0f;
	if (dist > 10.0f)dist = 10.0f;
	cameraPos = glm::vec3(dist * cos(glm::radians(vertical_angle)) * sin(glm::radians(horizontal_angle)), dist * sin(glm::radians(vertical_angle)), dist * cos(glm::radians(vertical_angle)) * cos(glm::radians(horizontal_angle)));
	view = glm::lookAt(cameraPos, // camera position
		glm::vec3(0.0f, 0.4f, 0.0f), // target position
		glm::vec3(0.0f, 1.0f, 0.0f)); // up vector
}

GLuint loadCubemap(vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		int nrComponents;
		image = stbi_load(faces[i], &width, &height, &nrComponents, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		stbi_image_free(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}


void loadParticleEffect(Shader* shader , Model* robot , const int& animationIndex)
{
	Animation& animation = robot->animations[animationIndex];
	string path = ".\\" + animation.name + "ParticleEffect.txt";
	fstream file;
	std::string strIn = "";
	file.open(path, ios::in);
	particleEffects.clear();
	while (std::getline(file, strIn))
	{
		float time = std::stof(strIn);
		while (std::getline(file, strIn)&&strIn != "endTime")
		{
			string partName = strIn;
			std::getline(file, strIn);
			float lifeTime = std::stof(strIn);
			std::getline(file, strIn);
			float particleLifeTime = std::stof(strIn);
			std::getline(file, strIn);
			int nrParticles = std::stoi(strIn);
			std::getline(file, strIn);
			float angle = std::stof(strIn);
			std::getline(file, strIn);
			float heightIncrement = std::stof(strIn);
			std::getline(file, strIn);
			float radius = std::stof(strIn);
			std::getline(file, strIn);
			std::stringstream ss(strIn);
			glm::vec4 color;
			ss >> color.x >> color.y >> color.z >> color.a;
			std::getline(file, strIn);
			std::stringstream ss1(strIn);
			glm::vec3 translation;
			ss1 >> translation.x >> translation.y >> translation.z;
			std::getline(file, strIn);
			std::stringstream ss2(strIn);
			glm::vec3 rotation;
			ss2 >> rotation.x >> rotation.y >> rotation.z;
			std::getline(file, strIn);
			std::stringstream ss3(strIn);
			glm::vec3 scale;
			ss3 >> scale.x >> scale.y >> scale.z;
			particleEffects[time].push_back(new ParticleEffect(shader));
			particleEffects[time].back()->partName = partName;
			particleEffects[time].back()->startTime = time;
			particleEffects[time].back()->lifeTime = lifeTime;
			particleEffects[time].back()->particleLifeTime = particleLifeTime;
			particleEffects[time].back()->nrParticles = nrParticles;
			particleEffects[time].back()->angle = angle;
			particleEffects[time].back()->heightIncrement = heightIncrement;
			particleEffects[time].back()->radius = radius;
			particleEffects[time].back()->color = color;
			particleEffects[time].back()->translation = translation;
			particleEffects[time].back()->rotation = rotation;
			particleEffects[time].back()->scale = scale;
		}
	}
}

void saveParticleEffect(Model* robot , const int& animationIndex)
{
	Animation& animation = robot->animations[animationIndex];
	string path = ".\\" + animation.name + "ParticleEffect.txt";
	fstream file;
	file.open(path, ios::out);
	for (const auto& [time , effects] : particleEffects)
	{
		file << time << "\n";
		for (const auto& effect : effects)
		{
			file << effect->partName << "\n";
			file << effect->lifeTime << "\n";
			file << effect->particleLifeTime << "\n";
			file <<effect->nrParticles<<"\n";
			file << effect->angle << "\n";
			file <<effect->heightIncrement<<"\n";
			file <<effect->radius<<"\n";
			file << effect->color.x << " " << effect->color.y << " " << effect->color.z <<" " << effect->color.a << "\n";
			file <<effect->translation.x<<" "<<effect->translation.y<<" "<<effect->translation.z<<"\n";
			file << effect->rotation.x << " " << effect->rotation.y << " " << effect->rotation.z << "\n";
			file << effect->scale.x << " " << effect->scale.y << " " << effect->scale.z << "\n";
		}
		file << "endTime\n";
	}
}