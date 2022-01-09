// Sample code for bung 2

#include "vec.h"
#include "mat.h"

// might be you have to swith to
// #include "glut.h" depending on your GLUT installation
#include "glut.h"
#include <math.h>

////////////////////////////////////////////////////////////
//
// system relevant global variables
//

// window width and height (choose an appropriate size)
const int g_iWidth = 400;
const int g_iHeight = 400;

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

CVec4f origin;
CVec4f xw, yw, zw;

float fFocus;
CVec4f eyePoint;
CVec4f viewOrigin;
CVec4f viewLeft, viewUp, viewDir, viewMinusDir;


CVec4f cuboid1[8];
CVec4f cuboid2[8];
CVec4f cuboid3[8];


float xangle;
float yangle;
float zangle;

float theta;
float cosTheta;
float sinTheta;

CMat4f rotationMatrixNX;
CMat4f rotationMatrixPX;
CMat4f rotationMatrixNY;
CMat4f rotationMatrixPY;
CMat4f rotationMatrixNZ;
CMat4f rotationMatrixPZ;

CMat4f translationMatrixNX;
CMat4f translationMatrixPX;
CMat4f translationMatrixNY;
CMat4f translationMatrixPY;
CMat4f translationMatrixNZ;
CMat4f translationMatrixPZ;

float ywData[4];
float originData[4];
float viewDirData[4];

//
/////////////////////////////////////////////////////////////

class Color {
public:

	Color(float r = 1.0f, float g = 1.0f, float b = 1.0f) {
		this->r = r;
		this->g = g;
		this->b = b;
	}

	float r, g, b;
};

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


/*Rotation view to view matrices*/
CMat4f rz1(CVec4f b, float ) {
	float d = sqrt(b(0) * b(0) + b(1) * b(1));
	if (fabs(d)<0.000001) {
		float m[4][4] = {	{	1,	0,	0,	0	},
								{	0,	1,	0,	0	},
								{	0,	0,	1,	0	},
								{	0,	0,	0,	1	} };
		CMat4f rotation(m);
		return rotation;
	}
	else {
		float m[4][4] = { {	b(0) / d,	b(1) / d,	0,	0},
								{	-b(1) / d,	b(0) / d,	0,	0},
								{	0,			0,			1,	0},
								{	0,			0,			0,	1} };
		CMat4f rotation(m);
		return rotation;
	}
}

CMat4f ry1(CVec4f b, float ) {
	float d = sqrt(b(0) * b(0) + b(1) * b(1));
	float m[4][4] = { {	b(2),	0,	-d,		0},
							{	0,		1,	0,		0},
							{	d,		0,	b(2),	0},
							{	0,		0,	0,		1} };
	CMat4f rotation(m);
	return rotation;
}

CMat4f rz2(CVec4f b, float alpha) {
	float c = cos(alpha);
	float s = sin(alpha);
	float d = sqrt(b(0) * b(0) + b(1) * b(1));
	float m[4][4] = { {	c,	-s,	0,	0},
							{	s,	c,	0,	0},
							{	0,	0,	1,	0},
							{	0,	0,	0,	1} };
	CMat4f rotation(m);
	return rotation;
}

CMat4f rz1inv(CVec4f b, float ) {
	float d = sqrt(b(0) * b(0) + b(1) * b(1));
	if (fabs(d)<0.000001) {
		float m[4][4] = { {	1,	0,	0,	0	},
							{	0,	1,	0,	0	},
							{	0,	0,	1,	0	},
							{	0,	0,	0,	1	} };
		CMat4f rotation(m);
		return rotation;
	}
	else {
		float m[4][4] = { {	b(0) / d,	-b(1) / d,	0,	0},
						{	b(1) / d,	b(0) / d,	0,	0},
						{	0,			0,			1,	0},
						{	0,			0,			0,	1} };
		CMat4f rotation(m);
		return rotation;
	}
}

CMat4f ry1inv(CVec4f b, float ) {
	float d = sqrt(b(0) * b(0) + b(1) * b(1));
	float m[4][4] = { {	b(2),	0,	d,		0},
					{	0,		1,	0,		0},
					{	-d,		0,	b(2),	0},
					{	0,		0,	0,		1} };
	CMat4f rotation(m);
	return rotation;
}

/*
rotation matrices
*/
CMat4f positiveRotationX() {
	float m[4][4] = { { 1,	0,			0,			0},
						{ 0,	cosTheta,	-sinTheta,	0},
						{ 0,	sinTheta,	cosTheta,	0},
						{ 0,	0,			0,			1}
	};

	CMat4f rotationX(m);
	return rotationX;
}

