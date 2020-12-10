#pragma once
#include "Window.h"

class WHierarchy : public Window
{
public:

public:
	WHierarchy(std::string name);					// Constructor
	~WHierarchy();									// Destructor

	void Draw() override;							// Operations performed when Rendering this window

};