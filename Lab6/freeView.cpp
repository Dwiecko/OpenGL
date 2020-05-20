#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "obj_loader.hpp"
#include "shader_stuff.h"

// ---------------------------------------
// NOWE : OSWIETLENIE - parametry
// ---------------------------------------
typedef struct SLightParameters
{
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;
	glm::vec4 Position;
	float Attenuation;

	glm::vec3 __offset;


} SLightParameters;

SLightParameters Light1 = {
	glm::vec4(0.3, 0.3, 0.3, 1.0),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	glm::vec4(0.0, 3.0, 0.0, 1.0),
	0.01f,
};

typedef struct DirectionLightParameters
{
	glm::vec3 Direction;
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;

} DirectionLightParameters;

DirectionLightParameters Light2 = {
	glm::vec3(-0.2f, -1.0f, -0.3f),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	glm::vec4(0.0, 3.0, 0.0, 1.0)
};

// ---------------------------------------
// NOWE : MATERIALY - parametry
// ---------------------------------------
typedef struct SMaterial
{

	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	float Shininess;

} SMaterial;


SMaterial Material1 = {
	glm::vec3(0.8f, 0.8f, 0.2f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(0.7f, 0.7f, 0.7f),
	32.0f
};




// ---------------------------------------
GLuint program;
GLuint vBuffer_coord;
GLuint vBuffer_normal;
GLuint vArray;

GLuint lightDirection;

// ---------------------------------------
// Macierze przeksztalcen i rzutowania
// ---------------------------------------
glm::mat4x4 matProjection;
glm::mat4x4 matView;
glm::mat4x4 matModel;
glm::mat3x3 matNormal = glm::transpose(glm::inverse(glm::mat3(matModel)));

// --------------------------------------
std::vector<glm::vec3> OBJ_vertices;
std::vector<glm::vec2> OBJ_uvs;
std::vector<glm::vec3> OBJ_normals;

// ---------------------------------------
void DisplayScene()
{
	// Czyszczenie ramki
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//glutSetCursor(GLUT_CURSOR_NONE);
	
	GLfloat current_frame = glfwGetTime();
	deltaTime = current_frame - lastFrame;
	lastFrame = current_frame;

	// ------------------------------
	// 1. Geometria sceny
	// Ustawiamy macierz jednostkowa
	matView = glm::mat4x4(1.0);
	matView = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(glGetUniformLocation(program, "matProjection"), 1, GL_FALSE, glm::value_ptr(matProjection));
	glUniformMatrix4fv(glGetUniformLocation(program, "matView"), 1, GL_FALSE, glm::value_ptr(matView));

	// Nowe! wazne dla Specular light
	glUniform3fv(glGetUniformLocation(program, "Camera_Position"), 1, &cameraPos[0]);


	glDrawArrays(GL_TRIANGLES, 0, OBJ_vertices.size());

	glutSwapBuffers();
}


// ---------------------------------------
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	matProjection = glm::perspectiveFov(glm::radians(fov), (float)width, (float)height, 0.1f, 100.f);
}

// ---------------------------------------------------
void Initialize()
{

	_scene_translate_z = -10.0f;

	if (!loadOBJ("scene.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
	{
		printf("Not loaded!\n");
		exit(1);
	}


	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// 1. Programowanie potoku
	program = glCreateProgram();

	glAttachShader(program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader(program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	//glAttachShader(program, LoadShader(GL_FRAGMENT_SHADER, "direction.glsl"));


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


	// Wektory normalne
	glGenBuffers(1, &vBuffer_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_normal);
	glBufferData(GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);



	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);

	glBindVertexArray(vArray);
	glUseProgram(program);


	glUniform3fv(glGetUniformLocation(program, "myMaterial.Ambient"), 1, &Material1.Ambient[0]);
	glUniform3fv(glGetUniformLocation(program, "myMaterial.Diffuse"), 1, &Material1.Diffuse[0]);
	glUniform3fv(glGetUniformLocation(program, "myMaterial.Specular"), 1, &Material1.Specular[0]);
	glUniform1f(glGetUniformLocation(program, "myMaterial.Shininess"), Material1.Shininess);

	glUniform4fv(glGetUniformLocation(program, "myLight.Ambient"), 1, &Light1.Ambient[0]);
	glUniform4fv(glGetUniformLocation(program, "myLight.Diffuse"), 1, &Light1.Diffuse[0]);
	glUniform4fv(glGetUniformLocation(program, "myLight.Specular"), 1, &Light1.Specular[0]);
	glUniform4fv(glGetUniformLocation(program, "myLight.Position"), 1, &Light1.Position[0]);
	glUniform1f(glGetUniformLocation(program, "myLight.Attenuation"), Light1.Attenuation);
		
	glUniform3fv(glGetUniformLocation(program, "secondLight.Direction"), 1, &Light2.Direction[0]);
	glUniform4fv(glGetUniformLocation(program, "secondLight.Ambient"), 1, &Light2.Ambient[0]);
	glUniform4fv(glGetUniformLocation(program, "secondLight.Diffuse"), 1, &Light2.Diffuse[0]);
	glUniform4fv(glGetUniformLocation(program, "secondLight.Specular"), 1, &Light2.Specular[0]);
}

void Animation(int f)
{
	glutPostRedisplay();
	glutTimerFunc(10, Animation, 0);
}

// ---------------------------------------------------
int main(int argc, char* argv[])
{
	// GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(width, heigth);
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

	glutPassiveMotionFunc(MouseMotion);
	
	glutMouseWheelFunc(MouseWheel);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);

	glutTimerFunc(10, Animation, 0);

	glutMainLoop();

	// Cleaning();
	glDeleteProgram(program);
	glDeleteBuffers(1, &vBuffer_coord);
	glDeleteBuffers(1, &vBuffer_normal);
	glDeleteVertexArrays(1, &vArray);

	return 0;
}
