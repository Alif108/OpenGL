#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))

#define WHEEL_RADIUS 15
#define CHANGE_DIRECTION_RATE 2
#define STEP_SIZE 2
#define INCLINATION_RATE 3
#define INCLINATION_ANGLE 30

#define ROBOT_LENGTH 60
#define ROBOT_WIDTH 40
#define RADIUS_SIZE_RATE 0.5
#define MAX_RADIUS 25
#define MIN_RADIUS 5

struct point
{
	double x,y,z;
};

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;

double direction_angle;                 // angle to change direction
double rotation_angle;                  // angle for inner rotation
double radius;                          // radius of the wheel
struct point translatedPoint;           // to determine the translation

double robot_length;
double robot_width;


/// ------------------------- Draw Necessary Shapes ------------------ ///

void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		{
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
				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}
		glEnd();
	}
}


void drawRectangle(double width, double height)
{
    glBegin(GL_QUADS);
    {
        glVertex3f(width/2, height/2, 0);
		glVertex3f(width/2, -height/2, 0);
		glVertex3f(-width/2, -height/2, 0);
		glVertex3f(-width/2, height/2, 0);
    }
    glEnd();
}

void drawWheelStructure(double radius, double wheel_width, int segments)
{
    /// DRAW CIRCLES -------------------------------
    // draw two circles in a given width and then
    // connect the points using GL_QUADS

    int i;
    struct point first_circle[100];
    struct point second_circle[100];
    glColor3f(0.7,0.7,0.7);

    //generate points
    for(i=0; i<=segments; i++)
    {
        first_circle[i].x = radius*cos(((double)i/(double)segments)*2*pi);
        first_circle[i].y = radius*sin(((double)i/(double)segments)*2*pi);
        first_circle[i].z = -wheel_width/2;

        second_circle[i].x = radius*cos(((double)i/(double)segments)*2*pi);
        second_circle[i].y = radius*sin(((double)i/(double)segments)*2*pi);
        second_circle[i].z = wheel_width/2;
    }

    //draw segments using generated points
    for(i=0; i<segments; i++)
    {
        glBegin(GL_QUADS);
        {
			glVertex3f(first_circle[i].x, first_circle[i].y, first_circle[i].z);
			glVertex3f(second_circle[i].x, second_circle[i].y, second_circle[i].z);
			glVertex3f(second_circle[i+1].x, second_circle[i+1].y, second_circle[i+1].z);
			glVertex3f(first_circle[i+1].x, first_circle[i+1].y, first_circle[i].z);
        }
        glEnd();
    }

    /// DRAW SPOKES WITH RECTANGLES -------------------------------
    // horizontal spoke
    glPushMatrix();
    glColor3f(0.87, 0.92, 0.2);
    glRotatef(90, 0, 1, 0);
    drawRectangle(wheel_width, radius*2);
    glPopMatrix();

    // vertical spoke
    glPushMatrix();
    glColor3f(0.7, 0.7, 0);
    glRotatef(90, 1, 0, 0);
    glRotatef(90, 0, 0, 1);
    drawRectangle(wheel_width, radius*2);
    glPopMatrix();
}

/// -------------------------- Move Functions ------------------------- ///

void moveWheelRight()
{
    direction_angle -= CHANGE_DIRECTION_RATE;
}

void moveWheelLeft()
{
    direction_angle += CHANGE_DIRECTION_RATE;
}

void moveWheelForward()
{
    translatedPoint.x += STEP_SIZE * cos(direction_angle * pi/180);
    translatedPoint.y += STEP_SIZE * sin(direction_angle * pi/180);
    translatedPoint.z = 0;

    rotation_angle += 360 * STEP_SIZE / (2 * pi * radius);      //  theta = s * 360 / 2*pi*r
}

void moveWheelBackward()
{
    translatedPoint.x -= STEP_SIZE * cos(direction_angle * pi/180);
    translatedPoint.y -= STEP_SIZE * sin(direction_angle * pi/180);
    translatedPoint.z = 0;

    rotation_angle -= 360 * STEP_SIZE / (2 * pi * radius);      //  theta = s * 360 / 2*pi*r
}

