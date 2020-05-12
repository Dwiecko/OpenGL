#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_stuff.h"
#include "texture_loader.hpp"

// ---------------------------------------
const char* ObjFileName = "uvtemplate.bmp";
const char* SecondObjFileName = "bubbles.bmp";
const int TexturesCount = 2;

GLuint program;
GLuint vBuffer_pos;
GLuint vBuffer_color;
GLuint vBuffer_uv;
GLuint vArray;

GLuint* Textures = new GLuint[2];


// ---------------------------------------
glm::mat4x4 Matrix_proj;	// projection matrix
glm::mat4x4 Matrix_mv;		// modelview matrix
glm::mat4x4 Matrix_proj_mv;	// projection*modelview matrix



// ---------------------------------------
GLfloat vertices_pos[] =
{
	-1.0f, -1.0f, 0.0f,	// bottom left
	 1.0f, -1.0f, 0.0f,	// bottom right
	 0.0f,  1.0f, 0.0f, // upper
};

GLfloat vertices_color[] =
{
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
};

GLfloat vertices_tex[] =
{
	0.0f, 0.0f, // for bottom left vertex
	1.0f, 0.0f, // for bottom right vertex
	0.5f, 1.0f, // for upper vertex
};



// ---------------------------------------
void DisplayScene()
{

	// 0. Czyszczenie ramki (jak zwykle)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1. Geometria sceny  (jak zwykle)
	Matrix_mv = glm::mat4x4(1.0);
	Matrix_mv = glm::translate(Matrix_mv, glm::vec3(_scene_translate_x, _scene_translate_y, _scene_translate_z));
	Matrix_mv = glm::rotate(Matrix_mv, _scene_rotate_x, glm::vec3(1.0f, 0.0f, 0.0f));
	Matrix_mv = glm::rotate(Matrix_mv, _scene_rotate_y, glm::vec3(0.0f, 1.0f, 0.0f));
	Matrix_proj_mv = Matrix_proj * Matrix_mv;
	glUniformMatrix4fv(glGetUniformLocation(program, "Matrix_proj_mv"), 1, GL_FALSE, glm::value_ptr(Matrix_proj_mv));
	
	// 2. Wlaczanie/aktywowanie tekstur (NOWE!)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Textures[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Textures[1]);

	// 3. Przekazanie identyfikatora samplera (w przypadku 1 tekstury opcjonalne)
	glUniform1i(glGetUniformLocation(program, "tex0"), 0);
	glUniform1i(glGetUniformLocation(program, "tex1"), 1);

	// 4. Generowanie obiektow na ekranie (jak zwykle)
	glDrawArrays(GL_TRIANGLES, 0, 3 * 3);

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
	int tex_width;
	int tex_height;
	unsigned char* tex_data;

	Textures = new GLuint[2];
	glGenTextures(TexturesCount, Textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	loadBMP_custom(ObjFileName, tex_width, tex_height, &tex_data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	free(tex_data);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Textures[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	loadBMP_custom(SecondObjFileName, tex_width, tex_height, &tex_data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	free(tex_data);


	// -------------------------------------------------------
	// Reszta bez zmian
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Programowanie potoku
	program = glCreateProgram();

	glAttachShader(program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader(program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram(program);

	// Vertex arrays

	glGenVertexArrays(1, &vArray);
	glBindVertexArray(vArray);

	// Wspolrzedne wierzchokow
	glGenBuffers(1, &vBuffer_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_pos), vertices_pos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// Kolor wierzcholkow
	glGenBuffers(1, &vBuffer_color);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_color), vertices_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// Wspolrzedne textury UV
	glGenBuffers(1, &vBuffer_uv);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_tex), vertices_tex, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glEnable(GL_DEPTH_TEST);

	glBindVertexArray(vArray);
	glUseProgram(program);

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

	glutMainLoop();

	// Cleaning();
	glDeleteProgram(program);
	glDeleteBuffers(1, &vBuffer_pos);
	glDeleteBuffers(1, &vBuffer_uv);
	glDeleteBuffers(1, &vBuffer_color);
	glDeleteVertexArrays(1, &vArray);
	glDeleteTextures(2, Textures);

	return 0;
}
