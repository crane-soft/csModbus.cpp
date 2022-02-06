#pragma once
#include <mutex>
#include <condition_variable>

class Semaphore {
public:
	Semaphore(int count_ = 0)
		: count(count_)
	{}

	void Release() {
		std::unique_lock<std::mutex> lock(mtx);
		++count;
		cv.notify_one();
	}

	void Wait() {
		std::unique_lock<std::mutex> lock(mtx);
		while (count == 0) {
			cv.wait(lock);
		}
		count--;
	}

	void TryRelease()
	{
		if (count == 0) {
			Release();
		}
	}

private:
	std::mutex mtx;
	std::condition_variable cv;
	int count;
};
