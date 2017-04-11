//  Example: mouseMotion.c
//

#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAX_ARGS 100
#define MAX_MULT_S 100

int winW, winH;   // <---- window's width (W) and height (H)

int mouseX = 0, mouseY = 0; // <-- use globals to know last mouse click location
double move_x, move_y, move_angle;
int drawing_state = 0;

static int mode_program = 0; // 0 : Draw Mode , 1 : Selection Mode
static int mode_drawing = -1; // 0 : lines, 1 : rectangles, 2 : circles, 3 : triangle
static int mode_selection = -1; // 0 : select, 1 : rotation
static int mult_select;
static int line_width = 1;
static int line_color = 0;

static int count = 0;

static 	char * text;
FILE *fp;

struct line {
	double x1, y1;
	double x2, y2;
	int drawing_state;
	int w;
	int selected;
	int order;
	int c;

};

struct rect {
	double x1, y1;
	double x2, y2;
	double x3, y3;
	double x4, y4;
	int drawing_state;
	int w;
	int selected;
	int order;
	int c;
};

struct circle {
	double rad;
	double x, y;
	int drawing_state;
	int w;
	int selected;
	int order;
	int c;
};

struct triangle {
	double x1, y1;
	double x2, y2;
	double x3, y3;
	int drawing_state;
	int w;
	int selected;
	int order;
	int c;
};

struct penta {
	double rad;
	double x, y;
	double angle;
	int drawing_state;
	int w;
	int selected;
	int order;
	int c;
};

struct line line[MAX_ARGS];
struct rect rect[MAX_ARGS];
struct circle circle[MAX_ARGS];
struct triangle triangle[MAX_ARGS];
struct penta penta[MAX_ARGS];

int num_line = 0;
int num_rect = 0;
int num_circle = 0;
int num_triangle = 0;
int num_penta = 0;

int selected_shape;
int selected_num;
double selected_x, selected_y;
int selected_count = 0;
double min_distance = 10;

double get_distance(double x1, double y1, double x2, double y2) {

	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));

}

