// Sample code for Übung 2

#include "vec.h"
#include "mat.h"
#include <math.h>

// might be you have to swith to
// #include "glut.h" depending on your GLUT installation
#include "Uebung2/Uebung2/glut.h"

////////////////////////////////////////////////////////////
//
// system relevant global variables
//



// Auflösungen der gesamten Textur
// !!!ACHTUNG!!! nicht alle Texturauflösungen funktionieren!
// Stichwort ungefähr: POT2 Problematik
#define TEX_RES_X 300
#define TEX_RES_Y 300
// Anzahl der Pixel der Textur
#define TEX_RES TEX_RES_X*TEX_RES_Y
// Achsenlänge der Textur (Achsen sind asymmetrisch von -TexRes#/2 bis TesRes#/2-1)
#define TEX_HALF_X TEX_RES_X/2
#define TEX_HALF_Y TEX_RES_Y/2
// Konvertiert Indices von (x,y) Koordinaten in ein lineares Array
#define TO_LINEAR(x, y) (((x)) + TEX_RES_X*((y)))

// globaler Speicher für Texturdaten
char g_Buffer[3 * TEX_RES];
// Textur ID Name
GLuint g_TexID = 0;

// Auflösung des Hauptfensters (kann sich durch User ändern)
int g_WinWidth = 800;
int g_WinHeight = 800;

// Funktion organisiert die Textur.
// Kümmern Sie sich nicht weiter um diese Funktion, da
// sie momentan nur ein notwendiges Übel darstellt!
void manageTexture() {

	glEnable(GL_TEXTURE_2D);

	if (g_TexID == 0)	glGenTextures(1, &g_TexID);

	glBindTexture(GL_TEXTURE_2D, g_TexID);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_RES_X, TEX_RES_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, g_Buffer);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
}

// Callback Funktion um die Fenstergrößen anzupassen.
// Auch diese Funktion ist ein notwendiges Übel! Kümmern
// Sie sich im Moment nicht weiter darum.
void reshape(int w, int h) {

	g_WinWidth = w;
	g_WinHeight = h;

	glViewport(0, 0, w, h);					// Establish viewing area to cover entire window.

	glMatrixMode(GL_PROJECTION);			// Start modifying the projection matrix.
	glLoadIdentity();						// Reset project matrix.
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, 0, 1);	// Map abstract coords directly to window coords.

	glutPostRedisplay();
}




//
//
/////////////////////////////////////////////////////////////////////////////////////////
//
// Hier fängt der für Sie wirklich relevante Teil des Programms an.
//




// Eine überaus primitive Punktklasse
class Point {
public:

	Point(int x = 0, int y = 0) {
		this->x = x;
		this->y = y;
	}

	int x, y;


	void setCoordinates(int x, int y) {
		this->x = x;
		this->y = y;
	}
};

// Eine überaus primitive Farbklasse
class Color {
public:

	Color(float r = 1.0f, float g = 1.0f, float b = 1.0f) {
		this->r = r;
		this->g = g;
		this->b = b;
	}

	float r, g, b;
};

// Funktion löscht den Bildschirm mit der angegebenen Farbe
// Usage z.B.: clearImage (Color (1,1,1))
// löscht den Bildschirm in Weiß.
// Ohne Farbangabe ist der Standard Weiß
void clearImage(Color c = Color()) {
	for (int i = 0; i < TEX_RES; i++) {
		g_Buffer[3 * i] = 255.0 * c.r;
		g_Buffer[3 * i + 1] = 255.0 * c.g;
		g_Buffer[3 * i + 2] = 255.0 * c.b;
	}
}



// window width and height (choose an appropriate size)
const int g_iWidth  = 600;
const int g_iHeight = 600;

// global variable to tune the timer interval
int g_iTimerMSecs;

//
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// private, global variables ... replace by your own ones
//
// some global state variables used to describe ...
float g_iPos;		// ... position and ...
float g_iPosIncr;	// ... position increment (used in display1)

