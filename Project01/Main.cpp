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
	// Create the ground object
	Object ground;
	ground.Init(sceneShaderProgram);
	ground.pushVertex(glm::vec3(200.0f, 0.0f, 200.0f));
	ground.pushVertex(glm::vec3(-200.0f, 0.0f, 200.0f));
	ground.pushVertex(glm::vec3(200.0f, 0.0f, -200.0f));
	ground.pushVertex(glm::vec3(-200.0f, 0.0f, -200.0f));
	ground.pushVertex(glm::vec3(200.0f, 0.0f, -200.0f));
	ground.pushVertex(glm::vec3(-200.0f, 0.0f, 200.0f));
	ground.setPosition(position);


	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	//stbi_set_flip_vertically_on_load(true);


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
	androidBot.setMode(playMode::stop);

	fstream saved(".\\robotAnimation.txt", ios::in);
	androidBot.addAnimation(saved);
	saved.close();

	// setup imgui
	// -----------
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
		//cameaMove();

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
		androidBot.setScale(0.1f);
		androidBot.Draw();

		//ourAnimator.update((float)glfwGetTime());

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
		static float f = 0.1f;
		ImGui::SliderFloat("Scale", &f, 0.1f, 0.3f);
		androidBot.jointMesh["Body"]->joint.translation = glm::vec3(0.0f, 0.0f, 0.0f);
		ImGui::ColorEdit3("color", glm::value_ptr(color));
		const char** jointItems = new const char* [androidBot.joints.size()];
		for (size_t i = 0; i < androidBot.joints.size(); i++) {
			jointItems[i] = androidBot.joints[i].c_str();
		}
		ImGui::Combo("Joint", &selectedJoint, jointItems, androidBot.joints.size());
		jointState& slectedJoint = androidBot.jointMesh[androidBot.joints[selectedJoint]]->joint;

		ImGui::Text("Translation");
		ImGui::SameLine();

		if (ImGui::Button("reset"))
			slectedJoint.translation = glm::vec3(0.0f, 0.0f, 0.0f);
		ImGui::SliderFloat("posx", &slectedJoint.translation.x, -10.0f, 10.0f);
		ImGui::SliderFloat("posy", &slectedJoint.translation.y, -10.0f, 10.0f);
		ImGui::SliderFloat("posz", &slectedJoint.translation.z, -10.0f, 10.0f);
		ImGui::Text("scale");
		ImGui::SameLine();

		if (ImGui::Button("reset"))
			slectedJoint.scale = glm::vec3(1.0f, 1.0f, 1.0f);
		ImGui::SliderFloat("sclx", &slectedJoint.scale.x, 0.2f, 5.0f);
		ImGui::SliderFloat("scly", &slectedJoint.scale.y, 0.2f, 5.0f);
		ImGui::SliderFloat("sclz", &slectedJoint.scale.z, 0.2f, 5.0f);
		ImGui::Text("rotation");
		ImGui::SameLine();

		if (ImGui::Button("reset"))
			slectedJoint.rotation = glm::fquat(1, 0, 0, 0);
		glm::vec3 normal = glm::vec3(0.0f, 0.0f, -1.0f);
		float degree = 0.0f;
		ImGui::SliderAngle("rotx", &normal.x);
		ImGui::SliderAngle("roty", &normal.y);
		ImGui::SliderAngle("rotz", &normal.z);


		// Random Color Button
		if (ImGui::Button("Random Color"))
		{
			color = glm::vec4(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, 1.0f);
		}
		// Show FPS
		ImGui::Text(" frame generated in %.3f ms\n FPS: %.1f", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

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
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
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
		glfwSetWindowTitle(window, ("(" + std::to_string(xpos) + ", " + std::to_string(ypos) + ") angle (" + std::to_string(horizontal_angle) + ", " + std::to_string(vertical_angle) + ")").c_str());
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
	// reset the last position

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
	//glActiveTexture(GL_TEXTURE0);

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