CMat4f negativeRotationX() {
	float m[4][4] = { { 1,	0,			0,			0},
						{ 0,	cosTheta,	sinTheta,	0},
						{ 0,	-sinTheta,	cosTheta,	0},
						{ 0,	0,			0,			1}
	};

	CMat4f rotationX(m);
	return rotationX;
}

CMat4f positiveRotationY() {
	float m[4][4] = { { cosTheta,		0,	sinTheta,	0},
						{ 0,			1,	0,			0},
						{ -sinTheta,	0,	cosTheta,	0},
						{ 0,			0,	0,			1}
	};

	CMat4f rotationY(m);
	return rotationY;
}

CMat4f negativeRotationY() {
	float m[4][4] = { { cosTheta,		0,	-sinTheta,	0},
						{ 0,			1,	0,			0},
						{ sinTheta,	0,	cosTheta,	0},
						{ 0,			0,	0,			1}
	};

	CMat4f rotationY(m);
	return rotationY;
}

CMat4f positiveRotationZ() {
	float m[4][4] = { { cosTheta,	-sinTheta,	0,	0},
						{ sinTheta,	cosTheta,	0,	0},
						{ 0,		0,			1,	0},
						{ 0,		0,			0,	1}
	};

	CMat4f rotationZ(m);
	return rotationZ;
}

CMat4f negativeRotationZ() {
	float m[4][4] = { { cosTheta,		sinTheta,	0,	0},
						{ -sinTheta,	cosTheta,	0,	0},
						{ 0,			0,			1,	0},
						{ 0,			0,			0,	1}
	};

	CMat4f rotationZ(m);
	return rotationZ;
}


/*
translation matrices
*/
CMat4f positiveTranslationX() {
	float m[4][4] = { {	1,	0,	0,	5	},
					{	0,	1,	0,	0	},
					{	0,	0,	1,	0	},
					{	0,	0,	0,	1	}
	};

	CMat4f translationX(m);
	return translationX;
}

CMat4f negativeTranslationX() {
	float m[4][4] = { {	1,	0,	0,	- 5	},
					{	0,	1,	0,	0	},
					{	0,	0,	1,	0	},
					{	0,	0,	0,	1	}
	};

	CMat4f translationX(m);
	return translationX;
}

CMat4f positiveTranslationY() {
	float m[4][4] = { {	1,	0,	0,	0	},
					{	0,	1,	0,	5	},
					{	0,	0,	1,	0	},
					{	0,	0,	0,	1	}
	};

	CMat4f translationY(m);
	return translationY;
}

CMat4f negativeTranslationY() {
	float m[4][4] = { {	1,	0,	0,	0	},
					{	0,	1,	0,	-5	},
					{	0,	0,	1,	0	},
					{	0,	0,	0,	1	}
	};

	CMat4f translationY(m);
	return translationY;
}

CMat4f positiveTranslationZ() {
	float m[4][4] = { {	1,	0,	0,	0	},
					{	0,	1,	0,	0	},
					{	0,	0,	1,	5	},
					{	0,	0,	0,	1	}
	};

	CMat4f translationZ(m);
	return translationZ;
}

CMat4f negativeTranslationZ() {
	float m[4][4] = { {	1,	0,	0,	0	},
					{	0,	1,	0,	0	},
					{	0,	0,	1,	-5	},
					{	0,	0,	0,	1	}
	};

	CMat4f translationZ(m);
	return translationZ;
}


CMat4f getTranslationMatrix() {
	float m[4][4] = { { 1,	0,	0,	-viewOrigin(0)	},
					{	0,	1,	0,	-viewOrigin(1)	},
					{	0,	0,	1,	-viewOrigin(2)	},
					{	0,	0,	0,	1}
	};
	CMat4f translation(m);
	return translation;
}

CMat4f getTranslationInverseMatrix() {
	float m[4][4] = { { 1,	0,	0,	viewOrigin(0)	},
					{	0,	1,	0,	viewOrigin(1)	},
					{	0,	0,	1,	viewOrigin(2)	},
					{	0,	0,	0,	1}
	};
	CMat4f translation(m);
	return translation;
}

CMat4f setProjectionMatrix(float d) {
	if (d == 0) {

	}
	else {
		float m[4][4] = { {1,0,0,0}, {0,1,0,0}, {0,0,0,0}, {0,0,-1 / d, 1} };
		return CMat4f(m);
	}

}

/*
central projection of an arbitrary 3d point pView in homogenous
coordinates onto the projection plane

using setProjectionMatrix(float d)

*/

