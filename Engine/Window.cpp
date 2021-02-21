#include "Window.h"

Window::Window(std::string _name) : name(_name)
{
}

Window::~Window()
{
}

void Window::Enable(bool _active)
{
	this->active = _active;
}