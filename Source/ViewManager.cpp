#include "ViewManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

ViewManager::ViewManager(ShaderManager* pShaderManager)
    : m_pShaderManager(pShaderManager),
    m_pWindow(nullptr),
    m_viewMatrix(glm::mat4(1.0f)),
    m_projectionMatrix(glm::mat4(1.0f)), // Initialize here
    m_fov(45.0f),
    m_aspectRatio(1000.0f / 800.0f),
    m_nearPlane(0.1f),
    m_farPlane(100.0f),
    m_orthoSize(10.0f),
    m_bUsePerspective(true)
{
    std::cout << "[ViewManager] Initialized." << std::endl;
}

ViewManager::~ViewManager()
{
    m_pShaderManager = NULL;
    m_pWindow = NULL;
    std::cout << "[ViewManager] Destroyed." << std::endl;
}

GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
    GLFWwindow* window = glfwCreateWindow(1000, 800, windowTitle, NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    m_pWindow = window;
    return window;
}

void ViewManager::PrepareSceneView()
{
    if (m_pShaderManager) {
        m_pShaderManager->setMat4Value("view", m_viewMatrix);
        m_pShaderManager->setMat4Value("projection", m_projectionMatrix);
    }
}

void ViewManager::SetViewMatrix(const glm::mat4& view)
{
    m_viewMatrix = view;
    std::cout << "[ViewManager] View matrix updated." << std::endl;
}

void ViewManager::SetPerspectiveMode()
{
    m_bUsePerspective = true;
    m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
    std::cout << "[ViewManager] Switched to Perspective mode." << std::endl;
}

void ViewManager::SetOrthographicMode()
{
    m_bUsePerspective = false;
    float halfWidth = m_orthoSize * m_aspectRatio;
    float halfHeight = m_orthoSize;
    m_projectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_nearPlane, m_farPlane);
    std::cout << "[ViewManager] Switched to Orthographic mode." << std::endl;
}

void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
{
    // Implement mouse handling if needed
}

void ViewManager::ProcessKeyboardEvents()
{
    if (m_pWindow && glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_pWindow, true);
    }
}