CVec4f projectedZ(float fFocus, CVec4f pSicht) {

	// getting the projecting matrix
	CMat4f projM = setProjectionMatrix(fFocus);

	// projecting the point
	CVec4f matrix1 = projM * pSicht;
	return matrix1 * (1 / matrix1(3));	//normalization


}

CVec4f vectorialProduct(CVec4f v1, CVec4f v2) {
	CVec4f vr;
	float v[4];
	v[0] = v1(1) * v2(2) - v1(2) * v2(1);
	v[1] = v1(2) * v2(0) - v1(0) * v2(2);
	v[2] = v1(0) * v2(1) - v1(1) * v2(0);
	v[3] = 0;
	vr.setData(v);
	return vr;
}

/*
computes the complete transformation as 4×4-matrix for the coordinate from the view
coordinate system tot world coordinate system
*/
CMat4f getTransformViewToWorld(CVec4f ViewOrigin, CVec4f ViewDir, CVec4f ViewUp) {

	// computing the third axe
	CVec4f ViewLeft = vectorialProduct(ViewDir, ViewUp);
	
	// computing the transformation matrix transform = ( R, -ViewOrigin)
	// with R = { transpose(xv),
				// transpose(yv),
				// transpose(zv),
				// 0 0 0 }
	ViewLeft.norm();
	ViewDir.norm();
	ViewUp.norm();
	float m[4][4] = { {ViewLeft(0),		ViewUp(0),	-ViewDir(0),	-ViewOrigin(0)},
						{ ViewLeft(1),	ViewUp(1),		-ViewDir(1),		-ViewOrigin(1)},
						{ ViewLeft(2),	ViewUp(2),	-ViewDir(2),	-ViewOrigin(2)},
						{ 0,			0,				0,				1} };
	CMat4f transform(m);


	return transform;
}

/*
computes the complete transformation as 4×4-matrix for the coordinate from the world
coordinate system tot wiew coordinate system

using
getTransformViewToWorld(CVec4f ViewOrigin, CVec4f ViewDir, CVec4f ViewUp)
as the matrix we want is the inverse of the matrix given by this function

*/
CMat4f getTransformWorldToView(CVec4f ViewOrigin, CVec4f ViewDir, CVec4f ViewUp) {

	// computing the inverse of the matrix
	// transform = (R, -ViewOrigin)
	CMat4f transform = getTransformViewToWorld(ViewOrigin, ViewDir, ViewUp);

	// computing the transpose of R = transform[0:2][0:2]
	float RtArray[3][3] = { { transform(0, 0),	transform(1, 0),	transform(2, 0) },
						{	transform(0, 1),	transform(1, 1),	transform(2, 1)},
						{	transform(0, 2),	transform(1, 2),	transform(2, 2)} };
	CMat3f Rt(RtArray);

	// getting the origin in 1 x 3 dimension
	float originArray[3] = { ViewOrigin(0), ViewOrigin(1),  ViewOrigin(2) };
	CVec3f origin(originArray);

	// computing the last column of the matrix Rt * ViewOrigin
	CVec3f RtTimeOrigin = Rt * origin;

	// the transformation matrix transformInv = (Rt, Rt * ViewOrigin)
	float m[4][4] = { {Rt(0, 0),	Rt(0, 1),	Rt(0, 2),	-RtTimeOrigin(0)},
						{Rt(1, 0),	Rt(1, 1),	Rt(1, 2),	-RtTimeOrigin(1)},
						{Rt(2, 0),	Rt(2, 1),	Rt(2, 2),	-RtTimeOrigin(2)},
						{0,				0,				0,				1} };

	CMat4f transformInv(m);


	return transformInv;
}

/*
transforms the point pWorld with matTransf into view coordinates and projects it onto
the image plane using projectZ.

matTransf = getTransformWorldToView(CVec4f ViewOrigin, CVec4f ViewDir, CVec4f ViewUp)

using
projectedZ(float fFocus, CVec4f pSicht)

*/
CVec4f projectZallg(CMat4f matTransf, float fFocus, CVec4f pWorld) {

	// transformation into view coordinates
	CVec4f pView = matTransf * pWorld;
	// projection onto the image plan
	CVec4f pProj = projectedZ(fFocus, pView);


	return pProj;
}


