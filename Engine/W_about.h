#pragma once
#include "ImGUI/imgui.h"
#include "Window.h"
#include <map>

class WAbout : public Window
{
private:
	std::string engineName = "Placeholder Engine";
	std::string description = "Custom Game Engine for educational purposes, that tries to mimic features the likes of Unity.";
	std::string authors = "Pol 'Mefiso' Perez Granero.";
	std::map<std::string, std::string> libraries;
	std::string license = engineName + " is licensed under the GNU GPLv3, see LICENSE for more information.";
	std::string version = "v0.0.1";
public:
	WAbout(std::string name, int ID);
	~WAbout();

	void Draw() override;
};