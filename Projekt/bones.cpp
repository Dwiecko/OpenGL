#define GLM_FORCE_SWIZZLE

#include <fstream>
#include <stdio.h>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "texture_loader.hpp"

#include "shader_stuff.h"

#include "ft2build.h"
#include "freetype.h"
#include "text-ft.h"

enum {
	ZIEMIA,
	TLO,

	PODSTAWA,
    RAMIE,
    KULA,

	NUMBER_OF_OBJECTS
};

enum {
	NORMAL,
	COLLECTIVE,
	HARMFUL
};

// ---------------------------------------
const char* fontName = "arial.ttf";

int ObjectCollisionID = -1;
float hasCollision = 0.0f;
float maxHeight = 4.0f;
float FpsRate = 1.0f;

int Frame, CurrentTime, LastTime, ilosc_punktow, currentObjectID = 0;
int totalLife = 100;
int totalNumberOfObjects = 15;

glm::mat4x4 Matrix_proj;
glm::mat4x4 Matrix_mv;
glm::mat4x4 Matrix_proj_mv;


class CTriangle
{
public:
	glm::vec3 p[3];


	// Ax + By + Cz + D = 0
	float A, B, C, D;

	CTriangle() { }
	CTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
	{
		p[0] = p1;
		p[1] = p2;
		p[2] = p3;

		calculateEquation();
	}

	void calculateEquation(void)
	{
		glm::vec3 v1, v2;

		v1 = p[1] - p[0];
		v2 = p[2] - p[0];

		A = v1.y * v2.z - v1.z * v2.y;	 // A
		B = v1.z * v2.x - v1.x * v2.z;   // B
		C = v1.x * v2.y - v1.y * v2.x;   // C


		D = -(A * p[0].x + B * p[0].y + C * p[0].z);	// D

		if (C == 0)
		{
			printf("Uwaga! Trojkat pionowy.\n");
		}

	}

	inline float sign(glm::vec2 p1, glm::vec3 p2, glm::vec3 p3)
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}

	inline bool isInside(glm::vec2 point)
	{
		float s1, s2, s3;
		bool has_neg, has_pos;

		s1 = sign(point, p[0], p[1]);
		s2 = sign(point, p[1], p[2]);
		s3 = sign(point, p[2], p[0]);

		has_neg = (s1 < 0) || (s2 < 0) || (s3 < 0);
		has_pos = (s1 > 0) || (s2 > 0) || (s3 > 0);

		return !(has_neg && has_pos);
	}

	inline float altitude(glm::vec2 point)
	{
		if (C) return -(A * point.x + B * point.y + D) / C;
		else { return 99999; }
	}

};

class CGround
{
public:

	int Number_of_triangles = 0;
	std::vector<CTriangle> triangles;

	void CreateFromOBJ(std::vector<glm::vec3> vert)
	{
		glm::vec3 p1, p2, p3;

		std::vector<glm::vec3>::iterator it = vert.begin();
		while (it != vert.end())
		{
			p1 = *it++;
			p2 = *it++;
			p3 = *it++;

			triangles.push_back(CTriangle(p1.xzy(), p2.xzy(), p3.xzy()));

		}

		Number_of_triangles = triangles.size();
		printf("Created CGround of %d triangles.\n", Number_of_triangles);
	}

	float getAltitute(glm::vec2 point)
	{
		CTriangle* collTriangle = NULL;

		std::vector<CTriangle>::iterator it;
		for (it = triangles.begin(); it != triangles.end(); it++)
		{
			if ((*it).isInside(point))
			{
				collTriangle = &(*it);
				break;
			}
		}

		if (collTriangle)
		{
			return collTriangle->altitude(point);
		}
		else
		{
			printf("Brak ziemi pod nogami!\n");
			return 0.0;
		}
	}

};
class CBone;
std::vector<CBone> CollisionObjectsTable;

// -----------------------------------------
class CBone
{
public:
	int id;
    int VBO_Size;
	int objectType;

	GLuint Program;
	GLuint TextureID;
	GLuint vBuffer_coord;
	GLuint vBuffer_normal;
	GLuint vBuffer_uv;
	GLuint vArray;

	std::vector<glm::vec3> OBJ_vertices;
	std::vector<glm::vec2> OBJ_uvs;
	std::vector<glm::vec3> OBJ_normals;

	glm::vec3 Position;
	glm::vec3 OldPosition;
	glm::vec3 Light_Ambient = glm::vec3(0.1, 0.1, 0.1);
	glm::vec3 Light_Diffuse = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 Light_Position = glm::vec3(0.0, 1.0, -3.0);

	glm::mat4x4 matModel;

	float radius = 0.5f;
	float altitude;
	
	CBone* next = NULL;
	
