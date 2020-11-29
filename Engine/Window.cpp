#include "Window.h"

Window::Window(std::string name, int ID) : ID(ID), name(name)
{
}

Window::~Window()
{
}

void Window::Enable(bool active)
{
	this->active = active;
}

