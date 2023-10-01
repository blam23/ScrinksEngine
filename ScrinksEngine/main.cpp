#include "window.h"
#include "thread_pool.h"

using namespace scrinks;

int main()
{
    //threads::set_process_priority(threads::Priority::High);
    threads::setup();

    if (Window::init(1920, 1080, "Scrinks!"))
        Window::run_loop();
    
    Window::shutdown();

    return 0;
}
