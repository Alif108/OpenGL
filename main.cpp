#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))
#define ANGLE_OF_ROTATION 3.0
#define MAX_RADIUS 30
#define RADIUS_RATE 1

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double radius;

struct point
{
	double x,y,z;
};

struct point pos;           /// position of the camera
struct point u;             /// up direction
struct point r;             /// right direction
struct point l;             /// left direction

struct point vector_cross(struct point v1, struct point v2)
{
    struct point product;

    product.x = v1.y * v2.z - v2.y * v1.z;
    product.y = v2.x * v1.z - v1.x * v2.z;
    product.z = v1.x * v2.y - v2.x * v1.y;

    return product;
}

int vector_dot(struct point v1, struct point v2)
{
    return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

struct point vector_sum(struct point v1, struct point v2)
{
    struct point sum;
    sum.x = v1.x + v2.x;
    sum.y = v1.y + v2.y;
    sum.z = v1.z + v2.z;

    return sum;
}

struct point vector_subtract(struct point v1, struct point v2)
{
    struct point sum;
    sum.x = v1.x - v2.x;
    sum.y = v1.y - v2.y;
    sum.z = v1.z - v2.z;

    return sum;
}

struct point vector_scale(struct point v, double a)
{
    struct point scaled_vector;
    scaled_vector.x = a * v.x;
    scaled_vector.y = a * v.y;
    scaled_vector.z = a * v.z;

    return scaled_vector;
}

struct point vector_rotate(struct point vector_to_be_rotated, struct point axis, double angle_to_rotate)
{
    /// this function follows the "Axis-Angle Rotation" formula
    /// r_new = r * cos(x) + (n x r) * sin(x)
    struct point perpendicular = vector_cross(axis, vector_to_be_rotated);      // n x r vector
    return vector_sum(vector_scale(vector_to_be_rotated, cos(angle_to_rotate * pi/180)), vector_scale(perpendicular, sin(angle_to_rotate * pi/180)));
}

void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a, 0);
		glVertex3f( a,-a, 0);
		glVertex3f(-a,-a, 0);
		glVertex3f(-a, a, 0);
	}glEnd();
}


void drawCircle(double radius,  int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);

    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }

    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y, 0);
			glVertex3f(points[i+1].x,points[i+1].y, 0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0; i<=stacks; i++)
	{
		h = radius*sin(((double)i/(double)stacks)*(pi/2));
		r = radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0; j<=slices; j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0; i<stacks; i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		glColor3f(1, 0 ,0);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
				glVertex3f(points[i][j+1]. x,points[i][j+1].y, points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}


void drawCylinderOneFourth(double radius, int segments)
{
    struct point points[100][2];

    for(int i=0; i<segments; i++)
    {
        points[i][0].x = radius * cos(((double)i / (double)segments) * 2 * pi/4);
        points[i][0].y = radius * sin(((double)i / (double)segments) * 2 * pi/4);
        points[i][0].z = MAX_RADIUS - radius;

        points[i][1].x = radius * cos(((double)i / (double)segments) * 2 * pi/4);
        points[i][1].y = radius * sin(((double)i / (double)segments) * 2 * pi/4);
        points[i][1].z = -(MAX_RADIUS - radius);
    }

    for (int i=0; i<segments; i++)
	{
		glColor3f(0.0f, 0.4f, 0.0f);

		glBegin(GL_QUADS);
		{
			glVertex3f(points[i][0].x, points[i][0].y, points[i][0].z);
			glVertex3f(points[i][1].x, points[i][1].y, points[i][1].z);
			glVertex3f(points[i + 1][1].x, points[i + 1][1].y, points[i + 1][1].z);
			glVertex3f(points[i + 1][0].x, points[i + 1][0].y, points[i + 1][0].z);
		}
		glEnd();
	}
}

void drawSphereOneEighth(double radius,int slices,int stacks)
{
	struct point points[100][100];
	double h,r;

	//generate points
	for(int i=0; i<=stacks; i++)
	{
		h = radius*sin(((double)i/(double)stacks)*(pi/2));
		r = radius*cos(((double)i/(double)stacks)*(pi/2));

		for(int j=0; j<=slices/4; j++)
		{
			points[i][j].x = r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y = r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z = h;
		}
	}

	//draw quads using generated points
	for(int i=0; i<stacks; i++)
	{
		glColor3f(0.9f, 0.9f ,0);

		for(int j=0; j<slices/4; j++)               /// only 1/4 th of the upper hemisphere
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
				glVertex3f(points[i][j+1]. x,points[i][j+1].y, points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}glEnd();
		}
	}
}

