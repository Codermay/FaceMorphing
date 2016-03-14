#ifndef my_gl_lib_h
#define my_gl_lib_h
#include <gl/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <iostream>
#include "CScene.h"
#define VK_LEFT 100
#define VK_UP 101
#define VK_RIGHT 102
#define VK_DOWN 103
using namespace std;
using namespace glm;
class MyGLlib
{
private:
	GLuint shader_programe;
	Scene* Scenes[10];
	int num_scenes = 0;
	vec3 Pos;
	vec3 Rot;
	int WINDOW_HEIGHT, WINDOW_WIDTH;
public:
	MyGLlib(int ww, int hh) :
		WINDOW_HEIGHT(hh),
		WINDOW_WIDTH(ww)
	{
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			exit(0);
		}
		Pos = vec3(0, 0.5, 1);
		Rot = vec3(0, 0, 0);
		Scene* scene1 = new Scene();
		Add(scene1);
	}
	~MyGLlib(){
		for (int i = 0; i < num_scenes; i++)
			delete Scenes[i];
	}
	void Add(Scene* sce)
	{
		Scenes[num_scenes++] = sce;
	}
	void Render()
	{
		glEnable(GL_DEPTH_TEST); /* enable depth-testing */
		glDepthFunc(GL_LESS);/*depth-testing interprets a smaller value as "closer"*/

		mat4 rollPitchYaw = RotationX(Rot.v[1])*RotationY(Rot.v[2])*RotationZ(Rot.v[0]);
		mat4 finalRollPitchYaw = rollPitchYaw;
		vec3 finalUp = finalRollPitchYaw*vec3(0, 1, 0);
		vec3 finalForward = finalRollPitchYaw*vec3(0, 0, -1);
		vec3 EyePos = Pos;

		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		mat4 proj = GLM_perspective(45, float(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.01, 100.0f);
		
		mat4 view = GLM_look_at(EyePos, EyePos + finalForward, finalUp);


		glClearColor(1, 1, 1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (int i = 0; i < num_scenes; i++) {
			Scenes[i]->Render(view, proj, Pos, rollPitchYaw);
		}
	}
	void onKeyDown(unsigned char key, int x, int y)
	{
		if (key<123&& key>91)
			key -= 32;
		// Keyboard inputs to adjust player position
		if (key == 'W')							 Pos += RotationX(Rot.v[1])*RotationY(Rot.v[2])*RotationZ(Rot.v[0])*vec3(0, 0, -.05f);
		if (key == 'S')							 Pos += RotationX(Rot.v[1])*RotationY(Rot.v[2])*RotationZ(Rot.v[0])* vec3(0, 0, +.05f);
		if (key == 'D')                          Pos += RotationX(Rot.v[1])*RotationY(Rot.v[2])*RotationZ(Rot.v[0])*vec3(+.05f, 0, 0);
		if (key == 'A')                          Pos += RotationX(Rot.v[1])*RotationY(Rot.v[2])*RotationZ(Rot.v[0])* vec3(-.05f, 0, 0);
		if (key == 'R')                          Pos += RotationX(Rot.v[1])*RotationY(Rot.v[2])*RotationZ(Rot.v[0])* vec3(0, +.05f, 0);
		if (key == 'F')                          Pos += RotationX(Rot.v[1])*RotationY(Rot.v[2])*RotationZ(Rot.v[0])* vec3(0, -.05f, 0);
		if (key == 'X')                          { Rot=vec3(0,0,0); Pos = vec3(0, 0, 0); }
	}
	void onKeyUp(int key, int x, int y)
	{

	}
	void onSKeyDown(unsigned char key, int x, int y)
	{
		
		if (key == VK_LEFT)  Rot.v[2] += 3.0f;
		if (key == VK_RIGHT) Rot.v[2] -= 3.0f;
		if (key == VK_UP)     Rot.v[1] += 1.0f;
		if (key == VK_DOWN)   Rot.v[1] -= 1.0f;
	}
	void onSKeyUp(int key, int x, int y)
	{

	}
	//----------------------------------------------------------------
	void setWindowSize(int hh, int  ww)
	{
		WINDOW_HEIGHT = hh;
		WINDOW_WIDTH = ww;
	}
	void printInfo()
	{
		const GLubyte* renderer;
		const GLubyte* version;
		/* a vertex buffer object (VBO) is created here. this stores an array of data
		on the graphics adapter's memory. in our case - the vertex points */
		renderer = glGetString(GL_RENDERER); /* get renderer string */
		version = glGetString(GL_VERSION); /* version as a string */
		printf("Renderer: %s\n", renderer);
		printf("OpenGL version supported %s\n", version);
	}
};
#endif