#pragma once
#include <vector>
#include <GL/glew.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "Shader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 color;
};

class Polygon {
protected:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;

    glm::vec3 m_position{ 0.0f };
    glm::vec3 m_rotation{ 0.0f };
    glm::vec3 m_scale{ 1.0f };
    glm::mat4 m_model = glm::mat4(1.0f);

    glm::vec4 m_color{ 1.0f };
    bool m_useTexture = true;

    GLenum m_drawMode = GL_TRIANGLES;

public:
    Polygon() = default;

    Polygon(const std::vector<Vertex>& verts,
        const std::vector<unsigned int>& inds,
        GLenum drawMode = GL_TRIANGLES)
        : m_vertices(verts), m_indices(inds), m_drawMode(drawMode)
    {
        setupBuffers();
        updateModelMatrix();
    }

    Polygon(const std::vector<Vertex>& verts, GLenum drawMode = GL_TRIANGLES)
        : m_vertices(verts), m_drawMode(drawMode)
    {
        setupBuffers();
        updateModelMatrix();
    }

    ~Polygon() {
        if (m_EBO) glDeleteBuffers(1, &m_EBO);
        if (m_VBO) glDeleteBuffers(1, &m_VBO);
        if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    }

    void setPosition(const glm::vec3& p) { m_position = p; updateModelMatrix(); }
    void setRotation(const glm::vec3& r) { m_rotation = r; updateModelMatrix(); }
    void setScale(const glm::vec3& s) { m_scale = s; updateModelMatrix(); }
    void setColor(const glm::vec4& c) { m_color = c; }
    void setDrawMode(GLenum m) { m_drawMode = m; }

    void updateModelMatrix() {
        glm::mat4 model(1.0f);
        model = glm::translate(model, m_position);
        model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
        model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
        model = glm::scale(model, m_scale);
        m_model = model;
    }

    virtual void draw(Shader& shader, const glm::mat4& view, const glm::mat4& proj) {
        if (!shader.getID()) return;
        shader.bind();
        shader.setUniformMat4f("u_Model", m_model);
        shader.setUniformMat4f("u_View", view);
        shader.setUniformMat4f("u_Proj", proj);
        shader.setUniformVec4f("u_Color", m_color);
        shader.setUniform1i("u_UseTexture", m_useTexture ? 1 : 0);

        glBindVertexArray(m_VAO);
        if (!m_indices.empty()) {
            glDrawElements(m_drawMode, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, 0);
        }
        else {
            glDrawArrays(m_drawMode, 0, (GLsizei)m_vertices.size());
        }
        glBindVertexArray(0);
        shader.unbind();
    }

protected:
    void setupBuffers() {
        if (m_VAO) { glDeleteVertexArrays(1, &m_VAO); m_VAO = 0; }
        if (m_VBO) { glDeleteBuffers(1, &m_VBO); m_VBO = 0; }
        if (m_EBO) { glDeleteBuffers(1, &m_EBO); m_EBO = 0; }

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        if (!m_indices.empty())
            glGenBuffers(1, &m_EBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

        if (!m_indices.empty()) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);
        }

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        glBindVertexArray(0);
    }
};
