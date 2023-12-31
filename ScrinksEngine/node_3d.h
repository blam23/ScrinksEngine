#pragma once

#include "node.h"
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>

namespace scrinks::core
{
	struct Transform
	{
		glm::vec3 pos{};
		glm::quat rot{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 scale{ 1.0f };

		glm::mat4 matrix() const
		{
			static const glm::mat4 identity{ 1.0f };

			return
				  glm::translate(identity, pos)
				* glm::mat4_cast(rot) 
				* glm::scale(identity, scale);
		}
	};

	class Node3D : public Node
	{
	public:
		Node3D(Node* parent, const Transform& position);

	protected:
		virtual const std::string_view default_name() const { return "Node3D"; }
		virtual void setup_script_data() override;

	public:
		glm::mat4 mat4(float interpolate) const 
		{ 
			return glm::mix(m_previous.matrix(), m_current.matrix(), interpolate);
		}
		glm::vec3& pos() { return m_current.pos; }
		glm::quat& rot() { return m_current.rot; }
		glm::vec3& scale() { return m_current.scale; }

		void sync_fixed_update() override;

		void set_position(const glm::vec3& pos);
		void set_position(float x, float y, float z);
		void translate(const glm::vec3& offset);
		void translate(float x, float y, float z);
		void rotate(float angle, const glm::vec3& axis);
		void set_scale(float amount);
		void set_scale(float x, float y, float z);
		void set_rotation(glm::quat&);
		void set_rotation(const glm::vec3& rot);

	private:
		Transform m_current;
		Transform m_previous;
	};
}

