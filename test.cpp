#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <tinyfiledialogs.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// define ImGui docking
#define IMGUI_ENABLE_DOCKING
// main ImGui file
#include <imgui/imgui.h>
// ImGui backend files for OpenGL and GLFW
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <filesystem.h>
//#include <SrcXEngine/srcx.hpp>
#include <SrcXEngine/shader.h>
#include <SrcXEngine/camera.h>
#include <SrcXEngine/model.hpp>
//#include <SrcXEngine/objects/entity.hpp>

#include <cstdlib>
#include <cctype>
#include <iostream>
#include <random>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path, bool gamma_correction);
unsigned int loadCubemap(vector<std::string> faces, bool gamma_correction);
glm::vec3 floatToVec3(float v[3]);
glm::vec2 floatToVec2(float v[2]);
glm::vec4 floatToVec4(float v[4]);
void loadShaderUniforms(Shader shader);
std::string toString(const char* v);
char* stringToChar(std::string v);
void addToArr(const char* arr[], char* v);

void renderQuad();
void renderSphere();

// settings
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1040;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// menu mode bool to check if user is testing the scene or not
bool menuMode = true;
bool sceneFocused = false;
bool tabPressed = false;

// aspect ratio
glm::vec2 aspect = glm::vec2(16.0f, 9.0f);

// shader paths list
const char* vertexPaths[] = {
    "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/pbr/pbr.vert",
    "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/light/light.vert"
};

const char* geometryPaths[] = {
    "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/pbr/pbr.geom",
    "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/light/light.geom"
};

const char* fragmentPaths[] = {
    "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/pbr/pbr.frag",
    "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/light/light.frag"
};

// filters for shader extensions
const char* shaderFilters[] = { "*.*" };

// uniform struct
struct Uniform {
    char name[256];
    char fName[256];
    GLenum type;
    GLint location;
};
std::vector<Uniform> uniforms;

// uniform value struct
struct UniformValue {
    float floatValue;
    glm::vec3 vec3;
    glm::vec2 vec2;
    glm::vec4 vec4;
};
std::unordered_map<std::string, UniformValue> uniformValues;

GLint uniformCount;