CVec2i g_vecPos;		// same as above but in vector form ...
CVec2i g_vecPosIncr;	// (used in display2)


//CELESTIAL BODIES
CVec2f sunPos;
CVec2f earthPos;
CVec2f moonPos;

CVec3f sunPos3d;
CVec3f earthPos3d;
CVec3f moonPos3d;

int sunRay;
int earthRay;
int moonRay;

//Angles
float earthAngleIncr;
float moonAngleIncr;

float earthCos;
float earthSin;
float moonCos;
float moonSin;

CMat3f translationMatrix;
CMat3f earthRotation;
CMat3f moonRotation;


//
/////////////////////////////////////////////////////////////

// function to initialize our own variables
void init () 
{

	// init timer interval
	g_iTimerMSecs = 10;

	// init variables for display1
	g_iPos     = 0;
	g_iPosIncr = 2;

	// init variables for display2
	int aiPos    [2] = {0, 0};
	int aiPosIncr[2] = {2, 2};
	g_vecPos.setData (aiPos);
	g_vecPosIncr.setData (aiPosIncr);

	//init variables exo 1
	float sunP[2] = { 0.0, 0.0 };
	float earthP[2] = { 120.0, 0.0 };
	float moonP[2] = { 150.0, 00.0 };
	sunPos.setData(sunP);
	earthPos.setData(earthP);
	moonPos.setData(moonP);


	//init variables ex 2
	float earthP3[3] = { 120.0, 0.0 , 1.0 };
	float sunP3[3] = { 0.0, 0.0 , 1.0 };
	float moonP3[3] = { 150.0, 0.0 , 1.0 };
	sunPos3d.setData(sunP3);
	earthPos3d.setData(earthP3);
	moonPos3d.setData(moonP3);


	sunRay = 30;
	earthRay = 15;
	moonRay = 6;

	earthAngleIncr = 3.141592 / 180; //1°
	moonAngleIncr =  3.141592 / 180; //2°

	earthCos = cos(earthAngleIncr);
	earthSin = sin(earthAngleIncr);
	moonCos = cos(moonAngleIncr);
	moonSin = sin(moonAngleIncr);

}

CMat3f setTranslationMatrix(CVec3f p) {
	float translation[3][3] = { {1, 0, p(0)}, {0, 1, p(1)} , {0, 0, 1} };
	CMat3f returnM(translation);
	return returnM;
}

CMat3f setRotationMatrix(float ca, float sa) {
	float rotation[3][3] = { {ca, -sa, 0}, {sa, ca, 0} ,{0, 0, 1} };
	CMat3f returnM(rotation);
	return returnM;
}

// function to initialize the view to ortho-projection
void initGL () 
{
	glViewport (0, 0, g_iWidth, g_iHeight);	// Establish viewing area to cover entire window.

	glMatrixMode (GL_PROJECTION);			// Start modifying the projection matrix.
	glLoadIdentity ();						// Reset project matrix.
	glOrtho (-g_iWidth/2, g_iWidth/2, -g_iHeight/2, g_iHeight/2, 0, 1);	// Map abstract coords directly to window coords.

	// tell GL that we draw to the back buffer and
	// swap buffers when image is ready to avoid flickering
	glDrawBuffer (GL_BACK);

	// tell which color to use to clear image
	glClearColor (0,0,0,1);
}

CVec2f rotateOrigin(CVec2f p, float ca, float sa) {
	CVec2f returnP;
	float x = p(0) * ca - p(1) * sa;
	float y = p(1) * ca + p(0) * sa;
	float newP[2] = { x, y };
	returnP.setData(newP);
	return returnP;

}

CVec2f rotateMyPoint(CVec2f p, float ca, float sa, CVec2f myPoint) {
	float newP[2] = { p(0) - myPoint(0), p(1) - myPoint(1) };
	CVec2f returnP;
	returnP.setData(newP);
	returnP = rotateOrigin(returnP, ca, sa);
	returnP(0) = returnP(0) + myPoint(0);
	returnP(1) = returnP(1) + myPoint(1);
	return returnP;

}

