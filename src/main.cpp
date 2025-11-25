#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>

const char* vertexPath = "./shaders/vertex.glsl";
const char* fragementPath = "./shaders/fragment.glsl";
const char* containerJPG = "./textures/container.jpg";
const char* awesomefacePNG = "./textures/awesomeface.png";

void errorExit(std::string msg, int errorReturn);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
bool wireframeMode = false;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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

    // create shader program
    Shader shaderProgram(vertexPath, fragementPath);

    // set up vertex data (and buffers) and configure vertex attributes
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // load and create textures
    unsigned int texture1, texture2;

    // Texture 1
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(containerJPG, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
    // Texture 2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load(awesomefacePNG, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sample to which texture unit it belongs to
    shaderProgram.use();
    shaderProgram.setInt("texture1", 0);
    shaderProgram.setInt("texture2", 1);

    // resets lastFrame before entering render loop
    lastFrame = glfwGetTime();
    
    while(!glfwWindowShouldClose(window)) {
        // pre-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 

        // input
        processInput(window);
        
        // rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        
        // activate shader
        shaderProgram.use();

        // pass projection matrix to shader
        glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shaderProgram.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.getViewMatrix();
        shaderProgram.setMat4("view", view);

        // render boxes
        glBindVertexArray(VAO);
        for(unsigned int i = 0; i < 10; i++) {
            // calculate the model matrix for each object and pass to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle;

            if (i % 3 == 0) {
                angle = 20.0f * i + 20.0f * (float)glfwGetTime();
            } else {
                angle = 20.0f * i;
            }
            
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shaderProgram.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram.ID);

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.move(CameraMovement::Forward, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.move(CameraMovement::Backward, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.move(CameraMovement::Left, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.move(CameraMovement::Right, deltaTime);
    }

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
