#include "node_root.h"

using namespace scrinks::core;

void nodes::Root::start_game()
{
	load_script();
	run_func_checked("init");
}