// function to initialize our own variables
void init()
{

	// init timer interval
	g_iTimerMSecs = 10;

	// init variables for display1
	g_iPos = 0;
	g_iPosIncr = 2;

	// init variables for display2
	int aiPos[2] = { 0, 0 };
	int aiPosIncr[2] = { 2, 2 };
	g_vecPos.setData(aiPos);
	g_vecPosIncr.setData(aiPosIncr);



	// initialisation world coordinate
	
		//ORIGIN
	originData[0] =0;
	originData[1] = 0;
	originData[2] = 0;
	originData[3] = 1;
	origin.setData(originData);

		//AXES
	float xwData[4] = { 1, 0, 0, 0 };
	xw.setData(xwData);

	ywData[0] = 0;
	ywData[1] = 1;
	ywData[2] = 0;
	ywData[3] = 0;
	yw.setData(ywData);

	float zwData[4] = { 0, 0, 1, 0 };
	zw.setData(zwData);


	// initialisation view coordinate

		//ORIGIN
	viewOrigin.setData(originData);

		//AXES
	viewLeft.setData(xwData);

	viewUp.setData(ywData);

	viewMinusDir.setData(zwData);

	viewDirData[0] = 0;
	viewDirData[1] = 0;
	viewDirData[2] = -1;
	viewDirData[3] = 0;
	viewDir.setData(viewDirData);

		//CAMERA
	fFocus = 80;
	float eyePointData[4] = { 0, 0, 0, 1 };
	eyePoint.setData(eyePointData);



	//cuboid1
	float p1[4] = { 10, 20, 50, 1 };
	cuboid1[0].setData(p1);
	float p2[4] = { -10, 20, 50, 1 };
	cuboid1[1].setData(p2);
	float p3[4] = { -10, 20, 30, 1 };
	cuboid1[2].setData(p3);
	float p4[4] = { 10, 20, 30, 1 };
	cuboid1[3].setData(p4);
	float p5[4] = { 10, 0, 50, 1 };
	cuboid1[4].setData(p5);
	float p6[4] = { -10, 0, 50, 1 };
	cuboid1[5].setData(p6);
	float p7[4] = { -10, 0, 30, 1 };
	cuboid1[6].setData(p7);
	float p8[4] = { 10, 0, 30, 1 };
	cuboid1[7].setData(p8);

	//cuboid2
	float p12[4] = { 60, 20, 50, 1 };
	cuboid2[0].setData(p12);
	float p22[4] = { 40, 20, 50, 1 };
	cuboid2[1].setData(p22);
	float p32[4] = { 40, 20, 30, 1 };
	cuboid2[2].setData(p32);
	float p42[4] = { 60, 20, 30, 1 };
	cuboid2[3].setData(p42);
	float p52[4] = { 60, 0, 50, 1 };
	cuboid2[4].setData(p52);
	float p62[4] = { 40, 0, 50, 1 };
	cuboid2[5].setData(p62);
	float p72[4] = { 40, 0, 30, 1 };
	cuboid2[6].setData(p72);
	float p82[4] = { 60, 0, 30, 1 };
	cuboid2[7].setData(p82);

	//cuboid3
	float p13[4] = { 0, 0, 0, 1 };
	cuboid3[0].setData(p13);
	float p23[4] = { -20, 0, 0, 1 };
	cuboid3[1].setData(p23);
	float p33[4] = { -20, 0, -20, 1 };
	cuboid3[2].setData(p33);
	float p43[4] = { 0, 0, -20, 1 };
	cuboid3[3].setData(p43);
	float p53[4] = { 0, -20, 0, 1 };
	cuboid3[4].setData(p53);
	float p63[4] = { -20, -20, 0, 1 };
	cuboid3[5].setData(p63);
	float p73[4] = { -20, -20, -20, 1 };
	cuboid3[6].setData(p73);
	float p83[4] = { 0, -20, -20, 1 };
	cuboid3[7].setData(p83);

	// TO VISUALIZE FROM AN ARBITRARY PERSPECTIVE
	
		// rotation of 5°
	theta = 5 * 3.14159 / 180;
	cosTheta = cos(theta);
	sinTheta = sin(theta);


	xangle = 0;
	yangle = 0;
	zangle = 0;

	// ROTATION MATRICES VIEW AROUND WORLD
	rotationMatrixNX = negativeRotationX();
	rotationMatrixPX = positiveRotationX();
	rotationMatrixNY = negativeRotationY();
	rotationMatrixPY = positiveRotationY();
	rotationMatrixNZ = negativeRotationZ();
	rotationMatrixPZ = positiveRotationZ();

	// TRANSLATION MATRICES
	translationMatrixNX = negativeTranslationX();
	translationMatrixPX = positiveTranslationX();
	translationMatrixNY = negativeTranslationY();
	translationMatrixPY = positiveTranslationY();
	translationMatrixNZ = negativeTranslationZ();
	translationMatrixPZ = positiveTranslationZ();

}

