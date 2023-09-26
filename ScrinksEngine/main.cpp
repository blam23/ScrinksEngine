#include "deferred_renderer.h"

int main()
{
    if (scrinks::Renderer::init(1024, 720, "Test"))
        scrinks::Renderer::run_loop();
    
    scrinks::Renderer::shutdown();
}
