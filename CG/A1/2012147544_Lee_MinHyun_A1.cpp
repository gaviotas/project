//  Assignment 1: “3D Gasket” & “Snowy World”
//	CSI4105: Computer Graphics
//	2012147544 Lee_MinHyun

// standard glut include, get use to putting this in.
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int mode;	// the var for mode selection (mode 1: 3D GASKET, mode 2: snowy world)

///////////////////////////////////////////////////////////////////////////////////
// The part for "3D Gasket" 
///////////////////////////////////////////////////////////////////////////////////

/* GLOBAL VARIABLES */
/* initial tetrahedron */
GLfloat v[4][3] = { { 0.0, 0.0, 1.0 },
{ 0.0, 0.942809, -0.33333 },
{ -0.816497, -0.471405, -0.333333 },
{ 0.816497, -0.471405, -0.333333 } };

int n;		// the # of iteration	

void triangle(GLfloat *a, GLfloat *b, GLfloat *c)
/* display one triangle */
{
	glVertex3fv(a);
	glVertex3fv(b);
	glVertex3fv(c);
}

// draw 4 triangles which constitute the tetrahedron
void draw_triangle(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d)
{

	glColor3f(1.0, 0.0, 0.0);                                // assign color for each of the side
	triangle(a, b, c);                                       // draw triangle between a, b, c
	glColor3f(0.0, 1.0, 0.0);
	triangle(a, b, d);
	glColor3f(0.0, 0.0, 1.0);
	triangle(a, d, c);
	glColor3f(0.0, 0.0, 0.0);
	triangle(b, c, d);

}

void divide_tetrahedron(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d, int m)
{

	GLfloat ab[3], ac[3], ad[3], bc[3], bd[3], cd[3];
	int j;
	if (m>0)	// m is # of iteration
	{
		for (j = 0; j<3; j++) ab[j] = (a[j] + b[j]) / 2;
		for (j = 0; j<3; j++) ac[j] = (a[j] + c[j]) / 2;
		for (j = 0; j<3; j++) ad[j] = (a[j] + d[j]) / 2;
		for (j = 0; j<3; j++) bc[j] = (b[j] + c[j]) / 2;
		for (j = 0; j<3; j++) bd[j] = (b[j] + d[j]) / 2;
		for (j = 0; j<3; j++) cd[j] = (c[j] + d[j]) / 2;
		divide_tetrahedron(a, ab, ac, ad, m - 1);
		divide_tetrahedron(ab, b, bc, bd, m - 1);
		divide_tetrahedron(ac, bc, c, cd, m - 1);
		divide_tetrahedron(ad, bd, cd, d, m - 1);
	}
	else {
		draw_triangle(a, b, c, d);
	} /* draw triangle at end of recursion */
}

void display_mode_1(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // <- added to clear the z-buffer
	glLoadIdentity();
	glBegin(GL_TRIANGLES);
	divide_tetrahedron(v[0], v[1], v[2], v[3], n);
	glEnd();
	glFlush();
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-2.0, 2.0, -2.0 * (GLfloat)h / (GLfloat)w,
			2.0 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
	else
		glOrtho(-2.0 * (GLfloat)w / (GLfloat)h,
			2.0 * (GLfloat)w / (GLfloat)h, -2.0, 2.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////////
// The part for "3D Gasket" 
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The part for "Snowy World" 
///////////////////////////////////////////////////////////////////////////////////

// the struct for the snow
struct snow_state {
	double rad;
	double x_pos, y_pos, x_start, bottom;	// x_start : x_start position, bottom : distance from the bottom for piling
	double time_start, time, wind_time, time_ground;
	// time_start for random falling start
	// time : falling time
	// wind_time : the time in which the wind effects on snowflake 
	// time_ground : time after reaching the ground
	int moving_state, remaing_state;	// moving_state : snow is moving or not, remaing_state : snow is at the ground or not
	int color;
	int shape;
};
#define N 200	// the # of snow

/* GLOBAL VARIABLES */
static struct snow_state snow[N];
static double t_time, wind_time, sun_time;
static int pause, wind_from_left, wind_from_right, extra_wind;
static int night;
static double piling[100];

// calculate k/m as the square of the radius 
static double const_k_m(double r) {
	return pow(r, 2.0);
}

// calculate the moving distance of the snow
// d = (mg/k)(1-e^((-k/m)t)) * 0.03 by Mensuration by parts
double distance(double t, int i) {
	double k_m = const_k_m(snow[i].rad)*0.03;
	double d = (9.8) / (k_m)*(1 - pow(M_E, (-1.0)*(k_m)*t))*(0.01);
	return d;
}

void init(void)
{
	// setup opengl state
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(1, 1, 1);
	if (mode == 1) glEnable(GL_DEPTH_TEST);    // enable depth testing (i.e. z-buffer))
}

// Setup the viewing transform
// This setup up a display with coordiantes x axis -50 to 50, y axis -50 to 50 
// This callback is called anytime the window size changes (for example, you resize)
// in Windows. 
void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);  // set the projection matrix (will talk about this later)
	glLoadIdentity();             // This clear any existing matrix

								  // This is a common approach in OpenGL  -- the world does is always the same, no matter
								  // the window dim.  So, we need to normalize in case the width and height 
	if (w <= h)                    // normalize by which dimensin is longer 
		gluOrtho2D(-50.0, 50.0,
			-50.0*(GLfloat)h / (GLfloat)w, 50.0*(GLfloat)h / (GLfloat)w);
	else
		gluOrtho2D(-50.0*(GLfloat)w / (GLfloat)h,
			50.0*(GLfloat)w / (GLfloat)h, -50.0, 50.0);

	glMatrixMode(GL_MODELVIEW);    // set model transformation
	glLoadIdentity();              // to be empty (will talk about this later
}