// function to initialize the view to ortho-projection
void initGL()
{
	glViewport(0, 0, g_iWidth, g_iHeight);	// Establish viewing area to cover entire window.

	glMatrixMode(GL_PROJECTION);			// Start modifying the projection matrix.
	glLoadIdentity();						// Reset project matrix.
	glOrtho(-g_iWidth / 2, g_iWidth / 2, -g_iHeight / 2, g_iHeight / 2, 0, 1);	// Map abstract coords directly to window coords.

	// tell GL that we draw to the back buffer and
	// swap buffers when image is ready to avoid flickering
	glDrawBuffer(GL_BACK);

	// tell which color to use to clear image
	glClearColor(0, 0, 0, 1);
}


int min(int a, int b) { return a > b ? a : b; }
// timer callback function
void timer(int value)
{
	///////
	// update your variables here ...
	//

	int size2 = min(g_iWidth, g_iHeight) / 2;

	// variables for display1 ...
	if (g_iPos <= -size2 || g_iPos >= size2) g_iPosIncr = -g_iPosIncr;
	g_iPos += g_iPosIncr;

	// variables for display2 ...
	if (g_vecPos(1) <= -size2 || g_vecPos(1) >= size2) g_vecPosIncr = -g_vecPosIncr;
	g_vecPos += g_vecPosIncr;

	//
	///////

	// the last two lines should always be
	glutPostRedisplay();
	glutTimerFunc(g_iTimerMSecs, timer, 0);	// call timer for next iteration
}

void bhamLine(Point p1, Point p2, Color c) {




	// First point P1(x1, y1)
	int px1 = p1.x;
	int py1 = p1.y;

	// Last point P2(x2,y2)
	int px2 = p2.x;
	int py2 = p2.y;

	//Determining the octant
	int alpha, beta, gamma, delta;	// M'(k) = ( a * x + b * y + px1 , c * x + d * y + py1 )
	int epsilon, zeta, eta, theta;	// L = ( e * (px2 - px1) + z * (py2 - py1) , n * (px2 - px1) + t * (py2 - py1) )

	int deltaX = px2 - px1;
	int deltaY = py2 - py1;


	// slope 
	int s;
	if (deltaX == 0) {

		if (deltaY > 0) {
			s = 1;
		}
		else {
			s = -1;
		}
	}
	else {
		s = deltaY / deltaX;
	}


	if (deltaY > 0) {				// P1 "under" P2	

		if (deltaX >= 0) {			// P1 "on the left of" P2
									// case of a horizontal line

			if (s < 1) {			// low slope

				// 1st octant : 

				// M'(k) = ( 1*x + 0*y ; 0*x + 1*y) 
				alpha = 1;
				beta = 0;
				gamma = 0;
				delta = 1;

				// L = ( 1 * px2 + 0 * py2 , 0 * px2 + 1 * py2 )
				epsilon = 1;
				zeta = 0;
				eta = 0;
				theta = 1;

			}
			else {					// high slope

				// 2nd octant :

				// M'(k) = ( 0*x + 1*y ; 1*x + 0*y) 
				alpha = 0;
				beta = 1;
				gamma = 1;
				delta = 0;

				// L = ( 0 * px2 + 1 * py2 , 1 * px2 + 0 * py2 )
				epsilon = 0;
				zeta = 1;
				eta = 1;
				theta = 0;

			}

		}
		else {						// P1 "on the right of" P2

			if (s < -1) {			// high slope

				// 3rd octant :

				// M'(k) = ( 0*x + -1*y ; 1*x + 0*y) 
				alpha = 0;
				beta = -1;
				gamma = 1;
				delta = 0;

				// L = ( 0 * px2 + 1 * py2 , -1 * px2 + 0 * py2 )
				epsilon = 0;
				zeta = 1;
				eta = -1;
				theta = 0;


			}
			else {					// low slope

				// 4th octant : 

				// M'(k) = ( -1*x + 0*y ; 0*x + 1*y) 
				alpha = -1;
				beta = 0;
				gamma = 0;
				delta = 1;

				// L = ( -1*x + 0*y ; 0*x + 1*y) 
				epsilon = -1;
				zeta = 0;
				eta = 0;
				theta = 1;

			}

		}

	}
	else {						// P1 "above" P2

		if (deltaX < 0) {			// P1 "on the right of" P2

			if (s < 1) {			// low slope

				// 5th octant :

				// M'(k) = ( -1*x + 0*y ; 0*x + -1*y) 
				alpha = -1;
				beta = 0;
				gamma = 0;
				delta = -1;

				// L = ( -1*x + 0*y ; 0*x + -1*y) 
				epsilon = -1;
				zeta = 0;
				eta = 0;
				theta = -1;

			}
			else {					// high slope

				// 6th octant : 

				// M'(k) = ( 0*x + -1*y ; -1*x + 0*y) 
				alpha = 0;
				beta = -1;
				gamma = -1;
				delta = 0;

				// L = ( 0*x + -1*y ; -1*x + 0*y) 
				epsilon = 0;
				zeta = -1;
				eta = -1;
				theta = 0;

			}

		}
		else {						// P1 "on the left of" P2

			if (s <= -1) {			// high slope
									// case of a vertical line

				// 7th octant :

				// M'(k) = ( 0*x + 1*y ; -1*x + 0*y)
				alpha = 0;
				beta = 1;
				gamma = -1;
				delta = 0;

				// L = ( 0*x + -1*y ; 1*x + 0*y)
				epsilon = 0;
				zeta = -1;
				eta = 1;
				theta = 0;

			}
			else {				// low slope

				// 8th octant :

				// M'(k) = ( 1*x + 0*y ; 0*x + -1*y) 
				alpha = 1;
				beta = 0;
				gamma = 0;
				delta = -1;

				// L = ( 1*x + 0*y ; 0*x + -1*y) 
				epsilon = 1;
				zeta = 0;
				eta = 0;
				theta = -1;

			}

		}

	}



	//*/

	// Translation + reflexions

	// First point F(0, 0)
	int x1 = 0;
	int y1 = 0;

	// Last point L
	int x2 = epsilon * (px2 - px1) + zeta * (py2 - py1);
	int y2 = eta * (px2 - px1) + theta * (py2 - py1);


	// The point M(k) = (x,y) we´re working with
	int x = x1;
	int y = y1;

	// Variables for M(k) coordinates modifications
	int dx = x2 - x;
	int dy = y2 - y;
	int dne = 2 * (dy - dx);		// if the next point is North-East point NE
	int de = 2 * dy;				// if the next point is East point E

	// Variable for determine which point is the next one
	int d = 2 * dy - dx;

	// erster Punkt
	glBegin(GL_POINTS);
	glColor3f(c.r, c.g, c.b);
	glVertex2d(p1.x, p1.y);



	///*
	Point m(0, 0);
	while (x < x2) {				// while the point M(k) we´re working with is not the last point L

		if (d >= 0) {				// if M(k) is above the line :
									// The next point is the North-East point
			d = d + dne;			//
			x++;					// M(k+1) = (x+1, y+1)
			y++;

		}
		else {						// M(k) is below the line :
			d = d + de;
			x++;					// M(k+1) = (x+1, y)
		}

		// Coloring the real point M'(k) = ( a * x + b * y , c * x + d * y )
		m.setCoordinates(alpha * x + beta * y + px1, gamma * x + delta * y + py1);
		glVertex2d(m.x, m.y);
	}
	//*/


	// letzter Punkt
	glVertex2d(p2.x, p2.y);
	glEnd();
}


