#include <iostream>
#include <cstdlib>
#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneManager.h"
#include "ViewManager.h"
#include "ShapeMeshes.h"
#include "ShaderManager.h"

// Global variables for camera and movement
glm::vec3 g_CameraPosition(0.0f, 2.0f, 10.0f);
glm::vec3 g_CameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 g_CameraUp(0.0f, 1.0f, 0.0f);

float g_CameraYaw = -90.0f;
float g_CameraPitch = 0.0f;
float g_CameraSpeed = 2.5f;
float g_MouseSensitivity = 0.1f;

bool g_FirstMouse = true;
double g_LastMouseX = 400.0;
double g_LastMouseY = 300.0;

bool g_bUsePerspective = true; // Toggled by pressing 'O'

// Forward declarations
void UpdateCameraVectors();
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
bool InitializeGLFW();
bool InitializeGLEW();

int main(int argc, char* argv[])
{
    if (!InitializeGLFW())
        return(EXIT_FAILURE);

    ShaderManager* g_ShaderManager = new ShaderManager();
    ViewManager* g_ViewManager = new ViewManager(g_ShaderManager);
    GLFWwindow* g_Window = g_ViewManager->CreateDisplayWindow("CS330 Final Project");

    glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(g_Window, KeyCallback);
    glfwSetCursorPosCallback(g_Window, MouseCallback);
    glfwSetScrollCallback(g_Window, ScrollCallback);

    if (!InitializeGLEW())
        return(EXIT_FAILURE);

    g_ShaderManager->LoadShaders(
        "../../Utilities/shaders/vertexShader.glsl",
        "../../Utilities/shaders/fragmentShader.glsl");
    g_ShaderManager->use();

    SceneManager* g_SceneManager = new SceneManager(g_ShaderManager);
    g_SceneManager->PrepareScene();
    std::cout << "[Main] Scene prepared." << std::endl;

    while (!glfwWindowShouldClose(g_Window))
    {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(g_CameraPosition, g_CameraPosition + g_CameraFront, g_CameraUp);
        g_ViewManager->SetViewMatrix(view);

        if (g_bUsePerspective)
            g_ViewManager->SetPerspectiveMode();
        else
            g_ViewManager->SetOrthographicMode();

        g_ViewManager->PrepareSceneView();

        g_SceneManager->RenderScene();

        glfwSwapBuffers(g_Window);
        glfwPollEvents();

        // Print camera position for debugging
        std::cout << "[Debug] Camera Position: "
            << g_CameraPosition.x << ", "
            << g_CameraPosition.y << ", "
            << g_CameraPosition.z << std::endl;
    }

    if (g_SceneManager) { delete g_SceneManager; g_SceneManager = nullptr; }
    if (g_ViewManager) { delete g_ViewManager;  g_ViewManager = nullptr; }
    if (g_ShaderManager) { delete g_ShaderManager; g_ShaderManager = nullptr; }

    exit(EXIT_SUCCESS);
}

void UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(g_CameraYaw)) * cos(glm::radians(g_CameraPitch));
    front.y = sin(glm::radians(g_CameraPitch));
    front.z = sin(glm::radians(g_CameraYaw)) * cos(glm::radians(g_CameraPitch));
    g_CameraFront = glm::normalize(front);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
        std::cout << "[Input] Key pressed: " << key << std::endl;

    float deltaTime = 1.0f / 60.0f;
    float velocity = g_CameraSpeed * deltaTime;

    if ((key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_W)
            g_CameraPosition += g_CameraFront * velocity;
        else if (key == GLFW_KEY_S)
            g_CameraPosition -= g_CameraFront * velocity;
        else if (key == GLFW_KEY_A)
            g_CameraPosition -= glm::normalize(glm::cross(g_CameraFront, g_CameraUp)) * velocity;
        else if (key == GLFW_KEY_D)
            g_CameraPosition += glm::normalize(glm::cross(g_CameraFront, g_CameraUp)) * velocity;
        else if (key == GLFW_KEY_Q)
            g_CameraPosition.y += velocity;
        else if (key == GLFW_KEY_E)
            g_CameraPosition.y -= velocity;
        else if (key == GLFW_KEY_O && action == GLFW_PRESS)
        {
            g_bUsePerspective = !g_bUsePerspective;
            std::cout << "[Camera] Projection mode toggled: "
                << (g_bUsePerspective ? "Perspective" : "Orthographic") << std::endl;
        }
    }
    // Note: We do NOT reset the camera position anywhere here.
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (g_FirstMouse)
    {
        g_LastMouseX = xpos;
        g_LastMouseY = ypos;
        g_FirstMouse = false;
    }

    double xoffset = xpos - g_LastMouseX;
    double yoffset = g_LastMouseY - ypos;
    g_LastMouseX = xpos;
    g_LastMouseY = ypos;

    xoffset *= g_MouseSensitivity;
    yoffset *= g_MouseSensitivity;

    g_CameraYaw += (float)xoffset;
    g_CameraPitch += (float)yoffset;

    if (g_CameraPitch > 89.0f)  g_CameraPitch = 89.0f;
    if (g_CameraPitch < -89.0f) g_CameraPitch = -89.0f;

    UpdateCameraVectors();
    std::cout << "[Input] Mouse moved to: (" << xpos << ", " << ypos << ")" << std::endl;
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    g_CameraSpeed += (float)yoffset * 0.5f;
    if (g_CameraSpeed < 0.5f) g_CameraSpeed = 0.5f;
    std::cout << "[Input] Mouse scroll: yoffset=" << yoffset << ". New speed=" << g_CameraSpeed << std::endl;
}

bool InitializeGLFW()
{
    glfwInit();
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    return true;
}

bool InitializeGLEW()
{
    GLenum GLEWInitResult = glewInit();
    if (GLEW_OK != GLEWInitResult)
    {
        std::cerr << glewGetErrorString(GLEWInitResult) << std::endl;
        return false;
    }
    std::cout << "INFO: OpenGL Initialized\n";
    std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << "\n" << std::endl;
    return true;
}