CVec3f rotateOriginH(CVec3f p, float ca, float sa) {
	CVec3f returnP;
	CMat3f m;
	m = setRotationMatrix(ca, sa);
	returnP = m * p;
	return returnP;

}

CVec3f rotateMyPointH(CVec3f p, float ca, float sa, CVec3f myPoint) {
	CVec3f v;
	v(0) = -myPoint(0);
	v(1) = -myPoint(1);
	v(2) = 1;
	CMat3f m;
	m = setTranslationMatrix(v);
	CVec3f returnP;
	returnP = m * p;
	returnP = rotateOriginH(returnP, ca, sa);
	m = setTranslationMatrix(myPoint);
	returnP = m * returnP;

	return returnP;

}




int min (int a, int b) { return a>b? a: b; }
// timer callback function
void timer (int value) 
{
	///////
	// update your variables here ...
	//

	int size2 = min (g_iWidth, g_iHeight) / 2;

	// variables for display1 ...
	if (g_iPos<=-size2 || g_iPos>=size2) g_iPosIncr = -g_iPosIncr;
	g_iPos += g_iPosIncr;

	// variables for display2 ...
	if (g_vecPos(1)<=-size2 || g_vecPos(1)>=size2) g_vecPosIncr = -g_vecPosIncr; 
	g_vecPos += g_vecPosIncr;

	//*variables exo 1
	earthPos = rotateOrigin(earthPos, earthCos, earthSin);
	moonPos = rotateMyPoint(moonPos, earthCos, earthSin, sunPos);
	moonPos = rotateMyPoint(moonPos, moonCos, moonSin, earthPos);


	//variables ex 2
	earthPos3d = rotateOriginH(earthPos3d, earthCos, earthSin);
	moonPos3d = rotateMyPointH(moonPos3d, earthCos, earthSin, sunPos3d);
	moonPos3d = rotateMyPointH(moonPos3d, moonCos, moonSin, earthPos3d);
	
	

	// the last two lines should always be
	glutPostRedisplay ();
	glutTimerFunc (g_iTimerMSecs, timer, 0);	// call timer for next iteration
}



void bhamCircle(Point p, int r, Color c) {

	// Initialisation
	int x = 0;
	int y = r;
	int ray = r;
	int d = 5 - 4 * ray;

	// center of the circle
	int xc = p.x;
	int yc = p.y;

	int dSE, dE;						// if the next point is South-East point SE
										// if the next point is East point E

	// the eight firt points we will plot 
	Point p1(xc + ray, yc);
	Point p2(xc, yc + ray);				// (0, ray) translated in p
	Point p3(xc, yc + ray);
	Point p4(xc - ray, yc);				// (-ray, 0) translated in p
	Point p5(xc - ray, yc);
	Point p6(xc, yc - ray);				// (0, -ray) translated in p
	Point p7(xc, yc - ray);
	Point p8(xc + ray, yc);				// (ray, 0) translated in p

	// ploting the pole of the circle
	glBegin(GL_POINTS);
	glColor3f(c.r, c.g, c.b);
	glVertex2i(p2.x, p2.y);
	glVertex2i(p4.x, p4.y);
	glVertex2i(p6.x, p6.y);
	glVertex2i(p8.x, p8.y);



	while (y > x) {								// while we are in the left side of the line y = x (2nd octant)

		if (d > 0) {							// if M(k) is above the line :
												// the next point is SE
			dSE = 4 * (2 * (x - y) + 5);
			d = d + dSE;
			x++;								// M(k+1) = (x+1, y-1)
			y--;

		}
		else {									// if M(k) is under the line :
												// the next point is E
			dE = 4 * (2 * x + 3);
			d = d + dE;
			x++;								// M(k+1) = (x+1, y)
		}

		// modifying the eight points
		p1.setCoordinates(xc + y, yc + x);
		p2.setCoordinates(xc + x, yc + y);
		p3.setCoordinates(xc - x, yc + y);
		p4.setCoordinates(xc - y, yc + x);
		p5.setCoordinates(xc - y, yc - x);
		p6.setCoordinates(xc - x, yc - y);
		p7.setCoordinates(xc + x, yc - y);
		p8.setCoordinates(xc + y, yc - x);

		// ploting the eight new points
		glVertex2i(p1.x, p1.y);
		glVertex2i(p2.x, p2.y);
		glVertex2i(p3.x, p3.y);
		glVertex2i(p4.x, p4.y);
		glVertex2i(p5.x, p5.y);
		glVertex2i(p6.x, p6.y);
		glVertex2i(p7.x, p7.y);
		glVertex2i(p8.x, p8.y);

	}

	glEnd();
}



