#pragma once

# define DISABLE_COPY(Class) \
	Class(const Class &) = delete; \
	Class &operator=(const Class&) = delete;

# define DISABLE_MOVE(Class) \
	Class(Class&&) = delete; \
	Class &operator=(Class&&) = delete;

# define DISABLE_COPY_AND_MOVE(Class) \
	DISABLE_COPY(Class) \
	DISABLE_MOVE(Class)

template <typename T>
class Badge {
	friend T;
	Badge() = default;
};

