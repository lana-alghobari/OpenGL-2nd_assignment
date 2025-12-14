#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

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

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

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

    Sphere sunSphere(0.5f, 36, 18);
    Sphere smallSphere(0.3f, 36, 18);

    glm::vec3 sunPos = glm::vec3(-1.0f, 0.0f, 0.0f);

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
                                     {1.0f,1.0f,0.0f},
                                     {1.0f,1.0f,0.0f});
        lightingShader.setUniform1f("material.shininess", 32.0f);

        glm::mat4 modelSun = glm::translate(glm::mat4(1.0f), sunPos);
        modelSun = glm::scale(modelSun, glm::vec3(0.5f));
        lightingShader.setUniformMat4f("model", modelSun);
        lightingShader.setUniform1i("isEmissive", true);
        lightingShader.setUniformVec3f("emissiveColor", glm::vec3(1.0f,1.0f,0.0f));
        sunSphere.Draw(lightingShader);

        float orbitRadius = 1.5f;
        float orbitSpeed = 1.0f;
        glm::vec3 smallPos = sunPos + glm::vec3(
                orbitRadius * cos(currentFrame * orbitSpeed),
                0.0f,
                orbitRadius * sin(currentFrame * orbitSpeed)
        );

        glm::mat4 modelSmall = glm::translate(glm::mat4(1.0f), smallPos);
        float selfRotateSpeed = 2.0f;
        modelSmall = glm::rotate(modelSmall, currentFrame * selfRotateSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
        modelSmall = glm::scale(modelSmall, glm::vec3(0.3f));

        lightingShader.setUniformMat4f("model", modelSmall);
        lightingShader.setUniform1i("isEmissive", false);
        lightingShader.setUniformVec3f("objectColor", glm::vec3(0.8f,0.0f,0.0f));

        smallSphere.Draw(lightingShader);

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
