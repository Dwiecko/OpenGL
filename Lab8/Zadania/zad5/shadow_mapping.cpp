// References:
// https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
// Janusz Ganczarski, OpenGL. Podstawy programowania grafiki 3D, Helion.


#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "shader_stuff.h"


enum {
	SCENE = 0,
	NUMBER_OF_BUFFERS,
};

enum {
	PROGRAM_SCENE = 0,
	NUMBER_OF_PROGRAMS,
};


// ---------------------------------------
GLuint program[NUMBER_OF_PROGRAMS];
GLuint vBuffer_pos[NUMBER_OF_BUFFERS];
GLuint vBuffer_normals[NUMBER_OF_BUFFERS];
GLuint vBuffer_uvs[NUMBER_OF_BUFFERS];
GLuint vArray[NUMBER_OF_BUFFERS];





// ---------------------------------------
glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matModel;


int Window_Width;
int Window_Height;

// ---------------------------------------
std::vector<glm::vec3> OBJ_vertices[NUMBER_OF_BUFFERS];
std::vector<glm::vec3> OBJ_normals[NUMBER_OF_BUFFERS];
std::vector<glm::vec2> OBJ_uvs[NUMBER_OF_BUFFERS];






// ---------------------------------------
// Swiatlo kierunkowe
glm::vec3 Light_Direction = glm::normalize(glm::vec3(0.2, -0.8f, 1.1f));
glm::vec3 Light_Position = glm::vec3(0.0f, 5.0f, -25.0f);
glm::vec3 Camera_Position;


// ---------------------------------------
// Cienie
GLuint DepthMap_Program;
GLuint DepthMap_FrameBuffer;
GLuint DepthMap_Texture;
const unsigned int DepthMap_Width = 2024, DepthMap_Height = 2024;


glm::mat4 lightProj = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 1.0f, 50.0f);

glm::mat4 lightView = glm::lookAt(Light_Position,
	Light_Position + Light_Direction,
	glm::vec3(0.0f, 1.0f, 0.0f));


// -------------------------------------------------------
void DrawShadowMap()
{
	// AKTYWUJEMY program
	glUseProgram(DepthMap_Program);

	glUniformMatrix4fv(glGetUniformLocation(DepthMap_Program, "lightProj"), 1, GL_FALSE, glm::value_ptr(lightProj));
	glUniformMatrix4fv(glGetUniformLocation(DepthMap_Program, "lightView"), 1, GL_FALSE, glm::value_ptr(lightView));

	glBindVertexArray(vArray[SCENE]);
	glDrawArrays(GL_TRIANGLES, 0, OBJ_vertices[SCENE].size());
	glBindVertexArray(0);


	// WYLACZAMY program
	glUseProgram(0);

}

