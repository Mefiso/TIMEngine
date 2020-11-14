#pragma once
#include <string>

class Window
{
public:
	Window(std::string name, int ID);
	virtual ~Window();

	virtual void Draw() = 0;

	void Enable(bool active);

	bool isEnabled() const { return active; }
	inline const char* GetWindowName() { return name.c_str(); }
protected:
	bool active = true;
	int ID = -1;
	std::string name;
};