int find_min_dist() {
	// line part
	for (int i = 0; i < num_line; i++) {
		if (mode_selection == 0) {
			if (get_distance(line[i].x1, line[i].y1, mouseX, mouseY) < min_distance) {
				min_distance = get_distance(line[i].x1, line[i].y1, mouseX, mouseY);
				selected_shape = 0;
				selected_num = i;
				selected_x = line[i].x1;
				selected_y = line[i].y1;
			}
			if (get_distance(line[i].x2, line[i].y2, mouseX, mouseY) < min_distance) {
				min_distance = get_distance(line[i].x2, line[i].y2, mouseX, mouseY);
				selected_shape = 0;
				selected_num = i;
				selected_x = line[i].x2;
				selected_y = line[i].y2;
			}
		}
		else if (mode_selection == 1) {
			if (get_distance((line[i].x1+line[i].x2)/2, (line[i].y1+line[i].y2)/2, mouseX, mouseY) < min_distance) {
				min_distance = get_distance((line[i].x1 + line[i].x2) / 2, (line[i].y1 + line[i].y2) / 2, mouseX, mouseY);
				selected_shape = 0;
				selected_num = i;
				selected_x = (line[i].x1 + line[i].x2) / 2;
				selected_y = (line[i].y1 + line[i].y2) / 2;
			}
		}
	}
	// rect part
	for (int i = 0; i < num_rect; i++) {
		if (mode_selection == 0) {
			if (get_distance(rect[i].x1, rect[i].y1, mouseX, mouseY) < min_distance) {
				min_distance = get_distance(rect[i].x1, rect[i].y1, mouseX, mouseY);
				selected_num = i;
				selected_shape = 1;
				selected_x = rect[i].x1;
				selected_y = rect[i].y1;
			}
			if (get_distance(rect[i].x2, rect[i].y2, mouseX, mouseY) < min_distance) {
				min_distance = get_distance(rect[i].x2, rect[i].y2, mouseX, mouseY);
				selected_num = i;
				selected_shape = 1;
				selected_x = rect[i].x2;
				selected_y = rect[i].y2;
			}
			if (get_distance(rect[i].x3, rect[i].y3, mouseX, mouseY) < min_distance) {
				min_distance = get_distance(rect[i].x3, rect[i].y3, mouseX, mouseY);
				selected_num = i;
				selected_shape = 1;
				selected_x = rect[i].x3;
				selected_y = rect[i].y3;
			}
			if (get_distance(rect[i].x4, rect[i].y4, mouseX, mouseY) < min_distance) {
				min_distance = get_distance(rect[i].x4, rect[i].y4, mouseX, mouseY);
				selected_num = i;
				selected_shape = 1;
				selected_x = rect[i].x4;
				selected_y = rect[i].y4;
			}
		}
		else if (mode_selection == 1) {
			if (get_distance((rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2, mouseX, mouseY) < min_distance) {
				min_distance = get_distance((rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2, mouseX, mouseY);
				selected_num = i;
				selected_shape = 1;
				selected_x = (rect[i].x1 + rect[i].x2) / 2;
				selected_y = (rect[i].y1 + rect[i].y2) / 2;
			}
		}
	}
	// circle part
	for (int i = 0; i < num_circle; i++) {
		if (get_distance(circle[i].x, circle[i].y, mouseX, mouseY) < min_distance) {
			min_distance = get_distance(circle[i].x, circle[i].y, mouseX, mouseY);
			selected_num = i;
			selected_shape = 2;
			selected_x = circle[i].x;
			selected_y = circle[i].y;
		}
	}
	// triangle part
	for (int i = 0; i < num_triangle; i++) {
		if (mode_selection == 0) {
			if (get_distance(triangle[i].x1, triangle[i].y1, mouseX, mouseY) < min_distance) {
				min_distance = get_distance(triangle[i].x1, triangle[i].y1, mouseX, mouseY);
				selected_num = i;
				selected_shape = 3;
				selected_x = triangle[i].x1;
				selected_y = triangle[i].y1;
			}
			if (get_distance(triangle[i].x2, triangle[i].y2, mouseX, mouseY) < min_distance) {
				min_distance = get_distance(triangle[i].x2, triangle[i].y2, mouseX, mouseY);
				selected_num = i;
				selected_shape = 3;
				selected_x = triangle[i].x2;
				selected_y = triangle[i].y2;
			}
			if (get_distance(triangle[i].x3, triangle[i].y3, mouseX, mouseY) < min_distance) {
				min_distance = get_distance(triangle[i].x3, triangle[i].y3, mouseX, mouseY);
				selected_num = i;
				selected_shape = 3;
				selected_x = triangle[i].x3;
				selected_y = triangle[i].y3;
			}
		}
		else if (mode_selection == 1) {
			if (get_distance((triangle[i].x1 + triangle[i].x2 + triangle[i].x3) / 3, (triangle[i].y1 + triangle[i].y2 + triangle[i].y3) / 3, mouseX, mouseY) < min_distance) {
				min_distance = get_distance((triangle[i].x1 + triangle[i].x2 + triangle[i].x3) / 3, (triangle[i].y1 + triangle[i].y2 + triangle[i].y3) / 3, mouseX, mouseY);
				selected_num = i;
				selected_shape = 3;
				selected_x = (triangle[i].x1 + triangle[i].x2 + triangle[i].x3) / 3;
				selected_y = (triangle[i].y1 + triangle[i].y2 + triangle[i].y3) / 3;
			}
		}
	}
	// pentagon part
	for (int i = 0; i < num_penta; i++) {
		if (get_distance(penta[i].x, penta[i].y, mouseX, mouseY) < min_distance) {
			min_distance = get_distance(penta[i].x, penta[i].y, mouseX, mouseY);
			selected_num = i;
			selected_shape = 4;
			selected_x = penta[i].x;
			selected_y = penta[i].y;
		}
	}
	switch (selected_shape) {
	case 0 :
		if (line[selected_num].selected == 1) selected_count--;
		line[selected_num].selected = 1;
		break;
	case 1:
		if (rect[selected_num].selected == 1) selected_count--;
		rect[selected_num].selected = 1;
		break;
	case 2:
		if (circle[selected_num].selected == 1) selected_count--;
		circle[selected_num].selected = 1;
		break;
	case 3:
		if (triangle[selected_num].selected == 1) selected_count--;
		triangle[selected_num].selected = 1;
		break;
	case 4:
		if (penta[selected_num].selected == 1) selected_count--;
		penta[selected_num].selected = 1;
		break;
	}
	if (min_distance < 10) return 1;
	else return 0;
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
	case 'p':
		if (mode_program == 0) {
			mode_drawing = 4;
			text = "Pentagon Mode";
		}
		break;
	case 'o':
		if (mode_program == 1) {
			mode_selection = 1;
			text = "Rotation Mode";
		}
		break;
	case 'm' :
		if (mode_program == 1 && mode_selection == 0) {
			text = "Selection Mode";
		}
		else if (mode_program == 1 && mode_selection == 1) {
			text = "Rotation Mode";
		}
		mult_select = 1;
		break;
	case 'u' :
		int undo_shape;
		int undo_num;
		for (int i = 0; i < num_line; i++) {

			if (line[i].order == count-1) {

				undo_shape = 0;
				undo_num = i;
			}
		}
		for (int i = 0; i < num_rect; i++) {

			if (rect[i].order == count - 1) {
				undo_shape = 1;
				undo_num = i;
			}
		}
		for (int i = 0; i < num_circle; i++) {

			if (circle[i].order == count - 1) {
				undo_shape = 2;
				undo_num = i;
			}
		}
		for (int i = 0; i < num_triangle; i++) {

			if (triangle[i].order == count - 1) {
				undo_shape = 3;
				undo_num = i;
			}
		}
		for (int i = 0; i < num_penta; i++) {

			if (penta[i].order == count - 1) {
				undo_shape = 4;
				undo_num = i;
			}
		}
		switch (undo_shape) {
		case 0:
			line[undo_num].x1 = 0;
			line[undo_num].x2 = 0;
			line[undo_num].y1 = 0;
			line[undo_num].y2 = 0;
			line[undo_num].drawing_state = 0;
			num_line--;
			count--;
			break;
		case 1:
			rect[undo_num].x1 = 0;
			rect[undo_num].x2 = 0;
			rect[undo_num].x3 = 0;
			rect[undo_num].x4 = 0;
			rect[undo_num].y1 = 0;
			rect[undo_num].y2 = 0;
			rect[undo_num].y3 = 0;
			rect[undo_num].y4 = 0;

			rect[undo_num].drawing_state = 0;
			rect[undo_num].selected = 0;
			num_rect--;
			count--;
			break;
		case 2:
			circle[undo_num].x = 0;
			circle[undo_num].y = 0;
			circle[undo_num].rad = 0;
			circle[undo_num].drawing_state = 0;
			circle[undo_num].selected = 0;
			num_circle--;
			count--;
			break;
		case 3:
			triangle[undo_num].x1 = 0;
			triangle[undo_num].x2 = 0;
			triangle[undo_num].x3 = 0;
			triangle[undo_num].y1 = 0;
			triangle[undo_num].y2 = 0;
			triangle[undo_num].y3 = 0;

			triangle[undo_num].drawing_state = 0;
			triangle[undo_num].selected = 0;
			num_triangle--;
			count--;
			break;
		case 4:
			penta[undo_num].x = 0;
			penta[undo_num].y = 0;
			penta[undo_num].rad = 0;
			penta[undo_num].drawing_state = 0;
			penta[undo_num].selected = 0;
			num_penta--;
			count--;
			break;
		}
		if(count < 0) count = 0;
		break;
	case 'e' :
		for (int i = 0; i < num_line; i++) {
			line[i].x1 = 0;
			line[i].x2 = 0;
			line[i].y1 = 0;
			line[i].y2 = 0;
			line[i].drawing_state = 0;
			line[i].selected = 0;
		}
		for (int i = 0; i < num_rect; i++) {
			rect[i].x1 = 0;
			rect[i].x2 = 0;
			rect[i].x3 = 0;
			rect[i].x4 = 0;
			rect[i].y1 = 0;
			rect[i].y2 = 0;
			rect[i].y3 = 0;
			rect[i].y4 = 0;
			rect[i].drawing_state = 0;
			rect[i].selected = 0;
		}
		for (int i = 0; i < num_circle; i++) {
			circle[i].x = 0;
			circle[i].y = 0;
			circle[i].drawing_state = 0;
			circle[i].selected = 0;
			circle[i].rad = 0;
		}
		for (int i = 0; i < num_triangle; i++) {
			triangle[i].x1 = 0;
			triangle[i].x2 = 0;
			triangle[i].x3 = 0;
			triangle[i].y1 = 0;
			triangle[i].y2 = 0;
			triangle[i].y3 = 0;
			triangle[i].drawing_state = 0;
			triangle[i].selected = 0;
		}
		for (int i = 0; i < num_penta; i++) {
			penta[i].x = 0;
			penta[i].y = 0;
			penta[i].drawing_state = 0;
			penta[i].selected = 0;
			penta[i].rad = 0;
		}
		num_line = 0;
		num_rect = 0;
		num_circle = 0;
		num_triangle = 0;
		num_penta = 0;
		move_x = 0;
		move_y = 0;
		move_angle = 0;
		count = 0;
		break;
	case 'z' :
		fp = fopen("data.txt", "wt");
		if (fp == NULL) {
			printf(" file open failed");
			break;
		}
		fprintf(fp, "%d\n", count);
		fprintf(fp, "%d\n", num_line);
		for (int i = 0; i < num_line; i++) {
			fprintf(fp, "%lf %lf %lf %lf %d %d %d %d\n", line[i].x1, line[i].y1, line[i].x2, line[i].y2, line[i].w, line[i].drawing_state, line[i].selected, line[i].order);
		}
		fprintf(fp, "%d\n", num_rect);
		for (int i = 0; i < num_rect; i++) {
			fprintf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf %d %d %d %d\n", rect[i].x1, rect[i].y1, rect[i].x2, rect[i].y2, rect[i].x3, rect[i].y3, rect[i].x4, rect[i].y4, rect[i].w, rect[i].drawing_state, rect[i].selected, rect[i].order);
		}
		fprintf(fp, "%d\n", num_circle);
		for (int i = 0; i < num_circle; i++) {
			fprintf(fp, "%lf %lf %lf %d %d %d %d\n", circle[i].rad, circle[i].x, circle[i].y, circle[i].w, circle[i].drawing_state, circle[i].selected, circle[i].order);
		}
		fprintf(fp, "%d\n", num_triangle);
		for (int i = 0; i < num_triangle; i++) {
			fprintf(fp, "%lf %lf %lf %lf %lf %lf %d %d %d %d\n", triangle[i].x1, triangle[i].y1, triangle[i].x2, triangle[i].y2, triangle[i].x3, triangle[i].y3, triangle[i].w, triangle[i].drawing_state, triangle[i].selected, triangle[i].order);
		}
		fprintf(fp, "%d\n", num_penta);
		for (int i = 0; i < num_penta; i++) {
			fprintf(fp, "%lf %lf %lf %d %d %d %d\n", penta[i].rad, penta[i].x, penta[i].y, penta[i].w, penta[i].drawing_state, penta[i].selected, penta[i].order);
		}
		fclose(fp);
		break;
	case 'x':

		for (int i = 0; i < num_line; i++) {
			line[i].x1 = 0;
			line[i].x2 = 0;
			line[i].y1 = 0;
			line[i].y2 = 0;
			line[i].drawing_state = 0;
			line[i].selected = 0;
		}
		for (int i = 0; i < num_rect; i++) {
			rect[i].x1 = 0;
			rect[i].x2 = 0;
			rect[i].x3 = 0;
			rect[i].x4 = 0;
			rect[i].y1 = 0;
			rect[i].y2 = 0;
			rect[i].y3 = 0;
			rect[i].y4 = 0;
			rect[i].drawing_state = 0;
			rect[i].selected = 0;
		}
		for (int i = 0; i < num_circle; i++) {
			circle[i].x = 0;
			circle[i].y = 0;
			circle[i].drawing_state = 0;
			circle[i].selected = 0;
			circle[i].rad = 0;
		}
		for (int i = 0; i < num_triangle; i++) {
			triangle[i].x1 = 0;
			triangle[i].x2 = 0;
			triangle[i].x3 = 0;
			triangle[i].y1 = 0;
			triangle[i].y2 = 0;
			triangle[i].y3 = 0;
			triangle[i].drawing_state = 0;
			triangle[i].selected = 0;
		}
		for (int i = 0; i < num_penta; i++) {
			penta[i].x = 0;
			penta[i].y = 0;
			penta[i].drawing_state = 0;
			penta[i].selected = 0;
			penta[i].rad = 0;
		}
		num_line = 0;
		num_rect = 0;
		num_circle = 0;
		num_triangle = 0;
		num_penta = 0;
		move_x = 0;
		move_y = 0;
		move_angle = 0;
		count = 0;


		fp = fopen("data.txt", "rt");
		if (fp == NULL) {
			printf("file open failed");
			break;
		}
		fscanf(fp, "%d\n", &count);
		fscanf(fp, "%d\n", &num_line);
		for (int i = 0; i < num_line; i++) {
			fscanf(fp, "%lf %lf %lf %lf %d %d %d %d\n", &line[i].x1, &line[i].y1, &line[i].x2, &line[i].y2, &line[i].w, &line[i].drawing_state, &line[i].selected, &line[i].order);
		}
		fscanf(fp, "%d\n", &num_rect);
		for (int i = 0; i < num_rect; i++) {
			fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf %d %d %d %d\n", &rect[i].x1, &rect[i].y1, &rect[i].x2, &rect[i].y2, &rect[i].x3, &rect[i].y3, &rect[i].x4, &rect[i].y4, &rect[i].w, &rect[i].drawing_state, &rect[i].selected, &rect[i].order);
		}
		fscanf(fp, "%d\n", &num_circle);
		for (int i = 0; i < num_circle; i++) {
			fscanf(fp, "%lf %lf %lf %d %d %d %d\n", &circle[i].rad, &circle[i].x, &circle[i].y, &circle[i].w, &circle[i].drawing_state, &circle[i].selected, &circle[i].order);
		}
		fscanf(fp, "%d\n", &num_triangle);
		for (int i = 0; i < num_triangle; i++) {
			fscanf(fp, "%lf %lf %lf %lf %lf %lf %d %d %d %d\n", &triangle[i].x1, &triangle[i].y1, &triangle[i].x2, &triangle[i].y2, &triangle[i].x3, &triangle[i].y3, &triangle[i].w, &triangle[i].drawing_state, &triangle[i].selected, &triangle[i].order);
		}
		fscanf(fp, "%d\n", &num_penta);
		for (int i = 0; i < num_penta; i++) {
			fscanf(fp, "%lf %lf %lf %d %d %d %d\n", &penta[i].rad, &penta[i].x, &penta[i].y, &penta[i].w, &penta[i].drawing_state, &penta[i].selected, &penta[i].order);
		}
		fclose(fp);
		break;
	case '1' :
		line_width = 1;
		break;
	case '2':
		line_width = 2;
		break;
	case '3':
		line_width = 3;
		break;
	case '4':
		line_width = 4;
		break;
	case '5':
		line_width = 5;
		break;
	case '6':
		line_color = 0;
		break;
	case '7':
		line_color = 1;
		break;
	case '8':
		line_color = 2;
		break;
	case 27:  // ASCII value 27 = ESC key
		exit(0);
		break;
	default:
		text = "Hi, This is drawing pad!";
		break;
	}

}

void keyboard_up(unsigned char key, int x, int y) {
    switch (key) {
	case 'm' :
		mult_select = 0;
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
				line[num_line].w = line_width;
				line[num_line].c = line_color;
				break;
			case 1:
				rect[num_rect].drawing_state = 1;

				rect[num_rect].x1 = mouseX;
				rect[num_rect].y1 = mouseY;
				rect[num_rect].w = line_width;
				rect[num_rect].c = line_color;
				break;
			case 2:
				circle[num_circle].drawing_state = 1;

				circle[num_circle].x = mouseX;
				circle[num_circle].y = mouseY;
				circle[num_circle].w = line_width;
				circle[num_circle].c = line_color;
				break;
			case 3:
				triangle[num_triangle].drawing_state = 1;

				triangle[num_triangle].x1 = mouseX;
				triangle[num_triangle].y1 = mouseY;
				triangle[num_triangle].w = line_width;
				triangle[num_triangle].c = line_color;
				break;
			case 4:
				penta[num_penta].drawing_state = 1;

				penta[num_penta].x = mouseX;
				penta[num_penta].y = mouseY;
				penta[num_penta].w = line_width;
				penta[num_penta].c = line_color;
				break;
			}
		}

		else if (mode_program == 1) {
			if (find_min_dist() == 1) {
				selected_count++;
//				selected_x = mouseX;
//				selected_y = mouseY;
			}

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
				line[num_line].order = count;
				num_line++;
				count++;
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
				rect[num_rect-1].order = count;
				count++;
				break;
			case 2:
				circle[num_circle].drawing_state = 0;

				circle[num_circle].rad = get_distance(circle[num_circle].x, circle[num_circle].y, mouseX, mouseY);
				num_circle++;
				circle[num_circle-1].order = count;
				count++;
				break;
			case 3:
				triangle[num_triangle].drawing_state = 0;

				triangle[num_triangle].x2 = mouseX;
				triangle[num_triangle].y2 = mouseY;
				triangle[num_triangle].x3 = 2*triangle[num_triangle].x1-mouseX;
				triangle[num_triangle].y3 = mouseY;
				num_triangle++;
				triangle[num_triangle-1].order = count;
				count++;
				break;
			case 4:
				penta[num_penta].drawing_state = 0;

				penta[num_penta].rad = get_distance(penta[num_penta].x, penta[num_penta].y, mouseX, mouseY);
				num_penta++;
				penta[num_penta - 1].order = count;
				count++;
				break;
			}
		}

		else if (mode_program == 1 && mode_selection == 0) {
			for (int i = 0; i < MAX_ARGS; i++) {
				if (line[i].selected == 1) {
					line[i].x1 += move_x;
					line[i].x2 += move_x;
					line[i].y1 += move_y;
					line[i].y2 += move_y;
					selected_x += move_x;
					selected_y += move_y;
	//				printf("%d %d\n", selected_x, selected_y);
				}
				if (rect[i].selected == 1) {
					rect[i].x1 += move_x;
					rect[i].x2 += move_x;
					rect[i].x3 += move_x;
					rect[i].x4 += move_x;
					rect[i].y1 += move_y;
					rect[i].y2 += move_y;
					rect[i].y3 += move_y;
					rect[i].y4 += move_y;
					selected_x += move_x;
					selected_y += move_y;
				}
				if (circle[i].selected == 1) {
					circle[i].x += move_x;
					circle[i].y += move_y;
					selected_x += move_x;
					selected_y += move_y;
				}
				if (triangle[i].selected == 1) {
					triangle[i].x1 += move_x;
					triangle[i].x2 += move_x;
					triangle[i].x3 += move_x;
					triangle[i].y1 += move_y;
					triangle[i].y2 += move_y;
					triangle[i].y3 += move_y;
					selected_x += move_x;
					selected_y += move_y;
				}
				if (penta[i].selected == 1) {
					penta[i].x += move_x;
					penta[i].y += move_y;
					selected_x += move_x;
					selected_y += move_y;
				}
			}
			if (mult_select == 0) {
				for (int i = 0; i < MAX_ARGS; i++) {
					line[i].selected = 0;
					rect[i].selected = 0;
					circle[i].selected = 0;
					triangle[i].selected = 0;
					penta[i].selected = 0;
				}
				min_distance = 10;
				selected_count = 0;
				selected_shape = -1;
				selected_num = -1;

			}
			move_x = 0;
			move_y = 0;
			min_distance = 10;
		}
		else if (mode_program == 1 && mode_selection == 1) {
			double temp_angle;
			double temp1_x1, temp1_y1, temp1_x2, temp1_y2, temp1_x3, temp1_y3, temp1_x4, temp1_y4;
			double temp2_x1, temp2_y1, temp2_x2, temp2_y2, temp2_x3, temp2_y3, temp2_x4, temp2_y4;
			for (int i = 0; i < MAX_ARGS; i++) {
				if (line[i].selected == 1) {
					temp_angle = move_angle * 3.141592 / 180;
					temp1_x1 = line[i].x1;
					temp1_y1 = line[i].y1;
					temp1_x2 = line[i].x2;
					temp1_y2 = line[i].y2;

					line[i].x1 -= ((double)(temp1_x1 + temp1_x2) / 2.0);
					line[i].x2 -= ((double)(temp1_x1 + temp1_x2) / 2.0);
					line[i].y1 -= ((double)(temp1_y1 + temp1_y2) / 2.0);
					line[i].y2 -= ((double)(temp1_y1 + temp1_y2) / 2.0);

					temp2_x1 = line[i].x1;
					temp2_y1 = line[i].y1;
					temp2_x2 = line[i].x2;
					temp2_y2 = line[i].y2;

					line[i].x1 = ((double)temp2_x1*cos(temp_angle) - (double)temp2_y1*sin(temp_angle));
					line[i].x2 = ((double)temp2_x2*cos(temp_angle) - (double)temp2_y2*sin(temp_angle));
					line[i].y1 = ((double)temp2_x1*sin(temp_angle) + (double)temp2_y1*cos(temp_angle));
					line[i].y2 = ((double)temp2_x2*sin(temp_angle) + (double)temp2_y2*cos(temp_angle));

					line[i].x1 += ((double)(temp1_x1 + temp1_x2) / 2.0);
					line[i].x2 += ((double)(temp1_x1 + temp1_x2) / 2.0);
					line[i].y1 += ((double)(temp1_y1 + temp1_y2) / 2.0);
					line[i].y2 += ((double)(temp1_y1 + temp1_y2) / 2.0);
				}
				if (rect[i].selected == 1) {

					temp_angle = move_angle * 3.141592 / 180;
					temp1_x1 = rect[i].x1;
					temp1_y1 = rect[i].y1;
					temp1_x2 = rect[i].x2;
					temp1_y2 = rect[i].y2;
					temp1_x3 = rect[i].x3;
					temp1_y3 = rect[i].y3;
					temp1_x4 = rect[i].x4;
					temp1_y4 = rect[i].y4;

					rect[i].x1 -= ((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[i].x2 -= ((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[i].y1 -= ((double)(temp1_y1 + temp1_y2) / 2.0);
					rect[i].y2 -= ((double)(temp1_y1 + temp1_y2) / 2.0);
					rect[i].x3 -= ((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[i].x4 -= ((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[i].y3 -= ((double)(temp1_y1 + temp1_y2) / 2.0);
					rect[i].y4 -= ((double)(temp1_y1 + temp1_y2) / 2.0);

					temp2_x1 = rect[i].x1;
					temp2_y1 = rect[i].y1;
					temp2_x2 = rect[i].x2;
					temp2_y2 = rect[i].y2;
					temp2_x3 = rect[i].x3;
					temp2_y3 = rect[i].y3;
					temp2_x4 = rect[i].x4;
					temp2_y4 = rect[i].y4;

					rect[i].x1 = ((double)temp2_x1*cos(temp_angle) - (double)temp2_y1*sin(temp_angle));
					rect[i].x2 = ((double)temp2_x2*cos(temp_angle) - (double)temp2_y2*sin(temp_angle));
					rect[i].y1 = ((double)temp2_x1*sin(temp_angle) + (double)temp2_y1*cos(temp_angle));
					rect[i].y2 = ((double)temp2_x2*sin(temp_angle) + (double)temp2_y2*cos(temp_angle));
					rect[i].x3 = ((double)temp2_x3*cos(temp_angle) - (double)temp2_y3*sin(temp_angle));
					rect[i].x4 = ((double)temp2_x4*cos(temp_angle) - (double)temp2_y4*sin(temp_angle));
					rect[i].y3 = ((double)temp2_x3*sin(temp_angle) + (double)temp2_y3*cos(temp_angle));
					rect[i].y4 = ((double)temp2_x4*sin(temp_angle) + (double)temp2_y4*cos(temp_angle));

					rect[i].x1 += ((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[i].x2 += ((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[i].y1 += ((double)(temp1_y1 + temp1_y2) / 2.0);
					rect[i].y2 += ((double)(temp1_y1 + temp1_y2) / 2.0);
					rect[i].x3 += ((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[i].x4 += ((double)(temp1_x1 + temp1_x2) / 2.0);
					rect[i].y3 += ((double)(temp1_y1 + temp1_y2) / 2.0);
					rect[i].y4 += ((double)(temp1_y1 + temp1_y2) / 2.0);
				}
				if (circle[i].selected == 1) {
				}
				if (triangle[i].selected == 1) {

					temp_angle = move_angle * 3.141592 / 180;
					temp1_x1 = triangle[i].x1;
					temp1_y1 = triangle[i].y1;
					temp1_x2 = triangle[i].x2;
					temp1_y2 = triangle[i].y2;
					temp1_x3 = triangle[i].x3;
					temp1_y3 = triangle[i].y3;


					triangle[i].x1 -= ((double)(temp1_x1 + temp1_x2 + temp1_x3) / 3.0);
					triangle[i].x2 -= ((double)(temp1_x1 + temp1_x2 + temp1_x3) / 3.0);
					triangle[i].y1 -= ((double)(temp1_y1 + temp1_y2 + temp1_y3) / 3.0);
					triangle[i].y2 -= ((double)(temp1_y1 + temp1_y2 + temp1_y3) / 3.0);
					triangle[i].x3 -= ((double)(temp1_x1 + temp1_x2 + temp1_x3) / 3.0);
					triangle[i].y3 -= ((double)(temp1_y1 + temp1_y2 + temp1_y3) / 3.0);

					temp2_x1 = triangle[i].x1;
					temp2_y1 = triangle[i].y1;
					temp2_x2 = triangle[i].x2;
					temp2_y2 = triangle[i].y2;
					temp2_x3 = triangle[i].x3;
					temp2_y3 = triangle[i].y3;

					triangle[i].x1 = ((double)temp2_x1*cos(temp_angle) - (double)temp2_y1*sin(temp_angle));
					triangle[i].x2 = ((double)temp2_x2*cos(temp_angle) - (double)temp2_y2*sin(temp_angle));
					triangle[i].y1 = ((double)temp2_x1*sin(temp_angle) + (double)temp2_y1*cos(temp_angle));
					triangle[i].y2 = ((double)temp2_x2*sin(temp_angle) + (double)temp2_y2*cos(temp_angle));
					triangle[i].x3 = ((double)temp2_x3*cos(temp_angle) - (double)temp2_y3*sin(temp_angle));
					triangle[i].y3 = ((double)temp2_x3*sin(temp_angle) + (double)temp2_y3*cos(temp_angle));

					triangle[i].x1 += ((double)(temp1_x1 + temp1_x2 + temp1_x3) / 3.0);
					triangle[i].x2 += ((double)(temp1_x1 + temp1_x2 + temp1_x3) / 3.0);
					triangle[i].y1 += ((double)(temp1_y1 + temp1_y2 + temp1_y3) / 3.0);
					triangle[i].y2 += ((double)(temp1_y1 + temp1_y2 + temp1_y3) / 3.0);
					triangle[i].x3 += ((double)(temp1_x1 + temp1_x2 + temp1_x3) / 3.0);
					triangle[i].y3 += ((double)(temp1_y1 + temp1_y2 + temp1_y3) / 3.0);
				}

				if (penta[i].selected == 1) {
					penta[i].angle += move_angle;
					printf("%lf\n", move_angle);
				}
			}
			if (mult_select == 0) {
			  for (int i = 0; i < MAX_ARGS; i++) {
			  line[i].selected = 0;
			  rect[i].selected = 0;
			  circle[i].selected = 0;
			  triangle[i].selected = 0;
			  penta[i].selected = 0;
			  }
			  min_distance = 10;
			  selected_count = 0;
			  selected_shape = -1;
			  selected_num = -1;

			  }
			move_angle = 0;
			min_distance = 10;
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
		//	printf("Mouse motion (%i %i) with a button down \n", mouseX, mouseY); // we don't know which button!  Need a global to record it from mousePress() function

		if (mode_program == 1 && mode_selection == 0 && find_min_dist() == 1) {
			move_x = mouseX - selected_x;
			move_y = mouseY - selected_y;
		}
		else if (mode_program == 1 && mode_selection == 1 && find_min_dist() == 1) {
			move_angle = get_distance(mouseX, mouseY, selected_x, selected_y);
		}
	glutPostRedisplay();
}

// call anytime mouse moves and no button presed
void mousePassiveMotion(int x, int y)
{
	printf("Mouse motion (%i %i) _no_ button pressed \n", x, winH - y); // we don't know which button!  Need a global to record it from mousePress() function
}

void draw_line(int i) {
	glLineWidth(line[i].w);
	glPushMatrix();
	if(line[i].selected == 1) glTranslated(move_x, move_y, 0);

	if (mode_program == 1 && mode_selection == 0 && line[i].selected == 1) {
			glColor3f(1, 0, 0);
			glBegin(GL_LINE_LOOP);
			glVertex2f(line[i].x1 -5, line[i].y1 - 5);
			glVertex2f(line[i].x1 +5, line[i].y1 - 5);
			glVertex2f(line[i].x1 +5, line[i].y1 + 5);
			glVertex2f(line[i].x1 -5, line[i].y1 + 5);
			glEnd();

			glBegin(GL_LINE_LOOP);
			glVertex2f(line[i].x2 - 5, line[i].y2 - 5);
			glVertex2f(line[i].x2 + 5, line[i].y2 - 5);
			glVertex2f(line[i].x2 + 5, line[i].y2 + 5);
			glVertex2f(line[i].x2 - 5, line[i].y2 + 5);
			glEnd();
	}

	else if (mode_program == 1 && mode_selection == 1 && line[i].selected == 1) {
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f((line[i].x1+line[i].x2)/2 - 5, (line[i].y1+line[i].y2)/2 - 5);
		glVertex2f((line[i].x1+line[i].x2)/2 + 5, (line[i].y1+line[i].y2)/2 - 5);
		glVertex2f((line[i].x1+line[i].x2)/2 + 5, (line[i].y1+line[i].y2)/2 + 5);
		glVertex2f((line[i].x1+line[i].x2)/2 - 5, (line[i].y1+line[i].y2)/2 + 5);
		glEnd();
	}

	if (mode_program == 1 && mode_selection == 1 && line[i].selected == 1) {
		glTranslated((line[i].x1 + line[i].x2) / 2, (line[i].y1 + line[i].y2) / 2, 0);
		glRotated(move_angle, 0, 0, 1);
		glTranslated((-1)*(line[i].x1 + line[i].x2) / 2, (-1)*(line[i].y1 + line[i].y2) / 2, 0);
	}

    switch(line[i].c) {
        case 0:
            glColor3f(0, 0, 0);
            break;
        case 1:
            glColor3f(0,1,0);
            break;
        case 2:
            glColor3f(0,0,1);
        break;
	}
	glBegin(GL_LINES);
	glVertex2f(line[i].x1, line[i].y1);
	if (line[i].drawing_state == 1) glVertex2f(mouseX, mouseY);
	else glVertex2f(line[i].x2, line[i].y2);
	glEnd();
	glPopMatrix();
}
void draw_rect(int i) {
	glLineWidth(rect[i].w);
	glPushMatrix();
	if (rect[i].selected == 1) glTranslated(move_x, move_y, 0);

	if (mode_program == 1 && mode_selection == 0 && rect[i].selected == 1) {
		glColor3f(1, 0, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(rect[i].x1 - 5, rect[i].y1 - 5);
		glVertex2f(rect[i].x1 + 5, rect[i].y1 - 5);
		glVertex2f(rect[i].x1 + 5, rect[i].y1 + 5);
		glVertex2f(rect[i].x1 - 5, rect[i].y1 + 5);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex2f(rect[i].x2 - 5, rect[i].y2 - 5);
		glVertex2f(rect[i].x2 + 5, rect[i].y2 - 5);
		glVertex2f(rect[i].x2 + 5, rect[i].y2 + 5);
		glVertex2f(rect[i].x2 - 5, rect[i].y2 + 5);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex2f(rect[i].x3 - 5, rect[i].y3 - 5);
		glVertex2f(rect[i].x3 + 5, rect[i].y3 - 5);
		glVertex2f(rect[i].x3 + 5, rect[i].y3 + 5);
		glVertex2f(rect[i].x3 - 5, rect[i].y3 + 5);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex2f(rect[i].x4 - 5, rect[i].y4 - 5);
		glVertex2f(rect[i].x4 + 5, rect[i].y4 - 5);
		glVertex2f(rect[i].x4 + 5, rect[i].y4 + 5);
		glVertex2f(rect[i].x4 - 5, rect[i].y4 + 5);
		glEnd();
	}

	else if (mode_program == 1 && mode_selection == 1 && rect[i].selected == 1) {
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f((rect[i].x1 + rect[i].x2) / 2 - 5, (rect[i].y1 + rect[i].y2) / 2 - 5);
		glVertex2f((rect[i].x1 + rect[i].x2) / 2 + 5, (rect[i].y1 + rect[i].y2) / 2 - 5);
		glVertex2f((rect[i].x1 + rect[i].x2) / 2 + 5, (rect[i].y1 + rect[i].y2) / 2 + 5);
		glVertex2f((rect[i].x1 + rect[i].x2) / 2 - 5, (rect[i].y1 + rect[i].y2) / 2 + 5);
		glEnd();
	}


	if (mode_program == 1 && mode_selection == 1 && rect[i].selected == 1) {
		glTranslated((rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2, 0);
		glRotated(move_angle, 0, 0, 1);
		glTranslated((-1)*(rect[i].x1 + rect[i].x2) / 2, (-1)*(rect[i].y1 + rect[i].y2) / 2, 0);
	}

	switch(rect[i].c) {
        case 0:
            glColor3f(0, 0, 0);
            break;
        case 1:
            glColor3f(1,0,0);
            break;
        case 2:
            glColor3f(0,0,1);
        break;
	}
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
	glLineWidth(circle[i].w);
	glPushMatrix();
	if (circle[i].selected == 1) glTranslated(move_x, move_y, 0);

	if (mode_program == 1 && mode_selection == 0 && circle[i].selected == 1) {
		glColor3f(1, 0, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(circle[i].x - 5, circle[i].y - 5);
		glVertex2f(circle[i].x + 5, circle[i].y - 5);
		glVertex2f(circle[i].x + 5, circle[i].y + 5);
		glVertex2f(circle[i].x - 5, circle[i].y + 5);
		glEnd();
	}

	else if (mode_program == 1 && mode_selection == 1 && circle[i].selected == 1) {
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(circle[i].x - 5, circle[i].y - 5);
		glVertex2f(circle[i].x + 5, circle[i].y - 5);
		glVertex2f(circle[i].x + 5, circle[i].y + 5);
		glVertex2f(circle[i].x - 5, circle[i].y + 5);
		glEnd();
	}

		switch(circle[i].c) {
        case 0:
            glColor3f(0, 0, 0);
            break;
        case 1:
            glColor3f(1,0,0);
            break;
        case 2:
            glColor3f(0,0,1);
        break;
	}

	glBegin(GL_LINE_LOOP);
	if (circle[i].drawing_state == 1) {
		circle[i].rad = get_distance(mouseX, mouseY, circle[i].x, circle[i].y);
//		printf("%d %d %d %d %f\n", mouseX, circle[i].x, mouseY, circle[i].y, circle[i].rad);
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
void draw_triangle(int i) {
	glLineWidth(triangle[i].w);
	glPushMatrix();
	if (triangle[i].selected == 1) glTranslated(move_x, move_y, 0);

	if (mode_program == 1 && mode_selection == 0 && triangle[i].selected == 1) {
		glColor3f(1, 0, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(triangle[i].x1 - 5, triangle[i].y1 - 5);
		glVertex2f(triangle[i].x1 + 5, triangle[i].y1 - 5);
		glVertex2f(triangle[i].x1 + 5, triangle[i].y1 + 5);
		glVertex2f(triangle[i].x1 - 5, triangle[i].y1 + 5);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex2f(triangle[i].x2 - 5, triangle[i].y2 - 5);
		glVertex2f(triangle[i].x2 + 5, triangle[i].y2 - 5);
		glVertex2f(triangle[i].x2 + 5, triangle[i].y2 + 5);
		glVertex2f(triangle[i].x2 - 5, triangle[i].y2 + 5);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex2f(triangle[i].x3 - 5, triangle[i].y3 - 5);
		glVertex2f(triangle[i].x3 + 5, triangle[i].y3 - 5);
		glVertex2f(triangle[i].x3 + 5, triangle[i].y3 + 5);
		glVertex2f(triangle[i].x3 - 5, triangle[i].y3 + 5);
		glEnd();
	}

	else if (mode_program == 1 && mode_selection == 1 && triangle[i].selected == 1) {
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f((triangle[i].x1 + triangle[i].x2 + triangle[i].x3) / 3 - 5, (triangle[i].y1 + triangle[i].y2 + triangle[i].y3) / 3 - 5);
		glVertex2f((triangle[i].x1 + triangle[i].x2 + triangle[i].x3) / 3 + 5, (triangle[i].y1 + triangle[i].y2 + triangle[i].y3) / 3 - 5);
		glVertex2f((triangle[i].x1 + triangle[i].x2 + triangle[i].x3) / 3 + 5, (triangle[i].y1 + triangle[i].y2 + triangle[i].y3) / 3 + 5);
		glVertex2f((triangle[i].x1 + triangle[i].x2 + triangle[i].x3) / 3 - 5, (triangle[i].y1 + triangle[i].y2 + triangle[i].y3) / 3 + 5);
		glEnd();
	}


	if (mode_program == 1 && mode_selection == 1 && triangle[i].selected == 1) {
		glTranslated((triangle[i].x1 + triangle[i].x2 + triangle[i].x3) / 3, (triangle[i].y1 + triangle[i].y2 + triangle[i].y3) / 3, 0);
		glRotated(move_angle, 0, 0, 1);
		glTranslated((-1)*(triangle[i].x1 + triangle[i].x2 + triangle[i].x3) / 3, (-1)*(triangle[i].y1 + triangle[i].y2 + triangle[i].y3) / 3, 0);
	}

		switch(triangle[i].c) {
        case 0:
            glColor3f(0, 0, 0);
            break;
        case 1:
            glColor3f(1,0,0);
            break;
        case 2:
            glColor3f(0,0,1);
        break;
	}

	glBegin(GL_LINE_LOOP);
	glVertex2f(triangle[i].x1, triangle[i].y1);
	if (triangle[i].drawing_state == 1)
	{
		glVertex2f(2*triangle[i].x1-mouseX, mouseY);
		glVertex2f(mouseX, mouseY);
	}
	else {
		glVertex2f(triangle[i].x3, triangle[i].y3);
		glVertex2f(triangle[i].x2, triangle[i].y2);
	}
	glEnd();
	glPopMatrix();
}
void draw_penta(int i) {
	glLineWidth(penta[i].w);
	glPushMatrix();
	if (penta[i].selected == 1) glTranslated(move_x, move_y, 0);

	if (mode_program == 1 && mode_selection == 0 && penta[i].selected == 1) {
		glColor3f(1, 0, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(penta[i].x - 5, penta[i].y - 5);
		glVertex2f(penta[i].x + 5, penta[i].y - 5);
		glVertex2f(penta[i].x + 5, penta[i].y + 5);
		glVertex2f(penta[i].x - 5, penta[i].y + 5);
		glEnd();
	}

	else if (mode_program == 1 && mode_selection == 1 && penta[i].selected == 1) {
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(penta[i].x - 5, penta[i].y - 5);
		glVertex2f(penta[i].x + 5, penta[i].y - 5);
		glVertex2f(penta[i].x + 5, penta[i].y + 5);
		glVertex2f(penta[i].x - 5, penta[i].y + 5);
		glEnd();
	}

	if (mode_program == 1 && mode_selection == 1 && penta[i].selected == 1) {
		glTranslated(penta[i].x, penta[i].y, 0);
		glRotated(move_angle, 0, 0, 1);
		glTranslated((-1)*penta[i].x, (-1)*penta[i].y, 0);
	}

		switch(penta[i].c) {
        case 0:
            glColor3f(0, 0, 0);
            break;
        case 1:
            glColor3f(1,0,0);
            break;
        case 2:
            glColor3f(0,0,1);
        break;
	}

	glBegin(GL_LINE_LOOP);
	if (penta[i].drawing_state == 1) {
		penta[i].rad = get_distance(mouseX, mouseY, penta[i].x, penta[i].y);
		//		printf("%d %d %d %d %f\n", mouseX, penta[i].x, mouseY, penta[i].y, penta[i].rad);
	}
	for (int j = 0+penta[i].angle; j < 360+ penta[i].angle; j += 72) {
		double angle, x, y;
		angle = j*3.141592 / 180;
		x = penta[i].rad*cos(angle);
		y = penta[i].rad*sin(angle);
		glVertex2f(penta[i].x + x, penta[i].y + y);
	}
	glEnd();
	glPopMatrix();
}
void display()
{
//	printf("%d\n", count);
/*	if (GetKeyState(0x4D) & 0x8000) {
		mult_select = 1;
	}
	else mult_select = 0;
*/
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


	for (int j = 0; j < num_line + 1; j++) {
		draw_line(j);
	}
	for (int j = 0; j < num_rect + 1; j++) {
		draw_rect(j);
	}
	for (int j = 0; j < num_circle + 1; j++) {
		draw_circle(j);
	}
	for (int j = 0; j < num_triangle + 1; j++) {
		draw_triangle(j);
	}
	for (int j = 0; j < num_penta + 1; j++) {
		draw_penta(j);
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

		line[j].selected = 0;
	}

	for (int j = 0; j < MAX_ARGS; j++) {
		rect[j].x1 = -1;
		rect[j].y1 = -1;
		rect[j].x2 = -1;
		rect[j].y2 = -1;
		rect[j].drawing_state = 0;

		rect[j].selected = 0;
	}
	selected_num = -1;
	selected_shape = -1;


	glutInit(&argc, argv);   // not necessary unless on Unix
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(512, 512);
	glutCreateWindow("mouseMotion (lec3)");

	glutReshapeFunc(reshape);       // register respace (anytime window changes)
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboard_up);     // register keyboard func
	glutMouseFunc(mousePress);      // register mouse press funct
	glutMotionFunc(mouseMotion);     // ** Note, the name is just glutMotionFunc (not glutMouseMotionFunc)
	glutPassiveMotionFunc(mousePassiveMotion); // Passive motion
	glutIdleFunc(myidle);
	glutDisplayFunc(display);       // register display function
	glutMainLoop();

	return 1;

}

