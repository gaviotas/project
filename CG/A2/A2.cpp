//  Example: mouseMotion.c
//

#include <stdlib.h>
#include <gl\glut.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAX_ARGS 100
#define MAX_MULT_S 100

int winW, winH;   // <---- window's width (W) and height (H)

int mouseX = 0, mouseY = 0; // <-- use globals to know last mouse click location

int drawing_state = 0;

static int mode_program = 0; // 0 : Draw Mode , 1 : Selection Mode
static int mode_drawing = -1; // 0 : lines, 1 : rectangles, 2 : circles, 3 : triangle 
static int mode_selection = -1; // 0 : select, 1 : rotation
static int mult_select;

static 	char * text;

struct line {
	int x1, y1;
	int x2, y2;
	int drawing_state;

	int t_x, t_y;
	int r_a;

};

struct rect {
	int x1, y1;
	int x2, y2;
	int x3, y3;
	int x4, y4;
	int drawing_state;

	int t_x, t_y;
	int r_a;
};

struct circle {
	double rad;
	int x, y;
	int drawing_state;

	int t_x, t_y;
	int r_a;
};

struct line line[MAX_ARGS];
struct rect rect[MAX_ARGS];
struct circle circle[MAX_ARGS];

int num_line = 0;
int num_rect = 0;
int num_circle = 0;

int selected_shape[MAX_MULT_S];
int selected_num[MAX_MULT_S];
int selected_x[MAX_MULT_S], selected_y[MAX_MULT_S];
int selected_count = 0;
double min_distance[MAX_MULT_S];

double get_distance(int x1, int y1, int x2, int y2) {

	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));

}

void find_min_dist() {
	// line part
	for (int i = 0; i < num_line; i++) {
		if (mode_selection == 0) {
			if (get_distance(line[i].x1, line[i].y1, mouseX, mouseY) < min_distance[i]) {
				min_distance[i] = get_distance(line[i].x1, line[i].y1, mouseX, mouseY);
				selected_num[selected_count] = i;
				selected_shape[selected_count] = 0;
				selected_x[selected_count] = line[i].x1;
				selected_y[selected_count] = line[i].y1;
			}
			if (get_distance(line[i].x2, line[i].y2, mouseX, mouseY) < min_distance[i]) {
				min_distance[i] = get_distance(line[i].x2, line[i].y2, mouseX, mouseY);
				selected_num[selected_count] = i;
				selected_shape[selected_count] = 0;
				selected_x[selected_count] = line[i].x2;
				selected_y[selected_count] = line[i].y2;
			}
		}
		else if (mode_selection == 1) {
			if (get_distance((line[i].x1+line[i].x2)/2, (line[i].y1+line[i].y2)/2, mouseX, mouseY) < min_distance[i]) {
				min_distance[i] = get_distance((line[i].x1 + line[i].x2) / 2, (line[i].y1 + line[i].y2) / 2, mouseX, mouseY);
				selected_num[selected_count] = i;
				selected_shape[selected_count] = 0;
				selected_x[selected_count] = (line[i].x1 + line[i].x2) / 2;
				selected_y[selected_count] = (line[i].y1 + line[i].y2) / 2;
			}
		}
	}
	// rect part
	for (int i = 0; i < num_rect; i++) {
		if (mode_selection == 0) {
			if (get_distance(rect[i].x1, rect[i].y1, mouseX, mouseY) < min_distance[i]) {
				min_distance[i] = get_distance(rect[i].x1, rect[i].y1, mouseX, mouseY);
				selected_num[selected_count] = i;
				selected_shape[selected_count] = 1;
				selected_x[selected_count] = rect[i].x1;
				selected_y[selected_count] = rect[i].y1;
			}
			if (get_distance(rect[i].x2, rect[i].y2, mouseX, mouseY) < min_distance[i]) {
				min_distance[i] = get_distance(rect[i].x2, rect[i].y2, mouseX, mouseY);
				selected_num[selected_count] = i;
				selected_shape[selected_count] = 1;
				selected_x[selected_count] = rect[i].x2;
				selected_y[selected_count] = rect[i].y2;
			}
			if (get_distance(rect[i].x3, rect[i].y3, mouseX, mouseY) < min_distance[i]) {
				min_distance[i] = get_distance(rect[i].x3, rect[i].y3, mouseX, mouseY);
				selected_num[selected_count] = i;
				selected_shape[selected_count] = 1;
				selected_x[selected_count] = rect[i].x3;
				selected_y[selected_count] = rect[i].y3;
			}
			if (get_distance(rect[i].x4, rect[i].y4, mouseX, mouseY) < min_distance[i]) {
				min_distance[i] = get_distance(rect[i].x4, rect[i].y4, mouseX, mouseY);
				selected_num[selected_count] = i;
				selected_shape[selected_count] = 1;
				selected_x[selected_count] = rect[i].x4;
				selected_y[selected_count] = rect[i].y4;
			}
		}
		else if (mode_selection == 1) {
			if (get_distance((rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2, mouseX, mouseY) < min_distance[i]) {
				min_distance[i] = get_distance((rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2, mouseX, mouseY);
				selected_num[selected_count] = i;
				selected_shape[selected_count] = 1;
				selected_x[selected_count] = (rect[i].x1 + rect[i].x2) / 2;
				selected_y[selected_count] = (rect[i].y1 + rect[i].y2) / 2;
			}
		}
	}
	// circle part
	for (int i = 0; i < num_circle; i++) {
		if (get_distance(circle[i].x, circle[i].y, mouseX, mouseY) < min_distance[i]) {
			min_distance[i] = get_distance(circle[i].x, circle[i].y, mouseX, mouseY);
			selected_num[selected_count] = i;
			selected_shape[selected_count] = 2;
			selected_x[selected_count] = circle[i].x;
			selected_y[selected_count] = circle[i].y;
		}
	}
}

void draw_text(const char *text, int length, int x, int y) {
	glMatrixMode(GL_PROJECTION);
	double *matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX, matrix);
	glLoadIdentity();
	glOrtho(0, 800, 0, 600, -5, -5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x, y);
	for (int i = 0; i < length; i++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);
	}
	glPopMatrix;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);
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