/*
draws the wireframe of a projected 3d cuboid
*/
void drawProjektedZ(CVec4f Points[8]) {

	//up face
	// first edge
	Point p1(Points[0](0), Points[0](1));
	Point p2(Points[1](0), Points[1](1));
	Color c(0, 1, 0);
	bhamLine(p1, p2, c);

	// edge 2
	p1.setCoordinates(Points[1](0), Points[1](1));
	p2.setCoordinates(Points[2](0), Points[2](1));

	bhamLine(p1, p2, c);

	// edge 3
	p1.setCoordinates(Points[2](0), Points[2](1));
	p2.setCoordinates(Points[3](0), Points[3](1));

	bhamLine(p1, p2, c);

	// edge 4
	p1.setCoordinates(Points[3](0), Points[3](1));
	p2.setCoordinates(Points[0](0), Points[0](1));

	bhamLine(p1, p2, c);

	//down face
	// first edge
	p1.setCoordinates(Points[4](0), Points[4](1));
	p2.setCoordinates(Points[5](0), Points[5](1));

	bhamLine(p1, p2, c);

	// edge 2
	p1.setCoordinates(Points[5](0), Points[5](1));
	p2.setCoordinates(Points[6](0), Points[6](1));

	bhamLine(p1, p2, c);

	// edge 3
	p1.setCoordinates(Points[6](0), Points[6](1));
	p2.setCoordinates(Points[7](0), Points[7](1));

	bhamLine(p1, p2, c);

	// edge 4
	p1.setCoordinates(Points[7](0), Points[7](1));
	p2.setCoordinates(Points[4](0), Points[4](1));

	bhamLine(p1, p2, c);

	//links
	// first edge
	p1.setCoordinates(Points[0](0), Points[0](1));
	p2.setCoordinates(Points[4](0), Points[4](1));

	bhamLine(p1, p2, c);

	// edge 2
	p1.setCoordinates(Points[1](0), Points[1](1));
	p2.setCoordinates(Points[5](0), Points[5](1));

	bhamLine(p1, p2, c);

	// edge 3
	p1.setCoordinates(Points[2](0), Points[2](1));
	p2.setCoordinates(Points[6](0), Points[6](1));

	bhamLine(p1, p2, c);

	// edge 4
	p1.setCoordinates(Points[3](0), Points[3](1));
	p2.setCoordinates(Points[7](0), Points[7](1));

	bhamLine(p1, p2, c);

}


