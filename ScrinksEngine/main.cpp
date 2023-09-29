#include "window.h"
#include "lua_engine.h"

int main()
{
    if (!scrinks::lua::setup())
        return 1;

    if (scrinks::Window::init(1920, 1080, "Scrinks!"))
        scrinks::Window::run_loop();
    
    scrinks::Window::shutdown();

    return 0;
}
