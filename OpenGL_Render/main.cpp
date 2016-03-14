//
//  main.cpp
//  IM project 3
//
//  Created by cpsfaculty on 10/25/14.
//  Copyright (c) 2014 ___Ju Shen___. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include "callbackFunctions.h"

void initGlut_cb()
{
	// register callbacks
	glutDisplayFunc(onDisplay);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMouseMotion);
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialFunc(onSKeyDown);
	glutSpecialUpFunc(onSKeyUp);
	glutReshapeFunc(changeSize);
	glutIdleFunc(onDisplay);

	glutIgnoreKeyRepeat(0);
	//glutSpecialFunc(pressKey);
	//glutSpecialUpFunc(releaseKey);

}

int main(int argc, char * argv[])
{
	int WINDOW_WIDHT = 800;
	int WINDOW_HEIGHT = 600;
	/*Initialize glut stuff*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(WINDOW_WIDHT, WINDOW_HEIGHT);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("OpenGL-Xinzhong");

	
	//loadFile("bunn_low.obj");

	//initialize(); //You may creat an "initialize()" function here if there are any variables need to be assigned    

	/*Register GL stuff with the GLUT*/

	initGlut_cb();

	initGL(WINDOW_WIDHT, WINDOW_HEIGHT);
	/*Initialize the loop*/
	glutMainLoop();


	return 0;
}
