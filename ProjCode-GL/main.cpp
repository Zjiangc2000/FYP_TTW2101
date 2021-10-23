#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"

#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"

// for loading textures
#include "HeaderFiles/stb_image.h"
#include "HeaderFiles/Texture.h"

// for customizing shaders
#include "HeaderFiles/Shader.h"

#include <iostream>
#include <fstream>

#include <vector>

// screen setting
const int SCR_WIDTH = 1600;
const int SCR_HEIGHT = 800;

// used to control skybox buffer arrays
GLuint skyboxVAO;
GLuint skyboxVBO;
// used to control testing triangle buffer arrays
GLuint vaoID, vboID, eboID;

// used to control the shader
Shader shaderobjsky, shaderobj1;

GLuint cubemapTexture;

// variables for camera and mouse events
glm::vec3 cameraPos = glm::vec3(0.0f, 4.0f, 7.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;
GLfloat fov = 45.0f;
int firstMouse = 1;
int mouse_left = 0, mouse_right = 0;
GLfloat mouse_lastX = SCR_WIDTH / 2.0;
GLfloat mouse_lastY = SCR_HEIGHT / 2.0;
GLfloat mouse_X, mouse_Y;	// record cursor positions

// array used to control keyboard events
bool keys[1024];

float x_delta = 0.1f;
int x_press_num = 0;

GLuint loadCubemap(std::vector<const GLchar*> faces)
{
    int width, height, BPP;
    unsigned char* image;
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (GLuint i = 0; i < faces.size(); i++)
    {
        // tell stb_image.h to flip loaded texture's on the y-axis.
        stbi_set_flip_vertically_on_load(true);
        // load the texture data into "data"
        image = stbi_load(faces[i], &width, &height, &BPP, 0);
        if (image)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                0, GL_RGB, GL_UNSIGNED_BYTE, image);
            stbi_image_free(image);
        }
        else
        {
            std::cout << "Failed to load cubemap texture" << std::endl;
            stbi_image_free(image);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

void get_OpenGL_info() {
    // OpenGL information
    const GLubyte* name = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* glversion = glGetString(GL_VERSION);
    std::cout << "OpenGL company: " << name << std::endl;
    std::cout << "Renderer name: " << renderer << std::endl;
    std::cout << "OpenGL version: " << glversion << std::endl;
}

void sendDataToOpenGL() {
    
    // skybox cubemap
    GLfloat skyboxVertices[] =
    {
        // Positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

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
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);
    // Cubemap (Skybox)
    std::vector<const GLchar*> faces;
    faces.push_back("Textures/skybox/right.bmp");
    faces.push_back("Textures/skybox/left.bmp");
    faces.push_back("Textures/skybox/top.bmp");
    faces.push_back("Textures/skybox/bottom.bmp");
    faces.push_back("Textures/skybox/back.bmp");
    faces.push_back("Textures/skybox/front.bmp");
    cubemapTexture = loadCubemap(faces);
    

    // Testing 3D plane
    glGenVertexArrays(1, &vaoID);
    glGenBuffers(1, &vboID);
    glGenBuffers(1, &eboID);

    
    const GLfloat squarePlane[] =
    {
        +5.0f, -0.001f, +5.0f, // position 0
        +0.0f, +0.0f, +1.0f, // color

        +5.0f, -0.001f, -5.0f, // position 1
        +0.0f, +0.0f, +1.0f,

        -5.0f, -0.001f, +5.0f, // position 2
        +0.0f, +0.0f, +1.0f,

        -5.0f, -0.001f, -5.0f, // position 3
        +0.0f, +0.0f, +1.0f,
    };
    GLushort indices0[] = { 0, 1, 3, 0, 3, 2 };
    glBindVertexArray(vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squarePlane), squarePlane, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices0), indices0, GL_STATIC_DRAW);
    // vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), 0);
    // vertex color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (char*)(3 * sizeof(GL_FLOAT)));
    // unbind VAO, VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void initializedGL(void) {
    // run only once

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW not OK." << std::endl;
    }

    get_OpenGL_info();
    sendDataToOpenGL();
    
    // set up the vertex shader and fragment shader
    shaderobj1.setupShader("VertexShaderCode.glsl", "FragmentShaderCode.glsl");
    shaderobjsky.setupShader("VertexShaderCode1.glsl", "FragmentShaderCode1.glsl");
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); // this line is for controling face culling, 
                            // in opengl triangles whose vertices are defined counterclockwise is considered "fronting"
}

