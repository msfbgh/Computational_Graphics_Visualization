/*CS 330 Module 7 Final Project 3D Scene*/
/*Marian Farah*/

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"      // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h" // Camera class


using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "CS 330 Module 7 Final Project 3D Scene - Marian Farah"; // Macro for window title
    const int STRIDE = 7;
    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint pyramidVao, planeVao, cylinderVao, cubeVao, cube2Vao, cube3Vao;  // Handle for the vertex array object
        GLuint pyramidVbo, planeVbo, cubeVbo, cube2Vbo, cube3Vbo;  // Handle for the vertex buffer object
        GLuint pyramidVertices, planeVertices, cylinderVertices, cubeVertices, cube2Vertices, cube3Vertices;  // Number of indices of the mesh
        GLuint cylinderVbos[2];
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh gMesh;

    // Texture
    GLuint gTextureId;      //pyramid
    GLuint gPlanePattern;    //plane
    GLuint gCylinderPattern;    //Cylinder
    GLuint gCubePattern;    //First cube
    GLuint gCube2Pattern;   //Second cube
    GLuint gCube3Pattern;   //Third cube

    // Shader programs
    GLuint gProgramId; //Pyramid
    GLuint gLampProgramId;  //lamp
    GLuint gPlaneProgramId; //plane
    GLuint gCylinderProgramId;  //Cylinder
    GLuint gCubeProgramId;  //First cube
    GLuint gCube2ProgramId; //Second cube
    GLuint gCube3ProgramId; //Third cube

    glm::vec2 gUVScale(5.0f, 5.0f);
    GLint gTexWrapMode = GL_REPEAT;

    // camera
    Camera gCamera(glm::vec3(0.0f, 0.0f, 8.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    bool ortho = false;

    // Cube and light color
    glm::vec3 gObjectColor(1.0f, 0.2f, 0.0f);

    glm::vec3 gLightColor(1.0f, 0.9f, 1.2f);
    glm::vec3 gLightPosition(4.0f, 1.2f, 3.0f);
    glm::vec3 gLightScale(0.8f);

    glm::mat4 rotationPyramidX = glm::rotate(-1.57f, glm::vec3(1.0, 0.0f, 0.0f));
    glm::mat4 rotationPyramidY = glm::rotate(0.1f, glm::vec3(0.0, 1.0f, 0.0f));
    glm::mat4 rotationPyramidZ = glm::rotate(-0.5f, glm::vec3(0.0, 0.0f, 1.0f));

    glm::mat4 rotationPyramid = rotationPyramidX * rotationPyramidY * rotationPyramidZ;

    //cube position
    glm::vec3 gCubePosition(-2.0f, -0.5f, 1.0f); //0.75f, -0.1f, -1.0f
    glm::vec3 gCubeScale(1.0f);

    //Pyramid position (pyramid)
    glm::vec3 gPyramidPosition(-2.0f, 0.51f, 1.0f);//-2.0f, 0.4f, 1.9f
    glm::vec3 gPyramidScale(1.0f);

    //Plane position
    glm::vec3 gPlanePosition(0.0f, -0.5f, 0.0f);
    glm::vec3 gPlaneScale(3.0f);

    //Coke can position (Cylinder)
    glm::vec3 gCylinderPosition(2.0f, -0.1f, 1.0f);//-2.0f, -0.32f, 2.0f
    glm::vec3 gCylinderScale(4.0f);

    //Second Cube position
    glm::vec3 gCube2Position(0.0f, -0.5f, 2.0f); //0.75f, -0.5f, -1.0f
    glm::vec3 gCube2Scale(1.0f);

    //Third Cube position
    glm::vec3 gCube3Position(0.0f, 0.20f, 1.0f); //0.75f, -0.5f, -1.0f
    glm::vec3 gCube3Scale(1.0f);

    // Lamp animation
    bool gIsLampOrbiting = true;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UCreateCylinder(GLfloat verts[], GLushort indices[], int numSides, float radius, float halfLen);
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);


/* Cube Vertex Shader Source Code*/
const GLchar* cubeVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Cube Fragment Shader Source Code*/
const GLchar* cubeFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU
// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    float ambientStrength = 0.5f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 0.3f; // Set specular light strength
    float highlightSize = 2.0f; // Set specular highlight size8.0f;
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Calculate phong result
    vec3 objectColor = texture(uTexture, vertexTextureCoordinate).xyz;
    vec3 keyResult = (ambient + diffuse + specular);
    vec3 lightingResult = keyResult ;
    //vec3 phong = (lightingResult)*objectColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0f); // Send lighting results to GPU
}
);


