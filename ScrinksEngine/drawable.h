#pragma once

#include "helpers.h"
#include "shader.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

namespace scrinks::render
{
	template <class T>
	class Drawable
	{
	protected:
		Drawable()
		{
			drawList.push_back(static_cast<T*>(this));
		}

		virtual ~Drawable()
		{
			drawList.erase(std::find(drawList.begin(), drawList.end(), this));
		}

	public:
		static void draw_list(std::shared_ptr<Shader> shader, float interpolate)
		{
			for (auto drawable : drawList)
			{
				if (drawable->visible())
					drawable->draw(shader, interpolate);
			}
		}

	private:
		static inline std::vector<T*> drawList{};
	};
}
