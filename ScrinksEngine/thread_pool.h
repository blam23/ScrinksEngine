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

	constexpr uint8_t MainThreadID = 0;
	constexpr uint8_t BackgroundThreadID = 1;

	class ThreadAllocator
	{
	public:
		ThreadAllocator(size_t initialSize = 1024 * 1024 * 20)
		{
			m_data.resize(initialSize);
		}

		~ThreadAllocator()
		{
			while (m_freeBlocks)
			{
				const auto next = m_freeBlocks->m_nextBlock;
				delete m_freeBlocks;
				m_freeBlocks = next;
			}
		}

		void* allocate(size_t size)
		{
			if (auto free_block = get_free_block(size); free_block)
				return free_block;

			if (m_data.size() < m_offset + size)
				throw std::exception("ruh roh");

			const auto ret{ m_offset };
			m_offset += size;

			return (void*)(&m_data[ret]);
		}

		void deallocate(void* ptr, size_t size)
		{
			FreeBlock* ret = new FreeBlock{ (size_t)ptr, size, m_freeBlocks };
			m_freeBlocks = ret;
		}

	private:
		struct FreeBlock
		{
			size_t m_offset;
			size_t m_size;

			FreeBlock* m_nextBlock;
		};

		void* get_free_block(size_t size)
		{
			FreeBlock* block{ m_freeBlocks };

			while (block)
			{
				if (block->m_size >= size)
				{
					const auto ret{ (void*)m_freeBlocks->m_offset };
					const auto next{ m_freeBlocks->m_nextBlock };
					
					delete m_freeBlocks;
					m_freeBlocks = next;

					memset(ret, 0, size);
					return ret;
				}
				
				block = block->m_nextBlock;
			}

			return nullptr;
		}

		std::vector<std::uint8_t> m_data{};
		size_t m_offset{};
		FreeBlock* m_freeBlocks{};
	};

	void register_node(ID thread, void* node);
	void unregister_node(ID thread, void* node);

	std::uint8_t assign_thread(threads::Group group);
	void* allocate(ID thread, size_t size);
	void deallocate(ID thread, void* ptr, size_t size);

	template <typename T_Node, class... T_Args>
	T_Node* new_node_on_thread(ID thread, T_Args... args)
	{
		T_Node* ret{ std::construct_at(reinterpret_cast<T_Node*>(allocate(thread, sizeof(T_Node))), thread, args...) };
		register_node(thread, ret);
		return ret;
	}
	template <typename T_Node, class... T_Args>

	T_Node* new_node(Group group, T_Args... args)
	{
		const auto thread{ assign_thread(group) };
		T_Node* ret{ std::construct_at(reinterpret_cast<T_Node*>(allocate(thread, sizeof(T_Node))), thread, args...) };
		register_node(thread, ret);
		ret->m_allocated = sizeof(T_Node);
		return ret;
	}

	template <typename T_Node>
	void deallocate(T_Node* node)
	{
		unregister_node(node->thread_id(), node);
		deallocate(node->thread_id(), node, node->m_allocated);
	}

	bool on_my_thread(ID id);
	
	using FuncType = std::function<void(void*)>;
	using FuncTypePtr = std::shared_ptr<FuncType>;
	bool on_window_thread();
	void dispatch_and_wait(FuncType func);
	void dispatch_async(FuncType func); // TODO: return event to wait for if you need to sync?
	void await_previous();
	void dispatch_singular_and_wait(ID thread, FuncType func);
	void setup();
	void shutdown();
	void set_process_priority(Priority);
}

