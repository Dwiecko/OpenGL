#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "texture_loader.hpp"
#include "shader_stuff.h"


enum {
	SCENE = 0,
	SCREEN,
	NUMBER_OF_BUFFERS,
};

enum {
	PROGRAM_SCENE = 0,
	PROGRAM_CHANGED = 1,

	NUMBER_OF_PROGRAMS
};

// ---------------------------------------
GLuint program[NUMBER_OF_PROGRAMS];
GLuint vBuffer_pos[NUMBER_OF_BUFFERS];
GLuint vBuffer_uv[NUMBER_OF_BUFFERS];
GLuint vBuffer_normal[NUMBER_OF_BUFFERS];
GLuint vArray[NUMBER_OF_BUFFERS];

GLuint FrameBufferID;
GLuint DepthBufferID;

GLuint TextureBuffer;
GLuint TextureID;

// ---------------------------------------
glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matModel;




// ---------------------------------------
std::vector<glm::vec3> OBJ_vertices[2];
std::vector<glm::vec2> OBJ_uvs[2];
std::vector<glm::vec3> OBJ_normals[2];



int Window_Width = 800;
int Window_Height = 600;


// ---------------------------------------
// Kwadrat wspolrzedne
GLfloat vertices_pos[] =
{
    -1.0f, -1.0f, 0.0f,
	 2.0f, -1.0f, 0.0f,
     2.0f,  2.0f, 0.0f,

     2.0f,  2.0f, 0.0f,
	-1.0f,  2.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
};

// Kwadrat tekstura
GLfloat vertices_tex[] =
{
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
};




// ---------------------------------------
void RenderBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferID);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// AKTYWUJEMY program
	glUseProgram(program[PROGRAM_CHANGED]);


	glUniformMatrix4fv(glGetUniformLocation(program[PROGRAM_CHANGED], "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
	glUniformMatrix4fv(glGetUniformLocation(program[PROGRAM_CHANGED], "matView"), 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(glGetUniformLocation(program[PROGRAM_CHANGED], "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));


	// AKTYWUJEMY tekstury
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program[PROGRAM_CHANGED], "tex0"), 0);


	// Scena
	glBindVertexArray(vArray[SCENE]);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glDrawArrays(GL_TRIANGLES, 0, OBJ_vertices[SCENE].size());


	// Ekran
	glBindVertexArray(vArray[SCREEN]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(0);
}

// ---------------------------------------
void RenderFinalScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// AKTYWUJEMY program
	glUseProgram( program[PROGRAM_SCENE] );


	glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matProj" ), 1, GL_FALSE, glm::value_ptr(matProj) );
	glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );
	glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );


	// AKTYWUJEMY tekstury
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program[PROGRAM_SCENE], "tex0"), 0);


	// Scena
	glBindVertexArray( vArray[SCENE] );
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[SCENE].size() );


	// Ekran
	glBindVertexArray( vArray[SCREEN] );
	glBindTexture(GL_TEXTURE_2D, TextureBuffer);
	glDrawArrays( GL_TRIANGLES, 0, 6 );



    // WYLACZAMY program
    glUseProgram(0);
}


// ---------------------------------------
void DisplayScene()
{


	// Macierz widoku
	matView = glm::mat4x4( 1.0 );
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );

    // Macierz modelu
    matModel =  glm::mat4x4( 1.0 );


	RenderBuffer();

	RenderFinalScene();


	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
	Window_Width = width;
	Window_Height = height;

	glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}

// ---------------------------------------------------
void Initialize()
{
	_scene_translate_z = -5;
	_scene_translate_y = -2;

	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

    // ---------------------------------------
	// Ladowanie pliku OBJ
	if (!loadOBJ("scene.obj", OBJ_vertices[SCENE], OBJ_uvs[SCENE], OBJ_normals[SCENE]))
	{
		printf("Not loaded!\n");
		exit(1);
	}


	// ---------------------------------------
	// Tworzenie tekstury
	int tex_width;
	int tex_height;
	unsigned char *tex_data;
	loadBMP_custom("sand.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	// Tworzenie potoku OpenGL
	program[PROGRAM_SCENE] = glCreateProgram();
	glAttachShader( program[PROGRAM_SCENE], LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program[PROGRAM_SCENE], LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( program[PROGRAM_SCENE] );


	program[PROGRAM_CHANGED] = glCreateProgram();
	glAttachShader(program[PROGRAM_CHANGED], LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader(program[PROGRAM_CHANGED], LoadShader(GL_FRAGMENT_SHADER, "fragment_screen.glsl"));
	LinkAndValidateProgram(program[PROGRAM_CHANGED]);

	// SCENA
	glGenVertexArrays( 1, &vArray[SCENE] );
	glBindVertexArray( vArray[SCENE] );

	glGenBuffers( 1, &vBuffer_pos[SCENE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[SCENE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[SCENE].size() * sizeof(glm::vec3), &(OBJ_vertices[SCENE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[SCENE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[SCENE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[SCENE].size() * sizeof(glm::vec2), &(OBJ_uvs[SCENE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );

	// EKRAN
	glGenVertexArrays( 1, &vArray[SCREEN] );
	glBindVertexArray( vArray[SCREEN] );

	glGenBuffers( 1, &vBuffer_pos[SCREEN] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[SCREEN] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_pos), vertices_pos, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[SCREEN] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[SCREEN] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_tex), vertices_tex, GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );


    // Inne ustawienia openGL i sceny
	glEnable( GL_DEPTH_TEST );

	// 1. Generowanie bufora ramki
	glGenFramebuffers(1, &FrameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferID);

	// 2. Stworzenie tekstury do ktorej nastapi rendering
	glGenTextures(1, &TextureBuffer);
	glBindTexture(GL_TEXTURE_2D, TextureBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window_Width, Window_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureBuffer, 0);

	glGenRenderbuffers(1, &DepthBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, DepthBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Window_Width, Window_Height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthBufferID);
}



// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( Window_Width, Window_Height );
	glutCreateWindow( "OpenGL" );

	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( GLEW_OK != err )
	{
		printf("GLEW Error\n");
		exit(1);
	}

	// OpenGL
	if( !GLEW_VERSION_3_2 )
	{
		printf("Brak OpenGL 3.2!\n");
		exit(1);
	}


	Initialize();
	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutMouseWheelFunc( MouseWheel );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( SpecialKeys );

	glutMainLoop();


	return 0;
}
