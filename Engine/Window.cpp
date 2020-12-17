#include "Window.h"

Window::Window(std::string name) : name(name)
{
}

Window::~Window()
{
}

void Window::Enable(bool active)
{
	this->active = active;
}