// -------------------------------------------------------
void DrawToScreen()
{
	// Macierz widoku
	matView = glm::mat4x4(1.0);
	matView = glm::translate(matView, glm::vec3(_scene_translate_x, _scene_translate_y, _scene_translate_z));
	matView = glm::rotate(matView, _scene_rotate_x, glm::vec3(1.0f, 0.0f, 0.0f));
	matView = glm::rotate(matView, _scene_rotate_y, glm::vec3(0.0f, 1.0f, 0.0f));

	Camera_Position = ExtractCameraPos(matView);


	// Macierz modelu
	matModel = glm::mat4x4(1.0);

	// AKTYWUJEMY program
	glUseProgram(program[PROGRAM_SCENE]);


	glUniformMatrix4fv(glGetUniformLocation(program[PROGRAM_SCENE], "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
	glUniformMatrix4fv(glGetUniformLocation(program[PROGRAM_SCENE], "matView"), 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(glGetUniformLocation(program[PROGRAM_SCENE], "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));

	glUniformMatrix4fv(glGetUniformLocation(program[PROGRAM_SCENE], "lightProj"), 1, GL_FALSE, glm::value_ptr(lightProj));
	glUniformMatrix4fv(glGetUniformLocation(program[PROGRAM_SCENE], "lightView"), 1, GL_FALSE, glm::value_ptr(lightView));


	// Swiatlo kierunkowe
	glUniform3fv(glGetUniformLocation(program[PROGRAM_SCENE], "Light_Direction"), 1, glm::value_ptr(Light_Direction));
	glUniform3fv(glGetUniformLocation(program[PROGRAM_SCENE], "Camera_Position"), 1, glm::value_ptr(Camera_Position));


	// Shadow map textura na slot 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, DepthMap_Texture);
	glUniform1i(glGetUniformLocation(program[PROGRAM_SCENE], "shadowMap"), 1);


	// Scena
	glBindVertexArray(vArray[SCENE]);
	glDrawArrays(GL_TRIANGLES, 0, OBJ_vertices[SCENE].size());
	glBindVertexArray(0);


	// WYLACZAMY program
	glUseProgram(0);
}

// -------------------------------------------------------
void DisplayScene()
{



	// 1. Renderowanie z pozycji swiatla do textury DepthMap
	glViewport(0, 0, DepthMap_Width, DepthMap_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMap_FrameBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	DrawShadowMap();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// 2. Renderowanie z pozycji kamery na ekran
	glViewport(0, 0, Window_Width, Window_Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawToScreen();


	glutSwapBuffers();
}


// ---------------------------------------
void Reshape(int width, int height)
{
	Window_Width = width;
	Window_Height = height;

	glViewport(0, 0, width, height);
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f);
}

// ---------------------------------------------------
void Initialize()
{
	_scene_translate_z = -25;
	_scene_translate_y = -5;

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// ---------------------------------------
	// Ladowanie pliku OBJ
	if (!loadOBJ("scene.obj", OBJ_vertices[SCENE], OBJ_uvs[SCENE], OBJ_normals[SCENE]))
	{
		printf("Not loaded!\n");
		exit(1);
	}



	// Tworzenie potoku OpenGL
	program[PROGRAM_SCENE] = glCreateProgram();
	glAttachShader(program[PROGRAM_SCENE], LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader(program[PROGRAM_SCENE], LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram(program[PROGRAM_SCENE]);


	// SCENA
	glGenVertexArrays(1, &vArray[SCENE]);
	glBindVertexArray(vArray[SCENE]);

	glGenBuffers(1, &vBuffer_pos[SCENE]);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos[SCENE]);
	glBufferData(GL_ARRAY_BUFFER, OBJ_vertices[SCENE].size() * sizeof(glm::vec3), &(OBJ_vertices[SCENE])[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &vBuffer_normals[SCENE]);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_normals[SCENE]);
	glBufferData(GL_ARRAY_BUFFER, OBJ_normals[SCENE].size() * sizeof(glm::vec3), &(OBJ_normals[SCENE])[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);


	// Inne ustawienia openGL i sceny
	glEnable(GL_DEPTH_TEST);

	// Shadow mapping

	// 1. Texture
	glGenTextures(1, &DepthMap_Texture);
	glBindTexture(GL_TEXTURE_2D, DepthMap_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DepthMap_Width, DepthMap_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// 2. Frame buffer object
	glGenFramebuffers(1, &DepthMap_FrameBuffer);

	// 3. Attaching texture to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMap_FrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMap_Texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 4. OpenGL program for shadow map
	// Tworzenie potoku OpenGL
	DepthMap_Program = glCreateProgram();
	glAttachShader(DepthMap_Program, LoadShader(GL_VERTEX_SHADER, "depthmap.vertex.glsl"));
	glAttachShader(DepthMap_Program, LoadShader(GL_FRAGMENT_SHADER, "depthmap.fragment.glsl"));
	LinkAndValidateProgram(DepthMap_Program);

}



// ---------------------------------------------------
int main(int argc, char* argv[])
{
	// GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(Window_Width, Window_Height);
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
