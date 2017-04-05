//  Example: mouseMotion.c
//

#include<stdlib.h>
#include<gl\glut.h>
#include<stdio.h>
#include<math.h>

int winW, winH;   // <---- window's width (W) and height (H)

int mouseX = 0, mouseY = 0; // <-- use globals to know last mouse click location

int drawing_state = 0;

static int mode_program = 0; // 0 : Draw Mode , 1 : Selection Mode
static int mode_drawing = -1; // 0 : lines, 1 : rectangles, 2 : circles, 3 : triangle 

struct line {
	int vertex_s_x, vertex_s_y;
	int vertex_f_x, vertex_f_y;
	int drawing_state;
};

struct rect {
	int vertex_x_1, vertex_y_1;
	int vertex_x_2, vertex_y_2;
	int drawing_state;
};

struct circle {
	float rad;
	int x, y;
	int drawing_state;
};

struct line line[100];
struct rect rect[100];
struct circle circle[100];

int num_line = 0;
int num_rect = 0;
int num_circle = 0;

float get_distance(int x1, int y1, int x2, int y2) {

	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));

}
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
	switch (key) {
	case 'd' :
		mode_program = 0;
		break;
	case 's':
		mode_program = 1;
		break;
	case 'l' :
		mode_drawing = 0;
		break;
	case 'r':
		mode_drawing = 1;
		break;
	case 'c':
		mode_drawing = 2;
		break;
	case 't':
		mode_drawing = 3;
		break;
	case 27:  // ASCII value 27 = ESC key
		exit(0);
		break;
	
	}
		
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

		switch (mode_drawing) {
		case 0 :
			line[num_line].drawing_state = 1;

			line[num_line].vertex_s_x = mouseX;
			line[num_line].vertex_s_y = mouseY;
			break;
		case 1:
			rect[num_rect].drawing_state = 1;

			rect[num_rect].vertex_x_1 = mouseX;
			rect[num_rect].vertex_y_1 = mouseY;
			break;
		case 2:
			circle[num_circle].drawing_state = 1;

			circle[num_circle].x = mouseX;
			circle[num_circle].y = mouseY;
			break;
		}
		glutPostRedisplay();
	}
	if (button == GLUT_LEFT_BUTTON  && state == GLUT_UP)
	{
		printf("LEFT Mouse Up\n");
		mouseX = x;
		mouseY = winH - y;
		switch (mode_drawing) {
		case 0 :
			line[num_line].drawing_state = 0;

			line[num_line].vertex_f_x = mouseX;
			line[num_line].vertex_f_y = mouseY;
			num_line++;
			break;
		case 1:
			rect[num_rect].drawing_state = 0;

			rect[num_rect].vertex_x_2 = mouseX;
			rect[num_rect].vertex_y_2 = mouseY;
			num_rect++;
			break;
		case 2:
			circle[num_circle].drawing_state = 0;

			circle[num_circle].rad = get_distance(circle[num_circle].x, circle[num_circle].y, mouseX, mouseY);
			num_circle++;
			break;
		}
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
void draw_rect(int i) {
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
	glVertex2i(rect[i].vertex_x_1, rect[i].vertex_y_1);
	if (rect[i].drawing_state == 1)
	{ 
		glVertex2i(rect[i].vertex_x_1, mouseY);
		glVertex2i(mouseX, mouseY);
		glVertex2i(mouseX, rect[i].vertex_y_1); 
	}
	else {
		glVertex2i(rect[i].vertex_x_1, rect[i].vertex_y_2);
		glVertex2i(rect[i].vertex_x_2, rect[i].vertex_y_2);
		glVertex2i(rect[i].vertex_x_2, rect[i].vertex_y_1);
	}
	glEnd();
}
void draw_circle(int i) {
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
	if (circle[i].drawing_state == 1) {
		circle[i].rad = get_distance(mouseX, mouseY, circle[i].x, circle[i].y);
		printf("%d %d %d %d %f\n", mouseX, circle[i].x, mouseY, circle[i].y, circle[i].rad);
	}
	for (int j = 0; j < 360; j++) {
		double angle, x, y;
		angle = j*3.141592 / 180;
		x = circle[i].rad*cos(angle);
		y = circle[i].rad*sin(angle);
		glVertex2f(circle[i].x+x, circle[i].y+y);
	}
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
	for (int j = 0; j < num_line+1; j++) {
		draw_line(j);
	}
	for (int j = 0; j < num_rect + 1; j++) {
		draw_rect(j);
	}
	for (int j = 0; j < num_circle + 1; j++) {
		draw_circle(j);
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
