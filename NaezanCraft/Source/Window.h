#pragma once

#include <string>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "MessageLog.h"
#include "Event/EventSystem.h"

//Singleton? not?
class Window
{
public:
	Window(const std::string& name = "NaezanCraft",
		uint32_t  width = 1280, uint32_t height = 720);
	~Window();

	void Init();
	void Update();
	void Shutdown();

	void SetEventBindings();
	void SetEventCallbacks();

	static inline std::unique_ptr<Window> CreateCraftWindow(const std::string& name = "NaezanCraft", uint32_t  width = 1280, uint32_t height = 720)
	{
		return std::make_unique<Window>(name, width, height);
	}

	inline std::string GetWindowName() const { return windowName; }
	inline uint32_t GetWidth() const { return Width; }
	inline uint32_t GetHeight() const { return Height; }
	inline GLFWwindow* GetWindowObject() const { return window; }//static?

	//TO DO Sync
	
	//Callback Function
	void OnWindowPos(const Event& event);
	void OnWindowSize(const Event& event);
	void OnWindowClose(const Event& event);
	void OnWindowMaximize(const Event& event);

	void OnFramebufferSize(const Event& event);

	void OnKey(const Event& event);
	void OnMouseButton(const Event& event);
	void OnCursorPos(const Event& event);
	void OnScroll(const Event& event);

	//TO DO add imgui Callback Function

private:
	static Dispatcher eventDispatcher;

private:
	std::string windowName;
	uint32_t Width;
	uint32_t Height;
	GLFWwindow* window;

	unsigned int VBO, VAO, EBO, shaderProgram;
};