void myidle() {
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'd' :
		mode_program = 0;
		text = "Drawing Mode";
		break;
	case 's':
		mode_program = 1;
		mode_drawing = -1;
		mode_selection = 0;
		text = "Selection Mode";
		break;
	case 'l' :
		if (mode_program == 0) {
			mode_drawing = 0;
			text = "Line Mode";
		}
		break;
	case 'r':
		if (mode_program == 0) {
			mode_drawing = 1;
			text = "Rect Mode";
		}
		break;
	case 'c':
		if (mode_program == 0) {
			mode_drawing = 2;
			text = "Circle Mode";
		}
		break;
	case 't':
		if (mode_program == 0) {
			mode_drawing = 3;
			text = "Triangle Mode";
		}
		break;
	case 'o':
		if (mode_program == 1) {
			mode_selection = 1;
			text = "Rotation Mode";
		}
		break;

	case 27:  // ASCII value 27 = ESC key
		exit(0);
		break;
	default:
		text = "Hi, This is drawing pad!";
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

		if (mode_program == 0) {
			switch (mode_drawing) {
			case 0:
				line[num_line].drawing_state = 1;

				line[num_line].x1 = mouseX;
				line[num_line].y1 = mouseY;
				break;
			case 1:
				rect[num_rect].drawing_state = 1;

				rect[num_rect].x1 = mouseX;
				rect[num_rect].y1 = mouseY;
				break;
			case 2:
				circle[num_circle].drawing_state = 1;

				circle[num_circle].x = mouseX;
				circle[num_circle].y = mouseY;
				break;
			}
		}

		else if (mode_program == 1) {
			find_min_dist();
			selected_count++;
		}
		

		glutPostRedisplay();
	}
	if (button == GLUT_LEFT_BUTTON  && state == GLUT_UP)
	{
		printf("LEFT Mouse Up\n");
		mouseX = x;
		mouseY = winH - y;

		if (mode_program == 0) {
			switch (mode_drawing) {
			case 0:
				line[num_line].drawing_state = 0;

				line[num_line].x2 = mouseX;
				line[num_line].y2 = mouseY;
				num_line++;
				break;
			case 1:
				rect[num_rect].drawing_state = 0;

				rect[num_rect].x2 = mouseX;
				rect[num_rect].y2 = mouseY;
				rect[num_rect].x3 = rect[num_rect].x1;
				rect[num_rect].y3 = mouseY;
				rect[num_rect].x4 = mouseX;
				rect[num_rect].y4 = rect[num_rect].y1;

				num_rect++;
				break;
			case 2:
				circle[num_circle].drawing_state = 0;

				circle[num_circle].rad = get_distance(circle[num_circle].x, circle[num_circle].y, mouseX, mouseY);
				num_circle++;
				break;
			}
		}

		else if (mode_program == 1 && mode_selection == 0) {
			for (int i = 0; i < selected_count; i++) {
				switch (selected_shape[i]) {
				case 0:
					line[selected_num[i]].x1 += line[selected_num[i]].t_x;
					line[selected_num[i]].x2 += line[selected_num[i]].t_x;
					line[selected_num[i]].y1 += line[selected_num[i]].t_y;
					line[selected_num[i]].y2 += line[selected_num[i]].t_y;
					line[selected_num[i]].t_x = 0;
					line[selected_num[i]].t_y = 0;
				case 1:
					rect[selected_num[i]].x1 += rect[selected_num[i]].t_x;
					rect[selected_num[i]].x2 += rect[selected_num[i]].t_x;
					rect[selected_num[i]].x3 += rect[selected_num[i]].t_x;
					rect[selected_num[i]].x4 += rect[selected_num[i]].t_x;
					rect[selected_num[i]].y1 += rect[selected_num[i]].t_y;
					rect[selected_num[i]].y2 += rect[selected_num[i]].t_y;
					rect[selected_num[i]].y3 += rect[selected_num[i]].t_y;
					rect[selected_num[i]].y4 += rect[selected_num[i]].t_y;
					rect[selected_num[i]].t_x = 0;
					rect[selected_num[i]].t_y = 0;
				case 2:
					circle[selected_num[i]].x += circle[selected_num[i]].t_x;
					circle[selected_num[i]].y += circle[selected_num[i]].t_y;
					circle[selected_num[i]].t_x = 0;
					circle[selected_num[i]].t_y = 0;
				}
			}
			if (mult_select == 0) {
				for (int i = 0; i < selected_count; i++) {
					selected_shape[i] = -1;
					selected_num[i] = -1;
					selected_x[i] = -1;
					selected_y[i] = -1;
					min_distance[i] = 10;
				}
				selected_count = 0;
			}
		}
		else if (mode_program == 1 && mode_selection == 1) {
			double temp_angle;
			int temp1_x1, temp1_y1, temp1_x2, temp1_y2, temp1_x3, temp1_y3, temp1_x4, temp1_y4;
			int temp2_x1, temp2_y1, temp2_x2, temp2_y2, temp2_x3, temp2_y3, temp2_x4, temp2_y4;
			for (int i = 0; i < selected_count; i++) {
				switch (selected_shape[i]) {
				case 0:
					temp_angle = line[selected_num[i]].r_a * 3.141592 / 180;
					temp1_x1 = line[selected_num[i]].x1;
					temp1_y1 = line[selected_num[i]].y1;
					temp1_x2 = line[selected_num[i]].x2;
					temp1_y2 = line[selected_num[i]].y2;

					line[selected_num[i]].x1 -= (int)((double)(temp1_x1 + temp1_x2) / 2.0);
					line[selected_num[i]].x2 -= (int)((double)(temp1_x1 + temp1_x2) / 2.0);
					line[selected_num[i]].y1 -= (int)((double)(temp1_y1 + temp1_y2) / 2.0);
					line[selected_num[i]].y2 -= (int)((double)(temp1_y1 + temp1_y2) / 2.0);

					temp2_x1 = line[selected_num[i]].x1;
					temp2_y1 = line[selected_num[i]].y1;
					temp2_x2 = line[selected_num[i]].x2;
					temp2_y2 = line[selected_num[i]].y2;

					line[selected_num[i]].x1 = (int)((double)temp2_x1*cos(temp_angle) - (double)temp2_y1*sin(temp_angle));
					line[selected_num[i]].x2 = (int)((double)temp2_x2*cos(temp_angle) - (double)temp2_y2*sin(temp_angle));
					line[selected_num[i]].y1 = (int)((double)temp2_x1*sin(temp_angle) + (double)temp2_y1*cos(temp_angle));
					line[selected_num[i]].y2 = (int)((double)temp2_x2*sin(temp_angle) + (double)temp2_y2*cos(temp_angle));

					line[selected_num[i]].x1 += (int)((double)(temp1_x1 + temp1_x2) / 2.0);
					line[selected_num[i]].x2 += (int)((double)(temp1_x1 + temp1_x2) / 2.0);
					line[selected_num[i]].y1 += (int)((double)(temp1_y1 + temp1_y2) / 2.0);
					line[selected_num[i]].y2 += (int)((double)(temp1_y1 + temp1_y2) / 2.0);
					line[selected_num[i]].r_a = 0;
				case 1:
					temp_angle = rect[selected_num[i]].r_a * 3.141592 / 180;
					temp1_x1 = rect[selected_num[i]].x1;
					temp1_y1 = rect[selected_num[i]].y1;
					temp1_x2 = rect[selected_num[i]].x2;
					temp1_y2 = rect[selected_num[i]].y2;
					temp1_x3 = rect[selected_num[i]].x3;
					temp1_y3 = rect[selected_num[i]].y3;
					temp1_x4 = rect[selected_num[i]].x4;
					temp1_y4 = rect[selected_num[i]].y4;

					rect[selected_num[i]].x1 -= (int)((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[selected_num[i]].x2 -= (int)((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[selected_num[i]].y1 -= (int)((double)(temp1_y1 + temp1_y2) / 2.0);
					rect[selected_num[i]].y2 -= (int)((double)(temp1_y1 + temp1_y2) / 2.0);
					rect[selected_num[i]].x3 -= (int)((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[selected_num[i]].x4 -= (int)((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[selected_num[i]].y3 -= (int)((double)(temp1_y1 + temp1_y2) / 2.0);
					rect[selected_num[i]].y4 -= (int)((double)(temp1_y1 + temp1_y2) / 2.0);

					temp2_x1 = rect[selected_num[i]].x1;
					temp2_y1 = rect[selected_num[i]].y1;
					temp2_x2 = rect[selected_num[i]].x2;
					temp2_y2 = rect[selected_num[i]].y2;
					temp2_x3 = rect[selected_num[i]].x3;
					temp2_y3 = rect[selected_num[i]].y3;
					temp2_x4 = rect[selected_num[i]].x4;
					temp2_y4 = rect[selected_num[i]].y4;

					rect[selected_num[i]].x1 = (int)((double)temp2_x1*cos(temp_angle) - (double)temp2_y1*sin(temp_angle));
					rect[selected_num[i]].x2 = (int)((double)temp2_x2*cos(temp_angle) - (double)temp2_y2*sin(temp_angle));
					rect[selected_num[i]].y1 = (int)((double)temp2_x1*sin(temp_angle) + (double)temp2_y1*cos(temp_angle));
					rect[selected_num[i]].y2 = (int)((double)temp2_x2*sin(temp_angle) + (double)temp2_y2*cos(temp_angle));
					rect[selected_num[i]].x3 = (int)((double)temp2_x3*cos(temp_angle) - (double)temp2_y3*sin(temp_angle));
					rect[selected_num[i]].x4 = (int)((double)temp2_x4*cos(temp_angle) - (double)temp2_y4*sin(temp_angle));
					rect[selected_num[i]].y3 = (int)((double)temp2_x3*sin(temp_angle) + (double)temp2_y3*cos(temp_angle));
					rect[selected_num[i]].y4 = (int)((double)temp2_x4*sin(temp_angle) + (double)temp2_y4*cos(temp_angle));

					rect[selected_num[i]].x1 += (int)((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[selected_num[i]].x2 += (int)((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[selected_num[i]].y1 += (int)((double)(temp1_y1 + temp1_y2) / 2.0);
					rect[selected_num[i]].y2 += (int)((double)(temp1_y1 + temp1_y2) / 2.0);
					rect[selected_num[i]].x3 += (int)((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[selected_num[i]].x4 += (int)((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[selected_num[i]].y3 += (int)((double)(temp1_y1 + temp1_y2) / 2.0);
					rect[selected_num[i]].y4 += (int)((double)(temp1_y1 + temp1_y2) / 2.0);
					rect[selected_num[i]].r_a = 0;
				case 2:
					circle[selected_num[i]].x += circle[selected_num[i]].t_x;
					circle[selected_num[i]].y += circle[selected_num[i]].t_y;
					circle[selected_num[i]].t_x = 0;
					circle[selected_num[i]].t_y = 0;
				}

				if (mult_select == 0) {
					selected_shape[0] = -1;
					selected_num[0] = -1;
					selected_x[0] = -1;
					selected_y[0] = -1;
					min_distance[i] = 10;
				}
				else {
					selected_shape[i] = -1;
					selected_num[i] = -1;
					selected_x[i] = -1;
					selected_y[i] = -1;
					min_distance[i] = 10;
				}

			}
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

		int move_x, move_y;

		mouseX = x;
		mouseY = winH - y;
		//	printf("Mouse motion (%i %i) with a button down \n", mouseX, mouseY); // we don't know which button!  Need a global to record it from mousePress() function

		if (mode_program == 1 && mode_selection == 0) {
			switch (selected_shape[selected_count]) {
			case 0:
				move_x = mouseX - selected_x[selected_count-1];
				move_y = mouseY - selected_y[selected_count-1];
				for (int i = 0; i < selected_count; i++) {
					line[selected_num[i]].t_x = move_x;
					line[selected_num[i]].t_y = move_y;
				}
				break;
			case 1:
				move_x = mouseX - selected_x[selected_count];
				move_y = mouseY - selected_y[selected_count];
				rect[selected_num[selected_count]].t_x = move_x;
				rect[selected_num[selected_count]].t_y = move_y;
				break;
			case 2:
				move_x = mouseX - selected_x[selected_count];
				move_y = mouseY - selected_y[selected_count];
				circle[selected_num[selected_count]].t_x = move_x;
				circle[selected_num[selected_count]].t_y = move_y;
				break;
			}
		}
		else if (mode_program == 1 && mode_selection == 1) {
			switch (selected_shape[selected_count]) {
			case 0:
				line[selected_num[selected_count]].r_a = get_distance(mouseX, mouseY, selected_x[selected_count], selected_y[selected_count]);
				break;
			case 1:
				rect[selected_num[selected_count]].r_a = get_distance(mouseX, mouseY, selected_x[selected_count], selected_y[selected_count]);
				break;
			case 2:
				circle[selected_num[selected_count]].r_a = get_distance(mouseX, mouseY, selected_x[selected_count], selected_y[selected_count]);
				break;
			}
		}

	glutPostRedisplay();
}

// call anytime mouse moves and no button presed
void mousePassiveMotion(int x, int y)
{
//	printf("Mouse motion (%i %i) _no_ button pressed \n", x, winH - y); // we don't know which button!  Need a global to record it from mousePress() function
}

void draw_line(int i) {
	glLineWidth(2.0);
	glPushMatrix();
	glTranslated(line[i].t_x, line[i].t_y, 0);

	for (int j = 0; j < selected_count; j++) {
		if (mode_program == 1 && i == selected_num[j] && selected_shape[j] == 0 && mode_selection == 0) {
			glColor3f(1, 0, 0);
			glBegin(GL_LINE_LOOP);
			glVertex2i(selected_x[j] - 5, selected_y[j] - 5);
			glVertex2i(selected_x[j] + 5, selected_y[j] - 5);
			glVertex2i(selected_x[j] + 5, selected_y[j] + 5);
			glVertex2i(selected_x[j] - 5, selected_y[j] + 5);
			glEnd();
		}

		else if (mode_program == 1 && i == selected_num[selected_count] && selected_shape[selected_count] == 0 && mode_selection == 1) {
			glColor3f(0, 1, 0);
			glBegin(GL_LINE_LOOP);
			glVertex2i(selected_x[selected_count] - 5, selected_y[selected_count] - 5);
			glVertex2i(selected_x[selected_count] + 5, selected_y[selected_count] - 5);
			glVertex2i(selected_x[selected_count] + 5, selected_y[selected_count] + 5);
			glVertex2i(selected_x[selected_count] - 5, selected_y[selected_count] + 5);
			glEnd();
		}
	}
	if (mode_program == 1 && mode_selection == 1) {
		glTranslated((line[i].x1 + line[i].x2) / 2, (line[i].y1 + line[i].y2) / 2, 0);
		glRotated(line[i].r_a, 0, 0, 1);
		glTranslated((-1)*(line[i].x1 + line[i].x2) / 2, (-1)*(line[i].y1 + line[i].y2) / 2, 0);
	}

	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2i(line[i].x1, line[i].y1);
	if (line[i].drawing_state == 1) glVertex2i(mouseX, mouseY);
	else glVertex2i(line[i].x2, line[i].y2);
	glEnd();
	glPopMatrix();
}
void draw_rect(int i) {
	double angle;
	angle = rect[i].r_a * 3.141592 / 180;
	glLineWidth(2.0);
	glPushMatrix();
	glTranslated(rect[i].t_x, rect[i].t_y, 0);

	if (mode_program == 1 && i == selected_num[selected_count] && selected_shape[selected_count] == 1 && mode_selection == 0) {
		glColor3f(1, 0, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2i(selected_x[selected_count] - 5, selected_y[selected_count] - 5);
		glVertex2i(selected_x[selected_count] + 5, selected_y[selected_count] - 5);
		glVertex2i(selected_x[selected_count] + 5, selected_y[selected_count] + 5);
		glVertex2i(selected_x[selected_count] - 5, selected_y[selected_count] + 5);
		glEnd();
	}

	else if (mode_program == 1 && i == selected_num[selected_count] && selected_shape[selected_count] == 1 && mode_selection == 1) {
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2i(selected_x[selected_count] - 5, selected_y[selected_count] - 5);
		glVertex2i(selected_x[selected_count] + 5, selected_y[selected_count] - 5);
		glVertex2i(selected_x[selected_count] + 5, selected_y[selected_count] + 5);
		glVertex2i(selected_x[selected_count] - 5, selected_y[selected_count] + 5);
		glEnd();
	}


	if (mode_program == 1 && mode_selection == 1) {
		glTranslated((rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2, 0);
		glRotated(rect[i].r_a, 0, 0, 1);
		glTranslated((-1)*(rect[i].x1 + rect[i].x2) / 2, (-1)*(rect[i].y1 + rect[i].y2) / 2, 0);
	}

	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(rect[i].x1, rect[i].y1);
	if (rect[i].drawing_state == 1)
	{ 
		glVertex2f(rect[i].x1, mouseY);
		glVertex2f(mouseX, mouseY);
		glVertex2f(mouseX, rect[i].y1); 
	}
	else {
		glVertex2f(rect[i].x3, rect[i].y3);
		glVertex2f(rect[i].x2, rect[i].y2);
		glVertex2f(rect[i].x4, rect[i].y4);
	}
	glEnd();
	glPopMatrix();
}
void draw_circle(int i) {
	glLineWidth(2.0);
	glPushMatrix();
	glTranslated(circle[i].t_x, circle[i].t_y, 0);

	if (i == selected_num[selected_count] && selected_shape[selected_count] == 2 && mode_selection == 0) {
		glColor3f(1, 0, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2i(selected_x[selected_count] - 5, selected_y[selected_count] - 5);
		glVertex2i(selected_x[selected_count] + 5, selected_y[selected_count] - 5);
		glVertex2i(selected_x[selected_count] + 5, selected_y[selected_count] + 5);
		glVertex2i(selected_x[selected_count] - 5, selected_y[selected_count] + 5);
		glEnd();

	}
	glColor3f(0, 0, 0);

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
	glPopMatrix();
}
void display()
{
	if (GetKeyState(0x4D) & 0x8000) {
		mult_select = 1;
	}
	else mult_select = 0;

	// Clear Color = black (you can change it)
	// Clear the buffer
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0, 0, 0);
/*
	glPointSize(4.0);
	glBegin(GL_POINTS);
	glVertex2i(line.x1, line.y1);
	glEnd();
	*/

	draw_text(text, strlen(text), 0, 0);


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
	text = "Hi, This is drawing pad!";

	for (int j = 0; j < MAX_ARGS; j++) {
		line[j].x1 = -1;
		line[j].y1 = -1;
		line[j].x2 = -1;
		line[j].y2 = -1;
		line[j].drawing_state = 0;

		line[j].t_x = 0;
		line[j].t_y = 0;
	}

	for (int j = 0; j < MAX_ARGS; j++) {
		rect[j].x1 = -1;
		rect[j].y1 = -1;
		rect[j].x2 = -1;
		rect[j].y2 = -1;
		rect[j].drawing_state = 0;

		rect[j].t_x = 0;
		rect[j].t_y = 0;
		rect[j].r_a = 0;
	}

	for (int j = 0; j < MAX_MULT_S; j++) {
		min_distance[j] = 10;
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
	glutIdleFunc(myidle);
	glutDisplayFunc(display);       // register display function
	glutMainLoop();

	return 1;

}