/*
draws the wireframe of a projected 3d cuboid

using 
projectedZ(float fFocus, CVec4f pSicht)
to project each point of the cuboid

using
drawProjektedZ(CVec4f Points[8])
to drawthe projected cuboid

*/
void drawQuader(CVec4f Cuboid[8], float fFocus, Color c) {
	CVec4f projCuboid[8];
	CMat4f matTransf = getTransformWorldToView(viewOrigin, viewDir, viewUp);

	// projecting each point

	for (int k = 0; k < 8; k++) {
		projCuboid[k] = projectZallg(matTransf, fFocus, Cuboid[k]);
	}

	//plotting the projected cuboid
	drawProjektedZ(projCuboid);


}

/*

// functions from last assigment, not used


CMat3f setTranslationMatrix(CVec4f p) {
	float translation[3][3] = { {1, 0, p(0)}, {0, 1, p(1)} , {0, 0, 1} };
	CMat3f returnM(translation);
	return returnM;
}

CMat3f setRotationMatrix(float ca, float sa) {
	float rotation[3][3] = { {ca, -sa, 0}, {sa, ca, 0} ,{0, 0, 1} };
	CMat3f returnM(rotation);
	return returnM;
}
*/




// display callback function
void display1(void)
{

	glClear(GL_COLOR_BUFFER_BIT);


	drawQuader(cuboid1, fFocus, Color(0, 1, 0));
	drawQuader(cuboid2, fFocus, Color(1, 1, 0));
	drawQuader(cuboid3, fFocus, Color(0, 1, 1));

	//
	///////

	// In double buffer mode the last
	// two lines should alsways be
	glFlush();
	glutSwapBuffers(); // swap front and back buffer
}

// display callback function
void display2(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	///////
	// display your data here ...
	//

	glBegin(GL_QUADS);
	glColor3f(1, 0, 0);
	glVertex2i(-g_vecPos(1), -g_vecPos(2));
	glColor3f(0, 1, 0);
	glVertex2i(g_vecPos(1), -g_vecPos(2));
	glColor3f(0, 0, 1);
	glVertex2i(g_vecPos(1), g_vecPos(2));
	glColor3f(1, 1, 0);
	glVertex2i(-g_vecPos(1), g_vecPos(2));
	glEnd();



	//
	///////

	// In double buffer mode the last
	// two lines should alsways be
	glFlush();
	glutSwapBuffers(); // swap front and back buffer
}


// faire la translation avant ? 
void rotationViewAroundWorld(CMat4f rotationMatrix) {
	viewOrigin = rotationMatrix * viewOrigin;
	viewUp = rotationMatrix * viewUp;
	viewDir = rotationMatrix * viewDir;
}

CVec4f rotationViewAroundView(CVec4f axe, float alpha, CVec4f toModify) {

	
	
	CMat4f rz = rz1(axe, alpha);
	CMat4f ry = ry1(axe, alpha);
	CMat4f rz_2 = rz2(axe, alpha);
	CMat4f ryinv = ry1inv(axe, alpha);
	CMat4f rzinv = rz1inv(axe, alpha);
	CMat4f finalMatrix = rzinv * ryinv * rz_2 * ry * rz;

	CVec4f v = finalMatrix * toModify;
	return v;
	


	//CMat4f translation = getTranslationMatrix();
	//CMat4f translationInverse = getTranslationInverseMatrix();
	//viewOrigin = translationInverse * rotationMatrix * translation * viewOrigin;
	//viewUp = translationInverse * rotationMatrix * translation * viewUp;
	//viewDir = translationInverse * rotationMatrix * translation * viewDir;
	//viewUp = translationInverse * rotationMatrix  * viewUp;
	//viewDir = translationInverse * rotationMatrix * viewDir;
}

void translationViewAroundWorld(CMat4f translationMatrix) {
	viewOrigin = translationMatrix * viewOrigin;
}