void drawWheel()
{
    glPushMatrix();

    glTranslated(translatedPoint.x, translatedPoint.y, translatedPoint.z);      // translate to position
    glRotatef(direction_angle, 0, 0, 1);        // rotate the wheel to direction

    glTranslatef(0, 0, radius);                 // shift on xy-plane
    glRotatef(90, 1, 0, 0);                     // make the wheel stand up  :3

    glRotatef(-rotation_angle, 0, 0, 1);        // rotate the wheel around its own axis

    drawWheelStructure(radius, 10, 20);         // draw the actual wheel structure

    glPopMatrix();
}

void drawAntenna()
{
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
    {
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 15);
    }
    glEnd();

    glBegin(GL_LINES);
    {
        glVertex3f(0, 0, 15);
        glVertex3f(-robot_length, 0, 15);
    }
    glEnd();
}

void drawRobot()
{
    /// ------------ draw 4 wheels ----------- ///
    glPushMatrix();
    glTranslated(robot_length/2, robot_width/2, 0);
    drawWheel();
    glPopMatrix();

    glPushMatrix();
    glTranslated(robot_length/2, -robot_width/2, 0);
    drawWheel();
    glPopMatrix();

    glPushMatrix();
    glTranslated(-robot_length/2, robot_width/2, 0);
    drawWheel();
    glPopMatrix();

    glPushMatrix();
    glTranslated(-robot_length/2, -robot_width/2, 0);
    drawWheel();
    glPopMatrix();

    /// ------------- robot roof ------------- ///
    glPushMatrix();
    glColor3f(0.5, 0, 1);
    glTranslated(translatedPoint.x, translatedPoint.y, translatedPoint.z);      // translate to position
    glTranslated(0, 0, 2*radius);                                               // roof will be on height=diameter of wheel
    drawRectangle(robot_length, robot_width);
    glPopMatrix();

    /// ------------- robot antenna ------------ ///
    glPushMatrix();
    glTranslated(translatedPoint.x, translatedPoint.y, translatedPoint.z);      // translate to position
    glRotatef(direction_angle, 0, 0, 1);                                        // rotate the antenna to direction
    glTranslatef(0, 0, 2*radius);                                               // on roof
    drawAntenna();
    glPopMatrix();
}

/// --------------------------- Key Functions ---------------------- ///

void keyboardListener(unsigned char key, int x,int y){
	switch(key){
		case '1':
			drawgrid = 1 - drawgrid;
			break;
        case 'd':
            moveWheelRight();
			break;
        case 'D':
            moveWheelRight();
			break;
        case 'a':
            moveWheelLeft();
			break;
        case 'A':
            moveWheelLeft();
			break;
        case 'w':
            moveWheelBackward();
            break;
        case 'W':
            moveWheelBackward();
            break;
        case 's':
            moveWheelForward();
            break;
        case 'S':
            moveWheelForward();
            break;
        case 'c':
            if(radius > MIN_RADIUS)
                radius -= RADIUS_SIZE_RATE;
            break;
        case 'C':
            if(radius > MIN_RADIUS)
                radius -= RADIUS_SIZE_RATE;
            break;
        case 'x':
            if(radius < MAX_RADIUS)
                radius += RADIUS_SIZE_RATE;
            break;
        case 'X':
            if(radius < MAX_RADIUS)
                radius += RADIUS_SIZE_RATE;
            break;
		default:
			break;
	}
}

void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraHeight -= 3.0;
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraHeight += 3.0;
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
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

/// -------------------------- Display Functions ------------------------- ///

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

	gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();

	drawGrid();

//	drawWheel();
    drawRobot();

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid = 1;
	drawaxes = 0;
	cameraHeight = 150.0;
	cameraAngle = 1.0;

	direction_angle = 0;
	rotation_angle = 0;
	radius = WHEEL_RADIUS;
	robot_length = ROBOT_LENGTH;
	robot_width = ROBOT_WIDTH;

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

/// ------------------- Driver Function ------------------- ///

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