int main() {
    stbi_set_flip_vertically_on_load(true);
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // hint GLFW to use 4x buffer (for 4x MSAA)
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ghgt", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // build and compile shaders
    // -------------------------
    //Shader shader("shaders/pbr/pbr.vert", "shaders/pbr/pbr.frag", "shaders/pbr/pbr.geom");
    Shader lightShader("/Users/vyacheslav/OpenGL_C++_test_scene/shaders/light/light.vert", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/light/light.frag", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/light/light.geom");
    Shader equirectangularShader("/Users/vyacheslav/OpenGL_C++_test_scene/shaders/equirectangular/skybox.vert", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/equirectangular/skybox.frag", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/equirectangular/skybox.geom");
    Shader convShader("/Users/vyacheslav/OpenGL_C++_test_scene/shaders/convolution/skybox copy.vert", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/convolution/skybox copy.frag", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/convolution/skybox copy.geom");
    Shader brdfShader("/Users/vyacheslav/OpenGL_C++_test_scene/shaders/brdf/brdf.vert", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/brdf/brdf.frag", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/brdf/brdf.geom");
    Shader prefilterShader("/Users/vyacheslav/OpenGL_C++_test_scene/shaders/pre-filter/pre-filter.vert", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/pre-filter/pre-filter.frag", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/pre-filter/pre-filter.geom");
    Shader skyboxShader("/Users/vyacheslav/OpenGL_C++_test_scene/shaders/skybox/skybox.vert", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/skybox/skybox.frag", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/skybox/skybox.geom");
    Shader hdrShader("/Users/vyacheslav/OpenGL_C++_test_scene/shaders/hdr/hdr.vert", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/hdr/hdr.frag", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/hdr/hdr.geom");
    Shader depthShader("/Users/vyacheslav/OpenGL_C++_test_scene/shaders/depth/depth.vert", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/depth/depth.frag", "/Users/vyacheslav/OpenGL_C++_test_scene/shaders/depth/depth.geom");
    Shader blurShader("shaders/blur/blur.vert", "shaders/blur/blur.frag", "shaders/blur/blur.geom");
    Shader bloomShader("shaders/bloom/bloom.vert", "shaders/bloom/bloom.frag", "shaders/bloom/bloom.geom");
    Shader g_bufferShader("shaders/g_buffer/g_buffer.vert", "shaders/g_buffer/g_buffer.frag", "shaders/g_buffer/g_buffer.geom");
    Shader gLightingShader("shaders/gLighting/gLighting.vert", "shaders/gLighting/gLighting.frag", "shaders/gLighting/gLighting.geom");
    Shader ssaoShader("shaders/ssao/ssao.vert", "shaders/ssao/ssao.frag", "shaders/ssao/ssao.geom");
    Shader ssaoBlurShader("shaders/ssao/ssao.vert", "shaders/ssao/ssaoBlur.frag", "shaders/ssao/ssao.geom");

    Model backpack(FileSystem::getPath("resources/backpack/backpack.obj").c_str());

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    float cubeVertices2[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    float lightVertices[] = {
        // Front face
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,

        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        // Back face
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,

        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,

        // Left face
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,

        // Right face
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,

        // Top face
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,

        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        // Bottom face
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,

        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f
    };

    /*float floorVertices[] = {
        // pos               // normal         // texCoords // tangent       // bitangent
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, // bottom left
        1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, // bottom right
        1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, // top right

        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, // bottom left (again)
        1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, // top right (again)
        -1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f  // top left
    };*/

    float floorVertices[] = {
      // position            normal              tex coords
        -1.0f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
         1.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,

        -1.0f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
         1.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
         1.0f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f
    };

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices2), &cubeVertices2, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    // floor VAO
    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3  * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6  * sizeof(float)));
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // light VAO
    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), &lightVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // generate depth map framebuffer
    /*unsigned int depthCubemapFBO;
    glGenFramebuffers(1, &depthCubemapFBO);

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, depthCubemapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    unsigned int depthRBO;
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);

    unsigned int colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);


    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    GL_RENDERBUFFER, captureRBO);

    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load textures
    // -------------
    unsigned int cubeTexture = loadTexture(FileSystem::getPath("resources/textures/container.jpg").c_str(), false);
    unsigned int floorTexture = loadTexture(FileSystem::getPath("resources/textures/floor_light_planks.jpg").c_str(), false);
    unsigned int brickTexture = loadTexture(FileSystem::getPath("resources/textures/wood.png").c_str(), true);
    unsigned int normalTexture = loadTexture(FileSystem::getPath("resources/textures/toy_box_normal.png").c_str(), false);
    unsigned int dispTexture = loadTexture(FileSystem::getPath("resources/textures/toy_box_disp.png").c_str(), false);

    // load PBR textures
    // -----------------
    stbi_set_flip_vertically_on_load(false);
    unsigned int albedoTex = loadTexture(FileSystem::getPath("resources/textures/earth/earth_21k.jpg").c_str(), false);
    unsigned int metallicTex = loadTexture(FileSystem::getPath("resources/textures/earth/earth_metallic.jpg").c_str(), false);
    unsigned int roughnessTex = loadTexture(FileSystem::getPath("resources/textures/earth/earth_metallic.jpg").c_str(), false);
    stbi_set_flip_vertically_on_load(false);

    // load height map
    unsigned int heightMap = loadTexture(FileSystem::getPath("resources/textures/ukraine_height-2.png").c_str(), false);

    vector<std::string> faces1
    {
        FileSystem::getPath("resources/textures/skybox/right.jpg"),
        FileSystem::getPath("resources/textures/skybox/left.jpg"),
        FileSystem::getPath("resources/textures/skybox/top.jpg"),
        FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
        FileSystem::getPath("resources/textures/skybox/front.jpg"),
        FileSystem::getPath("resources/textures/skybox/back.jpg")
    };
    vector<std::string> faces2
    {
        FileSystem::getPath("resources/textures/Standard-Cube-Map/right.png"),
        FileSystem::getPath("resources/textures/Standard-Cube-Map/left.png"),
        FileSystem::getPath("resources/textures/Standard-Cube-Map/top.png"),
        FileSystem::getPath("resources/textures/Standard-Cube-Map/bottom.png"),
        FileSystem::getPath("resources/textures/Standard-Cube-Map/front.png"),
        FileSystem::getPath("resources/textures/Standard-Cube-Map/back.png")
    };
    stbi_set_flip_vertically_on_load(false);
    unsigned int cubemapTexture = loadCubemap(faces1, true);
    stbi_set_flip_vertically_on_load(true);

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    const char* oldHdrPath = "/Users/vyacheslav/Downloads/newport_loft.hdr";
    const char* hdrPath = "/Users/vyacheslav/Downloads/newport_loft.hdr";
    float *data = stbi_loadf(hdrPath, &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data) {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    stbi_set_flip_vertically_on_load(false);

    // shader configuration
    // --------------------
    /*shader.use();
    shader.setInt("albedoMap", 0);
    shader.setInt("metallicMap", 1);
    shader.setInt("roughnessMap", 2);
    shader.setInt("irradianceMap", 3);
    shader.setInt("prefilterMap", 4);
    shader.setInt("brdfLUT", 5);*/

    /*shader.setBool("flipHor", false);

    shader.setBool("invertMetallic", true);
    shader.setBool("invertRoughness", true);*/

    //shader.setInt("aoMap", 3);
    //shader.setInt("shadowMap", 1);
    //depthShader.use();
    //depthShader.setInt("depthMap", 0);

    hdrShader.use();
    hdrShader.setInt("hdrTex", 0);

    blurShader.use();
    blurShader.setInt("image", 0);

    bloomShader.use();
    bloomShader.setInt("scene", 0);
    bloomShader.setInt("bloomBlur", 1);

    gLightingShader.use();
    gLightingShader.setInt("gPosition", 0);
    gLightingShader.setInt("gNormal", 1);
    gLightingShader.setInt("gAlbedoSpec", 2);
    gLightingShader.setInt("ssao", 3);

    ssaoShader.use();
    ssaoShader.setInt("gPosition", 0);
    ssaoShader.setInt("gNormal", 1);
    ssaoShader.setInt("texNoise", 2);

    ssaoBlurShader.use();
    ssaoBlurShader.setInt("ssaoInput", 0);

    //skyboxShader.use();
    //skyboxShader.setInt("skybox", 0);

    /*glm::vec3 lightPositions[] = {
        glm::vec3( 0.0f,  0.0f,  3.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f),
        glm::vec3( 0.0f,  3.0f,  0.0f),
        glm::vec3( 0.0f, -3.0f,  0.0f),
        glm::vec3( 3.0f,  0.0f,  0.0f),
        glm::vec3(-3.0f,  0.0f,  0.0f)
    };*/

    /*glm::vec3 lightPositions[] = {
        glm::vec3(0.0f, 2.0f, 0.0f)
    };

    glm::vec3 lightColors[] = {
        glm::vec3(1.0f)
    };*/
    
    glm::vec3 backpackPositions[] = {
        glm::vec3(-5.0f,  0.0f, -6.0f),
        glm::vec3( 0.0f,  0.0f, -6.0f),
        glm::vec3( 5.0f,  0.0f, -6.0f),

        glm::vec3(-5.0f,  0.0f,  0.0f),
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 5.0f,  0.0f,  0.0f),

        glm::vec3(-5.0f,  0.0f,  6.0f),
        glm::vec3( 0.0f,  0.0f,  6.0f),
        glm::vec3( 5.0f,  0.0f,  6.0f)
    };

    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // generate noise texture
    // ----------------------
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }

    unsigned int noiseTexture; glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    float lightPos[] = { 0.0f, 0.0f, 3.0f };
    float lightColor[] = { 1.0f, 1.0f, 1.0f };

    glm::mat4 captureProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f, 0.0f, 0.0f),  glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),  glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    equirectangularShader.use();
    equirectangularShader.setInt("equirectangularMap", 0);
    equirectangularShader.setMat4("projection", captureProj);
    equirectangularShader.setFloat("exposure", 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i) {
        equirectangularShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    convShader.use();
    convShader.setInt("environmentMap", 0);
    convShader.setMat4("projection", captureProj);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i) {
        convShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    prefilterShader.use();
    prefilterShader.setInt("environmentMap", 0);
    prefilterShader.setMat4("projection", captureProj);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
        unsigned int mipWidth = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i) {
            prefilterShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(skyboxVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF(FileSystem::getPath("resources/fonts/sans-serif.ttf").c_str(), 18.0f);
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    // Object properties
    // Material
    float albedo[3];
    float metallic = 0.0f;
    float roughness = 0.0f;
    // Transform
    float position[3] = { 0.0f, 0.0f, 0.0f };
    float rotation[3] = { 0.0f, 0.0f, 0.0f };
    float scale[3] = { 1.0f, 1.0f, 1.0f };
    // Mesh renderer
    bool renderMesh = true;

    glm::mat4 lastView = camera.GetViewMatrix();

    const char* items[] = { "PBR" };
    static int currentItem = 0;
    static int lastItem = -1;

    Shader shader(vertexPaths[currentItem], fragmentPaths[currentItem], geometryPaths[currentItem]);

    bool isLightingEnvironment = false;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // load shader and its uniforms based on item selected in dropdown and assign texture IDs to it
        if (currentItem != lastItem) {
            shader = Shader(vertexPaths[currentItem], fragmentPaths[currentItem], geometryPaths[currentItem]);
            loadShaderUniforms(shader);
        }

        lastItem = currentItem;

        shader.use();
        shader.setInt("albedoMap", 0);
        shader.setInt("metallicMap", 1);
        shader.setInt("roughnessMap", 2);
        shader.setInt("irradianceMap", 3);
        shader.setInt("prefilterMap", 4);
        shader.setInt("brdfLUT", 5);

        // input
        // -----
        processInput(window);

        glfwSetInputMode(window, GLFW_CURSOR, menuMode ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view;
            if (!menuMode) {
                view = camera.GetViewMatrix();
                lastView = view;
            } else {
                view = lastView;
            }

            shader.use();
            shader.setMat4("projection", projection);
            shader.setMat4("view", view);

            /*Transform backpackT(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
            shader.setMat4("model", backpackT.model);
            shader.setVec3("lightPos[0]", glm::vec3(0.0f, 0.0f, 3.0f));
            backpack.Draw(shader);*/

            shader.setVec3("albedo", glm::vec3(albedo[0], albedo[1], albedo[2]));
            shader.setFloat("ao", 1.0f);
            for (unsigned int i = 0; i < 1; i++) {
                shader.setVec3("lightPositions[" + std::to_string(i) + "]", glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
                shader.setVec3("lightColors[" + std::to_string(i) + "]", glm::vec3(lightColor[0], lightColor[1], lightColor[2]));
            }

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(position[0], position[1], position[2]));
            model = glm::rotate(model, rotation[0], glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, rotation[1], glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, rotation[2], glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(scale[0], scale[1], scale[2]));
            shader.setMat4("model", model);
            shader.setVec3("camPos", camera.Position);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, albedoTex);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, metallicTex);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, roughnessTex);
            // bind irradiance map
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
            // bind pre-filter map
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
            // bind BRDF LUT texture
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

            if (renderMesh)
                renderSphere();

            /*for (unsigned int i = 0; i < 7; i++) {
                shader.setFloat("metallic", (float)i / 7.0);
                for (unsigned int j = 0; j < 7; j++) {
                    Transform sphere(glm::vec3(j*3, i*3, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
                    shader.setMat4("model", sphere.model);
                    shader.setVec3("camPos", camera.Position);
                    

                    shader.setFloat("roughness", glm::clamp((float)j / 7.0f, 0.05f, 1.0f));

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, albedoTex);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, metallicTex);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, roughnessTex);
                    // bind irradiance map
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
                    // bind pre-filter map
                    glActiveTexture(GL_TEXTURE4);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
                    // bind BRDF LUT texture
                    glActiveTexture(GL_TEXTURE5);
                    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

                    renderSphere();
                }
            }*/

            /*Transform sphere(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
            shader.setMat4("model", sphere.model);
            shader.setVec3("camPos", camera.Position);
                    
            shader.setFloat("metallic", 0.7f);
            shader.setFloat("roughness", 0.2f);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, albedoTex);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, metallicTex);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, roughnessTex);
            // bind irradiance map
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
            // bind pre-filter map
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
            // bind BRDF LUT texture
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

            renderSphere();*/

            // render skybox
            // set depth function to GL_LEQUAL so fragment passes if it is equal to previous value
            glDepthFunc(GL_LEQUAL);
            // setup matrices and other values
            skyboxShader.use();
            //skyboxShader.setFloat("exposure", 1.0);
            skyboxShader.setInt("environmentMap", 0);
            skyboxShader.setMat4("view", view);
            skyboxShader.setMat4("projection", projection);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
                
            glBindVertexArray(skyboxVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // set depth func to GL_LESS back
            glDepthFunc(GL_LESS);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /*glDepthFunc(GL_LEQUAL);
        // setup matrices and other values
        skyboxShader.use();
        skyboxShader.setFloat("exposure", 1.0);
        // always put skybox on the player (camera) position
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthFunc(GL_LESS);*/

        /*hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        renderQuad();*/

        ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));


        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags windowFlags =  ImGuiWindowFlags_NoDocking |
                                        ImGuiWindowFlags_NoTitleBar |
                                        ImGuiWindowFlags_NoCollapse |
                                        ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoBringToFrontOnFocus |
                                        ImGuiWindowFlags_NoNavFocus |
                                        ImGuiWindowFlags_NoBackground;

        ImGui::Begin("MainWindow", nullptr, windowFlags);

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Lighting")) {
                if (ImGui::MenuItem("Environment")) {
                    isLightingEnvironment = true;
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        ImGuiID dockSpaceID = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockSpaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        
        ImGui::End();

        ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoNavFocus);

        ImVec2 ViewportPanelSize = ImGui::GetContentRegionAvail();
        //ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        //ImGui::SetNextWindowSize(ImVec2(300.0f, SCR_HEIGHT));

        ImGui::Image((ImTextureID)(uintptr_t)colorBuffer, ImVec2(ViewportPanelSize.x, ViewportPanelSize.x / (aspect.x / aspect.y)), ImVec2(0, 1), ImVec2(1, 0));
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        std::string fps = std::string("FPS: ") + std::to_string(1.0f/deltaTime);
        drawList->AddText(
            viewport->Pos,
            IM_COL32(255, 255, 255, 255),
            fps.c_str()
        );

        ImGui::End();

        ImGui::Begin("Properties");

        shader.use();
        if (ImGui::CollapsingHeader("Material")) {
            ImGui::Combo("Shader", &currentItem, items, IM_ARRAYSIZE(items));

            if (ImGui::Button("Load vertex shader")) {
                const char* vertexPath = tinyfd_openFileDialog("Load vertex shader file", "", 0, NULL, NULL, 0);
            }

            if (ImGui::Button("Load fragment shader")) {
                const char* fragmentPath = tinyfd_openFileDialog("Load fragment shader", "", 0, NULL, NULL, 0);
            }

            if (ImGui::Button("Load geometry shader")) {
                const char* geometryPath = tinyfd_openFileDialog("Load geometry shader", "", 0, NULL, NULL, 0);
            }
            
            for (unsigned int i = 0; i < uniformCount; ++i) {
                if (uniforms[i].type == GL_FLOAT_MAT4 || uniforms[i].type == GL_FLOAT_MAT3 || uniforms[i].type == GL_FLOAT_MAT2)
                    continue;

                if (uniforms[i].type == GL_FLOAT) {
                    ImGui::DragFloat(uniforms[i].fName, &uniformValues[uniforms[i].name].floatValue, 0.01f, 0.0f, 1.0f);
                    shader.setFloat(std::string() + uniforms[i].name, uniformValues[uniforms[i].name].floatValue);
                }

                if (uniforms[i].type == GL_FLOAT_VEC3) {
                    float val[3] = { uniformValues[uniforms[i].name].vec3.x, uniformValues[uniforms[i].name].vec3.y, uniformValues[uniforms[i].name].vec3.z };
                    ImGui::DragFloat3(uniforms[i].fName, val, 0.1f, -1000.0f, 1000.0f);
                    shader.setVec3(uniforms[i].name, floatToVec3(val));
                    uniformValues[uniforms[i].name].vec3 = floatToVec3(val);
                }

                if (uniforms[i].type == GL_FLOAT_VEC2) {
                    float val[2] = { uniformValues[uniforms[i].name].vec2.x, uniformValues[uniforms[i].name].vec2.y };
                    ImGui::DragFloat2(uniforms[i].fName, val, 0.1f, -1000.0f, 1000.0f);
                    shader.setVec2(uniforms[i].name, floatToVec2(val));
                    uniformValues[uniforms[i].name].vec2 = floatToVec2(val);
                }

                if (uniforms[i].type == GL_FLOAT_VEC4) {
                    float val[4] = { uniformValues[uniforms[i].name].vec4.x, uniformValues[uniforms[i].name].vec4.y, uniformValues[uniforms[i].name].vec4.z, uniformValues[uniforms[i].name].vec4.w };
                    ImGui::DragFloat4(uniforms[i].fName, val, 0.1f, -1000.0f, 1000.0f);
                    shader.setVec4(uniforms[i].name, floatToVec4(val));
                    uniformValues[uniforms[i].name].vec4 = floatToVec4(val);
                }
            }
        }

        if (ImGui::CollapsingHeader("Transform")) {
            ImGui::DragFloat3("Position", position, 0.1f, -1000000000.0f, 1000000000.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::DragFloat3("Rotation", rotation, 0.1f, -180.0f, 180.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::DragFloat3("Scale", scale, 0.1f, -1000000000.0f, 1000000000.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        }

        if (ImGui::CollapsingHeader("Mesh")) {
            ImGui::Checkbox("Render mesh", &renderMesh);
        }

        if (ImGui::CollapsingHeader("Scripts")) {
            // TODO: add "Load script" button (for custom C++ scripts)
        }

        ImGui::End();


        ImGui::Begin("Hierarchy");

        if (ImGui::CollapsingHeader("Sphere")) {
            // no children
        }

        ImGui::End();

        // ---------------------
        // MAIN MENU BAR WINDOWS
        // ---------------------

        if (isLightingEnvironment)
        {
            ImGui::Begin("Environment", &isLightingEnvironment);
            if (ImGui::Button("Load HDR texture")) 
            {
                const char* hdrFilter = "*.hdr";
                const char* hdrPath = tinyfd_openFileDialog("Load HDR texture", "", 1, &hdrFilter, "HDR texture", 0);
            }
            ImGui::End();
        }
        

        ImGui::PopStyleColor(3);
        

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // end ImGui session
    // -----------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, &floorVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    //glDeleteBuffers(1, &floorVBO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    // close window if escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // movement controls
    if (!menuMode) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    // toggle cursor
    if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS && !tabPressed) {
        sceneFocused = !sceneFocused;
        if (!sceneFocused) {
            menuMode = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            menuMode = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            ImGui::SetWindowFocus("Scene");
        }
    }
    tabPressed = (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path, bool gamma_correction)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (gamma_correction) {
            if (nrComponents == 3)
                format = GL_SRGB;
            else if (nrComponents == 4)
                format = GL_SRGB_ALPHA;
        } else {
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        if (gamma_correction) {
            if (format == GL_SRGB_ALPHA)
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            else
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces, bool gamma_correction)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            if (gamma_correction) {
                if (nrChannels == 4) {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                } else if (nrChannels == 3) {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                }
            } else {
                if (nrChannels == 4) {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                } else if (nrChannels == 3) {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                }
            }
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void renderQuad() {
    float quadVertices[] = {
      // positions           tex coords
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f
    };

    //glDisable(GL_DEPTH_TEST);
    //glDepthMask(GL_FALSE);

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    //glEnable(GL_DEPTH_TEST);
    //glDepthMask(GL_TRUE);
}

unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);           
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);        
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));        
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

