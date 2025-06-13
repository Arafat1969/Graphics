#ifdef __linux__
#include <GL/glut.h>
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <GL/glut.h> 
#elif defined(__APPLE__)
#include <GLUT/glut.h> 
#else
#include <GL/glut.h>
#endif

#include <cstdio>
#include <cmath>
#include<ctime>
#include<chrono>

GLfloat secondTick = 0.0f;

int animationSpeed=20;

void init(){
	printf("Window is shown\n");

	glClearColor(0.0,0.0,0.0,1.0);

	glEnable(GL_DEPTH_TEST);
}

tm* getCurrentTime(){
	std::time_t currentTime = std:: time(nullptr);
	std::tm* localTime = std::localtime(&currentTime);
	return localTime;
}

int getHour(){
	tm* localTime= getCurrentTime();
	return localTime->tm_hour;
}

int getMinute(){
	tm* localTime = getCurrentTime();
	return localTime->tm_min;
}

int getSeconds(){
	tm* localTime = getCurrentTime();
	return localTime->tm_sec;
}

void drawSquare(double x , double y)
{
    glBegin(GL_QUADS);
    {
        glVertex3d(x+0.02, y+0.02, 0); 
        glVertex3d(x+0.02, y-0.02, 0); 
        glVertex3d(x-0.02, y-0.02, 0);
        glVertex3d(x-0.02, y+0.02, 0);
    }
    glEnd();
}

void drawCircle(double a,int segments){
    glBegin(GL_POINTS);
	{
		glColor3f(1.0f,1.0f,1.0f);
		glVertex3f(0.0f,0.0f,0.0f);
	}
	glEnd();

	glBegin(GL_LINE_LOOP);
	{	
		glColor3f(1.0f,1.0f,1.0f);
		for (int i = 0; i <= segments; i++) {
			float angle = 2.0f * M_PI * i / segments;
			float x = a * cos(angle);
			float y = a * sin(angle);
			glVertex2f(x, y);
		}

	}
	glEnd();
}

void drawClockticks(){
	for(int i=0;i<=60;i++){
		double a;
		float angle = 2.0f * M_PI * i / 60;
		if(i%5==0){
			a=.50;
		}else{
			a=.53;
		}
		glBegin(GL_LINES);
		{
			float x1=0.56 * cos(angle);
			float y1=0.56 * sin(angle);
			float x2=a * cos(angle);
			float y2=a * sin(angle);
			glVertex3d(x1,y1,0.00);
			glVertex3d(x2,y2,0.00);	
		}
		glEnd();
	}
}

void drawLine(double a,double angle){

	glBegin(GL_LINES);
	{
		float x = a * cos(angle);
		float y = a * sin(angle);
		glVertex3d(0.0f,0.0f,0.0f);
		glVertex3d(x,y,0.0f);
	}
	glEnd();
}

void drawAxes(double length){
	glBegin(GL_LINES);
	{
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f(-length,0.0f,0.0f);
		glVertex3f(length,0.0f,0.0f);

		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,-length,0.0f);
		glVertex3f(0.0f,length,0.0f);

		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f,0.0f,-length);
		glVertex3f(0.0f,0.0f,length);
	}
	glEnd();
}

void display(){
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
	
	gluLookAt(
		0.0,0.0,3.0,
		0.0,0.0,0.0,
		0.0,1.0,0.0
	);
	
	// drawAxes(1.5);

	float a= 0.6;
	glPushMatrix();
	{
		glColor3f(1.0f,1.0f,1.0f);
		drawCircle(a,60);
	}
	glPopMatrix();

	drawClockticks();


	int i1 = getSeconds();
	// printf("seconds %d\n",i1);
	// float angle1 = -(2.0f * M_PI * i1 / 60.0)+ (M_PI/2.0);
	float angle1 = -(M_PI / 180.0f) * secondTick + (M_PI / 2.0f);
	glPushMatrix();
	{
		//glRotatef(-secondTick,0,0,1);
		glColor3f(1.0f,0.0f,0.0f);
		drawLine(0.47,angle1);
	}
	glPopMatrix();

	int i2= getMinute();
	// printf("minute %d\n",i2);
	float angle2 = -(2.0f * M_PI * i2 / 60.0)+(M_PI / 2.0);
	glPushMatrix();
	{
		glColor3f(1.0f,1.0f,1.0f);
		drawLine(0.37,angle2);
	}
	glPopMatrix();

	int i3=getHour();
	float i4= (float)i3+ i2/60.0;
	// printf("hour %d\n",i3);
	float angle3 = -(2.0f * M_PI * i4 / 12.0)+(M_PI/2.0);
	glPushMatrix();
	{
		glColor3f(1.0f,1.0f,1.0f);
		drawLine(0.27,angle3);
	}
	glPopMatrix();

	double x1 = a * cos(angle1);
	double y1 = a * sin(angle1);

	glPushMatrix();
	{
		glColor3f(1.0f,0.0f,0.0f);
		drawSquare(x1,y1);
	}
	glPopMatrix();

	double x2 = a * cos(angle2);
	double y2 = a * sin(angle2);

	glPushMatrix();
	{
		glColor3f(1.0f,1.0f,1.0f);
		drawSquare(x2,y2);
	}
	glPopMatrix();

	double x3 = a * cos(angle3);
	double y3 = a * sin(angle3);
	glPushMatrix();
	{
		glColor3f(1.0f,1.0f,1.0f);
		drawSquare(x3,y3);
	}
	glPopMatrix();

	glutSwapBuffers();

}

void reshape(int width,int height){
	
	if(height == 0){
		height=1;
	}

	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float aspectRatio = (float)width / (float)height;
	gluPerspective(45.0f,aspectRatio, 0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void timerFunction(int value){

	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	int seconds = (millis / 1000) % 60;
	int milliseconds = millis % 1000;
	secondTick = (seconds + milliseconds / 1000.0f) * 6.0f;
	



	glutPostRedisplay();
	glutTimerFunc(animationSpeed,timerFunction,0);
}

int main(int argc, char **argv){
	glutInit(&argc, argv);
	//printf("1 \n");

    glutInitWindowSize(800, 600);
	//printf("2 \n");

    glutInitWindowPosition(100, 100);	
	// printf("3 \n");

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	//// printf("4 \n");

	glutCreateWindow("My first opneGL practise ");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutTimerFunc(animationSpeed,timerFunction,0);

	init();
	

	glutMainLoop();

	return 0;

}