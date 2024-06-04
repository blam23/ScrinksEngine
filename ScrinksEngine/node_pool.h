#pragma once

#ifdef NODE_POOL_TEST

#include "helpers.h"
#include <array>

namespace scrinks::core
{
	// Not thread safe - should be used per thread instead.
	template <class T, size_t Size>
	class Pool
	{
	public:
		DISABLE_COPY_AND_MOVE(Pool);

		Pool() {}

		template <
			class... T_Args,
			typename = std::enable_if_t<std::is_base_of<Node, T>::value>
		>
		std::pair<size_t, T*> reserve(T_Args&&... args)
		{
			const auto ptr{ get_next() };
			T* ret{ &m_storage[ptr] };
			std::construct_at(ret, args...);
			return { ptr, ret };
		}

		void release(size_t idx)
		{
			m_free.push_back(idx);
		}
		
	private:
		size_t get_next()
		{
			if (m_free.size() > 0)
			{
				size_t ret{ m_free.front() };
				m_free.pop_front();
				return ret;
			}

			return m_ptr++;
		}

		T m_storage[Size]{};
		std::list<size_t> m_free{};
		size_t m_ptr;
	};
}

#endif
