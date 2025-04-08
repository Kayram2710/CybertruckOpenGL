// CybertruckOpenGL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define TINYOBJLOADER_IMPLEMENTATION
#include "include/tiny_obj_loader.h"

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <GL/glew.h>    
#include <GLFW/glfw3.h> 
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// Shader sources
const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 transform;
    void main() {
        gl_Position = transform * vec4(aPos, 1.0);
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 330 core
    out vec3 color;
    uniform vec3 ourColor;
    void main(){
        color = ourColor;
    }
)glsl";

// Vertex struct
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

bool loadOBJModel(const std::string& path, std::vector<Vertex>& outVertices) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
    if (!warn.empty()) std::cout << "WARN: " << warn << std::endl;
    if (!err.empty()) std::cerr << "ERR: " << err << std::endl;
    if (!ret) return false;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex v{};
            v.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };
            if (!attrib.normals.empty() && index.normal_index >= 0) {
                v.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }
            else {
                v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
            }
            outVertices.push_back(v);
        }
    }

    return true;
}

// Input controls
void processInput(GLFWwindow* window, glm::mat4& transform, float d, float s) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        transform = glm::translate(transform, glm::vec3(0.0f, d, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        transform = glm::translate(transform, glm::vec3(0.0f, -d, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        transform = glm::translate(transform, glm::vec3(-d, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        transform = glm::translate(transform, glm::vec3(d, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        transform = glm::rotate(transform, glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        transform = glm::rotate(transform, glm::radians(-3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        transform = glm::rotate(transform, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        transform = glm::rotate(transform, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        transform = glm::rotate(transform, glm::radians(3.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        transform = glm::rotate(transform, glm::radians(-3.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        transform = glm::scale(transform, glm::vec3(1.0f, 1.0f, s));
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        transform = glm::scale(transform, glm::vec3(1.0f, 1.0f, 1.0f / s));
}

int main() {
    //======================OUTPUT======================
    //Direct std::out to txt file
    FILE* pFile = nullptr;
    errno_t err = freopen_s(&pFile, "sample_output.txt", "w", stdout);
    if (err != 0) {
        std::cerr << "Error redirecting stdout" << std::endl;
        return 1;
    }
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Cybertruck Test Loader", nullptr, nullptr);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glewInit();
    glEnable(GL_DEPTH_TEST);
    // line thickness
    glLineWidth(3.0f);


    // Compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "ourColor");

    // Load OBJ
    std::vector<Vertex> modelVertices;
    if (!loadOBJModel("Cybertruck.obj", modelVertices)) {
        std::cerr << "Failed to load square.obj" << std::endl;
        return -1;
    }

    // Upload to GPU
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), modelVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.15f));  

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),   
        glm::vec3(0.0f, 0.0f, 0.0f),   
        glm::vec3(0.0f, 1.0f, 0.0f)    
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        1024.0f / 768.0f,
        0.1f,
        100.0f
    );

    glm::mat4 interactive = glm::mat4(1.0f); 
    float d = 0.01f, s = 1.05f;

    
    while (!glfwWindowShouldClose(window)) {
        // Process keyboard input to update the transformation matrix
        processInput(window, interactive, d, s);

        // If any transformation key is pressed, output current matrix and transformed vertices.
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        {
            //Output
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                std::cout << "Pressed W: Move Cybertruck upwards" << std::endl;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                std::cout << "Pressed S: Move Cybertruck downwards" << std::endl;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                std::cout << "Pressed A: Move Cybertruck left" << std::endl;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                std::cout << "Pressed D: Move Cybertruck right" << std::endl;
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
                std::cout << "Pressed Q: Rotate Cybertruck along z axis anticlockwise" << std::endl;
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
                std::cout << "Pressed E: Rotate Cybertruck along z axis clockwise" << std::endl;
            if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
                std::cout << "Pressed R: Scale Cybertruck down along z axis" << std::endl;
            if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
                std::cout << "Pressed F: Scale Cybertruck up along z axis" << std::endl;

            std::cout << "Current Transformation Matrix:" << std::endl;
            // Print matrix in row-major order for clarity
            for (int row = 0; row < 4; row++) {
                std::cout << interactive[0][row] << " "
                    << interactive[1][row] << " "
                    << interactive[2][row] << " "
                    << interactive[3][row] << std::endl;
            }
            std::cout << "-----------------------------" << std::endl;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Final MVP matrix: projection * view * interactive * model
        glm::mat4 finalTransform = projection * view * interactive * model;

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(finalTransform));
        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

        glBindVertexArray(VAO);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
        glDrawArrays(GL_TRIANGLES, 0, modelVertices.size());
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}