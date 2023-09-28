#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace scrinks::render
{
	enum class Direction
	{
		Forward,
		Backward,
		Left,
		Right,
		Up,
		Down
	};

	class Camera
	{
	public:
		Camera() = default;

		void update_view();
		const glm::mat4& view();
		inline const glm::vec3& pos() const { return m_pos; }
		inline const glm::vec3& forward() const { return m_forward; }
		inline const float fov() const { return m_fov; }
		void set_fov(float value) { m_fov = value; }

	protected:
		inline void flag_changed() { m_requiresRecalc = true; }

	protected:
		//glm::vec3 m_pos{ 0.0, 2.0, 0.0 };
		glm::vec3 m_pos{ -3.342f, 4.76f, -0.695f };
		glm::vec3 m_forward{ 1.0, 0.0, 0.0 };
		glm::mat4 m_view{};
		float m_fov{ 90.0f };
		bool m_requiresRecalc{ false };
	};

	class IsoCamera : public Camera
	{
	public:
		IsoCamera(float scale);
	private:

		float m_scale;
	};

	class FPSCamera : public Camera
	{
	public:
		FPSCamera(float sensitivity = 10.f, float moveSpeed = 10.0f, float minMaxPitch = 89.0f);

		void process_mouse(float dX, float dY, float deltaTime);
		void move_relative_to_target(float amount, Direction dir);

	public:
		float m_sensitivity;
		float m_minMaxPitch;
		float m_moveSpeed;

	private:
		float m_yaw;
		float m_pitch;
		void calc_forward();
	};
}