// display callback function triangulo
void display1 (void) 
{

	glClear(GL_COLOR_BUFFER_BIT);
	//drawing the sun
	Point sun = Point(sunPos(0), sunPos(1));
	Point earth = Point(earthPos(0), earthPos(1));
	Point moon = Point(moonPos(0), moonPos(1));
	bhamCircle(sun, sunRay, Color(1, 1, 0));
	bhamCircle(earth, earthRay, Color(0, 1, 0));
	bhamCircle(moon, moonRay, Color(1, 1, 1));


	//
	///////

	// In double buffer mode the last
	// two lines should alsways be
	glFlush();
	glutSwapBuffers(); // swap front and back buffer
}

// display callback function cuadrado
void display2 (void) 
{
	glClear(GL_COLOR_BUFFER_BIT);
	//drawing
	Point sun = Point(sunPos3d(0), sunPos3d(1));
	Point earth = Point(earthPos3d(0), earthPos3d(1));
	Point moon = Point(moonPos3d(0), moonPos3d(1));
	bhamCircle(sun, sunRay, Color(0, 1, 0));
	bhamCircle(earth, earthRay, Color(1, 0, 0));
	bhamCircle(moon, moonRay, Color(0, 0, 1));


	//
	///////

	// In double buffer mode the last
	// two lines should alsways be
	glFlush();
	glutSwapBuffers(); // swap front and back buffer
}

//display ex3 (planets)
void display3(void) {

	glClear(GL_COLOR_BUFFER_BIT);

	///////
	// display your data here ...
	//

	glBegin(GL_POLYGON);
	glColor3f(1, 0, 0);
	glVertex2i(-g_vecPos(1), -g_vecPos(2));
	glColor3f(0, 1, 0);
	glVertex2i(g_vecPos(1), -g_vecPos(2));
	glColor3f(0, 0, 1);
	glVertex2i(g_vecPos(1), g_vecPos(2));
	glEnd();

	//
	///////

	// In double buffer mode the last
	// two lines should alsways be
	glFlush();
	glutSwapBuffers(); // swap front and back buffer
}





void keyboard (unsigned char key, int x, int y) 
{
	switch (key) {
		case 'q':
		case 'Q':
			exit (0); // quit program
			break;
		case '1':
			glutDisplayFunc (display1);
			//glutPostRedisplay ();	// not needed since timer triggers redisplay
			break;
		case '2':
			glutDisplayFunc (display2);
			//glutPostRedisplay ();	// not needed since timer triggers redisplay
			break;
		default:
			// do nothing ...
			break;
	};
}




int main (int argc, char **argv) 
{
	glutInit (&argc, argv);
	// we have to use double buffer to avoid flickering
	// TODO: lookup "double buffer", what is it for, how is it used ...
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutCreateWindow ("Übung 2");

	init ();	// init my variables first
	initGL ();	// init the GL (i.e. view settings, ...)

	// assign callbacks
	glutTimerFunc (10, timer, 0);
	glutKeyboardFunc (keyboard);
	glutDisplayFunc (display1);
	// you might want to add a resize function analog to
	// Übung1 using code similar to the initGL function ...

	// start main loop
	glutMainLoop ();

	return 0;
}
