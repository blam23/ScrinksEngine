#include "node_sprite.h"
#include "game.h"

using namespace scrinks::core::nodes;

Sprite::Sprite(Node* parent, float tileIndex, float x, float y)
	: Node2D{ parent, { { x, y }, { 1.0f, 1.0f }, 0 } }
	, m_tileIndex{ tileIndex }
{
	scrinks::render::SpriteInstance si;
	fill_sprite_data(si, 0.0f);

	m_index = Game::sprite_renderer()->push(si);

	s_sprites.push_back(this);
}

Sprite::~Sprite()
{
	scrinks::render::SpriteInstance si{ -1000, -1000, -1 };
	Game::sprite_renderer()->update_instance(m_index, si);

	s_sprites.erase(std::find(s_sprites.begin(), s_sprites.end(), this));
}

void Sprite::fill_sprite_data(scrinks::render::SpriteInstance& out, float interpolate) const
{
	Transform2D tf;
	get_transform(interpolate, tf);

	// todo: rotation & scale
	out.x = tf.pos.x;
	out.y = tf.pos.y;
	out.tileIndex = m_tileIndex;
}

void Sprite::setup_script_data()
{
	m_script_env["self"] = this;

	Node2D::setup_script_data();
}

//void Sprite::fixed_update()
//{
//	Node2D::fixed_update();
//}

void Sprite::update_sprite_data(float interpolate)
{
	scrinks::render::SpriteInstance si;

	for (const auto* sprite : s_sprites)
	{
		sprite->fill_sprite_data(si, interpolate);
		Game::sprite_renderer()->update_instance(sprite->m_index, si);
	}

	Game::sprite_renderer()->bind_instance_data();
}
