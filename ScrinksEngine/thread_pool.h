#pragma once

#include "helpers.h"
#include <vector>
#include <thread>
#include <functional>
#include "spdlog/pattern_formatter.h"

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
		None,           // Invalid
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

	//consteval static Reference INAVLID_REFERENCE{ nullptr, Group::None };

	constexpr uint8_t MainThreadID = 0;
	constexpr uint8_t BackgroundThreadID = 1;
	static inline thread_local std::string CurrentThreadID{ "ma" };

	void register_node(Reference& thread, void* node);
	void unregister_node(Reference& thread, void* node);

	bool on_my_thread(Reference& manager);
	
	using FuncType = std::function<void(void*)>;
	using FuncTypePtr = std::shared_ptr<FuncType>;
	bool on_window_thread();
	void dispatch_and_wait(FuncType func, bool singularAction = false);
	void dispatch_async(FuncType func, bool singularAction = false);
	void await_previous();
	void dispatch_singular_and_wait(const Reference& thread, FuncType func);
	void setup();
	void shutdown();
	void set_process_priority(Priority);
	auto get_total_entity_count() -> size_t;

	class thread_pool_formatter : public spdlog::custom_flag_formatter
	{
	public:
		void format(const spdlog::details::log_msg&, const std::tm&, spdlog::memory_buf_t& dest);
		std::unique_ptr<custom_flag_formatter> clone() const override;
	};
}

