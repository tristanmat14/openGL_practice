#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

const char* vertexPath = "./shaders/vertex.glsl";
const char* textureFragPath = "./shaders/texture_fragment.glsl";
const char* lightingFragPath = "./shaders/lighting_fragment.glsl";
const char* lightSourceFragPath = "./shaders/light_source_fragment.glsl";

const char* containerJPG = "./textures/container.jpg";
const char* containerMetalPNG = "./textures/container_metal.png";
const char* awesomefacePNG = "./textures/awesomeface.png";
const char* containerMetalSpecularPNG = "./textures/container_metal_specular.png";

void errorExit(std::string msg, int errorReturn);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
bool wireframeMode = false;

// Camera
Camera camera{glm::vec3(20.0f, 14.5f, 15.2f), glm::vec3(-0.6512f, -0.4769f, -0.5903f)};

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(int argc, char* argv[]) {

    // argument handling
    if (argc == 2) {
        std::string arg = argv[1];
        if (arg == "--w") {
            wireframeMode = true;
        } else {
            std::cout << "Incorrect Program usage" << std::endl;
            exit(1);
        }
    } else if (argc > 1) {
        std::cout << "Incorrect Program usage" << std::endl;
        exit(1);
    }

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
    
    // load all OpenGL function pointers using glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        errorExit("Failed to initialize GLAD", -1);
    }

    // if the wireframe mode is true, then render using GL_LINE
    if (wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // create shader programs
    Shader shaderProgram(vertexPath, lightingFragPath);
    Shader lightSourceShader(vertexPath, lightSourceFragPath);

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal vector attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // unbind VAO
    glBindVertexArray(0);

    // load and create textures
    unsigned int diffuseMap, specularMap;

    // Texture 1
    glGenTextures(1, &diffuseMap);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(containerMetalPNG, &width, &height, &nrChannels, 0);
    if (data) {
        // GL_RGB for JPG, GL_RGBA for PNG
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // Texture 2
    glGenTextures(1, &specularMap);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(containerMetalSpecularPNG, &width, &height, &nrChannels, 0);
    if (data) {
        // GL_RGB for JPG, GL_RGBA for PNG
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
    // tell opengl for each sample to which texture unit it belongs to
    shaderProgram.use();
    shaderProgram.setInt("material.diffuse", 0);
    shaderProgram.setInt("material.specular", 1);
    shaderProgram.setFloat("material.shininess", 32.0f);
    
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // using the same VBO as the cubes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0); // unbind

    // define the cube positions
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 10.0f,  0.0f,  0.0f), 
        glm::vec3( 0.0f,  10.0f,  0.0f), 
        glm::vec3( 0.0f,  0.0f,  10.0f), 
        glm::vec3( 0.0f,  0.0f,  -10.0f), 
        glm::vec3( 0.0f,  -10.0f,  0.0f), 
        glm::vec3( -10.0f,  0.0f,  0.0f), 
    };

    glm::vec3 lightPosRotationAxis = glm::normalize(glm::vec3(0.0f, 2.0f, 1.0f));
    float lightAngularFreq = 0.5; // rad/s
    glm::vec4 lightPosition(8.0f, 8.0f, 8.0f, 1.0f);  
    glm::vec4 lightDirection(-0.2, -1.0f, -0.3f, 0.0f); 
    glm::vec3 lightColor(1.0f, 0.83f, 0.67f);

    // resets lastFrame before entering render loop
    lastFrame = glfwGetTime();
    
    while (!glfwWindowShouldClose(window)) {
        // pre-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 

        // calculate light position and color
        float angleRad = lightAngularFreq * deltaTime;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleRad, lightPosRotationAxis);
        lightPosition = rotation * lightPosition;
        
        // input
        processInput(window);
        
        // rendering commands
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // activate shader
        shaderProgram.use();
        shaderProgram.setVec3("viewPos", camera.getPosition());

        shaderProgram.setVec3("light.ambient",  glm::vec3(0.2f) * lightColor);
        shaderProgram.setVec3("light.diffuse",  glm::vec3(0.4f) * lightColor);
        shaderProgram.setVec3("light.specular", glm::vec3(1.0f) * lightColor); 

        shaderProgram.setVec3("light.position", camera.getPosition());
        shaderProgram.setVec3("light.direction", camera.getDirection());
        shaderProgram.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        shaderProgram.setInt("light.type", 3);

        shaderProgram.setFloat("light.constant",  1.0f);
        shaderProgram.setFloat("light.linear",    0.027f);
        shaderProgram.setFloat("light.quadratic", 0.0028f);

        float aspectRatio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
        glm::mat4 viewProjection = camera.getViewProjectionMatrix(aspectRatio);
        shaderProgram.setMat4("viewProjection", viewProjection);

        // render boxes
        int i = 0;
        glBindVertexArray(VAO);
        for (auto pos : cubePositions) {
            // calculate the model matrix for each object and pass to shader before drawing
            glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));
            float angle = 20.0f * i++;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shaderProgram.setMat4("model", model);
            shaderProgram.setMat3("normalMatrix", normalMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // render light source
        lightSourceShader.use();
        
        lightSourceShader.setMat4("viewProjection", viewProjection);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(lightPosition));
        model = glm::scale(model, glm::vec3(0.2f));
        
        lightSourceShader.setMat4("model", model);
        lightSourceShader.setVec3("lightColor", lightColor);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram.ID);
    glDeleteProgram(lightSourceShader.ID);

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