/* Lamp Shader Source Code*/
const GLchar* lampVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

        //Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);


/* Fragment Shader Source Code*/
const GLchar* lampFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

void main()
{
    fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object

    // Create the shader programs
    if (!UCreateShaderProgram(cubeVertexShaderSource, cubeFragmentShaderSource, gProgramId)) 
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLampProgramId)) //lamp
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(cubeVertexShaderSource, cubeFragmentShaderSource, gPlaneProgramId))  //plane
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(cubeVertexShaderSource, cubeFragmentShaderSource, gCylinderProgramId))  //Cylinder
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(cubeVertexShaderSource, cubeFragmentShaderSource, gCubeProgramId))  //first cube
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(cubeVertexShaderSource, cubeFragmentShaderSource, gCube2ProgramId))  //second cube
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(cubeVertexShaderSource, cubeFragmentShaderSource, gCube3ProgramId))  //third cube
        return EXIT_FAILURE;

    // Load texture
    const char* texFilename = "red.jpg";  
    if (!UCreateTexture(texFilename, gTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    texFilename = "white.png";              //plane
    if (!UCreateTexture(texFilename, gPlanePattern))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    texFilename = "peach.png";          //Cylinder
    if (!UCreateTexture(texFilename, gCylinderPattern))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    texFilename = "yellow.png";               //cube
    if (!UCreateTexture(texFilename, gCubePattern))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "red.jpg";               //cube2
    if (!UCreateTexture(texFilename, gCube2Pattern))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "cardboard.png";               //cube3
    if (!UCreateTexture(texFilename, gCube3Pattern))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;

    }
        // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
        glUseProgram(gProgramId);
        // We set the texture as texture unit 0
        glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 0);

        // Sets the background color of the window to black (it will be implicitely used by glClear)
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // render loop
        // -----------

        while (!glfwWindowShouldClose(gWindow))
        {
            // per-frame timing
            // --------------------
            float currentFrame = glfwGetTime();
            gDeltaTime = currentFrame - gLastFrame;
            gLastFrame = currentFrame;

            // input
            // -----
            UProcessInput(gWindow);

            // Render this frame
            URender();

            glfwPollEvents();
        }

        // Release mesh data
        UDestroyMesh(gMesh);

        // Release texture
        UDestroyTexture(gTextureId);

        // Release shader programs
        UDestroyShaderProgram(gProgramId);
        UDestroyShaderProgram(gLampProgramId);
        UDestroyShaderProgram(gPlaneProgramId);
        UDestroyShaderProgram(gCylinderProgramId);
        UDestroyShaderProgram(gCubeProgramId);
        UDestroyShaderProgram(gCube2ProgramId);
        UDestroyShaderProgram(gCube3ProgramId);
        exit(EXIT_SUCCESS); // Terminates the program successfully
    }


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);


    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) //creates the ortho projection
    {

        gCamera.Position = glm::vec3(-2.5f, 5.2f, -1.5f);
        gCamera.Pitch = -100.0f;
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) { //resets the ortho projection

        gCamera.Position = glm::vec3(0.0f, 0.0f, 5.0f);
        gCamera.Pitch = 0.0f;
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);


    // Add stubs for Q/E Upward/Downward movement
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && gTexWrapMode != GL_REPEAT)
    {
        glBindTexture(GL_TEXTURE_2D, gTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_REPEAT;

        cout << "Current Texture Wrapping Mode: REPEAT" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && gTexWrapMode != GL_MIRRORED_REPEAT)
    {
        glBindTexture(GL_TEXTURE_2D, gTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_MIRRORED_REPEAT;

        cout << "Current Texture Wrapping Mode: MIRRORED REPEAT" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && gTexWrapMode != GL_CLAMP_TO_EDGE)
    {
        glBindTexture(GL_TEXTURE_2D, gTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_CLAMP_TO_EDGE;

        cout << "Current Texture Wrapping Mode: CLAMP TO EDGE" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && gTexWrapMode != GL_CLAMP_TO_BORDER)
    {
        float color[] = { 1.0f, 0.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

        glBindTexture(GL_TEXTURE_2D, gTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_CLAMP_TO_BORDER;

        cout << "Current Texture Wrapping Mode: CLAMP TO BORDER" << endl;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
    {
        gUVScale += 0.1f;
        cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
    {
        gUVScale -= 0.1f;
        cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
    }


    // Pause and resume lamp orbiting
    static bool isLKeyDown = false;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !gIsLampOrbiting)
        gIsLampOrbiting = true;

    //Press the "K" key to start the lamp orbit to see the shadows. 

    else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && gIsLampOrbiting)
        gIsLampOrbiting = false;

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------


//Create Cylinder can
void UCreateCylinder(GLfloat verts[], GLushort indices[], int numSides, float radius, float halfLen) {
    const float TWO_PI = 2.0f * 3.1415926f;
    const float radiansPerSide = TWO_PI / numSides;

    int currentVertex = 0;

    verts[0] = 0.0f;        //0 x
    verts[1] = halfLen;     //0 y
    verts[2] = 0.0f;        //0 z
    verts[3] = 0.0f;        //0 r
    verts[4] = 0.0f;        //0 g    
    verts[5] = 0.0f;        //0 b
    verts[6] = 1.0f;        //0 a
    currentVertex++;
    verts[7] = 0.0f;        //1 x
    verts[8] = -halfLen;    //1 y
    verts[9] = 0.0f;        //1 z
    verts[10] = 0.0f;       //1 r
    verts[11] = 0.0f;       //1 g
    verts[12] = 0.0f;       //1 b
    verts[13] = 1.0f;       //1 a
    currentVertex++;

    int currentTriangle = 0;
    for (int edge = 0; edge < numSides; edge++) {
        float theta = ((float)edge) * radiansPerSide;

        verts[currentVertex * STRIDE + 0] = radius * cos(theta);    //x
        verts[currentVertex * STRIDE + 1] = halfLen;                //y
        verts[currentVertex * STRIDE + 2] = radius * sin(theta);    //z
        verts[currentVertex * STRIDE + 3] = 1.0f;                   //r
        verts[currentVertex * STRIDE + 4] = 0.0f;                   //g
        verts[currentVertex * STRIDE + 5] = 0.0f;                   //b
        verts[currentVertex * STRIDE + 6] = 1.0f;                   //a
        currentVertex++;

        verts[currentVertex * STRIDE + 0] = radius * cos(theta);    //x
        verts[currentVertex * STRIDE + 1] = -halfLen;               //y
        verts[currentVertex * STRIDE + 2] = radius * sin(theta);    //z
        verts[currentVertex * STRIDE + 3] = 1.0f;                   //r
        verts[currentVertex * STRIDE + 4] = 0.0f;                   //g
        verts[currentVertex * STRIDE + 5] = 0.0f;                   //b
        verts[currentVertex * STRIDE + 6] = 1.0f;                   //a
        currentVertex++;


        if (edge > 0) {
            //top triangle
            indices[(3 * currentTriangle) + 0] = 0;
            indices[(3 * currentTriangle) + 1] = currentVertex - 4;
            indices[(3 * currentTriangle) + 2] = currentVertex - 2;
            currentTriangle++;

            //bottom triangle
            indices[(3 * currentTriangle) + 0] = 1;
            indices[(3 * currentTriangle) + 1] = currentVertex - 3;
            indices[(3 * currentTriangle) + 2] = currentVertex - 1;
            currentTriangle++;

            //triangle for 1/2 rectangular side
            indices[(3 * currentTriangle) + 0] = currentVertex - 4;
            indices[(3 * currentTriangle) + 1] = currentVertex - 3;
            indices[(3 * currentTriangle) + 2] = currentVertex - 1;
            currentTriangle++;

            //traingle for second 1/2 rectangular side
            indices[(3 * currentTriangle) + 0] = currentVertex - 1;
            indices[(3 * currentTriangle) + 1] = currentVertex - 2;
            indices[(3 * currentTriangle) + 2] = currentVertex - 4;
            currentTriangle++;
        }
    }

    indices[(3 * currentTriangle) + 0] = 0;
    indices[(3 * currentTriangle) + 1] = currentVertex - 2;
    indices[(3 * currentTriangle) + 2] = 2;
    currentTriangle++;
    indices[(3 * currentTriangle) + 0] = 1;
    indices[(3 * currentTriangle) + 1] = currentVertex - 1;
    indices[(3 * currentTriangle) + 2] = 3;
    currentTriangle++;
    indices[(3 * currentTriangle) + 0] = currentVertex - 2;
    indices[(3 * currentTriangle) + 1] = currentVertex - 1;
    indices[(3 * currentTriangle) + 2] = 3;
    currentTriangle++;
    indices[(3 * currentTriangle) + 0] = 3;
    indices[(3 * currentTriangle) + 1] = 2;
    indices[(3 * currentTriangle) + 2] = currentVertex - 2;
    currentTriangle++;
}


// Functioned called to render a frame
void URender()
{
    // Lamp orbits around the origin
    const float angularVelocity = glm::radians(45.0f);
    if (!gIsLampOrbiting)
    {
        glm::vec4 newPosition = glm::rotate(angularVelocity * gDeltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(gLightPosition, 1.0f);
        gLightPosition.x = newPosition.x;
        gLightPosition.y = newPosition.y;
        gLightPosition.z = newPosition.z;
    }

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate the VAO 
    glBindVertexArray(gMesh.pyramidVao);

    // CUBE: draw
    //----------------
    // Set the shader to be used
    glUseProgram(gProgramId);

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = glm::translate(gPyramidPosition) * glm::scale(gPyramidScale);

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Setup views and projections
    if (ortho) {
        GLfloat oWidth = (GLfloat)WINDOW_WIDTH * 0.01f; // 10% of width
        GLfloat oHeight = (GLfloat)WINDOW_HEIGHT * 0.01f; // 10% of height

        view = gCamera.GetViewMatrix();
        projection = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
    }
    else {
        view = gCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.pyramidVertices);


    //Draw plane

    glUseProgram(gProgramId);

    glBindTexture(GL_TEXTURE_2D, gPlanePattern);
    glActiveTexture(GL_TEXTURE0);

    model = glm::translate(gPlanePosition) * glm::scale(gPlaneScale);

    modelLoc = glGetUniformLocation(gPlaneProgramId, "model");
    viewLoc = glGetUniformLocation(gPlaneProgramId, "view");
    projLoc = glGetUniformLocation(gPlaneProgramId, "projection");


    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(gMesh.planeVao);

    glDrawArrays(GL_TRIANGLES, 0, gMesh.planeVertices);


    //Draw Cylinder can
    glUseProgram(gProgramId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gCylinderPattern);


    model = glm::translate(gCylinderPosition) * glm::scale(gCylinderScale);

    modelLoc = glGetUniformLocation(gCylinderProgramId, "model");
    viewLoc = glGetUniformLocation(gCylinderProgramId, "view");
    projLoc = glGetUniformLocation(gCylinderProgramId, "projection");


    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(gMesh.cylinderVao);

    glDrawElements(GL_TRIANGLES, gMesh.cylinderVertices, GL_UNSIGNED_SHORT, NULL);


    //Draw cube

    glUseProgram(gProgramId);
    glBindTexture(GL_TEXTURE_2D, gCubePattern);
    glActiveTexture(GL_TEXTURE0);

    model = glm::translate(gCubePosition) * glm::scale(gCubeScale);

    modelLoc = glGetUniformLocation(gCubeProgramId, "model");
    viewLoc = glGetUniformLocation(gCubeProgramId, "view");
    projLoc = glGetUniformLocation(gCubeProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(gMesh.cubeVao);

    glDrawArrays(GL_TRIANGLES, 0, gMesh.cubeVertices);


    //Draw cube2

    glUseProgram(gProgramId);
    glBindTexture(GL_TEXTURE_2D, gCube2Pattern);
    glActiveTexture(GL_TEXTURE0);

    model = glm::translate(gCube2Position) * glm::scale(gCube2Scale);

    modelLoc = glGetUniformLocation(gCube2ProgramId, "model");
    viewLoc = glGetUniformLocation(gCube2ProgramId, "view");
    projLoc = glGetUniformLocation(gCube2ProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(gMesh.cube2Vao);

    glDrawArrays(GL_TRIANGLES, 0, gMesh.cube2Vertices);


    //Draw cube3

    glUseProgram(gProgramId);
    glBindTexture(GL_TEXTURE_2D, gCube3Pattern);
    glActiveTexture(GL_TEXTURE0);

    model = glm::translate(gCube2Position) * glm::scale(gCube3Scale);

    modelLoc = glGetUniformLocation(gCube3ProgramId, "model");
    viewLoc = glGetUniformLocation(gCube3ProgramId, "view");
    projLoc = glGetUniformLocation(gCube3ProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(gMesh.cube3Vao);

    glDrawArrays(GL_TRIANGLES, 0, gMesh.cube3Vertices);

    // LAMP: draw lamp
    glUseProgram(gLampProgramId);

    //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(gLightPosition) * glm::scale(gLightScale);

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(gLampProgramId, "model");
    viewLoc = glGetUniformLocation(gLampProgramId, "view");
    projLoc = glGetUniformLocation(gLampProgramId, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //glDrawArrays(GL_TRIANGLES, 0, gMesh.cylinderVertices);
    glDrawArrays(GL_TRIANGLES, 0, gMesh.cubeVertices);

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, gMesh.cubeVertices);

    //COMMENT OUT       COMMENT OUT         COMMENT OUT         COMMENT OUT

    //glDrawArrays(GL_TRIANGLES, 0, gMesh.cylinderVertices);

    // Deactivate the Vertex Array Object and shader program
    glBindVertexArray(0);
    glUseProgram(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.

}

// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh)
{
    const int NUM_SIDES = 100;
    const int NUM_VERTICES = STRIDE * (2 + (2 * NUM_SIDES));
    const int NUM_INDICES = 12 * NUM_SIDES;
    GLfloat verts[NUM_VERTICES];
    GLushort indices[NUM_INDICES];


    // Position and Color data
    GLfloat pyramidVerts[] = {
        //Positions                Normals                Textures

       -0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,      0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,      1.0f, 0.0f,
        0.0f,  0.5f,  0.0f,     0.0f,  0.0f, -1.0f,      0.5f, 1.0f,

       -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,      0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,      1.0f, 0.0f,
        0.0f,  0.5f,  0.0f,     0.0f,  0.0f,  1.0f,      0.5f, 1.0f,

       -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,      0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,      0.0f, 0.0f,
        0.0f,  0.5f,  0.0f,    -1.0f,  0.0f,  0.0f,      0.5f, 1.0f,

        0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,      0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,      0.0f, 0.0f,
        0.0f,  0.5f,  0.0f,     1.0f,  0.0f,  0.0f,      0.5f, 1.0f,

        0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,      1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,      1.0f, 0.0f,
        0.0f,  0.5f,  0.0f,     0.0f, -1.0f,  0.0f,      0.5f, 1.0f,

       -0.5f, -0.5f,  0.5f,     0.0f,  1.0f,  0.0f,      0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,     0.0f,  1.0f,  0.0f,      0.0f, 1.0f,
        0.0f,  0.5f,  0.0f,     0.0f,  1.0f,  0.0f,      0.5f, 1.0f

    };
    GLfloat planeVerts[] = {
        //Positions                      Normals                Textures
          2.0f, -0.2f,  1.0f,        0.0f,  0.0f, 1.0f,        1.0f, 0.0f,
         -2.0f, -0.2f, -1.0f,        0.0f,  0.0f, 1.0f,        0.0f, 0.0f,
          2.0f, -0.2f, -1.0f,        0.0f,  0.0f, 1.0f,        0.0f, 1.0f,

          2.0f, -0.2f,  1.0f,        0.0f,  0.0f, 1.0f,        1.0f, 0.0f,
         -2.0f, -0.2f,  1.0f,        0.0f,  0.0f, 1.0f,        1.0f, 1.0f,
         -2.0f, -0.2f, -1.0f,        0.0f,  0.0f, 1.0f,        0.0f, 0.0f,
    };

    GLfloat cubeVerts[] = {
        //Positions                      Normals                Textures

         -0.5f, -0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        0.0f, 0.0f,
          0.5f, -0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        1.0f, 1.0f,
          0.5f,  0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        0.0f, 0.0f,


         -0.5f, -0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        0.0f, 0.0f,
          0.5f, -0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        1.0f, 1.0f,
         -0.5f,  0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        0.0f, 0.0f,


         -0.5f,  0.5f,  0.5f,      -1.0f,  0.0f,  0.0f,        1.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,      -1.0f,  0.0f,  0.0f,        1.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,      -1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,      -1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,      -1.0f,  0.0f,  0.0f,        0.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,      -1.0f,  0.0f,  0.0f,        1.0f, 0.0f,


          0.5f,  0.5f,  0.5f,       1.0f,  0.0f,  0.0f,        1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,       1.0f,  0.0f,  0.0f,        1.0f, 1.0f,
          0.5f, -0.5f, -0.5f,       1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,       1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
          0.5f, -0.5f,  0.5f,       1.0f,  0.0f,  0.0f,        0.0f, 0.0f,
          0.5f,  0.5f,  0.5f,       1.0f,  0.0f,  0.0f,        1.0f, 0.0f,


         -0.5f, -0.5f, -0.5f,       0.0f, -1.0f,  0.0f,        0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,       0.0f, -1.0f,  0.0f,        1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,       0.0f, -1.0f,  0.0f,        1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,       0.0f, -1.0f,  0.0f,        1.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,       0.0f, -1.0f,  0.0f,        0.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,       0.0f, -1.0f,  0.0f,        0.0f, 1.0f,


         -0.5f,  0.5f, -0.5f,       0.0f,  1.0f,  0.0f,        0.0f, 1.0f,
          0.5f,  0.5f, -0.5f,       0.0f,  1.0f,  0.0f,        1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,       0.0f,  1.0f,  0.0f,        1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,       0.0f,  1.0f,  0.0f,        1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,       0.0f,  1.0f,  0.0f,        0.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,       0.0f,  1.0f,  0.0f,        0.0f, 1.0f
    };

    GLfloat cube2Verts[] = {
        //Positions                      Normals                Textures

         -0.5f, -0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        0.0f, 0.0f,
          0.5f, -0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        1.0f, 1.0f,
          0.5f,  0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        0.0f, 0.0f,


         -0.5f, -0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        0.0f, 0.0f,
          0.5f, -0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        1.0f, 1.0f,
         -0.5f,  0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        0.0f, 0.0f,


         -0.5f,  0.5f,  0.5f,      -1.0f,  0.0f,  0.0f,        1.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,      -1.0f,  0.0f,  0.0f,        1.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,      -1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,      -1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,      -1.0f,  0.0f,  0.0f,        0.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,      -1.0f,  0.0f,  0.0f,        1.0f, 0.0f,


          0.5f,  0.5f,  0.5f,       1.0f,  0.0f,  0.0f,        1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,       1.0f,  0.0f,  0.0f,        1.0f, 1.0f,
          0.5f, -0.5f, -0.5f,       1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,       1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
          0.5f, -0.5f,  0.5f,       1.0f,  0.0f,  0.0f,        0.0f, 0.0f,
          0.5f,  0.5f,  0.5f,       1.0f,  0.0f,  0.0f,        1.0f, 0.0f,


         -0.5f, -0.5f, -0.5f,       0.0f, -1.0f,  0.0f,        0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,       0.0f, -1.0f,  0.0f,        1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,       0.0f, -1.0f,  0.0f,        1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,       0.0f, -1.0f,  0.0f,        1.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,       0.0f, -1.0f,  0.0f,        0.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,       0.0f, -1.0f,  0.0f,        0.0f, 1.0f,


         -0.5f,  0.5f, -0.5f,       0.0f,  1.0f,  0.0f,        0.0f, 1.0f,
          0.5f,  0.5f, -0.5f,       0.0f,  1.0f,  0.0f,        1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,       0.0f,  1.0f,  0.0f,        1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,       0.0f,  1.0f,  0.0f,        1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,       0.0f,  1.0f,  0.0f,        0.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,       0.0f,  1.0f,  0.0f,        0.0f, 1.0f
    };

    GLfloat cube3Verts[] = {
        //Positions                      Normals                Textures

         -0.5f, -0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        0.0f, 0.0f,
          0.5f, -0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        1.0f, 1.0f,
          0.5f,  0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,       0.0f,  0.0f, -1.0f,        0.0f, 0.0f,


         -0.5f, -0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        0.0f, 0.0f,
          0.5f, -0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        1.0f, 1.0f,
         -0.5f,  0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,       0.0f,  0.0f,  1.0f,        0.0f, 0.0f,


         -0.5f,  0.5f,  0.5f,      -1.0f,  0.0f,  0.0f,        1.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,      -1.0f,  0.0f,  0.0f,        1.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,      -1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,      -1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,      -1.0f,  0.0f,  0.0f,        0.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,      -1.0f,  0.0f,  0.0f,        1.0f, 0.0f,


          0.5f,  0.5f,  0.5f,       1.0f,  0.0f,  0.0f,        1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,       1.0f,  0.0f,  0.0f,        1.0f, 1.0f,
          0.5f, -0.5f, -0.5f,       1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,       1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
          0.5f, -0.5f,  0.5f,       1.0f,  0.0f,  0.0f,        0.0f, 0.0f,
          0.5f,  0.5f,  0.5f,       1.0f,  0.0f,  0.0f,        1.0f, 0.0f,


         -0.5f, -0.5f, -0.5f,       0.0f, -1.0f,  0.0f,        0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,       0.0f, -1.0f,  0.0f,        1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,       0.0f, -1.0f,  0.0f,        1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,       0.0f, -1.0f,  0.0f,        1.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,       0.0f, -1.0f,  0.0f,        0.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,       0.0f, -1.0f,  0.0f,        0.0f, 1.0f,


         -0.5f,  0.5f, -0.5f,       0.0f,  1.0f,  0.0f,        0.0f, 1.0f,
          0.5f,  0.5f, -0.5f,       0.0f,  1.0f,  0.0f,        1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,       0.0f,  1.0f,  0.0f,        1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,       0.0f,  1.0f,  0.0f,        1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,       0.0f,  1.0f,  0.0f,        0.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,       0.0f,  1.0f,  0.0f,        0.0f, 1.0f
    };

    //fill verts/indices arrays with data 
    UCreateCylinder(verts, indices, NUM_SIDES, 0.15f, 0.25f);

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.pyramidVertices = sizeof(pyramidVerts) / (sizeof(pyramidVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.pyramidVao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.pyramidVao);

    // Create 1 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.pyramidVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.pyramidVbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVerts), pyramidVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);


    //Draw plane
    mesh.planeVertices = sizeof(planeVerts) / (sizeof(planeVerts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.planeVao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.planeVao);

    // Create 1 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.planeVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.planeVbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts), planeVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);


    //Draw Cylinder
    const GLuint floatsPerVertex2 = 3;
    const GLuint floatsPerColor2 = 4;
    const GLuint floatsPerUV2 = 2;

    glGenVertexArrays(1, &mesh.cylinderVao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.cylinderVao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.cylinderVbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.cylinderVbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.cylinderVertices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.cylinderVbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride2 = sizeof(float) * (floatsPerVertex2 + floatsPerColor2);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex2, GL_FLOAT, GL_FALSE, stride2, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor2, GL_FLOAT, GL_FALSE, stride2, (char*)(sizeof(float) * floatsPerVertex2));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV2, GL_FLOAT, GL_FALSE, stride2, (char*)(sizeof(float) * (floatsPerVertex2 + floatsPerColor2)));
    glEnableVertexAttribArray(2);


    //Draw cube

    const GLuint cubeFloatsPerVertex = 3;
    const GLuint cubeFloatsPerNormal = 3;
    const GLuint cubeFloatsPerUV = 2;

    mesh.cubeVertices = sizeof(cubeVerts) / (sizeof(cubeVerts[0]) * (cubeFloatsPerVertex + cubeFloatsPerNormal + cubeFloatsPerUV));

    glGenVertexArrays(1, &mesh.cubeVao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.cubeVao);

    // Create 1 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.cubeVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.cubeVbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint cubeStride = sizeof(float) * (cubeFloatsPerVertex + cubeFloatsPerNormal + cubeFloatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, cubeFloatsPerVertex, GL_FLOAT, GL_FALSE, cubeStride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, cubeFloatsPerNormal, GL_FLOAT, GL_FALSE, cubeStride, (void*)(sizeof(float) * cubeFloatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, cubeFloatsPerUV, GL_FLOAT, GL_FALSE, cubeStride, (void*)(sizeof(float) * (cubeFloatsPerVertex + cubeFloatsPerNormal)));
    glEnableVertexAttribArray(2);


    //Draw cube2

    const GLuint cube2FloatsPerVertex = 3;
    const GLuint cube2FloatsPerNormal = 3;
    const GLuint cube2FloatsPerUV = 2;

    mesh.cube2Vertices = sizeof(cube2Verts) / (sizeof(cube2Verts[0]) * (cube2FloatsPerVertex + cube2FloatsPerNormal + cube2FloatsPerUV));

    glGenVertexArrays(1, &mesh.cube2Vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.cube2Vao);

    // Create 1 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.cube2Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.cube2Vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube2Verts), cube2Verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint cube2Stride = sizeof(float) * (cube2FloatsPerVertex + cube2FloatsPerNormal + cube2FloatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, cube2FloatsPerVertex, GL_FLOAT, GL_FALSE, cube2Stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, cube2FloatsPerNormal, GL_FLOAT, GL_FALSE, cube2Stride, (void*)(sizeof(float) * cube2FloatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, cube2FloatsPerUV, GL_FLOAT, GL_FALSE, cube2Stride, (void*)(sizeof(float) * (cube2FloatsPerVertex + cube2FloatsPerNormal)));
    glEnableVertexAttribArray(2);


    //Draw cube3

    const GLuint cube3FloatsPerVertex = 3;
    const GLuint cube3FloatsPerNormal = 3;
    const GLuint cube3FloatsPerUV = 2;

    mesh.cube2Vertices = sizeof(cube3Verts) / (sizeof(cube3Verts[0]) * (cube3FloatsPerVertex + cube3FloatsPerNormal + cube3FloatsPerUV));

    glGenVertexArrays(1, &mesh.cube3Vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.cube3Vao);

    // Create 1 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.cube3Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.cube3Vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube3Verts), cube3Verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint cube3Stride = sizeof(float) * (cube3FloatsPerVertex + cube3FloatsPerNormal + cube3FloatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, cube3FloatsPerVertex, GL_FLOAT, GL_FALSE, cube3Stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, cube3FloatsPerNormal, GL_FLOAT, GL_FALSE, cube3Stride, (void*)(sizeof(float)* cube3FloatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, cube3FloatsPerUV, GL_FLOAT, GL_FALSE, cube3Stride, (void*)(sizeof(float)* (cube3FloatsPerVertex + cube3FloatsPerNormal)));
    glEnableVertexAttribArray(2);

}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.pyramidVao);
    glDeleteBuffers(1, &mesh.pyramidVbo);
}


/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}


void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}