#pragma once

#include "node.h"

#include <list>

namespace scrinks::core
{
	template <typename T_Node, typename = std::enable_if_t<std::is_base_of<scrinks::core::Node, T_Node>::value>>
	class NodePool
	{
	public:
		template <class... T_Args>
		static T_Node* get(T_Args&&... args)
		{
			std::lock_guard lock{ s_node_list_lock };

			auto ptr{ s_free_nodes.begin() };
			if (ptr == s_free_nodes.end())
				return allocate(std::forward<T_Args>(args)...);

			const auto ret{ std::construct_at(*ptr, std::forward<T_Args>(args)...) };
			s_free_nodes.pop_front();
			return ret;
		}

		static void release(T_Node* node)
		{
			std::lock_guard lock{ s_node_list_lock };

			std::destroy_at(node);
			s_free_nodes.push_back(node);
		}

	private:
		inline static std::mutex s_node_list_lock{};
		inline static std::list<T_Node*> s_free_nodes{};

		template <class... T_Args>
		static T_Node* allocate(T_Args&&... args)
		{
			return new T_Node(std::forward<T_Args>(args)...);
		}
	};
}