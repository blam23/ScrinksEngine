#pragma once

#include "helpers.h"
#include <vector>
#include <thread>
#include <functional>

//
// The way this threading model works is a bit funky!
// Each operation will be performed by every thread
// It's then up to the methods called by the thread
// to determine if they want to do something or not.
// There is a helper function on_my_thread for this.
// 
// I did it this way to be different.
//  ~ A stupid person (me)
//

namespace scrinks::threads
{

	enum class Group
	{
		Main,           // High priority, single <main> thread
		Background,     // Low priority, single <background> thread
		Split,          // Split among pooled threads
	};

	enum class Priority
	{
		Normal,
		AboveNormal,
		High
	};

	using ID = std::uint8_t;

	// Use this to get a thread ID to run on.
	struct Reference
	{
		DISABLE_COPY_AND_MOVE(Reference);

		Reference(void*, Group);
		~Reference();

		ID m_thread_id;
		void* m_node;
	};

	constexpr uint8_t MainThreadID = 0;
	constexpr uint8_t BackgroundThreadID = 1;

	void register_node(Reference& thread, void* node);
	void unregister_node(Reference& thread, void* node);

	bool on_my_thread(Reference& manager);
	
	using FuncType = std::function<void(void*)>;
	using FuncTypePtr = std::shared_ptr<FuncType>;
	bool on_window_thread();
	void dispatch_and_wait(FuncType func);
	void dispatch_async(FuncType func); // TODO: return event to wait for if you need to sync?
	void await_previous();
	void dispatch_singular_and_wait(const Reference& thread, FuncType func);
	void setup();
	void shutdown();
	void set_process_priority(Priority);
}

