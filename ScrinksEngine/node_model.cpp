#include "node_model.h"

using namespace scrinks::core::nodes;


DynamicModel::DynamicModel(Node* parent, std::shared_ptr<render::Model> model, const Transform& transform)
	: m_model{ model }
	, m_visible{ true }
	, Node3D{ parent, transform }
{
}

void DynamicModel::draw(std::shared_ptr<render::Shader> shader, float interpolate)
{
	shader->set_param<const glm::mat4&>("model", mat4(interpolate));
	m_model->draw(shader);
}

void DynamicModel::setup_script_data()
{
	m_script_env["self"] = this;
	Node3D::setup_script_data();
}