/// this function draws the 6 sides of the cube
void drawSides()
{
    glColor3f(0.4f, 0.0f, 0.4f);

    glPushMatrix();
    glTranslatef(0, 0, +MAX_RADIUS);
    drawSquare(MAX_RADIUS-radius);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, -MAX_RADIUS);
    drawSquare(MAX_RADIUS-radius);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, -1, 0);
    glTranslatef(0, 0, +MAX_RADIUS);
    drawSquare(MAX_RADIUS-radius);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 0, +MAX_RADIUS);
    drawSquare(MAX_RADIUS-radius);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glTranslatef(0, 0, +MAX_RADIUS);
    drawSquare(MAX_RADIUS-radius);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, -1, 0, 0);
    glTranslatef(0, 0, +MAX_RADIUS);
    drawSquare(MAX_RADIUS-radius);
    glPopMatrix();
}

void drawWholeSphere()
{
    glPushMatrix();
    glTranslatef(MAX_RADIUS-radius, MAX_RADIUS-radius, MAX_RADIUS-radius);
    drawSphereOneEighth(radius, 24, 20);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(MAX_RADIUS-radius, -(MAX_RADIUS-radius), MAX_RADIUS-radius);
    glRotatef(90, 1, 0, 0);
    drawSphereOneEighth(radius, 24, 20);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(MAX_RADIUS-radius, -(MAX_RADIUS-radius), -(MAX_RADIUS-radius));
    glRotatef(180, 1, 0, 0);
    drawSphereOneEighth(radius, 24, 20);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(MAX_RADIUS-radius, MAX_RADIUS-radius, -(MAX_RADIUS-radius));
    glRotatef(270, 1, 0, 0);
    drawSphereOneEighth(radius, 24, 20);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-(MAX_RADIUS-radius), MAX_RADIUS-radius, MAX_RADIUS-radius);
    glRotatef(-90, 0, 1, 0);
    drawSphereOneEighth(radius, 24, 20);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-(MAX_RADIUS-radius), MAX_RADIUS-radius, -(MAX_RADIUS-radius));
    glRotatef(-180, 0, 1, 0);
    drawSphereOneEighth(radius, 24, 20);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-(MAX_RADIUS-radius), -(MAX_RADIUS-radius), MAX_RADIUS-radius);
    glRotatef(-180, 0, 0, 1);
    drawSphereOneEighth(radius, 24, 20);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-(MAX_RADIUS-radius), -(MAX_RADIUS-radius), -(MAX_RADIUS-radius));
    glRotatef(90, 1, 0, 0);
    glRotatef(90, 1, 0, 0);
    glRotatef(90, 0, -1, 0);
    drawSphereOneEighth(radius, 24, 20);
    glPopMatrix();
}