// Draw a snowflake
void drawSnow(int i, double y_offset, double x_offset)
{
	// give the snowflake transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBegin(GL_POLYGON);

	// for different snow color
	switch (snow[i].color)
	{
	case 0:
		glColor4f(0.82f, 0.887f, 1.0f, 1.0f);
		break;
	case 1:
		glColor4f(0.91f, 0.8f, 1.0f, 1.0f);
		break;
	case 2:
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		break;
	default:
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	}

	// for different snow shape
	switch (snow[i].shape)
	{
	case 0:
		for (int a = 0; a < 360; a++)
		{
			double angle = a*3.141592 / 180;
			double x = snow[i].rad*cos(angle);
			double y = snow[i].rad*sin(angle);
			glVertex2f(x + snow[i].x_start - x_offset, y + 100 + snow[i].rad - y_offset);
		}
		break;
	case 1:
		for (int a = 30; a < 360; a = a + 60)
		{
			double angle = a*3.141592 / 180;
			double x = snow[i].rad*cos(angle);
			double y = snow[i].rad*sin(angle);
			glVertex2f(x + snow[i].x_start - x_offset, y + 100 + snow[i].rad - y_offset);
		}
		break;
	case 2:
		for (int a = 15; a < 360; a = a + 90)
		{
			double angle = a*3.141592 / 180;
			double x = snow[i].rad*cos(angle);
			double y = snow[i].rad*sin(angle);
			glVertex2f(x + snow[i].x_start - x_offset, y + 100 + snow[i].rad - y_offset);
		}
		break;
	}
	glEnd();

	glFlush();
}

///////////////////////////////////////////////////////////////////////////////////
// The part to draw sun 
///////////////////////////////////////////////////////////////////////////////////
void DrawSquare() {
	glColor3f(1, 0, 0);
	glBegin(GL_POLYGON);
	glVertex3d(2.0, 0.0, 0.0);
	glVertex3d(0.0, 2.0, 0.0);
	glVertex3d(-2.0, 0.0, 0.0);
	glEnd();
}

void DrawCenter() {
	glColor3f(1, 0.8, 0);
	glBegin(GL_POLYGON);
	for (int a = 0; a < 360; a++) {
		double angle = a*3.141592 / 180;
		double x = 18.0*cos(angle);
		double y = 18.0*sin(angle);
		glVertex2f(x, y);
	}
	glEnd();
}
///////////////////////////////////////////////////////////////////////////////////
// The part to draw sun 
///////////////////////////////////////////////////////////////////////////////////

