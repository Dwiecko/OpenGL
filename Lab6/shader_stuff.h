#ifndef __SHADER_STUFF
#define __SHADER_STUFF

// References:
// https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/loading.php

#include <fstream>

// --------------------------------------------------------------
// Variables for control the view

GLfloat _scene_rotate_x = 0.0f;
GLfloat _scene_rotate_y = 0.0f;
GLfloat _scene_translate_x = 0.0f;
GLfloat _scene_translate_y = 0.0f;
GLfloat _scene_translate_z = -3.0f;
int _mouse_buttonState = GLUT_UP;
int _mouse_buttonX, _mouse_buttonY;
int width = 800;
int heigth = 600;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;
glm::vec3 position = glm::vec3(0, 0, 5);
glm::vec3 direction;
glm::vec3 right = glm::vec3(0.0f, 0.0f, -1.0f);;
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);;


glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float cameraSpeed = 0.1f;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = width, lastY = heigth;
bool firstMouse = true;
float fov = 60.0f;
// ------------------------------------------------------
//https://community.khronos.org/t/extracting-camera-position-from-a-modelview-matrix/68031
glm::vec3 ExtractCameraPos(const glm::mat4& a_modelView)
{
    // Get the 3 basis vector planes at the camera origin and transform them into model space.
    //
    // NOTE: Planes have to be transformed by the inverse transpose of a matrix
    //       Nice reference here: http://www.opengl.org/discussion_boards/showthread.php/159564-Clever-way-to-transform-plane-by-matrix
    //
    //       So for a transform to model space we need to do:
    //            inverse(transpose(inverse(MV)))
    //       This equals : transpose(MV) - see Lemma 5 in http://mathrefresher.blogspot.com.au/2007/06/transpose-of-matrix.html
    //
    // As each plane is simply (1,0,0,0), (0,1,0,0), (0,0,1,0) we can pull the data directly from the transpose matrix.
    //
    glm::mat4 modelViewT = transpose(a_modelView);

    // Get plane normals
    glm::vec3 n1(modelViewT[0]);
    glm::vec3 n2(modelViewT[1]);
    glm::vec3 n3(modelViewT[2]);

    // Get plane distances
    float d1(modelViewT[0].w);
    float d2(modelViewT[1].w);
    float d3(modelViewT[2].w);

    // Get the intersection of these 3 planes
    // http://paulbourke.net/geometry/3planes/
    glm::vec3 n2n3 = cross(n2, n3);
    glm::vec3 n3n1 = cross(n3, n1);
    glm::vec3 n1n2 = cross(n1, n2);

    glm::vec3 top = (n2n3 * d1) + (n3n1 * d2) + (n1n2 * d3);
    float denom = dot(n1, n2n3);

    return top / -denom;
}

// --------------------------------------------------------------
void SpecialKeys(int key, int x, int y)
{
    switch (key)
    {      
        case GLUT_KEY_LEFT: 
        {
            //_scene_translate_x += 0.1f;
            position -= right * 0.1f * speed;
            break;
        }

        case GLUT_KEY_RIGHT:
        {
            //_scene_translate_x -= 0.1f;
            position += right * 0.1f * speed;
            break;
        }

        case GLUT_KEY_UP:
        {
            //_scene_translate_y += 0.1f;
            //position += direction * 0.1f * speed;
            cameraPos += (cameraSpeed * cameraFront);
            break;
        }

        case GLUT_KEY_DOWN: 
        {
            //_scene_translate_y += 0.1f;
            position -= direction * 0.1f * speed;
            break;
        }
    }

    glutPostRedisplay();
}
// --------------------------------------------------------------
void Keyboard(unsigned char key, int x, int y)
{
    if (key == 27) {
        exit(0);
    }

    if (key == 'f') {
        glutFullScreen();
    }

    if (key == 'w') {
        cameraPos += cameraSpeed * cameraFront;
    }

    if (key == 's') {
        cameraPos -= cameraSpeed * cameraFront;
    }

    if (key == 'a') {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    if (key == 'd') {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    printf("\n%c: %f %f %f", (char)key, cameraPos[0], cameraPos[1], cameraPos[1]);

    glutPostRedisplay();
}

// --------------------------------------------------------------
void MouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {

        _mouse_buttonState = state;


        if (state == GLUT_DOWN)
        {
            _mouse_buttonX = x;
            _mouse_buttonY = y;
        }
    }
}

// --------------------------------------------------------------
void MouseMotion(int xpos, int ypos)
{
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

    //glutPostRedisplay();
}

// --------------------------------------------------------------
void MouseWheel(int button, int dir, int x, int y) { }


// --------------------------------------------------------------
unsigned long getFileLength(std::ifstream& file)
{
    if (!file.good()) return 0;

    unsigned long pos = file.tellg();
    file.seekg(0, std::ios::end);
    unsigned long len = file.tellg();
    file.seekg(std::ios::beg);

    return len;
}

// --------------------------------------------------------------
GLchar* LoadShaderFile(const char* filename)
{
    std::ifstream file;
    file.open(filename, std::ios::in); // opens as ASCII!
    if (!file) { printf("Can't open file %s !\n", filename); exit(1); }

    unsigned long len = getFileLength(file);

    if (len == 0) { printf("File %s is empty!\n", filename); exit(1); };   // Error: Empty File

    GLchar* ShaderSource = new GLchar[len + 1];
    if (ShaderSource == NULL) { printf("Can't reserve memory %d \n", len + 1); exit(1); }   // can't reserve memory

    // len isn't always strlen cause some characters are stripped in ascii read...
    // it is important to 0-terminate the real length later, len is just max possible value...
    ShaderSource[len] = 0;

    unsigned int i = 0;
    while (file.good())
    {
        ShaderSource[i] = file.get();
        if (!file.eof())
            i++;
    }

    ShaderSource[i] = 0;
    file.close();
    return ShaderSource;
}

// --------------------------------------------------------------
void CheckForErrors_Shader(GLuint shader)
{
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        printf("Blad!\n");
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        char* log = new char[logLength];
        glGetShaderInfoLog(shader, logLength, NULL, log);
        printf("LOG: %s\n", log);
        delete[] log;
        exit(1);
    }
}

// --------------------------------------------------------------
void CheckForErrors_Program(GLuint program, GLenum mode)
{
    GLint status;
    glGetProgramiv(program, mode, &status);
    if (status != GL_TRUE)
    {
        switch (mode)
        {
        case GL_LINK_STATUS:
            printf("Blad konsolidacji programu!\n");
            break;
        case GL_VALIDATE_STATUS:
            printf("Blad walidacji programu!\n");
            break;
        default:
            printf("Inny blad programu!\n");

        }
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        char* log = new char[logLength];
        glGetProgramInfoLog(program, logLength, NULL, log);
        printf("LOG: %s\n", log);
        delete[] log;
        exit(1);
    }
}

// --------------------------------------------------------------
GLuint LoadShader(GLuint MODE, const char* filename)
{
    // utworzenie obiektu shadera
    GLuint shader = glCreateShader(MODE);

    // Wczytanie kodu shadera z pliku
    const GLchar* code = LoadShaderFile(filename);

    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);
    CheckForErrors_Shader(shader);

    delete[] code;
    return shader;
}

// --------------------------------------------------------------
void LinkAndValidateProgram(GLuint program)
{
    // Konsolidacja programu
    glLinkProgram(program);
    CheckForErrors_Program(program, GL_LINK_STATUS);

    // Walidacja programu
    glValidateProgram(program);
    CheckForErrors_Program(program, GL_VALIDATE_STATUS);
}

#endif

