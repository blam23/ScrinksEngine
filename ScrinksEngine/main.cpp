#include "window.h"
#include "thread_pool.h"

using namespace scrinks;

int main()
{
    threads::setup();

    if (Window::init(1920, 1080, "Scrinks!", "examples/space/project.lua"))
        Window::run_loop();
    
    Window::shutdown();

    return 0;
}


