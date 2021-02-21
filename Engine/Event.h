#pragma once

struct Event
{
	enum EventType
	{
		window_resize,
		window_fullscreen,
		file_dropped,
		keyboard_event,
		rotate_event,
		orbit_event,
		wheel_event,
	} type;

	union
	{
		struct
		{
			const char* ptr = nullptr;
		} string;

		struct
		{
			int x = 0, y = 0;
		} point2d = { 0, 0 };
	};

	Event(EventType type) : type(type)
	{}
};