	bool isPart;

	CBone() { objectType = NORMAL; }

	void loadBMP(const char* path) {
		int tex_width;
		int tex_height;
		unsigned char* tex_data;

		loadBMP_custom(path, tex_width, tex_height, &tex_data);

		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	}

	void loadObject(const char* path) {
		if (!loadOBJ(path, OBJ_vertices, OBJ_uvs, OBJ_normals))
		{
			printf("Not loaded!\n");
			exit(1);
		}
	}

	void loadShaders(const char* fragment, const char* vertex) {
		glAttachShader(Program, LoadShader(GL_VERTEX_SHADER, vertex));
		glAttachShader(Program, LoadShader(GL_FRAGMENT_SHADER, fragment));
		LinkAndValidateProgram(Program);
	}

    void Draw(glm::mat4 _stackModel = glm::mat4(1.0))
    {
		glBindVertexArray(vArray);
		glUseProgram(Program);

		glm::mat4 localModel = _stackModel * matModel;
		glUniform3fv(glGetUniformLocation(Program, "Light_Ambient"), 1, &Light_Ambient[0]);
		glUniform3fv(glGetUniformLocation(Program, "Light_Diffuse"), 1, &Light_Diffuse[0]);
		glUniform3fv(glGetUniformLocation(Program, "Light_Position"), 1, &Light_Position[0]);

		glUniformMatrix4fv(glGetUniformLocation(Program, "Matrix_proj_mv"), 1, GL_FALSE, glm::value_ptr(localModel));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glBindVertexArray(vArray);
		glDrawArrays(GL_TRIANGLES, 0, OBJ_vertices.size());

		if (next != NULL) {
			next->Draw(localModel);
		}
    }

	void prepareVBO() {
		glGenVertexArrays(1, &vArray);
		glBindVertexArray(vArray);

		glGenBuffers(1, &vBuffer_coord);
		glBindBuffer(GL_ARRAY_BUFFER, vBuffer_coord);
		glBufferData(GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &vBuffer_normal);
		glBindBuffer(GL_ARRAY_BUFFER, vBuffer_normal);
		glBufferData(GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &vBuffer_uv);
		glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv);
		glBufferData(GL_ARRAY_BUFFER, OBJ_uvs.size() * sizeof(glm::vec2), &OBJ_vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);

		VBO_Size = OBJ_vertices.size();
	}

	void Set(
		const char* objFile, const char* texFile, 
		const char* fragmentFile, const char* vertexFile, 
		int _objectType, 
		glm::vec3 position = glm::vec3(0.0, 0.0, 0.0), 
		bool isPartOfBody = false)
	{
		TextureID = currentObjectID;
		id = currentObjectID;

		loadObject(objFile);
		loadBMP(texFile);

		Program = glCreateProgram();
		loadShaders(fragmentFile, vertexFile);
		prepareVBO();

		SetPosition(position.x, position.y, position.z);

		objectType = _objectType;
		isPart = isPartOfBody;
		currentObjectID += 1;
	}

	void SetPosition(float x, float y, float z)
	{
		printf("Nowa pozycja obiektu: %f %f %f\n", x, y, z);
		Position = glm::vec3(x, y, z);
		matModel = glm::translate(glm::mat4(1.0), Position);
	}

	void Obroc(float angle, float x, float y, float z) {
		this->matModel = glm::rotate(matModel, angle, glm::vec3(x, y, z));
	}

	void Przesun(float x, float z, CGround* _ground) {
		this->Position += glm::vec3(x, 0.0, z);
		this->matModel = glm::translate(matModel, glm::vec3(x, 0.0, z));
	}

	void Przesun(float x, float z, CGround* _ground, std::vector<CBone> objectsTable) {
		this->OldPosition = this->Position;

		this->Position += glm::vec3(x, 0.0, z);

		ObjectCollisionID = -1;
		bool isPointCollected = false;
		for (int i = 2; i < objectsTable.size(); i++) {
			if ((i != id) && (!objectsTable[i].isPart) && this->isCollision(objectsTable[i])) {
				if (objectsTable[i].objectType == HARMFUL) {
					printf("\n===========Kolizja z raniacym obiektem nr: %d!===========", i);
					totalLife -= 1;
					if (totalLife <= 0) {
						totalLife = 100;
						this->OldPosition = glm::vec3(0.0, 0.0, 0.0);
					}
					
					this->Position = this->OldPosition;
				}
				else if(objectsTable[i].objectType == COLLECTIVE) {
					printf("\n===========Kolizja z punktowym obiektem nr: %d!===========", i);
					ilosc_punktow += 1;
					isPointCollected = true;
				}

				hasCollision = 1.0f;
				ObjectCollisionID = i;
				break;
			}
		}
		if (isPointCollected) {
			//CollisionObjectsTable.erase(CollisionObjectsTable.begin() + ObjectCollisionID);
			CBone tempElement;
			CollisionObjectsTable[ObjectCollisionID] = tempElement;
			glutPostRedisplay();
		}
		float y = _ground->getAltitute(glm::vec2(this->Position.x, this->Position.z));
		printf("\nWysokosc: %f", y);

		if (y > maxHeight) {
			printf("\nUwazaj! Zbyt stromo");
			this->Position = this->OldPosition;
		}
		else {
			this->Position.y = y;

			this->matModel = glm::translate(glm::mat4(1.0), Position);

			this->OldPosition = this->Position;
		}
	}

	float getAltitude(CGround* _ground) {
		return _ground->getAltitute(glm::vec2(this->Position.x, this->Position.z));
	}

	bool isCollision(const CBone& otherInstance) {
		float distance = glm::distance(this->Position, otherInstance.Position);
		if (distance <= this->radius + otherInstance.radius) {
			return true;
		}
		return false;
	}
};

