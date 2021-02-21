#include "Config.h"
#include "Globals.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include "Leaks.h"

Config::Config()
{
}

Config::~Config()
{
}

bool Config::Read(const char* _filename)
{
	FILE* f;
	errno_t err;
	if ((err = fopen_s(&f, _filename, "rb")) != 0)
	{
		// File could not be opened. FILE* was set to NULL. error code is returned in err.
		LOG("[error] File could not be opened: %s", _filename, strerror(err));
		return false;
	}
	else
	{
		char readBuffer[65536];
		rapidjson::FileReadStream is(f, readBuffer, sizeof(readBuffer));

		d.ParseStream(is);
		fclose(f);
		return true;
	}
}

bool Config::Write(const char* _filename)
{
	FILE* f;
	errno_t err;
	if ((err = fopen_s(&f, _filename, "wb")) != 0)
	{
		// File could not be opened. FILE* was set to NULL. error code is returned in err.
		LOG("[error] File could not be opened: %s", _filename, strerror(err));
		return false;
	}
	else
	{
		char writeBuffer[65536];
		rapidjson::FileWriteStream os(f, writeBuffer, sizeof(writeBuffer));

		rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
		d.Accept(writer);

		fclose(f);
		return true;
	}
}
