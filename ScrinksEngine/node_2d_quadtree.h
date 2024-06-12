#pragma once

#include "node_2d.h"

namespace scrinks::core::nodes
{
	class QuadTree2D
	{
	public:
		struct Boundary
		{
			float x, y, w, h;
			auto contains(Node2D* node) const -> bool;
			auto intersects(const Boundary&) const -> bool;
		};

		QuadTree2D(Boundary, size_t capacity);
		virtual ~QuadTree2D() = default;

		constexpr auto divided() const -> bool { return m_upper_left != nullptr; }

	public:
		auto add_node(Node2D* node) -> void;
		auto clear() -> void;

		auto in_boundary(Node2D* node) const -> bool;
		auto intersecting(const Boundary&, std::vector<Node2D*>& found) const -> void;

	private:
		auto split() -> void;

		std::vector<Node2D*> m_nodes{};
		Boundary m_boundary;
		size_t m_capacity;

		// Quadrants
		std::unique_ptr<QuadTree2D> m_upper_left{ nullptr };
		std::unique_ptr<QuadTree2D> m_bottom_left{ nullptr };
		std::unique_ptr<QuadTree2D> m_upper_right{ nullptr };
		std::unique_ptr<QuadTree2D> m_bottom_right{ nullptr };
	};

	class QuadTree2DNode : public Node, public QuadTree2D
	{
	public:
		QuadTree2DNode(Node* parent, QuadTree2D::Boundary, size_t capacity);
		virtual ~QuadTree2DNode() {}

		virtual auto sync_fixed_update() -> void override;
		virtual auto child_added(Node& child) -> void override;
		virtual auto child_removed(Node& child) -> void override;

		auto recalculate() -> void;

		auto query(float x, float y, float w, float h) const -> std::vector<Node2D*>;

	protected:
		virtual const std::string_view default_name() const { return "QuadTree2D"; }
		virtual void setup_script_data() override;

	private:
		std::vector<Node2D*> m_node2d_children{};
	};

}