void paintGL(void) // always run
{
    glClearColor(0.09f, 0.09f, 0.44f, 1.0f); //specify the background color, this is just an example
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //**************************************************
    // Draw skybox first
    
    glDepthMask(GL_FALSE);
    shaderobjsky.use();
    glm::mat4 viewTransformMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    viewTransformMatrix = glm::mat4(glm::mat3(viewTransformMatrix));
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 500.0f);
    shaderobjsky.setMat4("viewTransformMatrix", viewTransformMatrix);
    shaderobjsky.setMat4("projectionMatrix", projectionMatrix);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    shaderobjsky.setInt("skybox", 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    
    //**************************************************


    // the testing squarePlane
    shaderobj1.use();
    // set the camera view and transform matrices
    viewTransformMatrix = glm::lookAt(cameraPos,
        cameraPos + cameraFront, cameraUp);
    projectionMatrix = glm::perspective(glm::radians(fov),
        (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 500.0f);
    // Set view, projection matrices	
    shaderobj1.setMat4("viewTransformMatrix", viewTransformMatrix);
    shaderobj1.setMat4("projectionMatrix", projectionMatrix);   
    
    // Set model transformation matrix and draw
    glBindVertexArray(vaoID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
    glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
    modelTransformMatrix = glm::translate(glm::mat4(1.0f),
        glm::vec3(x_delta * x_press_num, 1.0f, 0.0f));
    shaderobj1.setMat4("modelTransformMatrix", modelTransformMatrix);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    // unbind VAO, VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Sets the mouse-button callback for the current window.
        // Sets the mouse-button callback for the current window.
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouse_left = 1;
        firstMouse = 1;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouse_left = 0;
    }
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
    // Sets the cursor position callback for the current window
    mouse_X = x;
    mouse_Y = y;
    //std::cout << mouse_left << std::endl;
    if (mouse_left == 1) {
        if (firstMouse)
        {
            mouse_lastX = mouse_X;
            mouse_lastY = mouse_Y;
            firstMouse = 0;
        }

        GLfloat xoffset = mouse_X - mouse_lastX;
        GLfloat yoffset = mouse_lastY - mouse_Y;
        mouse_lastX = mouse_X;
        mouse_lastY = mouse_Y;

        GLfloat sensitivity = 0.05;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);
        cameraFront = front;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Sets the scoll callback for the current window.
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
    // Set the Keyboard callback for the current window.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // record keyboard states
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void do_movement()
{
    // control camera movements
    GLfloat cameraSpeed = 0.02f;
    if (keys[GLFW_KEY_W])
        cameraPos += cameraSpeed * cameraFront;
    if (keys[GLFW_KEY_S])
        cameraPos -= cameraSpeed * cameraFront;
    if (keys[GLFW_KEY_A])
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (keys[GLFW_KEY_D])
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    
    // control the plane position
    if (keys[GLFW_KEY_LEFT]) 
        x_press_num -= 1;
    if (keys[GLFW_KEY_RIGHT])
        x_press_num += 1;
}

int main(int argc, char* argv[]) {
    GLFWwindow* window;

    /* Initialize the glfw */
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    /* glfw: configure; necessary for MAC */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    /* do not allow resizing */
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, argv[0], NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /*register callback functions*/
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    initializedGL();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        paintGL();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        /* Movement events and light adjustments*/
        do_movement();
    }

    glfwTerminate();
    return 0;
}


