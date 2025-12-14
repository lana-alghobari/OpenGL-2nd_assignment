#pragma once
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "glm.hpp"

class Shader {
private:
    unsigned int m_ID= 0;

    struct Src { std::string vertex, fragment; };

    Src loadFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Shader: failed open " << path << std::endl;
        }
        std::string line;
        std::stringstream ss[2];
        int mode = -1;
        while (std::getline(file, line)) {
            if (line.find("#shader") != std::string::npos) {
                if (line.find("vertex") != std::string::npos) mode = 0;
              else mode = 1;
            } else {
            if (mode >= 0) ss[mode] << line << '\n';
            }
        }
        return { ss[0].str(), ss[1].str() };
    }

    static void checkCompile(unsigned int id, const std::string& type) {
        int success;
        char buf[1024];
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(id, 1024, nullptr, buf);
          std::cout << type << " SHADER ERROR:\n" << buf << std::endl;
        }
    }
    static void checkLink(unsigned int prog) {
        int success; char buf[1024];
        glGetProgramiv(prog, GL_LINK_STATUS, &success);
        if (!success) {
          glGetProgramInfoLog(prog, 1024, nullptr, buf);
            std::cout << "PROGRAM LINK ERROR:\n" << buf << std::endl;
        }
    }
    unsigned int compile(unsigned int type, const std::string& src) {
        unsigned int id = glCreateShader(type);
       const char* c = src.c_str();
     glShaderSource(id, 1, &c, nullptr);
       glCompileShader(id);
       if (type == GL_VERTEX_SHADER) checkCompile(id, "VERTEX");
     else checkCompile(id, "FRAGMENT");
        return id;
    }

    unsigned int createProgram(const std::string& vert, const std::string& frag) {
        unsigned int prog = glCreateProgram();
        unsigned int vs = compile(GL_VERTEX_SHADER, vert);
      unsigned int fs = compile(GL_FRAGMENT_SHADER, frag);
        glAttachShader(prog, vs);
       glAttachShader(prog, fs);
        glLinkProgram(prog);
        checkLink(prog);
        glValidateProgram(prog);
       glDeleteShader(vs);
       glDeleteShader(fs);
       return prog;
    }

public:
    Shader() = default;
    Shader(const std::string& shaderFile) {
        Src s = loadFromFile(shaderFile);
      m_ID = createProgram(s.vertex, s.fragment);
    }
    ~Shader() { if (m_ID) glDeleteProgram(m_ID); }

    void bind() const { glUseProgram(m_ID); }
  void unbind() const { glUseProgram(0); }
    unsigned int getID() const { return m_ID; }

    void setUniformMat4f(const std::string& name, const glm::mat4& m) const {
        int loc = glGetUniformLocation(m_ID, name.c_str());
        if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
    }
    void setUniformVec4f(const std::string& name, const glm::vec4& v) const {
        int loc = glGetUniformLocation(m_ID, name.c_str());
        if (loc != -1) glUniform4f(loc, v.x, v.y, v.z, v.w);
    }
    void setUniformVec3f(const std::string& name, const glm::vec3& v) const {
        int loc = glGetUniformLocation(m_ID, name.c_str());
        if (loc != -1) glUniform3f(loc, v.x, v.y, v.z);
    }
    void setUniform1i(const std::string& name, int v) const {
        int loc = glGetUniformLocation(m_ID, name.c_str());
        if (loc != -1) glUniform1i(loc, v);
    }
    void setUniform1f(const std::string& name, float v) const {
        int loc = glGetUniformLocation(m_ID, name.c_str());
        if (loc != -1) glUniform1f(loc, v);
    }
};
