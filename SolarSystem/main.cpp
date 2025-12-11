#include <glad/glad.h>
#include <iostream> 
#include <GL/glew.h>
#include <glfw3.h>
#include <assimp/camera.h>
#include "additional/Shader.h"
#include "Cube.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include <additional/shader_m.h>
#include <additional/camera.h>
#include <additional/model.h>
#include "additional/stb_image.h"


using namespace std;
using namespace glm;

int scr_width = 1500;
int scr_height = 750;

Camera camera(vec3(0.0f, 0.0f, 4.0f));
float lastX = scr_width / 2.0f;
float lastY = scr_height / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

vector<GLuint> loadTextures(vector<string> paths, GLuint wrapOption = GL_REPEAT, GLuint filterOption = GL_LINEAR) {
	vector<GLuint> textures = {};

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterOption);

	for (string path : paths)
	{
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);


		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		stbi_image_free(data);

		textures.push_back(texture);
	}

	return textures;
}

int main() {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "Solar System", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); 

	Shader allShader("model.vs", "model.fs");
	//add the lighting later 

	glEnable(GL_DEPTH_TEST);

	vector<string> texturePaths = {};
	texturePaths.push_back("./textures/Hyades.jpg");
	texturePaths.push_back("./textures/Sun.jpg");
	texturePaths.push_back("./textures/Moon.jpg");
	texturePaths.push_back("./textures/Earth.jpg");
	texturePaths.push_back("./textures/Mars.jpg");
	vector<GLuint> textures = loadTextures(texturePaths);

	Model earth("./models/Earth.fbx"); 
	Model moon("./models/Moon.obj"); 
	Model sun("./models/Sun.fbx");
	Model Mars("./models/Mars.obj");

	while (!glfwWindowShouldClose(window)) {
		allShader.use();
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		float time = (float)glfwGetTime();

		processInput(window);

		glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4 projection = mat4(1.0f);
		projection = perspective(radians(45.0f), (float)scr_width / (float)scr_height, 0.1f, 100.0f);
		allShader.setMat4("projection", projection);

		mat4 view = mat4(1.0f);
		view = view = camera.GetViewMatrix();
		allShader.setMat4("view", view);

		mat4 trans = mat4(1.0f);
		trans = rotate(trans, time, vec3(1.0f, 0.0f, 1.0f));

		allShader.setVec3("viewPos", camera.Position);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		earth.Draw(allShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
