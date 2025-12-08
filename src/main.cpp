#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>

#include <iostream>
#include <map>

void errorExit(std::string msg, int errorReturn);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const std::string& fileName);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH  / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    glfwInit();

    // gets the width and height of the primary monitor
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    if (!primary) errorExit("Failed to get primary monitor", -1);
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    if (!mode) errorExit("Failed to get video mode", -1);

    SCR_WIDTH = mode->width;
    SCR_HEIGHT = mode->height;

    // set glfw hints
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", primary, NULL);
    if (!window) errorExit("Failed to create GLFW window", -1);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        errorExit("Failed to initialize GLAD", -1);
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_ALWAYS); // always pass the depth test
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    glStencilMask(0x00); // disable writing to stencil mask as default
    glEnable(GL_CULL_FACE);

    // build and compile shaders
    Shader shader("depth_testing.vs", "depth_testing.fs");
    Shader outlineShader("depth_testing.vs", "stencil_outline.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float cubeVertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left (ccw)
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         

        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right (ccw)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left (ccw)
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right

        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right (ccw)
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right (ccw)
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left (ccw)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        // Right face
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left (ccw)
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         

        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right (ccw)
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right (ccw)
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left

        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left (ccw)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left (ccw)
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right  

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right (ccw)
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
    };
    float planeVertices[] = {
        // positions          // texture Coords
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f, // close-right (cw)
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f, // close-left
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f, // far-left

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f, // close-right (cw)
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f, // far-left
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f  // far-right								
    };
    float quadVertices[] = {
        // positions          // texture Coords
         0.0f,  0.5f,  0.0f,  0.0f, 0.0f, // top-left (cw)
         1.0f, -0.5f,  0.0f,  1.0f, 1.0f, // bottom-right
         0.0f, -0.5f,  0.0f,  0.0f, 1.0f, // bottom-left

         0.0f,  0.5f,  0.0f,  0.0f, 0.0f, // top-left (ccw)
         1.0f, -0.5f,  0.0f,  1.0f, 1.0f, // bottom-right
         1.0f,  0.5f,  0.0f,  1.0f, 0.0f  // top-right							
    };

    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    // quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // load textures
    unsigned int cubeTexture  = loadTexture("marble.jpg");
    unsigned int floorTexture = loadTexture("metal.png");
    unsigned int vegitationTexture = loadTexture("grass.png");
    unsigned int windowTexture = loadTexture("window.png");

    // shader configuration
    shader.use();
    shader.setInt("texture1", 0);

    // cube positions
    std::vector<glm::vec3> cubes{
        glm::vec3(-1.0f, 0.0f, -1.0f),
        glm::vec3(2.0f, 0.0f, 0.0f)
    };
    // vegitation
    std::vector<glm::vec3> vegitation{
        glm::vec3(-1.5f, 0.0f, -0.48f),
        glm::vec3(1.5f, 0.0f, 0.51f),
        glm::vec3(0.0f, 0.0f, 0.7f),
        glm::vec3(-0.3f, 0.0f, -2.3f),
        glm::vec3(0.5f, 0.0f, -0.6f)
    };
    std::vector<glm::vec3> windows = vegitation;

    // render loop
    while(!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glStencilMask(0xFF); // enable writing to stencil buffer for clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glStencilMask(0x00); // disable writing to stencil buffer again

        // initial setup
        float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
        glm::mat4 viewProj = camera.getViewProjectionMatrix(aspectRatio);
        shader.use();
        shader.setMat4("viewProj", viewProj);
        outlineShader.use();
        outlineShader.setMat4("viewProj", viewProj);

        // floor
        shader.use();
        glDisable(GL_CULL_FACE);
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        shader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);

        // cubes
        shader.use();
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        for (auto cubePos : cubes) {
            glStencilMask(0x01); // enable writing to only the first bit of the stencil buffer
            glStencilFunc(GL_ALWAYS, 0x01, 0x01); // for every fragment we render, set the first bit in the stencil
            glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePos);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glStencilMask(0x00); // disable writing to the stencil buffer
        }

        // windows
        shader.use();
        glDisable(GL_CULL_FACE);
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, windowTexture);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        std::map<float, glm::vec3> sortedWindows;
        for (auto windowPos : windows) {
            float distance = glm::length(camera.getPosition() - windowPos);
            sortedWindows[-distance] = windowPos;
        }
        for (auto sortedWindow : sortedWindows) {
            glm::vec3 windowPos = sortedWindow.second;
            glm::mat4 model = glm::translate(glm::mat4(1.0f), windowPos);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);

        // outline
        outlineShader.use();
        glBindVertexArray(cubeVAO);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        float outlineScale = 1.05f;
        for (auto cubePos : cubes) {
            break;
            glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePos);
            model = glm::scale(model, glm::vec3(outlineScale));
            outlineShader.setMat4("model", model);
            outlineShader.setVec3("outlineColor", glm::vec3(0.0f, 0.28, 0.26));
            glStencilFunc(GL_NOTEQUAL, 0x01, 0x01); // check if the first bit is set
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
      

        // swap buffers and poll io events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &quadVBO);

    glfwTerminate();
    return 0;
}

void errorExit(std::string msg, int errorReturn) {
    glfwTerminate();
    std::cout << msg << std::endl;
    exit(errorReturn);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void) window; // ignore unused variable warning
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    auto cameraMovement = CameraMovement();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraMovement.addMovement(MovementDirection::Forward);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraMovement.addMovement(MovementDirection::Backward);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraMovement.addMovement(MovementDirection::Left);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraMovement.addMovement(MovementDirection::Right);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraMovement.addMovement(MovementDirection::Up);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        cameraMovement.addMovement(MovementDirection::Down);
    }

    camera.move(cameraMovement, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
    static bool firstMouse = true;
    static double lastX, lastY;

    if (firstMouse) {
        lastX = xPos; 
        lastY = yPos;
        firstMouse = false;
        return;
    }

    double deltaX = xPos - lastX;
    double deltaY = yPos - lastY;

    lastX = xPos;
    lastY = yPos;

    camera.rotate(deltaX, -deltaY);
}

unsigned int loadTexture(const std::string& fileName) {
    std::string pathString = "resources/textures/" + fileName;
    char const *path = pathString.c_str();

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLint param = GL_REPEAT;
        GLenum format;
        if (nrComponents == 1) {
            format = GL_RED;
        } else if (nrComponents == 3) {
            format = GL_RGB;
        } else if (nrComponents == 4) {
            format = GL_RGBA;
            param = GL_CLAMP_TO_EDGE;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
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