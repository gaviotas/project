//  Example: mouseMotion.c
//

#include<stdlib.h>
#include<gl\glut.h>
#include<stdio.h>

int winW, winH;   // <---- window's width (W) and height (H)

int mouseX = 0, mouseY = 0; // <-- use globals to know last mouse click location

int drawing_state = 0;

struct line {
	int vertex_s_x, vertex_s_y;
	int vertex_f_x, vertex_f_y;
	int drawing_state;
};

struct line line[100];
int num_i = 0;
							// Track Window Dimensions
void reshape(int w, int h)
{
	winW = w;
	winH = h;

	glMatrixMode(GL_PROJECTION);  // set the projection matrix (will talk about this later)
	glLoadIdentity();             // This clear any existing matrix

	gluOrtho2D(0, w, 0, h);     // use new windows coords as dimensions

	glMatrixMode(GL_MODELVIEW);    // set model transformation
	glLoadIdentity();              // to be empty (will talk about this later
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 27) // ASCII value 27 = ESC key
		exit(0);
}

// MousePress: Called when mouse is pressed.  Most likely the mouseMotion func will
// be called immediately afterwards if the mouse button is still down.
// NOTE: a single click will cause this function to be called twice
// once for GLUT_DOWN and once for GLUT_UP
void mousePress(GLint button, GLint state, GLint x, GLint y)
{
	if (button == GLUT_LEFT_BUTTON  && state == GLUT_DOWN)
	{
		printf("LEFT Mouse Down\n");
		mouseX = x;
		mouseY = winH - y;
		line[num_i].drawing_state = 1;

		line[num_i].vertex_s_x = mouseX;
		line[num_i].vertex_s_y = mouseY;		
		glutPostRedisplay();
	}
	if (button == GLUT_LEFT_BUTTON  && state == GLUT_UP)
	{
		printf("LEFT Mouse Up\n");
		mouseX = x;
		mouseY = winH - y;
		line[num_i].drawing_state = 0;

		line[num_i].vertex_f_x = mouseX;
		line[num_i].vertex_f_y = mouseY;
		num_i++;

		glutPostRedisplay();
	}

	if (button == GLUT_RIGHT_BUTTON  && state == GLUT_DOWN)
	{
		printf("RIGHT Mouse Down\n");
		mouseX = x;
		mouseY = winH - y;
		glutPostRedisplay();
	}
	if (button == GLUT_RIGHT_BUTTON  && state == GLUT_UP)
	{
		printf("RIGHT Mouse Up\n");
		mouseX = x;
		mouseY = winH - y;
		glutPostRedisplay();
	}

}

// MouseMotion: Called anytime mouse moves and button _is_ pressed
// note that we don't know which button is pressed, so this works
// with left/right/middle button
// if we want to know which button, we need to set a global variable in the
// mousePress callback
void mouseMotion(int x, int y)
{
	mouseX = x;
	mouseY = winH - y;
	printf("Mouse motion (%i %i) with a button down \n", mouseX, mouseY); // we don't know which button!  Need a global to record it from mousePress() function
	glutPostRedisplay();
}

// call anytime mouse moves and no button presed
void mousePassiveMotion(int x, int y)
{
	printf("Mouse motion (%i %i) _no_ button pressed \n", x, winH - y); // we don't know which button!  Need a global to record it from mousePress() function
}

void draw_line(int i) {
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex2i(line[i].vertex_s_x, line[i].vertex_s_y);
	if (line[i].drawing_state == 1) glVertex2i(mouseX, mouseY);
	else glVertex2i(line[i].vertex_f_x, line[i].vertex_f_y);
	glEnd();
}

void display()
{
	// Clear Color = black (you can change it)
	// Clear the buffer
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0, 0, 0);
/*
	glPointSize(4.0);
	glBegin(GL_POINTS);
	glVertex2i(line.vertex_s_x, line.vertex_s_y);
	glEnd();
	*/
	for (int j = 0; j < num_i+1; j++) {
		draw_line(j);
	}

	glFlush();

	glutSwapBuffers();

}


// HEY, look, i'm ignoring the command line!
// 
int main(int argc, char** argv)
{

	for (int j = 0; j < 100; j++) {
		line[j].vertex_s_x = -1;
		line[j].vertex_s_y = -1;
		line[j].vertex_f_x = -1;
		line[j].vertex_f_y = -1;
		line[j].drawing_state = 0;
	}
	glutInit(&argc, argv);   // not necessary unless on Unix
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(512, 512);
	glutCreateWindow("mouseMotion (lec3)");

	glutReshapeFunc(reshape);       // register respace (anytime window changes)
	glutKeyboardFunc(keyboard);     // register keyboard func
	glutMouseFunc(mousePress);      // register mouse press funct
	glutMotionFunc(mouseMotion);     // ** Note, the name is just glutMotionFunc (not glutMouseMotionFunc)
	glutPassiveMotionFunc(mousePassiveMotion); // Passive motion                 
	glutDisplayFunc(display);       // register display function
	glutMainLoop();

	return 1;

}
