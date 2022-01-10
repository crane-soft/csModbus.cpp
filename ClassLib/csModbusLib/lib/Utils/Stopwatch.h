#pragma once
#include <chrono>

class Stopwatch
{

private:
	std::chrono::high_resolution_clock::time_point last_time_point;
	std::chrono::duration<double> time_duration;

public:
	Stopwatch() :
		last_time_point{ std::chrono::high_resolution_clock::now() },
		time_duration{ std::chrono::duration<double>::zero() }
	{

	}

	void Restart()
	{
		last_time_point = std::chrono::high_resolution_clock::now();
	}

	int64_t ElapsedMicroseconds()
	{
		time_duration = std::chrono::high_resolution_clock::now() - last_time_point;
		return std::chrono::duration_cast<std::chrono::microseconds > (time_duration).count();
	}

	int64_t ElapsedMilliseconds()
	{
		time_duration = std::chrono::high_resolution_clock::now() - last_time_point;
		return std::chrono::duration_cast<std::chrono::milliseconds > (time_duration).count();
	}
};
