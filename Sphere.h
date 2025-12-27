#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include "stb_image.h"
#include <vector>
#include <iostream>
#include "Shader.h"

class Sphere {
private:
    unsigned int VAO, VBO, EBO;
    unsigned int textureID;
    int indexCount;

    void generateSphere(float radius, unsigned int sectorCount, unsigned int stackCount) {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        const float PI = 3.1415926f;

        for(unsigned int i = 0; i <= stackCount; ++i){
            float stackAngle = PI/2 - i * PI / stackCount;
            float xy = radius * cos(stackAngle);
            float z = radius * sin(stackAngle);

            for(unsigned int j = 0; j <= sectorCount; ++j){
                float sectorAngle = j * 2 * PI / sectorCount;
               float x = xy * cos(sectorAngle);
                float y = xy * sin(sectorAngle);

                vertices.push_back(x);
              vertices.push_back(y);
              vertices.push_back(z);

                glm::vec3 n = glm::normalize(glm::vec3(x,y,z));
                vertices.push_back(n.x);
                vertices.push_back(n.y);
                vertices.push_back(n.z);

                // tex coords
                vertices.push_back((float)j/sectorCount);
                vertices.push_back((float)i/stackCount);
            }
        }

        // indices
        for(unsigned int i = 0; i < stackCount; ++i){
            unsigned int k1 = i * (sectorCount + 1);
            unsigned int k2 = k1 + sectorCount + 1;
            for(unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2){
                if(i != 0) indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1+1);

                if(i != (stackCount-1)) indices.push_back(k1+1);
                indices.push_back(k2);
                indices.push_back(k2+1);
            }
        }

        indexCount = indices.size();

        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);
        glGenBuffers(1,&EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

public:
    Sphere(float radius=1.0f, unsigned int sectorCount=36, unsigned int stackCount=18, const char* texPath=nullptr) {
        generateSphere(radius, sectorCount, stackCount);
        std::cout<<3;

        if(texPath){
            std::cout<<4;

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            std::cout<<5;

            int width, height, nrChannels;
            stbi_set_flip_vertically_on_load(true);
            std::cout<<6;

            unsigned char *data = stbi_load(texPath, &width, &height, &nrChannels, 0);
            std::cout<<7;

            if(data){
                std::cout<<3;
                //GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;
                GLenum format;
                if (nrChannels == 1) format = GL_RED;
                else if (nrChannels == 3) format = GL_RGB;
                else if (nrChannels == 4) format = GL_RGBA;
                else {
                    std::cout << "Unsupported nrChannels: " << nrChannels << std::endl;
                    stbi_image_free(data);
                    textureID = 0;
                    return;
                }
                glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,format,GL_UNSIGNED_BYTE,data);
                glGenerateMipmap(GL_TEXTURE_2D);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else {
                std::cout << "Failed to load texture: " << texPath << std::endl;
            }
            stbi_image_free(data);
        } else {
            textureID = 0;
        }
    }

    void Draw(Shader &shader){
        if(textureID){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);
            shader.setUniform1i("textureSample", 0);
            //shader.setUniform1i("material.texture_diffuse1", 0);
        }
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};
