#pragma once
#include "Window.h"
#include <map>

class WAbout : public Window
{
private:
	std::string engineName = "TIMEngine (a.k.a. TIME)";					
	std::string description = "The Incredible Mefiso's Engine is a custom Game Engine for educational purposes, that tries to mimic features the likes of Unity. More information on README file.";
	std::string authors = "Pol 'Mefiso' Perez Granero.\nFrancesc 'Xesk' Porta Solsona";
	std::map<std::string, std::string> libraries;			// Map containing the used Libraries' names and their versions
	std::string license = engineName + " is licensed under the GNU GPLv3, see LICENSE for more information.";
	std::string version = "v0.0.1";
public:
	WAbout(std::string name);								// Constructor
	~WAbout();												// Destructor

	void Draw() override;									// Operations performed when Rendering this window
};