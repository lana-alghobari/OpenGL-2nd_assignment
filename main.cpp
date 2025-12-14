//#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "stb_image.h"
#include <array>

#include "Shader.h"
#include "Sphere.h"

glm::vec3 camPos   = glm::vec3(0.0f, 0.0f, 8.0f);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp    = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f, pitch = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;
float fov = 45.0f;
float deltaTime = 0.0f, lastFrame = 0.0f;
float moonOrbitSpeed = 0.5f;
float earthOrbitSpeed = 0.01f;
float currentMoonSpeed = moonOrbitSpeed; 
float currentEarthSpeed = earthOrbitSpeed;
glm::vec3 sunPos = glm::vec3(-1.0f, 0.0f, 0.0f);
glm::vec3 earthPos;
glm::vec3 moonPos;
std::array<glm::vec3, 3> lastPos;
float earthAngle = 0.0f;
float moonAngle = 0.0f;
bool moonInfront = false;   


void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
bool areAlignedOrSmth(glm::vec3 sunPos, glm::vec3 earthPos, glm::vec3 moonPos);

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Sphere Light Test", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glewInit() != GLEW_OK) { std::cout << "GLEW FAILED\n"; return -1; }

    glEnable(GL_DEPTH_TEST);

    Shader lightingShader("../HW-model.fs"); 

    Sphere sun(0.5f, 36, 18 ,"../textures/Sun.jpg");
    Sphere earth(0.3f, 36, 18, "../textures/Earth.jpg");
    Sphere moon(0.15, 36, 18, "../textures/Moon.jpg");


    glDisable(GL_CULL_FACE);

    while(!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f/600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(camPos, camPos + camFront, camUp);

        lightingShader.bind();
        lightingShader.setUniformMat4f("projection", projection);
        lightingShader.setUniformMat4f("view", view);
        lightingShader.setUniformVec3f("viewPos", camPos);

        lightingShader.setPointLight("pointLights[0]",
                                     sunPos,
                                     1.0f, 0.022f, 0.0019f,
                                     {0.2f,0.2f,0.2f},
                                     { 1.0f, 0.8f, 0.5f },
                                     { 1.0f, 0.8f, 0.5f });
        lightingShader.setUniform1f("material.shininess", 50.0f);

        glm::mat4 modelSun = glm::translate(glm::mat4(1.0f), sunPos);
        modelSun = glm::scale(modelSun, glm::vec3(0.5f));
        lightingShader.setUniformMat4f("model", modelSun);
        lightingShader.setUniform1i("isEmissive", true);
        lightingShader.setUniformVec3f("emissiveColor", glm::vec3(1.0f, 0.2f, 0.0f));
        sun.Draw(lightingShader);
        earthAngle += currentEarthSpeed * deltaTime;
        moonAngle += currentMoonSpeed * deltaTime;
        float earthOrbitRadius = 3.0f;
        earthPos = sunPos + glm::vec3(
            earthOrbitRadius * cos(earthAngle),
            0.0f,
            earthOrbitRadius * sin(earthAngle)
        );

        float moonOrbitRadius = 0.5f;
         moonPos = earthPos + glm::vec3(
            moonOrbitRadius * cos(moonAngle),
            0.0f,
            moonOrbitRadius * sin(moonAngle)
        );

        glm::mat4 earthModel = glm::translate(glm::mat4(1.0f), earthPos);
        float selfRotateSpeed = 0.5f;
        earthModel = glm::rotate(earthModel, currentFrame * selfRotateSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
        earthModel = glm::scale(earthModel, glm::vec3(0.3f));


        lightingShader.setUniformMat4f("model", earthModel);
        lightingShader.setUniform1i("isEmissive", false);
        lightingShader.setUniformVec3f("objectColor", glm::vec3(0.2f, 0.4f, 0.8f));
         lightingShader.setUniformVec3f("moonPos", moonPos);
         lightingShader.setUniform1f("moonRadius", 0.15f);


        earth.Draw(lightingShader);


        glm::mat4 moonModel = glm::translate(glm::mat4(1.0f), moonPos);
        moonModel = glm::rotate(moonModel, currentFrame * selfRotateSpeed, glm::vec3(0.7f, 0.7f, 0.7f));
        moonModel = glm::scale(moonModel, glm::vec3(0.15f));

        lightingShader.setUniformMat4f("model", moonModel);
        lightingShader.setUniform1i("isEmissive", false);
        lightingShader.setUniformVec3f("objectColor", glm::vec3(0.7f, 0.7f, 0.7f));
        lightingShader.setUniformVec3f("earthPos", earthPos);
        lightingShader.setUniform1f("earthRadius", 0.3f);
        lightingShader.setUniformVec3f("sunPos", sunPos);

        moon.Draw(lightingShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window){
    float speed = 5.0f * deltaTime;
    if(glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS) camPos += speed * camFront;
    if(glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS) camPos -= speed * camFront;
    if(glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS) camPos -= glm::normalize(glm::cross(camFront, camUp)) * speed;
    if(glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS) camPos += glm::normalize(glm::cross(camFront, camUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        if (!areAlignedOrSmth(sunPos, earthPos, moonPos)) {
            currentEarthSpeed += 0.1f * deltaTime;
            currentMoonSpeed += 0.1f * deltaTime;

        }
        else if(!moonInfront && areAlignedOrSmth(sunPos, earthPos, moonPos)) {
            currentEarthSpeed = 0.0f;
            currentMoonSpeed = 0.0f;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		currentEarthSpeed = earthOrbitSpeed;    
		currentMoonSpeed = moonOrbitSpeed;
        }
    
if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
    if (!areAlignedOrSmth(sunPos, earthPos, moonPos)) {
        currentEarthSpeed += 0.1f * deltaTime;
        currentMoonSpeed += 0.1f * deltaTime;

    }
    else if (moonInfront&&areAlignedOrSmth(sunPos, earthPos, moonPos)) {
        currentEarthSpeed = 0.0f;
        currentMoonSpeed = 0.0f;
    }
}
  //else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
  //    currentEarthSpeed = earthOrbitSpeed;    
  //    currentMoonSpeed = moonOrbitSpeed;
  //    }
}
bool areAlignedOrSmth(glm::vec3 sunPos, glm::vec3 earthPos, glm::vec3 moonPos)
{
    glm::vec3 sunToEarth = sunPos - earthPos;
    glm::vec3 EarthToMoon = moonPos - earthPos;
	glm::vec3 sunToMoon = sunPos - moonPos;
    glm::vec3 cross = glm::cross(sunToEarth, EarthToMoon);
    if (glm::length(sunToEarth) - glm::length(sunToMoon) < glm::length(sunToEarth)) { moonInfront = true; }
	else { moonInfront = false; }
    return glm::length(cross) < 0.01f;
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse){ lastX=(float)xpos; lastY=(float)ypos; firstMouse=false; }
    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos; lastY = (float)ypos;
    float sensitivity = 0.1f;
    xoffset *= sensitivity; yoffset *= sensitivity;
    yaw += xoffset; pitch += yoffset;
    if(pitch>89.0f) pitch=89.0f;
    if(pitch<-89.0f) pitch=-89.0f;
    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(dir);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    fov -= (float)yoffset;
    if(fov<1.0f) fov=1.0f;
    if(fov>45.0f) fov=45.0f;
}
