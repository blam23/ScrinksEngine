#include "deferred_renderer.h"

#include "editor_ui.h"
#include "debug.h"
#include "errors.h"
#include "shader.h"
#include "texture.h"
#include "render_target.h"

#include <iostream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

using namespace scrinks;

bool Renderer::s_vsync{ true };
bool Renderer::s_resized{ false };
GLFWwindow* Renderer::s_window{ nullptr };
Renderer::ViewResizeCallback Renderer::s_viewResizeCallback{ nullptr };
std::unique_ptr<render::GBuffer> Renderer::s_gBuffer{ nullptr };
GLenum Renderer::s_log_level{ GL_DEBUG_SEVERITY_LOW };

int Renderer::s_windowWidth { 0 };
int Renderer::s_windowHeight{ 0 };
int Renderer::s_viewWidth   { 0 };
int Renderer::s_viewHeight  { 0 };

// Used to combine our buffers into one image
std::unique_ptr<render::RenderTarget> renderedImage;
std::shared_ptr<render::Shader> lightingShader;
std::shared_ptr<render::Shader> renderShader;
GLuint emptyVAO{ 0 };

glm::mat4 projection{};

std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;

static void window_size_callback(GLFWwindow*, int width, int height)
{
    Renderer::s_windowWidth = width;
    Renderer::s_windowHeight = height;
}

void Renderer::handle_resize()
{
    if (s_viewWidth == 0)
    {
        s_viewWidth = s_windowWidth;
        s_viewHeight = s_windowHeight;
    }

    Renderer::s_gBuffer = std::make_unique<render::GBuffer>(Renderer::s_viewWidth, Renderer::s_viewHeight);
    renderedImage = std::make_unique<render::RenderTarget>("viewport", render::BufferFormat::rgb, Renderer::s_viewWidth, Renderer::s_viewHeight);
    projection = glm::perspective(glm::radians(90.0f), (float)s_viewWidth / (float)s_viewHeight, 0.1f, 100.0f);

    s_resized = false;
}

bool Renderer::setup_imgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->AddFontFromFileTTF("assets/fonts/fira.otf", 16);
    ImGui_ImplGlfw_InitForOpenGL(s_window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    return true;
}

