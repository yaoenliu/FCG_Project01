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


// Function prototypes here
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLuint loadCubemap(vector<const GLchar*> faces);

// camera and color variables
glm::mat4 proj; // projection matrix
glm::mat4 view; // view matrix
glm::vec4 color; // color 
glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // model position
glm::vec3 cameraPos = glm::vec3(0.0f, 0.3f, 3.0f); // camera position
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


ImGuiIO io;

// global shader info
ShaderInfo shaders[] = {
	{GL_VERTEX_SHADER, "vertexShader.glsl"},
	{GL_FRAGMENT_SHADER, "fragmentShader.glsl"},
	{GL_NONE, NULL}
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

int main()
{

	// Initialize GLFW
	if (!glfwInit())
		return -1;

	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
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

	// Set the shader program
	sceneShaderProgram = LoadShaders(shaders);

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

	// Cubemap (Skybox)
	vector<const GLchar*> faces;
	faces.push_back("skybox/right.jpg");
	faces.push_back("skybox/left.jpg");
	faces.push_back("skybox/top.jpg");
	faces.push_back("skybox/bottom.jpg");
	faces.push_back("skybox/back.jpg");
	faces.push_back("skybox/front.jpg");
	GLuint cubemapTexture = loadCubemap(faces);

	Shader skyboxShader("SkyboxVertexShader.glsl", "SkyboxFragmentShader.glsl");

	Shader ourShader("RobotVertexShader.glsl", "RobotFragmentShader.glsl");

	// load models
	Model androidBot("robot.obj");
	androidBot.setShader(&ourShader);

	// make animation
	float frameTime = 0.0f;
	androidBot.setMode(playMode::stop);

	fstream saved;
	saved.open(".\\basicAnimation.txt", ios::in);
	androidBot.addAnimation(saved);
	saved.close();
	saved.open(".\\squatAnimation.txt", ios::in);
	androidBot.addAnimation(saved);
	saved.close();
	saved.open(".\\Kame_Hame_Ha.txt", ios::in);
	androidBot.addAnimation(saved);
	saved.close();
	saved.open(".\\walkAnimation.txt", ios::in);
	androidBot.addAnimation(saved);
	saved.close();
	saved.open(".\\puuAnimation.txt", ios::in);
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

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
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

		// binding the uniform matrix view and projection
		ourShader.setMat4("projection", proj);
		ourShader.setMat4("view", view);

		// render the loaded model
		androidBot.setScale(0.005f);
		androidBot.Draw();

		ourShader.setVec3("viewPos", position);
		ourShader.setVec3("light.position", -50, 100, -50);
		ourShader.setVec3("light.ambient", 0.1, 0.6, 0.1);
		ourShader.setVec3("light.diffuse", 5, 5, 5);
		ourShader.setVec3("light.specular", 0, 0, 0);

		// Scene part
		// switch back to the Scene shader
		glUseProgram(sceneShaderProgram);
		// draw the scene
		// Poll for and process events
		glfwPollEvents();

		// imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Hello, world!");

		// select model part
		const char** playModeItems = new const char* [playModeStr.size()];
		for (size_t i = 0; i < playModeStr.size(); i++) {
			playModeItems[i] = playModeStr[i].c_str();
		}
		ImGui::Combo("Play Mode", &androidBot.playMode, playModeItems, playModeStr.size());

		// select animation part
		const char** aniItems = new const char* [androidBot.animations.size()];
		for (size_t i = 0; i < androidBot.animations.size(); i++) {
			aniItems[i] = androidBot.animations[i].name.c_str();
		}
		ImGui::Combo("Animation", &androidBot.curIndex, aniItems, androidBot.animations.size());

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
				androidBot.saveAnimation(saved, 0);
				saved.close();
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