void keyboard(unsigned char key, int x, int y)
{
	CVec4f vLeft = vectorialProduct(viewDir, viewUp);
	vLeft = vLeft*(1.0/vLeft.length());
	switch (key) {

	// decreasing the focal
	case 'f':
		fFocus -= 5;
		break;

	// increasing the focal
	case 'F':
		fFocus += 5;
		break;

	// rotate the view coordinate system in negative direction around xw of the world coordinate system
	case 'x':
		rotationViewAroundWorld(rotationMatrixNX);
		break;

	// rotate the view coordinate system in positive direction around xw of the world coordinate system
	case 'X':
		rotationViewAroundWorld(rotationMatrixPX);
		break;


	// rotate the view coordinate system in negative direction around yw of the world coordinate system
	case 'y':
		rotationViewAroundWorld(rotationMatrixNY);
		break;

	// rotate the view coordinate system in positive direction around yw of the world coordinate system
	case 'Y':
		rotationViewAroundWorld(rotationMatrixPY);
		break;


	// rotate the view coordinate system in negative direction around zw of the world coordinate system
	case 'z':
		rotationViewAroundWorld(rotationMatrixNZ);
		break;

	// rotate the view coordinate system in positive direction around zw of the world coordinate system
	case 'Z':
		rotationViewAroundWorld(rotationMatrixPZ);
		break;

		// rotate the view coordinate system in negative direction around xw of the view coordinate system
	case 'a':
		xangle -= theta;
		
		viewUp = rotationViewAroundView(vLeft, -theta, viewUp);
		viewDir = rotationViewAroundView(vLeft, -theta, viewDir);
		break;

		// rotate the view coordinate system in positive direction around xw of the view coordinate system
	case 'A':
		xangle += theta;
		viewUp = rotationViewAroundView(vLeft, theta, viewUp);
		viewDir = rotationViewAroundView(vLeft, theta, viewDir);
		break;

		// rotate the view coordinate system in negative direction around yw of the view coordinate system
	case 'b':
		yangle -= theta;
		viewDir = rotationViewAroundView(viewUp, -theta, viewDir);
		break;

		// rotate the view coordinate system in positive direction around yw of the view coordinate system
	case 'B':
		yangle += theta;
		viewDir = rotationViewAroundView(viewUp, theta, viewDir);
		break;

		// rotate the view coordinate system in negative direction around zw of the view coordinate system
	case 'c':
		zangle -= theta;
		viewUp = rotationViewAroundView(viewDir, -theta, viewUp);
		break;

		// rotate the view coordinate system in positive direction around zw of the view coordinate system
	case 'C':
		zangle += theta;
		viewUp = rotationViewAroundView(viewDir, theta, viewUp);
		break;

		// translate the view coordinate system in negative direction around xw of the world coordinate system
	case 'u':
		translationViewAroundWorld(translationMatrixNX);
		break;

		// translate the view coordinate system in positive direction around xw of the world coordinate system
	case 'U':
		translationViewAroundWorld(translationMatrixPX);
		break;

		// translate the view coordinate system in negative direction around yw of the world coordinate system
	case 'v':
		translationViewAroundWorld(translationMatrixNY);
		break;

		// translate the view coordinate system in positive direction around yw of the world coordinate system
	case 'V':
		translationViewAroundWorld(translationMatrixPY);
		break;

		// translate the view coordinate system in negative direction around zw of the world coordinate system
	case 'w':
		translationViewAroundWorld(translationMatrixNZ);
		break;

		// translate the view coordinate system in positive direction around zw of the world coordinate system
	case 'W':
		translationViewAroundWorld(translationMatrixPZ);
		break;

		//R resets the view coordinate system to world coordinate system
	case 'r':
	case 'R' :
		viewOrigin.setData(originData);
		viewDir.setData(viewDirData);
		viewUp.setData(ywData);
		break;
	
	
	case 'q':
	case 'Q':
		exit(0); // quit program
		break;
	case '1':
		glutDisplayFunc(display1);
		//glutPostRedisplay ();	// not needed since timer triggers redisplay
		break;
	case '2':
		glutDisplayFunc(display2);
		//glutPostRedisplay ();	// not needed since timer triggers redisplay
		break;
	default:
		// do nothing ...
		break;
	};

	viewDir = viewDir*(1.0/viewDir.length());
	viewUp = viewUp*(1.0/viewUp.length());
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	// we have to use double buffer to avoid flickering
	// TODO: lookup "double buffer", what is it for, how is it used ...
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("assigment 3");

	init();	// init my variables first
	initGL();	// init the GL (i.e. view settings, ...)

	// assign callbacks
	glutTimerFunc(10, timer, 0);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display1);
	// you might want to add a resize function analog to
	// �bung1 using code similar to the initGL function ...

	// start main loop
	glutMainLoop();

	return 0;
}
