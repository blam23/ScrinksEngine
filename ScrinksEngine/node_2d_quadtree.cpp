#pragma once

#include "node_2d_quadtree.h"
#include "spdlog/spdlog.h"

using namespace scrinks::core::nodes;

QuadTree2D::QuadTree2D(Boundary boundary, size_t capacity)
	: m_boundary { boundary }, m_capacity{ capacity }
{
}

auto QuadTree2D::add_node(Node2D* node) -> void
{
	if (!node)
	{
		spdlog::warn("Attempting to add null node to QuadTree.");
		return;
	}

	if (!in_boundary(node))
		return;

	if (m_nodes.size() < m_capacity)
	{
		m_nodes.push_back(node);
	}
	else
	{
		if (!divided())
			split();

		if (m_upper_left->in_boundary(node))
			m_upper_left->add_node(node);
		else if (m_upper_right->in_boundary(node))
			m_upper_right->add_node(node);
		else if (m_bottom_left->in_boundary(node))
			m_bottom_left->add_node(node);
		else if (m_bottom_right->in_boundary(node))
			m_bottom_right->add_node(node);
	}
}

auto QuadTree2D::in_boundary(Node2D* node) const -> bool
{
	return m_boundary.contains(node);
}

auto QuadTree2D::intersecting(const Boundary& range, std::vector<Node2D*>& found) const -> void
{
	if (!m_boundary.intersects(range))
		return;

	for (const auto n : m_nodes)
	{
		if (range.contains(n))
			found.push_back(n);
	}

	if (divided())
	{
		m_upper_left->intersecting(range, found);
		m_upper_right->intersecting(range, found);
		m_bottom_left->intersecting(range, found);
		m_bottom_right->intersecting(range, found);
	}
}

auto QuadTree2D::clear() -> void
{
	m_nodes.clear();
	m_upper_left = nullptr;
	m_upper_right = nullptr;
	m_bottom_left = nullptr;
	m_bottom_right = nullptr;
}

auto QuadTree2D::split() -> void
{
	const auto x{ m_boundary.x };
	const auto y{ m_boundary.y };
	const auto w{ m_boundary.w };
	const auto h{ m_boundary.h };

	m_upper_left = std::make_unique<QuadTree2D>(Boundary{ x, y, w / 2, h / 2 }, m_capacity);
	m_upper_right = std::make_unique<QuadTree2D>(Boundary{ x + w / 2, y, w / 2, h / 2 }, m_capacity);
	m_bottom_left = std::make_unique<QuadTree2D>(Boundary{ x, y + h / 2, w / 2, h / 2 }, m_capacity);
	m_bottom_right = std::make_unique<QuadTree2D>(Boundary{ x + w / 2, y + h / 2, w / 2, h / 2 }, m_capacity);
}

auto QuadTree2D::Boundary::contains(Node2D* node) const -> bool
{
	if (!node)
	{
		spdlog::warn("Attempting to boundary check null node.");
		return false;
	}

	const auto& pos{ node->pos() };
	return pos.x >= x && pos.x <= x + w && pos.y >= y && pos.y <= y + h;
}

auto QuadTree2D::Boundary::intersects(const Boundary& other) const -> bool
{
	return other.w + w + x > other.x
		&& other.h + h + y > other.y
		&& x <= other.w + w + other.x
		&& y <= h + other.h + other.y;
}

QuadTree2DNode::QuadTree2DNode(Node* parent, QuadTree2D::Boundary boundary, size_t capacity)
	: Node{ parent }
	, QuadTree2D{ boundary, capacity }
{
}

void scrinks::core::nodes::QuadTree2DNode::sync_fixed_update()
{
	recalculate();
	Node::sync_fixed_update();
}

auto QuadTree2DNode::child_added(Node& child) -> void
{
	const auto node2d{ reinterpret_cast<Node2D*>(&child) };
	if (node2d)
		m_node2d_children.push_back(node2d);
}

auto QuadTree2DNode::child_removed(Node& child) -> void
{
	const auto node2d{ reinterpret_cast<Node2D*>(&child) };
	if (node2d)
	{
		auto it = std::find(m_node2d_children.begin(), m_node2d_children.end(), node2d);
		if (it != m_node2d_children.end()) {
			m_node2d_children.erase(it);
		}
	}
}

auto QuadTree2DNode::recalculate() -> void
{
	QuadTree2D::clear();
	for (const auto n : m_node2d_children)
		QuadTree2D::add_node(n);
}

auto scrinks::core::nodes::QuadTree2DNode::query(float x, float y, float w, float h) const -> std::vector<Node2D*>
{
	std::vector<Node2D*> ret{};
	intersecting(Boundary{ x, y, w, h }, ret);
	return ret;
}

void QuadTree2DNode::setup_script_data()
{
}
