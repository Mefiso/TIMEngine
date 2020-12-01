#pragma once


struct Event
{
	enum EventType
	{
		window_resize,
		window_fullscreen,
		file_dropped,
	} type;

	union
	{
		struct
		{
			const char* ptr;
		} string;

		struct
		{
			int x, y;
		} point2d;
	};

	Event(EventType type) : type(type)
	{}
};
