#pragma once

#include "node.h"
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>

namespace scrinks::core
{
	struct Transform2D
	{
		glm::vec2 pos{};
		glm::vec2 scale{};
		float rot{};
	};

	class Node2D : public Node
	{
	public:
		Node2D(threads::ID thread, Node* parent, const Transform2D& pos);

	protected:
		virtual const std::string_view default_name() const { return "Node2D"; }
		virtual void setup_script_data() override;

	public:
		void get_transform(float interpolate, Transform2D& out) const
		{
			out.pos   = glm::mix(m_previous.pos, m_current.pos, interpolate);
			out.scale = glm::mix(m_previous.scale, m_current.scale, interpolate);
			out.rot   = glm::mix(m_previous.rot, m_current.rot, interpolate);
		}

		glm::vec2& pos() { return m_current.pos; }
		float rot() { return m_current.rot; }
		float* rot_ptr() { return &m_current.rot; }
		glm::vec2& scale() { return m_current.scale; }
		std::pair<float, float> get_position() { return std::make_pair(m_current.pos.x, m_current.pos.y); }

		void sync_fixed_update() override;
		void set_position(const glm::vec2&);
		void translate(const glm::vec2& offset);
		void rotate(float angle);
		void set_rotation(float angle);

	protected:
		Transform2D m_current;
		Transform2D m_previous;
	};
}

