#pragma once
#include "Polygon.h"
#include <array>
#include "additional/Shader.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

class Cuboid : public Polygon {
public:
    Cuboid(const glm::vec3& position,
        const glm::vec3& size,
        const std::array<glm::vec3, 6>& faceColors)
    {
        m_vertices = generateVerts(size, faceColors);
        m_indices = generateIndices();
        setupBuffers();
        setPosition(position);
        updateModelMatrix();
    }

    Cuboid(const glm::vec3& position, const glm::vec3& size, const glm::vec3& color)
    {
        std::array<glm::vec3, 6> cols;
        cols.fill(color);
        m_vertices = generateVerts(size, cols);
        m_indices = generateIndices();
        setupBuffers();
        setPosition(position);
        updateModelMatrix();
    }
    void drawRotated(Shader& shader, glm::mat4 view, glm::mat4 proj, float angle, glm::vec3 axis, glm::vec3 pivotPoint) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pivotPoint);
        model = glm::rotate(model, angle, axis);
        model = glm::translate(model, m_position - pivotPoint);
        model = glm::scale(model, m_scale);

        shader.bind();
        shader.setUniformMat4f("u_Model", model);
        shader.setUniformMat4f("u_View", view);
        shader.setUniformMat4f("u_Proj", proj);

        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    static std::vector<Vertex> generateVerts(const glm::vec3& size, const std::array<glm::vec3, 6>& faceColors) {
        float hx = size.x * 0.5f;
        float hy = size.y * 0.5f;
        float hz = size.z * 0.5f;

        glm::vec3 p0(-hx, -hy, hz);
        glm::vec3 p1(hx, -hy, hz);
        glm::vec3 p2(hx, hy, hz);
        glm::vec3 p3(-hx, hy, hz);
        glm::vec3 p4(-hx, -hy, -hz);
        glm::vec3 p5(hx, -hy, -hz);
        glm::vec3 p6(hx, hy, -hz);
        glm::vec3 p7(-hx, hy, -hz);

        std::vector<Vertex> verts;
        verts.reserve(24);

        //front face
        verts.push_back({ p0, {0,0}, faceColors[0] });
        verts.push_back({ p1, {1,0}, faceColors[0] });
        verts.push_back({ p2, {1,1}, faceColors[0] });
        verts.push_back({ p3, {0,1}, faceColors[0] });

        // back face
        verts.push_back({ p5, {0,0}, faceColors[1] });
        verts.push_back({ p4, {1,0}, faceColors[1] });
        verts.push_back({ p7, {1,1}, faceColors[1] });
        verts.push_back({ p6, {0,1}, faceColors[1] });

        // left face
        verts.push_back({ p4, {0,0}, faceColors[2] });
        verts.push_back({ p0, {1,0}, faceColors[2] });
        verts.push_back({ p3, {1,1}, faceColors[2] });
        verts.push_back({ p7, {0,1}, faceColors[2] });

        //right face
        verts.push_back({ p1, {0,0}, faceColors[3] });
        verts.push_back({ p5, {1,0}, faceColors[3] });
        verts.push_back({ p6, {1,1}, faceColors[3] });
        verts.push_back({ p2, {0,1}, faceColors[3] });

        // top   face
        verts.push_back({ p3, {0,0}, faceColors[4] });
        verts.push_back({ p2, {1,0}, faceColors[4] });
        verts.push_back({ p6, {1,1}, faceColors[4] });
        verts.push_back({ p7, {0,1}, faceColors[4] });

        // bottom   face
        verts.push_back({ p4, {0,0}, faceColors[5] });
        verts.push_back({ p5, {1,0}, faceColors[5] });
        verts.push_back({ p1, {1,1}, faceColors[5] });
        verts.push_back({ p0, {0,1}, faceColors[5] });

        return verts;
    }

    static std::vector<unsigned int> generateIndices() {
        return {
                0,1,2, 0,2,3,     // front
                4,5,6, 4,6,7,      // back
                8,9,10, 8,10,11,  //left
                12,13,14, 12,14,15,// right
                16,17,18, 16,18,19,// top
                20,21,22, 20,22,23    // bottom
        };
    }
};