glm::vec3 Camera_Position;
CGround myGround;
CGround tlo;

// ---------------------------------------
// Generate text after all programs
void GenerateText() {
	char txt[255];
	CurrentTime = glutGet(GLUT_ELAPSED_TIME);
	if ((CurrentTime - LastTime) > 1000)
	{
		FpsRate = ((double)Frame * 1000.0f / (((double)CurrentTime - (double)LastTime)));
		LastTime = CurrentTime;
		Frame = 0;
	}
	sprintf(txt, "FPS: %f", FpsRate);
	RenderText(txt, 600, 570, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));

	RenderText("ESC - Exit", 25, 570, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

	char zycie[255];
	sprintf(zycie, "Ilosc zycia: %d", totalLife);
	RenderText(zycie, 300, 570, 0.5f, glm::vec3(1.0, 0.0, 0.0f));

	char punkty[255];
	sprintf(zycie, "Punkty: %d", ilosc_punktow);
	RenderText(zycie, 450, 570, 0.5f, glm::vec3(1.0, 0.0, 0.0f));
}

void DisplayScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	for (CBone activeElement : CollisionObjectsTable) {
		Matrix_mv = glm::mat4x4(1.0);
		
		// normal camera
		//Matrix_mv = glm::translate(Matrix_mv, glm::vec3(
		//	_scene_translate_x, _scene_translate_y, _scene_translate_z));

		// 3-rd person camera
		Matrix_mv = glm::translate(Matrix_mv, glm::vec3(
			-(CollisionObjectsTable[PODSTAWA].Position.x), 
			_scene_translate_y,
			-(CollisionObjectsTable[PODSTAWA].Position.z + 10.0f)));

		Matrix_mv = glm::rotate(Matrix_mv, _scene_rotate_x, glm::vec3(1.0f, 0.0f, 0.0f));
		Matrix_mv = glm::rotate(Matrix_mv, _scene_rotate_y, glm::vec3(0.0f, 1.0f, 0.0f));
		Matrix_proj_mv = Matrix_proj * Matrix_mv;

		// Draw only whole parts, part of main body is drawn after updating model
		if (!activeElement.isPart) {
			activeElement.Draw(Matrix_proj_mv);
		}
	}

	GenerateText();

	glUseProgram(0);
	glutSwapBuffers();
}

// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
	Matrix_proj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f);
}

// --------------------------------------------------------------
void Keyboard( unsigned char key, int x, int y )
{
    float move_vec = 0.2;

    switch(key)
    {
		case 27:
			exit(0);
			break;

		case 'w': {
			CollisionObjectsTable[PODSTAWA].Przesun(0.0, -0.1, &myGround, CollisionObjectsTable);
			break;
		}

		case 's':
		{
			CollisionObjectsTable[PODSTAWA].Przesun(0.0, 0.1, &myGround, CollisionObjectsTable);
			break;
		}

		case 'a':
			CollisionObjectsTable[PODSTAWA].Przesun(-0.1, 0.0, &myGround, CollisionObjectsTable);
			break;

		case 'd':
			CollisionObjectsTable[PODSTAWA].Przesun(0.1, 0.0, &myGround, CollisionObjectsTable);
			break;
		
		case 'f': {
			CollisionObjectsTable[PODSTAWA].Obroc(-0.1, 0.0, 1.0, 0.0);
			break;
		}

		case 'h': {
			CollisionObjectsTable[PODSTAWA].Obroc(0.1, 0.0, 1.0, 0.0);
			break;
		}

		case 'q':
			CollisionObjectsTable[KULA].Obroc(0.1, 0.0, 1.0, 0.0);
			break;

		case 'e':
			CollisionObjectsTable[KULA].Obroc(-0.1, 0.0, 1.0, 0.0);
			break;

			// RESTART
		case 'r':
			CollisionObjectsTable[PODSTAWA].SetPosition(0.0, 0.0, 0.0);
			break;
    }

	printf("\nPozycja podstawy x:%f y:%f z:%f", 
		CollisionObjectsTable[PODSTAWA].Position.x, 
		CollisionObjectsTable[PODSTAWA].Position.y, 
		CollisionObjectsTable[PODSTAWA].Position.z);
	printf("\nTranslacja x:%f y:%f z:%f", _scene_translate_x, _scene_translate_y, _scene_translate_z);
	printf("\nPozycja kamery - x:%f y:%f z:%f", Camera_Position.x, Camera_Position.y, Camera_Position.z);

	//_scene_translate_x = -(Podstawa.Position.x);
	_scene_translate_y = -(CollisionObjectsTable[PODSTAWA].Position.y + 5.0f);
	//_scene_translate_z = -(Podstawa.Position.z + 10.0f);

    glutPostRedisplay();
}