bool Renderer::setup_glfw(int width, int height, const std::string& name)
{
    glfwSetErrorCallback(errors::glfw_error_callback);

    if (!glfwInit())
    {
        std::cerr << "GLFW Init failed" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    s_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (s_window == nullptr)
        return false;
    glfwMakeContextCurrent(s_window);
    set_vsync(true);

    if (gl3wInit())
    {
        std::cerr << "Failed to initialise OpenGL" << std::endl;
        return false;
    }

    if (!gl3wIsSupported(3, 2))
    {
        std::cerr << "Required OpenGL version 3.2 is not supported on this platform." << std::endl;
        return false;
    }

    glfwSetWindowSizeCallback(s_window, window_size_callback);

    glDebugMessageCallback(errors::gl_print_error_callback, &s_log_level);
    glEnable(GL_DEBUG_OUTPUT);

    return true;
}

bool Renderer::init(int width, int height, const std::string& name)
{
    Renderer::s_windowWidth = width;
    Renderer::s_windowHeight = height;

    if (!setup_glfw(width, height, name))
    {
        std::cerr << "Couldn't create OpenGL context, exiting application." << std::endl;
        exit(1);
    }

    if (!setup_imgui())
    {
        std::cerr << "Failed to initialise GUI, exiting application." << std::endl;
        exit(2);
    }

    handle_resize();

    lightingShader = render::ShaderManager::instance().load_and_store(
        "deferred_lighting_pass", { "assets/shaders/lighting_pass.vs", "assets/shaders/lighting_pass.fs" }
    );
    renderShader = render::ShaderManager::instance().load_and_store(
        "deferred_render", { "assets/shaders/render_single.vs", "assets/shaders/render_single.fs" }
    );

    const unsigned int NR_LIGHTS = 32;
    srand(13);
    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        // calculate slightly random offsets
        float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
        float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
        float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
        // also calculate random color
        float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        lightColors.push_back(glm::vec3(rColor, gColor, bColor));
    }


    render::TextureManager::instance().load_and_store("whytho", "assets/textures/whytho.png");
    render::TextureManager::instance().load_and_store("whytho_n", "assets/textures/whytho_n.png");
    render::TextureManager::instance().load_and_store("whytho_s", "assets/textures/whytho_s.png");
    render::TextureManager::instance().load_and_store("mc", "assets/textures/mc_text.png");

    glGenVertexArrays(1, &emptyVAO);

    editor::init();

    return true;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void test_quad()
{
    if (quadVAO == 0)
    {
        float vertices[]{
            // pos      // tex
            0.0f, 2.0f, 0.0f, 1.0f,
            2.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 2.0f, 0.0f, 1.0f,
            2.0f, 2.0f, 1.0f, 1.0f,
            2.0f, 0.0f, 1.0f, 0.0f
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    }

    auto textureA{ render::TextureManager::instance().get("whytho") };
    if (!textureA)
        return;

    auto textureN{ render::TextureManager::instance().get("whytho_n") };
    if (!textureN)
        return;

    auto textureD{ render::TextureManager::instance().get("whytho_s") };
    if (!textureD)
        return;

    if (renderShader)
    {
        if (renderShader->is_outdated())
            renderShader = render::ShaderManager::instance().get("deferred_render");

        renderShader->use_program();
    }

    textureA->bind(GL_TEXTURE0);
    textureN->bind(GL_TEXTURE1);
    textureD->bind(GL_TEXTURE2);
    renderShader->set_param("albedoTex", 0);
    renderShader->set_param("normalTex", 1);
    renderShader->set_param("depthTex", 2);
    renderShader->set_param("projection", projection);

    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::rotate(model, 0.75f, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model,
        glm::vec3
        (
            -2.0f,
            -4.0f,
            -2.0f
        ));

    renderShader->set_param("model", model);
    renderShader->set_param("projection", projection);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer::render_scene()
{
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, s_viewWidth, s_viewHeight);

    Renderer::s_gBuffer->bind_write();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    test_quad();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Renderer::render_gbuffer()
{
    glDisable(GL_DEPTH_TEST);

    renderedImage->bind_write();
    glClearColor(0.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    Renderer::s_gBuffer->bind_textures();

    if (lightingShader)
    {
        if (lightingShader->is_outdated())
            lightingShader = render::ShaderManager::instance().get("deferred_lighting_pass");

        lightingShader->use_program();
    }

    lightingShader->set_param("gPosition", 0);
    lightingShader->set_param("gNormal", 1);
    lightingShader->set_param("gAlbedoSpec", 2);

    // send light relevant uniforms
    for (unsigned int i = 0; i < lightPositions.size(); i++)
    {
        lightingShader->set_param("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
        lightingShader->set_param("lights[" + std::to_string(i) + "].Color", lightColors[i]);
        // update attenuation parameters and calculate radius
        const float linear = 0.7f;
        const float quadratic = 1.8f;
        lightingShader->set_param("lights[" + std::to_string(i) + "].Linear", linear);
        lightingShader->set_param("lights[" + std::to_string(i) + "].Quadratic", quadratic);
    }
    lightingShader->set_param("viewPos", glm::vec3(0.0f, 0.0f, 0.0f));

    glBindVertexArray(emptyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void scrinks::Renderer::resize_view_if_needed(int currentWidth, int currentHeight)
{
    if (Renderer::s_viewWidth != currentWidth || Renderer::s_viewHeight != currentHeight)
        Renderer::resize_view(currentWidth, currentHeight);
}

void scrinks::Renderer::run_loop()
{
    while (!glfwWindowShouldClose(s_window))
    {
        glfwPollEvents();

        if (s_resized)
            handle_resize();

        render_scene();
        render_gbuffer();
        editor::render_ui(renderedImage->tex_id());

        glfwSwapBuffers(s_window);
    }
}

void scrinks::Renderer::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(s_window);
    glfwTerminate();
}

void Renderer::set_vsync(bool enabled)
{
    s_vsync = enabled;
    glfwSwapInterval(enabled ? 1 : 0);
}

bool Renderer::is_vsync_enabled()
{
	return s_vsync;
}

void scrinks::Renderer::set_resize_callback(ViewResizeCallback callback)
{
    s_viewResizeCallback = callback;
}

void Renderer::resize_view(int width, int height)
{
    Renderer::s_viewWidth = width;
    Renderer::s_viewHeight = height;
    Renderer::s_resized = true;

    if (s_viewResizeCallback)
        s_viewResizeCallback(width, height);
}
