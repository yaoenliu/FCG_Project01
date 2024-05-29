#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class WaterFrameBuffers
{
private:
	int REFLECTION_WIDTH = 1920;
	int REFLECTION_HEIGHT = 1080;
	int REFRACTION_WIDTH = 1920;
	int REFRACTION_HEIGHT = 1080;
	int DISPLAY_WIDTH = 1920;
	int DISPLAY_HEIGHT = 1080;
	GLuint reflectionFrameBuffer;
	GLuint reflectionTexture;
	GLuint reflectionDepthBuffer;
	GLuint refractionFrameBuffer;
	GLuint refractionTexture;
	GLuint refractionDepthTexture;

public:
	WaterFrameBuffers();
	void cleanUp();
	void bindReflectionFrameBuffer();
	void bindRefractionFrameBuffer();
	void unbindCurrentFrameBuffer();
	GLuint getReflectionTexture();
	GLuint getRefractionTexture();
	GLuint getRefractionDepthTexture();
	void initReflectionFrameBuffer();
	void initRefractionFrameBuffer();
	void bindFrameBuffer(GLuint frameBuffer, int width, int height);
	GLuint createFrameBuffer();
	GLuint createTextureAttachment(int width, int height);
	GLuint createDepthTextureAttachment(int width, int height);
	GLuint createDepthBufferAttachment(int width, int height);
};