/* Display Function */
void display_mode_2(void)
{   
	double t;
	int temp;	// var for piling array x_position

	// Clear Color ( changing through time )
	if (t_time / 40.0 < 1.0)	glClearColor(0, 0.5 - t_time / 40.0, 1.0 - t_time / 40.0, 0);
	else {
		glClearColor(0, 0, 0, 0);
		night = 1;
	}

	// Clear the buffer
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (int i = 0; i < N; i++) {
		// part for random start time 
		if (snow[i].time - snow[i].time_start < 0) t = 0;
		else t = snow[i].time - snow[i].time_start;

		// compute the moving distance of each snow
		if (pause == 0) {
			snow[i].y_pos += distance(t, i);
		}

		// parts for computing x_offset affected by wind_from_right
		if (wind_from_right == 1 && snow[i].time_ground == 0) {
			snow[i].x_pos += distance(snow[i].wind_time, i);
			if (snow[i].x_start - snow[i].x_pos + snow[i].rad < -50) snow[i].x_pos = snow[i].x_pos - 100 - snow[i].rad * 2;
		}

		// parts for computing x_offset affected by wind_from_left
		if (wind_from_left == 1 && snow[i].time_ground == 0) {
			snow[i].x_pos -= distance(snow[i].wind_time, i);
			if (snow[i].x_start - snow[i].x_pos - snow[i].rad > 50) snow[i].x_pos = snow[i].x_pos + 100 + snow[i].rad * 2;
		}

		// parts for computing x_offset affected by extra_wind(upper parts, wind blowing)
		if (extra_wind == 1 && snow[i].time_ground == 0) {
			if (snow[i].y_pos < 100.0)
			{
				snow[i].x_pos -= distance(snow[i].wind_time, i);
				if (snow[i].x_start - snow[i].x_pos - snow[i].rad > 50) snow[i].x_pos = snow[i].x_pos + 100 + snow[i].rad * 2;
			}
		}
	}

	for (int i = 0; i < N; i++) {
		temp = (int)(snow[i].x_start - snow[i].x_pos + 50.0);

		// draw falling snow
		if (snow[i].y_pos < 200 + snow[i].rad - snow[i].bottom) {
			drawSnow(i, snow[i].y_pos, snow[i].x_pos);     // Draw with offset
		}

		// describe disappearing snow (while condition, draw each snow)
		else if (snow[i].time_ground < 3.0 && night == 0) {
			drawSnow(i, 200 + snow[i].rad - snow[i].bottom, snow[i].x_pos);
			snow[i].moving_state = 0;
		}

		// at (background == black), snow piling starts
		else if (night == 1 && snow[i].remaing_state == 1)
		{
			drawSnow(i, 200 + snow[i].rad - snow[i].bottom, snow[i].x_pos);
			snow[i].moving_state = 0;

			// when snowflake melts by sun, new snow appear
			if (snow[i].bottom < (-1.0)*snow[i].rad) {
				snow[i].rad = rand() % 40 / (double)10 + 2;
				snow[i].time_ground = 0.0;
				snow[i].time = 0.0;
				snow[i].wind_time = 0.0;
				snow[i].x_pos = 0.0;
				snow[i].y_pos = 0.0;
				snow[i].x_start = rand() % 100 - 50;
				snow[i].bottom = 0.0;
				snow[i].moving_state = 1;
				snow[i].remaing_state = 1;
				snow[i].color = rand() % 3;
				snow[i].shape = rand() % 3;
			}
		}
		// else case, new snowflake appear
		else {
				snow[i].rad = rand() % 40 / (double)10 + 2;
				snow[i].time_ground = 0.0;
				snow[i].time = 0.0;
				snow[i].wind_time = 0.0;
				snow[i].x_pos = 0.0;
				snow[i].y_pos = 0.0;
				snow[i].x_start = rand() % 100 - 50;
				snow[i].bottom = 0.0;
				snow[i].moving_state = 1;
				snow[i].remaing_state = 1;
				snow[i].color = rand() % 3;
				snow[i].shape = rand() % 3;
		}

		// when the snowflake reachs the ground, the piling[x_pos] increases
		if (snow[i].time_ground == 0.01 && pause == 0 && night == 1) {
			if (temp >= 0 && temp < 100) {
				piling[temp] += snow[i].rad / (double) 2.0;
			}
		}

		if (snow[i].time_ground == 0.01 && night == 0 && pause == 0) {
			snow[i].remaing_state = 0;
		}
	}

	// set the snow[i].bottom to the matching piling[x_pos]
	for (int i = 0; i < N; i++) {
		temp = (int)(snow[i].x_start - snow[i].x_pos + 50.0);
		if (snow[i].moving_state == 1) {
			snow[i].bottom = piling[temp];
		}
	}

	// describe melting snow by sun
	if (sun_time > 0 && pause == 0) {
		for (int i = 0; i < N; i++) {
			snow[i].bottom -= 0.01;
		}
	}

	// draw sun
	if (sun_time < 50) glTranslated(80.0 - sun_time, 60.0, 0.0);
	else glTranslated(30, 60.0, 0.0);

	glRotated(sun_time, 0.0, 0.0, 1.0);
	for (int i = 0; i < 12; i++) {
		glPushMatrix();
		glRotated(i*30.0, 0.0, 0.0, 1.0);
		glTranslated(0.0, 20.0, 0.0);
		glScaled(3, 3, 3);
		DrawSquare();
		glPopMatrix();
	}

	DrawCenter();
	glutSwapBuffers();   // Using double-buffer, swap the buffer to display
}

