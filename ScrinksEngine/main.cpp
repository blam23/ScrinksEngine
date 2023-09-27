#include "window.h"

int main()
{
    if (scrinks::Window::init(1920, 1080, "Scrinks!"))
        scrinks::Window::run_loop();
    
    scrinks::Window::shutdown();
}
