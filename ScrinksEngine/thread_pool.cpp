#include "thread_pool.h"

#include <condition_variable>
#include <thread>
#include <iostream>
#include <vector>
#include <forward_list>
#include "node.h"
#include "node_pool.h"
#include "spdlog/spdlog.h"

// TODO: Linux support
#pragma warning( push )
#pragma warning( disable : 5105)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "windows.h"
#pragma warning( pop )

#include <cassert>

using namespace scrinks;

bool exit_all_threads{ false };

std::vector<std::forward_list<void*>> s_threadNodes{};
struct AssignableThread
{
	DISABLE_COPY_AND_MOVE(AssignableThread);

	AssignableThread(size_t index)
		: m_thread(&AssignableThread::loop, this)
		, m_index{ index }
	{
	}

	void loop()
	{
		threads::CurrentThreadID = m_index < 10 ? "0" : "";
		threads::CurrentThreadID += std::to_string(m_index);
		
		spdlog::info("thread {} running", m_index);
		do
		{
			{
				std::unique_lock<std::mutex> waitLock { m_waitMutex };
				m_added.wait(waitLock, [&] () { return m_func || exit_all_threads; });

				if (exit_all_threads)
					return;

				if (m_singularAction)
				{
					(*m_func)(nullptr);
				}
				else
				{
					for (auto node : s_threadNodes[m_index])
					{
						(*m_func)(node);
					}
				}
				m_func = nullptr;
			}
			{
				std::lock_guard<std::mutex> completeLock { m_completeMutex };
				m_taskComplete = true;
				m_complete.notify_all();
			}
		} while (!exit_all_threads);
	}

	void assign(threads::FuncTypePtr func, bool singularAction = false)
	{
		std::lock_guard<std::mutex> waitLock { m_waitMutex };
		m_taskComplete = false;
		m_singularAction = singularAction;
		m_func = func;
		m_added.notify_all();
	}

	void shutdown()
	{
		std::lock_guard<std::mutex> waitLock { m_waitMutex };
		m_added.notify_all();
	}

	bool m_busy{ false };
	
	std::mutex m_waitMutex;
	std::mutex m_completeMutex;
	
	std::condition_variable m_added;
	std::condition_variable m_complete;
	bool m_taskComplete{ true };
	bool m_singularAction{ false };

	std::thread m_thread;

	threads::FuncTypePtr m_func{ nullptr };
	std::size_t m_index;
};

std::vector<std::unique_ptr<AssignableThread>> s_threads{};
std::vector<std::size_t> s_entityCounts{};

void threads::register_node(Reference& thread, void* node)
{
	// don't bother adding stuff if we're shutting down.
	if (exit_all_threads)
		return;

	s_threadNodes[thread.m_thread_id].push_front(node);
	s_entityCounts[thread.m_thread_id]++;
}

void threads::unregister_node(Reference& thread, void* node)
{
	// don't bother removing stuff if we're shutting down.
	if (exit_all_threads)
		return;

	s_threadNodes[thread.m_thread_id].remove(node);
	s_entityCounts[thread.m_thread_id]--;
}

void add_thread()
{
	s_threads.push_back(std::make_unique<AssignableThread>(s_threads.size()));
	s_threadNodes.push_back({});
	s_entityCounts.push_back(0);
}

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
	const auto concurrency{ std::thread::hardware_concurrency() };
	//const auto concurrency{ 4 };

	for (std::uint32_t i = 0; i < concurrency; i++)
		add_thread();

	gameWindowThread = std::this_thread::get_id();

	SetThreadPriority(GetCurrentThread(), HIGH_PRIORITY_CLASS);
}

void threads::shutdown()
{
	exit_all_threads = true;

	for (auto& thread : s_threads)
		thread->shutdown();

	for (auto& thread : s_threads)
		thread->m_thread.join();
}

std::atomic<uint8_t> lastBucket{ 0 };
uint8_t assign_thread(threads::Group group)
{
	switch (group)
	{
	case threads::Group::Main:
		return threads::MainThreadID;
	case threads::Group::Background:
		return threads::BackgroundThreadID;
	case threads::Group::Split:
		return lastBucket++ % s_threads.size();
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
void threads::dispatch_and_wait(threads::FuncType func, bool singularAction)
{
	assert_main_thread();

	const auto f = std::make_shared<threads::FuncType>(func);

	for (auto& thread : s_threads)
		thread->assign(f, singularAction);

	for (auto& thread : s_threads)
	{
		std::unique_lock<std::mutex> waitLock { thread->m_completeMutex };
		thread->m_complete.wait(waitLock, [&] () { return thread->m_taskComplete; });
	}
	
	for (auto& thread : s_threads)
		thread->m_taskComplete = false;
}

void threads::dispatch_async(threads::FuncType func, bool singularAction)
{
	assert_main_thread();

	const auto f = std::make_shared<threads::FuncType>(func);

	for (auto& thread : s_threads)
		thread->assign(f, singularAction);
}

void threads::await_previous()
{
	for (auto& thread : s_threads)
	{
		if (thread->m_taskComplete)
			continue;

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
	if (ref.m_thread_id >= s_threads.size() || s_threads[ref.m_thread_id] == nullptr)
	{
		spdlog::error("[tid={}] [#t={}] [ptr={}]", ref.m_thread_id, s_threads.size(), (void*)s_threads[ref.m_thread_id].get());
		return false;
	}
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

auto scrinks::threads::get_total_entity_count() -> size_t
{
	size_t ret{ 0 };
	for (const auto& count : s_entityCounts)
		ret += count;

	return ret;
}

// TODO: Use this for thread object count tracking
threads::Reference::Reference(void* node, threads::Group group)
	: m_thread_id{ assign_thread(group) }
	, m_node { node }
{
	register_node(*this, node);
}

threads::Reference::~Reference()
{
	unregister_node(*this, m_node);
}

void scrinks::threads::thread_pool_formatter::format(const spdlog::details::log_msg&, const std::tm&, spdlog::memory_buf_t& dest)
{
	dest.append(CurrentThreadID.data(), CurrentThreadID.data() + CurrentThreadID.size());
}

std::unique_ptr<spdlog::custom_flag_formatter> scrinks::threads::thread_pool_formatter::clone() const
{
	return spdlog::details::make_unique<thread_pool_formatter>();
}