// SETUP KEYBOARD
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case ' ':    /* for stop & resume motion */
		if (pause == 0) pause = 1;
		else pause = 0;
		break;
	case 'a':    /* for wind_from_right motion */
		if (wind_from_left == 1) {
			wind_from_left = 0;
			wind_time = 0;
		}
		if (extra_wind == 1) {
			extra_wind = 0;
			wind_time = 0;
		}
		if (wind_from_right == 0) wind_from_right = 1;
		else wind_from_right = 0;
		break;
	case 'd':    /* for wind_from_left motion */
		if (wind_from_right == 1) {
			wind_from_right = 0;
			wind_time = 0;
		}
		if (extra_wind == 1) {
			extra_wind = 0;
			wind_time = 0;
		}
		if (wind_from_left == 0) wind_from_left = 1;
		else wind_from_left = 0;
		break;
	case 'w':    /* for extra wind motion (only upper parts, wind blowing) */
		if (wind_from_left == 1) {
			wind_from_left = 0;
			wind_time = 0;
		}
		if (wind_from_right == 1) {
			wind_from_right = 0;
			wind_time = 0;
		}
		if (extra_wind == 0) extra_wind = 1;
		else extra_wind = 0;
		break;
	case 27:  /*  Escape Key  */
		exit(0);
		break;
	default:
		break;
	}
	if (wind_time == 0) {
		for (int i = 0; i < N; i++) {
			snow[i].wind_time = 0.0;
		}
	}
}

// Timer function
void Timer(int extra)   // ignore varialbe extra
{
	glutPostRedisplay();

	int temp;	// var for piling array x_position


	if (pause == 0) {	// while not stopped by ' ', time increases
		t_time += 0.03;
		for (int i = 0; i < N; i++) {
			snow[i].time += 0.03;
			temp = (int)(snow[i].x_start - snow[i].x_pos + 50.0);
			if (snow[i].y_pos > 200 + snow[i].rad - piling[temp]) {
				snow[i].time_ground += 0.01;	// after the snow reachs to the ground, time_ground increases
			}
		}
	}

	if (t_time >= 40 && pause == 0) sun_time += 0.1;	// the time for sun_rising

	if ((wind_from_right == 1 || wind_from_left == 1 || extra_wind == 1) && pause == 0) {
		for (int i = 0; i < N; i++) {
			if(snow[i].time > snow[i].time_start)
			snow[i].wind_time += 0.01;	// the time for wind blowing
		}
	}
	else if (wind_from_left == 0 || wind_from_right == 0) 
		for (int i = 0; i < N; i++) {
			snow[i].wind_time = 0;
		}
	glutTimerFunc(10, Timer, 0);  //   setnext timer to go off
}

///////////////////////////////////////////////////////////////////////////////////
// The part for "Snowy World" 
///////////////////////////////////////////////////////////////////////////////////

/*  Main Loop
*  Open window with initial window size, title bar,
*  RGB display mode, and handle input events.
*
*  We have registered a keyboard function.
*/
int main(int argc, char** argv)
{
	printf("('mode : 1' is \"3D Gasket\")\n");
	printf("('mode : 2' is \"Flying Objects\")\n");
	printf("Please, input the number you want to see : ");
	scanf_s("%d", &mode);

	switch (mode) {
	case 1:
		n = 3;                                          // levels of recursion
		glutInit(&argc, argv);                        // can ignore this
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);  // <- add depth buffer (z buffer)
		glutInitWindowSize(500, 500);                 // window size
		glutCreateWindow("3D Gasket");
		glutKeyboardFunc(keyboard);     // register keyboard (anytime keypressed)  
		glutReshapeFunc(myReshape);
		glutDisplayFunc(display_mode_1);

		init();  // Its back!

		glutMainLoop();
		break;
	case 2:
		srand((unsigned)time(NULL));

		// initialization
		for (int i = 0; i < N; i++) {
			snow[i].rad = rand() % 40 / (double)10 + 2;
			snow[i].time_start = rand() % 3000 / (double)100;
			snow[i].time = rand() % 10;
			snow[i].time_ground = 0.0;
			snow[i].wind_time;
			snow[i].time = 0.0;
			snow[i].x_pos = 0.0;
			snow[i].y_pos = 0.0;
			snow[i].x_start = rand() % 100 - 50;
			snow[i].bottom = 0.0;
			snow[i].moving_state = 1;
			snow[i].remaing_state = 1;
			snow[i].color = rand() % 3;
			snow[i].shape = rand() % 3;
		}
		for (int i = 0; i < 100; i++) {
			piling[i] = 0;
		}		
		t_time = 0;
		wind_time = 0;
		sun_time = 0;
		pause = 0;
		wind_from_right = 0;
		wind_from_left = 0;
		extra_wind = 0;
		night = 0;

		glutInit(&argc, argv);   // not necessary unless on Unix
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
		glutInitWindowSize(256, 512);
		glutCreateWindow("Snowy World");

		init();

		glutReshapeFunc(reshape);       // register respace (anytime window changes)
		glutKeyboardFunc(keyboard);     // register keyboard (anytime keypressed)                                        
		glutDisplayFunc(display_mode_2);       // register display function
		glutTimerFunc(0, Timer, 0);     // call first Timer, 0 wait, value passed = 0
		glutMainLoop();
		break;
	}

	return 0;
}
