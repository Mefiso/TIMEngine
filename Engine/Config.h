#pragma once
#include "rapidjson/document.h"

class Config {
private:
	rapidjson::Document d;
public:
	Config();
	~Config();

	bool Read(const char* _filename);
	bool Write(const char* _filename);
};