#pragma once

#include "node_3d.h"
#include "drawable.h"
#include "model.h"

namespace scrinks::core::nodes
{
	class DynamicModel
		: public render::Drawable<DynamicModel>
		, public Node3D
	{
	public:
		DynamicModel(Node* parent, std::shared_ptr<render::Model>, const Transform&);

		void draw(std::shared_ptr<render::Shader>, float interpolate);
		bool& visible() { return m_visible; }

	protected:
		virtual const std::string_view default_name() const { return "Model"; }

	private:
		std::shared_ptr<render::Model> m_model;
		bool m_visible;
	};
}
