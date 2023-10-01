#include "thread_pool.h"

#include <condition_variable>
#include <thread>
#include <iostream>

// TODO: Linux support
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "windows.h"
#include <cassert>

using namespace scrinks;

bool exit_all_threads{ false };

struct AssignableThread
{
	DISABLE_COPY_AND_MOVE(AssignableThread);

	AssignableThread()
		: m_thread(&AssignableThread::loop, this)
	{
	}

	void loop()
	{
		do
		{
			{
				std::unique_lock<std::mutex> waitLock { m_waitMutex };
				m_added.wait(waitLock, [&] () { return m_func; });

				(*m_func)();
				m_func = nullptr;
			}
			{
				std::lock_guard<std::mutex> completeLock { m_completeMutex };
				m_taskComplete = true;
				m_complete.notify_all();
			}
		} while (!exit_all_threads);
	}

	void assign(threads::FuncTypePtr func)
	{
		std::lock_guard<std::mutex> waitLock { m_waitMutex };
		m_func = func;
		m_added.notify_all();
	}

	bool m_busy{ false };
	
	std::mutex m_waitMutex;
	std::mutex m_completeMutex;
	
	std::condition_variable m_added;
	std::condition_variable m_complete;
	bool m_taskComplete{ false };

	std::thread m_thread;

	threads::FuncTypePtr m_func{ nullptr };
};

AssignableThread* mainThread;
AssignableThread* backgroundThread;
std::vector<std::unique_ptr<AssignableThread>> s_threads{};

void set_thread_priority(AssignableThread* thread, threads::Priority priority)
{
	DWORD threadPri{ THREAD_PRIORITY_NORMAL };

	switch (priority)
	{
	case threads::Priority::High:
		threadPri = THREAD_PRIORITY_HIGHEST;
		break;
	case threads::Priority::AboveNormal:
		threadPri = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	}

	SetThreadPriority(thread->m_thread.native_handle(), threadPri);
}

std::thread::id gameWindowThread;

void threads::setup()
{
	auto concurrency{ std::thread::hardware_concurrency() };

	s_threads.push_back(std::make_unique<AssignableThread>());
	s_threads.push_back(std::make_unique<AssignableThread>());

	mainThread = s_threads[0].get();
	backgroundThread = s_threads[1].get();

	set_thread_priority(mainThread, Priority::High);
	set_thread_priority(backgroundThread, Priority::Normal);

	for (std::uint32_t i = 0; i < concurrency - 2; i++)
		s_threads.push_back(std::make_unique<AssignableThread>());

	gameWindowThread = std::this_thread::get_id();
}

void threads::shutdown()
{
	exit_all_threads = true;

	for (auto& thread : s_threads)
		thread->m_thread.join();
}

// TODO: Realistically this should track counts and add to lowest.
std::atomic<std::uint8_t> lastBucket{ 2 };
std::uint8_t assign_thread(threads::Group group)
{
	switch (group)
	{
	case threads::Group::Main:
		return threads::MainThreadID;
	case threads::Group::Background:
		return threads::BackgroundThreadID;
	case threads::Group::Split:
		if (lastBucket >= s_threads.size())
			lastBucket = 2;
		return lastBucket++;
	}

	return 0;
}

void assert_main_thread()
{
	assert(threads::on_window_thread());
}

bool threads::on_window_thread()
{
	return std::this_thread::get_id() == gameWindowThread;
}

// Only call this from the game loop thread.
void threads::dispatch_and_wait(threads::FuncType func)
{
	assert_main_thread();

	const auto f = std::make_shared<threads::FuncType>(func);

	for (auto& thread : s_threads)
		thread->assign(f);

	for (auto& thread : s_threads)
	{
		std::unique_lock<std::mutex> waitLock { thread->m_completeMutex };
		thread->m_complete.wait(waitLock, [&] () { return thread->m_taskComplete; });
	}

	for (auto& thread : s_threads)
		thread->m_taskComplete = false;
}

// Only call this from the game loop thread.
void scrinks::threads::dispatch_singular_and_wait(const Reference& ref, FuncType func)
{
	assert_main_thread();

	const auto& thread{ s_threads[ref.m_thread_id] };

	const auto f = std::make_shared<threads::FuncType>(func);
	thread->assign(f);

	std::unique_lock<std::mutex> waitLock { thread->m_completeMutex };
	thread->m_complete.wait(waitLock, [&] () { return thread->m_taskComplete; });

	thread->m_taskComplete = false;
}

bool threads::on_my_thread(Reference& ref)
{
	//return true;
	auto real_thread_id{ std::this_thread::get_id() };
	return real_thread_id == s_threads[ref.m_thread_id]->m_thread.get_id();
}

void threads::set_process_priority(Priority priority)
{
	DWORD winPri{ NORMAL_PRIORITY_CLASS };
	
	switch (priority)
	{
	case Priority::High:
		winPri = HIGH_PRIORITY_CLASS;
		break;
	case Priority::AboveNormal:
		winPri = ABOVE_NORMAL_PRIORITY_CLASS;
		break;
	}

	SetPriorityClass(GetCurrentProcess(), winPri);
}

// TODO: Use this for thread object count tracking
threads::Reference::Reference(threads::Group group)
	: m_thread_id{ assign_thread(group) }
{
}

threads::Reference::~Reference()
{
}
