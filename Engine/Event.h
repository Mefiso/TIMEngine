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
