#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "texture_loader.hpp"
#include "shader_stuff.h"


// ---------------------------------------
const int Number_of_Birds = 100;
const int RES = 1000;
const int INST_SIZE = 10;
int Frame = 0;

GLuint program;
GLuint vBuffer_pos;
GLuint vBuffer_uv;
GLuint vBuffer_normal;
GLuint vArray;
GLuint vBuffer_instance;

// ---------------------------------------
glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matModel;

// ---------------------------------------
std::vector<glm::vec3> OBJ_vertices;
std::vector<glm::vec2> OBJ_uvs;
std::vector<glm::vec3> OBJ_normals;

GLuint TextureID;
glm::mat4 Table_of_Birds_matModel[Number_of_Birds];

// ---------------------------------------
void DisplayScene()
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Macierz widoku
	matView = glm::mat4x4(1.0);
	matView = glm::translate(matView, glm::vec3(_scene_translate_x, _scene_translate_y, _scene_translate_z));
	matView = glm::rotate(matView, _scene_rotate_x, glm::vec3(1.0f, 0.0f, 0.0f));
	matView = glm::rotate(matView, _scene_rotate_y, glm::vec3(0.0f, 1.0f, 0.0f));

	// Macierz modelu
	matModel = glm::mat4x4(1.0);


	// AKTYWUJEMY program (wazne! przed przekazaniem zmiennych uniform)
	glUseProgram(program);


	// Przekazujemy zmienne uniform do programu
	glUniformMatrix4fv(glGetUniformLocation(program, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
	glUniformMatrix4fv(glGetUniformLocation(program, "matView"), 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(glGetUniformLocation(program, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));

	glUniform1i(glGetUniformLocation(program, "Frame"), Frame);

	// AKTYWUJEMY tekstury
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glUniform1i(glGetUniformLocation(program, "tex0"), 0);


	// AKTYWUJEMY VAO
	glBindVertexArray(vArray);


	// Generowanie obiektow na ekranie
//	glDrawArrays(GL_TRIANGLES, 0, OBJ_vertices.size());
	glDrawArraysInstanced(GL_TRIANGLES, 0, OBJ_vertices.size(), Number_of_Birds);

	// WYLACZAMY tekstury
	glBindTexture(GL_TEXTURE_2D, 0);
	// WYLACZAMY VAO
	glBindVertexArray(0);
	// WYLACZAMY program
	glUseProgram(0);

	glutSwapBuffers();
}


// ---------------------------------------
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f);
}

void InitBirds() {
	for (int i = 0; i < Number_of_Birds; ++i)
	{
		float x = (rand() % RES) / (RES / INST_SIZE) - INST_SIZE / 2.0;
		float z = (rand() % RES) / (RES / INST_SIZE) - INST_SIZE / 2.0;
		float scale = (0.01 + (rand() % 20) / 1000.0);
		float y = (rand() % RES) / (RES / INST_SIZE) - INST_SIZE / 2.0;

		float angle = (rand() % 314) / 100.0;

		Table_of_Birds_matModel[i] = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
		Table_of_Birds_matModel[i] = glm::rotate(Table_of_Birds_matModel[i], angle, glm::vec3(0.0f, 1.0f, 0.0f));
		Table_of_Birds_matModel[i] = glm::scale(Table_of_Birds_matModel[i], glm::vec3(scale, scale, scale));
	}
}

// ---------------------------------------------------
void Initialize()
{

	InitBirds();


	// ---------------------------------------
	// Ladowanie pliku OBJ
	// https://free3d.com/pl/3d-model/humming-bird-74440.html
	if (!loadOBJ("helicopter.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
	{
		printf("Not loaded!\n");
		exit(1);
	}


	// ---------------------------------------
	// Tworzenie tekstury
	int tex_width;
	int tex_height;
	unsigned char* tex_data;
	loadBMP_custom("koliber.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);



	// Tworzenie potoku OpenGL
	program = glCreateProgram();
	glAttachShader(program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader(program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram(program);

	// VAO

	glGenVertexArrays(1, &vArray);
	glBindVertexArray(vArray);

	// VBO
	// Wspolrzedne wierzcholkow
	glGenBuffers(1, &vBuffer_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos);
	glBufferData(GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// Wspolrzedne textury UV
	glGenBuffers(1, &vBuffer_uv);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv);
	glBufferData(GL_ARRAY_BUFFER, OBJ_uvs.size() * sizeof(glm::vec2), &OBJ_uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// Wektory normalne
	glGenBuffers(1, &vBuffer_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_normal);
	glBufferData(GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);


	glGenBuffers(1, &vBuffer_instance);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_instance);
	glBufferData(GL_ARRAY_BUFFER, Number_of_Birds * sizeof(glm::mat4x4), &Table_of_Birds_matModel[0], GL_STATIC_DRAW);
	
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);


	// Inne ustawienia openGL i sceny
	glEnable(GL_DEPTH_TEST);

	// Odsuwanie widoku (na potrzeby przykladu)
	_scene_translate_z = -5;

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);




}

void Animation(int param) 
{
	Frame++;

	glutTimerFunc(10, Animation, 0);
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
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);
	glutTimerFunc(10, Animation, 0);
	glutMainLoop();

	// Cleaning();
	glDeleteProgram(program);
	glDeleteBuffers(1, &vBuffer_pos);
	glDeleteBuffers(1, &vBuffer_uv);
	glDeleteBuffers(1, &vBuffer_normal);
	glDeleteVertexArrays(1, &vArray);

	return 0;
}
