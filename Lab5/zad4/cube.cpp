#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


#include "./COpenGLProgram.hpp"
const int numberOfShapes = 3;
COpenGLProgram glprog[numberOfShapes];

glm::mat4x4 Matrix_proj;
glm::mat4x4 Matrix_mv;
glm::mat4x4 Matrix_proj_mv;

GLfloat _scene_translate_x;
GLfloat _scene_translate_y;
GLfloat _scene_translate_z = -5;
GLfloat _scene_rotate_x;
GLfloat _scene_rotate_y;
int _mouse_buttonState = GLUT_UP;
int _mouse_buttonX, _mouse_buttonY;

// ---------------------------------------
void PrepareGeometry(int i) {
    Matrix_mv = glm::mat4x4(1.0);
    Matrix_mv = glm::translate(Matrix_mv, glm::vec3(_scene_translate_x + i, _scene_translate_y - i, _scene_translate_z));
    Matrix_mv = glm::rotate(Matrix_mv, _scene_rotate_x, glm::vec3(1.0f, 0.0f, 0.0f));
    Matrix_mv = glm::rotate(Matrix_mv, _scene_rotate_y, glm::vec3(0.0f, 1.0f, 0.0f));
    Matrix_proj_mv = Matrix_proj * Matrix_mv;
    glprog[i].draw(Matrix_proj_mv);
}

void DisplayScene()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < numberOfShapes; i+= 1)
    {
        PrepareGeometry(i);
    }

    glutSwapBuffers();
}



// ---------------------------------------
void Reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    Matrix_proj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f);
}


void PrepareData(int index, const char* objectName, const char* textureName) {
    glprog[index] = COpenGLProgram();
    glprog[index].loadObject(objectName);
    glprog[index].loadBMP(textureName);
    glprog[index].createProgram();
    glprog[index].loadShaders("vertex.glsl", "fragment.glsl");
    glprog[index].prepareVBO();
}

// ---------------------------------------------------
void Initialize()
{
    PrepareData(0, "bone.obj", "bubbles.bmp");
    PrepareData(1, "scene.obj", "uvtemplate.bmp");
    PrepareData(2, "sky.obj", "multi.bmp");

    glEnable(GL_DEPTH_TEST);
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

void MouseWheel(int button, int dir, int x, int y) {
    if (dir > 0)
    {
        // Zoom in
        _scene_translate_z += 0.1f;
    }
    else
    {
        // Zoom out
        _scene_translate_z -= 0.1f;
    }
    glutPostRedisplay();
}

void MouseMotion(int x, int y) {
	if (_mouse_buttonState == GLUT_DOWN)
	{
		_scene_rotate_y += 2 * (x - _mouse_buttonX) / (float)glutGet(GLUT_WINDOW_WIDTH);
		_mouse_buttonX = x;
		_scene_rotate_x -= 2 * (_mouse_buttonY - y) / (float)glutGet(GLUT_WINDOW_HEIGHT);
		_mouse_buttonY = y;
		glutPostRedisplay();
	}
}

// --------------------------------------------------------------
void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:	// ESC key
        exit(0);
        break;
    }
}

    // --------------------------------------------------------------
void SpecialKeys(int key, int x, int y)
{
    switch (key)
    {

    case GLUT_KEY_LEFT:
        _scene_translate_x -= 0.1f;
        break;

    case GLUT_KEY_RIGHT:
        _scene_translate_x += 0.1f;
        break;

    case GLUT_KEY_UP:
        _scene_translate_y += 0.1f;
        break;

    case GLUT_KEY_DOWN:
        _scene_translate_y -= 0.1f;
        break;
    }

    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    // GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(500, 500);
    glutCreateWindow("OpenGL");

    // GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        printf("GLEW Error\n");
        exit(1);
    }

    // OpenGL
    if (!GLEW_VERSION_3_2)
    {
        printf("Brak OpenGL 3.2!\n");
        exit(1);
    }

    Initialize();
    glutDisplayFunc(DisplayScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
    glutMouseWheelFunc(MouseWheel);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecialKeys);

    glutMainLoop();

    return 0;
}