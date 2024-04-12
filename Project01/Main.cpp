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

// Function prototypes here
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// camera and color variables
glm::mat4 proj;
glm::mat4 view;
glm::vec4 color;
float angle = 90.0f;

// global shader info
ShaderInfo shaders[] = {
	{GL_VERTEX_SHADER, "vertexShader.glsl"},
	{GL_FRAGMENT_SHADER, "fragmentShader.glsl"},
	{GL_NONE, NULL}
};

int main()
{

	// Initialize GLFW
	if (!glfwInit())
		return -1;

	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
	// set up the camera
	proj = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);
	view = glm::lookAt(glm::vec3(0.0f, 2.0f, 3.0f), // camera position
		glm::vec3(0.0f, 0.0f, 0.0f), // target position
		glm::vec3(0.0f, 1.0f, 0.0f)); // up vector

	// seed the random number generator
	srand(time(NULL));

	// set the initial color
	color = glm::vec4(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, 1.0f);

	// detect if the window wasn't created
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Set callback function
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		return -1;

	// Set the shader program
	GLuint shaderProgram = LoadShaders(shaders);
	// Use the shader program
	glUseProgram(shaderProgram);

	// Create an object (triangle)
	Object obj;
	obj.Init(shaderProgram);
	obj.pushVertex(glm::vec3(-0.5f, -0.5f, 0.0f));
	obj.pushVertex(glm::vec3(0.5f, -0.5f, 0.0f));
	obj.pushVertex(glm::vec3(0.0f, 0.5f, 0.0f));

	// setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	// Setup Platform/Renderer bindings
	ImGui_ImplOpenGL3_Init("#version 330");



	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{

		// Render here
		glClear(GL_COLOR_BUFFER_BIT);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, &proj[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
		glUniform4fv(glGetUniformLocation(shaderProgram, "ourColor"), 1, &color[0]);

		obj.Draw();
		// Poll for and process events
		glfwPollEvents();

		// imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Hello, world!");
		ImGui::SetWindowSize(ImVec2(480, 270));
		ImGui::Text("This is some useful text.");
		// a slider for changing the rotation angle
		ImGui::SliderFloat("angle", &angle, -179.0f, 179.0f);
		if (angle == 0.0f) // make sure the triangle is always visible
			angle = 0.1f;
		view = glm::lookAt(glm::vec3(3 * cos(glm::radians(angle)), 2.0f, 3 * sin(glm::radians(angle))), // camera position
			glm::vec3(0.0f, 0.0f, 0.0f), // target position
			glm::vec3(0.0f, 1.0f, 0.0f)); // up vector
		ImGui::ColorEdit3("color", &color[0]);

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
}

// resize callback function
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// resize the display
	glViewport(0, 0, width, height);
	proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
}


