//
//  callbackFunctions.h
//  IM project 3
//
//  Created by cpsfaculty on 10/25/14.
//  Copyright (c) 2014 ___Ju Shen___. All rights reserved.
//

#ifndef IM_callbackFunctions_h
#define IM_callbackFunctions_h
#endif

#include "CmyGLlib.h"
MyGLlib* glRenderer;

void changeSize(int ww, int hh)
{
	glRenderer->setWindowSize(hh, ww);
}
void initGL(int ww,int hh)
{
	glRenderer = new MyGLlib(ww,hh);
	glRenderer->printInfo();
}
void onDisplay()
{
	glRenderer->Render();
	glFlush();
	glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y)
{
	glRenderer->onKeyDown(key, x, y);
	switch (key)
	{
	case 27:
		exit(1);
	}
	glutPostRedisplay();
}
void onKeyUp(unsigned char key, int x, int y)
{
	glRenderer->onKeyUp(key, x, y);
	glutPostRedisplay();
}
void onSKeyDown(int key, int x, int y)
{
	glRenderer->onSKeyDown(key, x, y);
	glutPostRedisplay();
}
void onSKeyUp(int key, int x, int y)
{
	glRenderer->onSKeyUp(key, x, y);
	glutPostRedisplay();
}
void onMouse(int button, int state, int x, int y)
{

	GLint specialKey = glutGetModifiers();
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			if (specialKey == GLUT_ACTIVE_SHIFT)
			{
			}
			else
			{
			}
		}
		else if (state == GLUT_UP)
		{
		}
		break;

	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)

			break;

	default:
		break;

	}

	glutPostRedisplay(); //This is an important function to let the window re-rendered. If you do not call this function explicitly
	//the window never get refereshed. So you should call this function in the onKeyboard event as well if you
	//want to refresh the display

}

void onMouseMotion(int x, int y)
{
	glutPostRedisplay(); //This is an important function to let the window re-rendered. If you do not call this function explicitly
	//the window never get refereshed. So you should call this function in the onKeyboard event as well if you
	//want to refresh the display
}

/*Define the 3D objects that want to show*/