void drawAllCylinders()
{
    /// -------------------------- Vertical Cylinders -------------------- ///

    glPushMatrix();
    glTranslatef(MAX_RADIUS-radius, MAX_RADIUS-radius, 0);
    drawCylinderOneFourth(radius, 25);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    glTranslatef(MAX_RADIUS-radius, MAX_RADIUS-radius, 0);
    drawCylinderOneFourth(radius, 25);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(MAX_RADIUS-radius, MAX_RADIUS-radius, 0);
    drawCylinderOneFourth(radius, 25);
    glPopMatrix();

    glPushMatrix();
    glRotatef(270, 0, 0, 1);
    glTranslatef(MAX_RADIUS-radius, MAX_RADIUS-radius, 0);
    drawCylinderOneFourth(radius, 25);
    glPopMatrix();


    /// -------------------------- Horizontal Cylinders (Upper) -------------------- ///

    glPushMatrix();
    glTranslatef(0, MAX_RADIUS-radius, MAX_RADIUS-radius);
    glRotatef(90, 0, -1, 0);
    drawCylinderOneFourth(radius, 25);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    glTranslatef(0, MAX_RADIUS-radius, MAX_RADIUS-radius);
    glRotatef(90, 0, -1, 0);
    drawCylinderOneFourth(radius, 25);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(0, MAX_RADIUS-radius, MAX_RADIUS-radius);
    glRotatef(90, 0, -1, 0);
    drawCylinderOneFourth(radius, 25);
    glPopMatrix();

    glPushMatrix();
    glRotatef(270, 0, 0, 1);
    glTranslatef(0, MAX_RADIUS-radius, MAX_RADIUS-radius);
    glRotatef(90, 0, -1, 0);
    drawCylinderOneFourth(radius, 25);
    glPopMatrix();


    /// -------------------------- Horizontal Cylinders (Lower) -------------------- ///

    glPushMatrix();
    glTranslatef(0, MAX_RADIUS-radius, -(MAX_RADIUS-radius));
    glRotatef(90, 0, 1, 0);
    drawCylinderOneFourth(radius, 25);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    glTranslatef(0, MAX_RADIUS-radius, -(MAX_RADIUS-radius));
    glRotatef(90, 0, 1, 0);
    drawCylinderOneFourth(radius, 25);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(0, MAX_RADIUS-radius, -(MAX_RADIUS-radius));
    glRotatef(90, 0, 1, 0);
    drawCylinderOneFourth(radius, 25);
    glPopMatrix();

    glPushMatrix();
    glRotatef(270, 0, 0, 1);
    glTranslatef(0, MAX_RADIUS-radius, -(MAX_RADIUS-radius));
    glRotatef(90, 0, 1, 0);
    drawCylinderOneFourth(radius, 25);
    glPopMatrix();
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
            l = vector_rotate(l, u, ANGLE_OF_ROTATION);
            r = vector_cross(l, u);
			break;
        case '2':
            l = vector_rotate(l, u, -ANGLE_OF_ROTATION);
            r = vector_cross(l, u);
			break;
        case '3':
            l = vector_rotate(l, r, ANGLE_OF_ROTATION);
            u = vector_cross(r, l);
            break;
        case '4':
            l = vector_rotate(l, r, -ANGLE_OF_ROTATION);
            u = vector_cross(r, l);
            break;
        case '5':
            u = vector_rotate(u, l, ANGLE_OF_ROTATION);
            r = vector_cross(l, u);
            break;
        case '6':
            u = vector_rotate(u, l, -ANGLE_OF_ROTATION);
            r = vector_cross(l, u);
            break;
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			pos = vector_subtract(pos, l);
			break;
		case GLUT_KEY_UP:		// up arrow key
			pos = vector_sum(pos, l);
			break;
		case GLUT_KEY_RIGHT:
			pos = vector_sum(pos, r);
			break;
		case GLUT_KEY_LEFT:
			pos = vector_subtract(pos, r);
			break;

		case GLUT_KEY_PAGE_UP:
		    pos = vector_sum(pos, u);
			break;
		case GLUT_KEY_PAGE_DOWN:
		    pos = vector_subtract(pos, u);
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
		    if(radius < MAX_RADIUS)
                radius += RADIUS_RATE;
			break;
		case GLUT_KEY_END:
		    if(radius > 0)
                radius -= RADIUS_RATE;
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);

    gluLookAt(pos.x, pos.y, pos.z,  pos.x + l.x, pos.y + l.y, pos.z + l.z,   u.x, u.y, u.z);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    //drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawCylinderOneFourth(radius, 25);
	drawWholeSphere();
	drawSides();
	drawAllCylinders();

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

    // initializing camera position
	pos.x = 100;
	pos.y = 100;
	pos.z = 0;

    // initializing up direction
	u.x = 0;
	u.y = 0;
	u.z = 1;

    // initializing down direction
	r.x = -1/sqrt(2);
	r.y = 1/sqrt(2);
	r.z = 0;

	// initializing look direction
	l = vector_cross(u, r);
	//l.x = -1/sqrt(2);
	//l.y = -1/sqrt(2);
	//l.z = 0;

	radius = 0;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
