#include <stdio.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "shader_stuff.h"

#include "ft2build.h"
#include "freetype.h"
#include "CFonts.h"

// ---------------------------------------
GLuint program;
GLuint vBuffer_coord;
GLuint vBuffer_normal;
GLuint vArray;

// ---------------------------------------
glm::mat4x4 Matrix_proj;
glm::mat4x4 Matrix_mv;
glm::mat4x4 Matrix_proj_mv;

std::vector<glm::vec3> OBJ_vertices;
std::vector<glm::vec2> OBJ_uvs;
std::vector<glm::vec3> OBJ_normals;

std::vector<CFonts> fonts;
int frame, currentTime, lastTime = 1;
float fpsRate = 1.0f;

// ---------------------------------------
void DisplayScene()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// -----------------------------------
	Matrix_mv = glm::mat4x4(1.0);
	Matrix_mv = glm::translate(Matrix_mv, glm::vec3(_scene_translate_x, _scene_translate_y, _scene_translate_z));
	Matrix_mv = glm::rotate(Matrix_mv, _scene_rotate_x, glm::vec3(1.0f, 0.0f, 0.0f));
	Matrix_mv = glm::rotate(Matrix_mv, _scene_rotate_y, glm::vec3(0.0f, 1.0f, 0.0f));


	glBindVertexArray(vArray);
	glUseProgram(program);

	Matrix_proj_mv = Matrix_proj * Matrix_mv;
	glUniformMatrix4fv(glGetUniformLocation(program, "Matrix_proj_mv"), 1, GL_FALSE, glm::value_ptr(Matrix_proj_mv));


	// -----------------------------------
	// Generowanie obiektow na ekranie

	glDrawArrays(GL_TRIANGLES, 0, OBJ_vertices.size());

	glUseProgram(0);
	glBindVertexArray(0);


	// -----------------------------------
	// NOWE: Generowanie tekstu na ekranie
	char txt[255];
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	if ((currentTime - lastTime) > 1000)
	{
		fpsRate = ((double)frame * 1000.0f / (((double)currentTime - (double)lastTime)));
		lastTime = currentTime;
		frame = 0;
	}
	sprintf(txt, "FPS: %f", fpsRate);
	fonts[2].RenderText(txt, 25, 25, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

	fonts[1].RenderText("ESC - Exit", 25, 450, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
	fonts[0].RenderText("Napis", 25, 70, 0.5f, glm::vec3(0.1, 0.1, 1.0f));

	glutSwapBuffers();
}


// ---------------------------------------
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	Matrix_proj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f);
}

// ---------------------------------------------------
void Initialize()
{
	if (!loadOBJ("scene.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
	{
		printf("Not loaded!\n");
		exit(1);
	}

	_scene_translate_z = -10;

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// 1. Programowanie potoku
	program = glCreateProgram();
	glAttachShader(program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader(program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram(program);


	// 2. Vertex arrays

	glGenVertexArrays(1, &vArray);
	glBindVertexArray(vArray);

	// Wspolrzedne wierzchokow
	glGenBuffers(1, &vBuffer_coord);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_coord);
	glBufferData(GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);

	// --------------------------------------------
	// NOWE:
	// Inicjalizacja programu do generowania tekstu
	CFonts caviarDreams("CaviarDreams.ttf", 36);
	CFonts windsong("Windsong.ttf", 36);
	CFonts arial("arial.ttf", 36);

	fonts = { windsong, caviarDreams, arial };
}


// ---------------------------------------------------
void Animation(int arg)
{
	frame++;
	glutTimerFunc(5, Animation, 0);
	glutPostRedisplay();
}

// ---------------------------------------------------
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
	//glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);

	glutTimerFunc(5, Animation, 0);
	glutMainLoop();

	// Cleaning();
	glDeleteProgram(program);
	glDeleteBuffers(1, &vBuffer_coord);
	glDeleteBuffers(1, &vBuffer_normal);
	glDeleteVertexArrays(1, &vArray);

	//    DeleteDrawText();
	return 0;
}
