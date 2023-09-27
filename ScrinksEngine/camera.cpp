#include "camera.h"
#include <glm/ext/matrix_transform.hpp>

using namespace scrinks::render;

static glm::vec3 s_up{ 0.0, -1.0, 0.0 };

void Camera::update_view()
{
	m_view = glm::lookAt(m_pos, m_pos + m_forward, s_up);
}

const glm::mat4& scrinks::render::Camera::view()
{
	if (m_requiresRecalc)
	{
		update_view();
		m_requiresRecalc = false;
	}

	return m_view;
}

FPSCamera::FPSCamera(float sensitivity, float moveSpeed, float minMaxPitch)
	: m_sensitivity{ sensitivity }
	, m_minMaxPitch{ minMaxPitch }
	, m_moveSpeed{ moveSpeed }
	, m_yaw{ 0.0f }
	, m_pitch { 0.0f }
{
	calc_forward();
}

void FPSCamera::calc_forward()
{
	glm::vec3 front
	{
		glm::cos(glm::radians(m_yaw))* glm::cos(glm::radians(m_pitch)),
		glm::sin(glm::radians(m_pitch)),
		glm::sin(glm::radians(m_yaw))* glm::cos(glm::radians(m_pitch))
	};
	m_forward = glm::normalize(front);
	flag_changed();
}

void FPSCamera::process_mouse(float dX, float dY, float dT)
{
	m_yaw   += dX * m_sensitivity * dT;
	m_pitch += dY * m_sensitivity * dT;

	m_pitch = glm::clamp(m_pitch, -m_minMaxPitch, m_minMaxPitch);

	calc_forward();
}

void scrinks::render::FPSCamera::move_relative_to_target(float amount, Direction dir)
{
	switch (dir)
	{
	case Direction::Forward:
		m_pos += m_forward * amount;
		break;
	case Direction::Backward:
		m_pos -= m_forward * amount;
		break;
	case Direction::Left:
		m_pos -= glm::normalize(glm::cross(m_forward, s_up)) * amount;
		break;
	case Direction::Right:
		m_pos += glm::normalize(glm::cross(m_forward, s_up)) * amount;
		break;
	default:
		break;
	}
	flag_changed();
}
