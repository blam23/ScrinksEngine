#pragma once

namespace core::threads
{
	enum class Group
	{
		Main,           // High priority, single thread
		Background,     // Low priority, single thread
		Split,          // High priority, any thread
		SplitBackground // Low priority, any thread
	};
}