glm::vec3 GetRandomObjectPosition() {
	float randomX = rand() % 15;
	float randomZ = rand() % 15;
	float y = myGround.getAltitute(glm::vec2(randomX, randomZ));

	return glm::vec3(randomX, y, randomZ);
}

void AddElementToObjectsTable(
	const char* objFile, const char* texFile,
	const char* fragmentFile, const char* vertexFile,
	int objectType = NORMAL,
	glm::vec3 position = glm::vec3(0.0, 0.0, 0.0),
	bool isPartOfBody = false) 
{
	CBone element;
	element.Set(objFile, texFile, fragmentFile, vertexFile, objectType, position, isPartOfBody);
	CollisionObjectsTable.push_back(element);
}

// ---------------------------------------------------
void Initialize()
{
	_scene_translate_z = -10;
	_scene_translate_y = -5;
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

	glEnable(GL_DEPTH_TEST);

	// Creating OBJ's
	AddElementToObjectsTable("obj/scena.obj", "obj/piasek.bmp", "fragment.glsl", "vertex.glsl");
	myGround.CreateFromOBJ(CollisionObjectsTable[ZIEMIA].OBJ_vertices);

	AddElementToObjectsTable("obj/tlo.obj", "obj/tlo.bmp", "fragment.glsl", "vertex.glsl");

	AddElementToObjectsTable("obj/podstawa.obj", "obj/uvtemplate.bmp", "fragment.glsl", "vertex.glsl", NORMAL, glm::vec3(-3.0, 0.0, 0.0), false);
	AddElementToObjectsTable("obj/ramie.obj", "obj/uvtemplate.bmp", "fragment.glsl", "vertex.glsl", NORMAL, glm::vec3(0.0, 4.0, 0.0), true);
	AddElementToObjectsTable("obj/kula.obj", "obj/uvtemplate.bmp", "fragment.glsl", "vertex.glsl", NORMAL, glm::vec3(-3.5, 0.0, 0.0), true);

	int additionalObjectsToCreate = (totalNumberOfObjects - NUMBER_OF_OBJECTS) / 2;
	for (int i = 0; i < additionalObjectsToCreate; i++)
	{
		AddElementToObjectsTable("obj/truskawka.obj", "obj/truskawka.bmp", "fragment.glsl", "vertex.glsl", COLLECTIVE, GetRandomObjectPosition(), false);
	}

	for (int i = 0; i < additionalObjectsToCreate; i++)
	{
		AddElementToObjectsTable("obj/kaktus.obj", "obj/kaktus.bmp", "fragment.glsl", "vertex.glsl", HARMFUL, GetRandomObjectPosition(), false);
	}

	// Objects reference
	CollisionObjectsTable[PODSTAWA].next = &CollisionObjectsTable[RAMIE];
	CollisionObjectsTable[RAMIE].next = &CollisionObjectsTable[KULA];

	// Font
	InitText(fontName, 36);

	glutPostRedisplay();
	glUseProgram(0);
}


// ---------------------------------------------------
void Animation(int arg)
{
	Frame++;
	glutTimerFunc(5, Animation, 0);

	for (CBone element : CollisionObjectsTable) {
		if (element.objectType == COLLECTIVE) {
			CollisionObjectsTable[element.id].Obroc(-0.1, 0.0, 1.0, 0.0);
		}
	}

	glutPostRedisplay();
}


// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 800, 600 );
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
	//glutMotionFunc( MouseMotion );
	glutMouseWheelFunc( MouseWheel );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( SpecialKeys );
	glutTimerFunc(5, Animation, 0);
	glutMainLoop();

	return 0;
}