glm::vec3 floatToVec3(float v[3]) {
    return glm::vec3(v[0],  v[1], v[2]);
}

glm::vec2 floatToVec2(float v[2]) {
    return glm::vec2(v[0], v[1]);
}

glm::vec4 floatToVec4(float v[4]) {
    return glm::vec4(v[0],  v[1], v[2], v[3]);
}

// store every uniform from loaded shader
void loadShaderUniforms(Shader shader) {
    // reset uniforms before loading new ones
    uniformCount = 0;
    uniforms.clear();
    uniformValues.clear();

    glGetProgramiv(shader.ID, GL_ACTIVE_UNIFORMS, &uniformCount);

    for (GLint i = 0; i < uniformCount; ++i) {
        char name[256];
        char oName[256];
        GLsizei length;
        GLint size;
        GLenum type;

        glGetActiveUniform(shader.ID, i, 256, &length, &size, &type, oName);

        GLint location = glGetUniformLocation(shader.ID, oName);

        // name formating (start from uppercase letter, put spaces between words)
        strcpy(name, oName);
        name[0] = static_cast<char>(std::toupper(name[0]));

        std::string sName = toString(name);
        for (unsigned int i = 0; i < strlen(name); i++) {
            if (std::isupper(sName[i+1]) && sName[i] != ' ')
                sName.insert(i+1, 1, ' ');
        }

        const char* cName = name;

        Uniform uniform;
        cName = sName.c_str();
        strcpy(uniform.name, oName);
        strcpy(uniform.fName, cName);
        uniform.type = type;
        uniform.location = location;
        uniforms.push_back(uniform);

        uniformValues[name] = UniformValue{ 0.0f };
    }
}

std::string toString(const char* v) {
    std::string r;
    for (unsigned int i = 0; i < strlen(v); i++)
        r.insert(i, 1, v[i]);

    return r;
}

char* stringToChar(std::string v) {
    char* r;
    for (unsigned int i = 0; i < v.size(); i++)
        r[i] = v[i];
    return r;
}

void addToArr(const char* arr[], char* v) {
    std::vector<std::string> sarr;
    for (size_t i = 0; i < sizeof(arr)/sizeof(arr[0]); i++)
        sarr.push_back(toString(arr[i]));

    sarr.push_back(toString(v));

    const char** rarr = new const char*[sarr.size()];

    for (size_t i = 0; i < sarr.size(); i++)
        rarr[i] = sarr[i].c_str();
  
    for (size_t i = 0; i < sarr.size(); i++)
        arr[i] = rarr[i];
}
