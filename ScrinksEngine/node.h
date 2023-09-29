#pragma once

#include "helpers.h"
#include "script.h"
#include <vector>
#include <cstdint>
#include <string>
#include <string_view>
#include <atomic>
#include <map>
#include <memory>

namespace scrinks::core
{
	class Node
	{
	public:
		using ID = std::size_t;
		constexpr static ID InvalidID{ (std::numeric_limits<ID>::max)() };

		Node(Node* m_parent);
		virtual ~Node();

	public:
		std::vector<Node*>& children() { return m_children; }
		const std::vector<Node*>& children() const { return m_children; }
		const char* name() const;
		const char* type() const;
		ID id() const { return m_id; }
		void rename(const std::string& name) { m_name = name; }

		virtual void fixed_update();
		virtual void attached() {}
		virtual void removed() {}

	protected:
		virtual const std::string_view default_name() const { return "Node"; }

	private:
		void claim_child(Node& node);
		void disown_child(Node& node);

	private:
		Node* m_parent;
		std::vector<Node*> m_children;
		std::shared_ptr<scrinks::lua::Script> m_script;
		std::string m_name;
		ID m_id;

		static std::atomic<ID> s_id;
	};
};
