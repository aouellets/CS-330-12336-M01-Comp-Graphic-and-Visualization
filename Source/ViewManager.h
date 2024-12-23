#pragma once

#include "ShaderManager.h"
#include "camera.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>

class ViewManager
{
public:
	ViewManager(ShaderManager* pShaderManager);
	~ViewManager();

	// create the initial OpenGL display window
	GLFWwindow* CreateDisplayWindow(const char* windowTitle);

	// prepare the conversion from 3D object display to 2D scene display
	void PrepareSceneView();

	// Set the view (camera) matrix
	void SetViewMatrix(const glm::mat4& view);

	// Switch to perspective projection
	void SetPerspectiveMode();

	// Switch to orthographic projection
	void SetOrthographicMode();

	// mouse position callback for mouse interaction with the 3D scene
	static void Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos);

private:
	ShaderManager* m_pShaderManager;
	GLFWwindow* m_pWindow;

	void ProcessKeyboardEvents();

	// Matrices for view and projection
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;

	// Projection parameters
	float m_fov;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;
	float m_orthoSize;

	bool m_bUsePerspective;
};