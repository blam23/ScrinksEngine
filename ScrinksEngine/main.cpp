#include "window.h"
#include "thread_pool.h"

using namespace scrinks;

int main()
{
    if (Window::init(1240, 720, "Scrinks!", "examples/space/project.lua"))
        Window::run_loop();
    
    Window::shutdown();

    return 0;
}


