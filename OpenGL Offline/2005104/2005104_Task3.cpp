#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<ctime>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include<GL/glut.h>
#endif


GLfloat eyex = 4, eyey = 4, eyez = 4;
GLfloat centerx = 0, centery = 0, centerz = 0;
GLfloat upx = 0, upy = 1, upz = 0;


GLfloat ballRadius = 0.1;
GLfloat ballX = 0.0, ballY = 0.0, ballZ = 0.0;
GLfloat ballVelX = 0.0, ballVelY = 0.0, ballVelZ = 0.0;
GLfloat ballRotX = 0.0, ballRotY = 0.0, ballRotZ = 0.0;


GLfloat gravity = 9.8;
GLfloat damping = 0.85;
GLfloat initialSpeed = 2.0;
GLboolean simulationRunning = GL_FALSE;
GLboolean showVelocityArrow = GL_FALSE;


GLfloat angVelX = 0.0f; 
GLfloat angVelY = 0.0f; 
GLfloat angVelZ = 0.0f; 


GLfloat prevX = 0.0, prevY = 0.0, prevZ = 0.0;

// Time tracking
GLfloat deltaTime = 0.016; 

struct vector
{
	double x,y,z;
	double len;
};
	



// void drawSphere(GLfloat radius, GLint slices, GLint stacks) {
//     glutSolidSphere(radius, slices, stacks);
// }

void resetBall() {
    if (simulationRunning) {
        printf("Cannot reset while simulation is running. Stop simulation first.\n");
        return;
    }
    
    static int firstTime = 1;
    if (firstTime) {
        srand(time(NULL));
        firstTime = 0;
    }
    
    GLfloat cubeSize = 1.5;
    GLfloat surfaceMargin = 0.1;
    
    ballX = ((GLfloat)rand() / RAND_MAX * (2.0f * (cubeSize - surfaceMargin - ballRadius))) - (cubeSize - surfaceMargin - ballRadius);
    ballZ = ((GLfloat)rand() / RAND_MAX * (2.0f * (cubeSize - surfaceMargin - ballRadius))) - (cubeSize - surfaceMargin - ballRadius);
    
    ballY = -cubeSize + ballRadius + 0.5f;
    
    ballVelX = ((GLfloat)rand() / RAND_MAX - 0.5f) * initialSpeed; 
    ballVelZ = ((GLfloat)rand() / RAND_MAX - 0.5f) * initialSpeed;
    ballVelY = ((GLfloat)rand() / RAND_MAX) * initialSpeed + initialSpeed/2.0f;
    
    ballRotX = 0.0f;
    ballRotY = 0.0f;
    ballRotZ = 0.0f;
    
    angVelX = 0.0f;
    angVelY = 0.0f; 
    angVelZ = 0.0f;
    
    prevX = ballX;
    prevY = ballY;
    prevZ = ballZ;
    
    printf("Reset ball with speed: %.2f\n", initialSpeed);
    simulationRunning = GL_TRUE;
}


void updateBall() {
    if (!simulationRunning) return;
    
    prevX = ballX;
    prevY = ballY;
    prevZ = ballZ;
    
    ballVelY -= gravity * deltaTime;
    
    ballX += ballVelX * deltaTime;
    ballY += ballVelY * deltaTime;
    ballZ += ballVelZ * deltaTime;
    
    ballRotX += angVelX * deltaTime * 180.0f / M_PI;
    ballRotY += angVelY * deltaTime * 180.0f / M_PI;
    ballRotZ += angVelZ * deltaTime * 180.0f / M_PI;
    
    GLfloat cubeSize = 1.5;
    GLfloat frictionCoeff = 0.01f;

    if (ballX + ballRadius > cubeSize) {
        ballX = cubeSize - ballRadius;
        ballVelX = -ballVelX * damping;
        angVelZ += ballVelY * 0.5f / ballRadius;
    } else if (ballX - ballRadius < -cubeSize) {
        ballX = -cubeSize + ballRadius;
        ballVelX = -ballVelX * damping;
        angVelZ -= ballVelY * 0.5f / ballRadius;
    }
    
    if (ballY + ballRadius > cubeSize) {
        ballY = cubeSize - ballRadius;
        ballVelY = -ballVelY * damping;
        angVelX += ballVelZ * 0.5f / ballRadius;
        angVelZ -= ballVelX * 0.5f / ballRadius;
    } else if (ballY - ballRadius < -cubeSize) {
        ballY = -cubeSize + ballRadius;
        ballVelY = -ballVelY * damping;
        angVelX -= ballVelZ * 0.5f / ballRadius;
        angVelZ += ballVelX * 0.5f / ballRadius;
        
        if (fabs(ballVelY) < 0.2) {
            ballVelY = 0;
        }
    }
    
    if (ballZ + ballRadius > cubeSize) {
        ballZ = cubeSize - ballRadius;
        ballVelZ = -ballVelZ * damping;
        angVelX -= ballVelY * 0.5f / ballRadius;
    } else if (ballZ - ballRadius < -cubeSize) {
        ballZ = -cubeSize + ballRadius;
        ballVelZ = -ballVelZ * damping;
        angVelX += ballVelY * 0.5f / ballRadius;
    }
    
    if (fabs(ballY - (-cubeSize + ballRadius)) < 0.01) {
        GLfloat horizontalSpeed = sqrt(ballVelX*ballVelX + ballVelZ*ballVelZ);
        
        if (horizontalSpeed > 0.001f) {
            GLfloat frictionX = -ballVelX / horizontalSpeed;
            GLfloat frictionZ = -ballVelZ / horizontalSpeed;
            
            GLfloat frictionMagnitude = frictionCoeff * gravity * deltaTime;
            
            if (frictionMagnitude > horizontalSpeed) {
                frictionMagnitude = horizontalSpeed;
            }
            
            ballVelX += frictionX * frictionMagnitude;
            ballVelZ += frictionZ * frictionMagnitude;
            
            angVelX = ballVelZ / ballRadius;
            angVelZ = -ballVelX / ballRadius;
        } else {
            if (horizontalSpeed < 0.000000005f) {
                ballVelX = 0.0f;
                ballVelZ = 0.0f;
                angVelX *= 0.99f;
                angVelZ *= 0.99f;
                simulationRunning = GL_FALSE;  
            }
        }
    }
    
    if (fabs(ballY - (-cubeSize + ballRadius)) > 0.01) {
        angVelX *= 0.999f; 
        angVelY *= 0.999f;
        angVelZ *= 0.999f;
    }
    
    ballVelX *= 0.999f;
    ballVelZ *= 0.999f;
}

void drawVelocityArrow() {
    if (!showVelocityArrow) return;
    
    GLfloat velMag = sqrt(ballVelX*ballVelX + ballVelY*ballVelY + ballVelZ*ballVelZ);
    
    if (velMag < 0.001) {
        if (!simulationRunning) {
            showVelocityArrow = GL_FALSE;
        }
        return;
    }
    
    GLfloat vx = ballVelX / velMag;
    GLfloat vy = ballVelY / velMag;
    GLfloat vz = ballVelZ / velMag;
    
    const GLfloat arrowLength = 0.5f;
    const GLfloat arrowHeadLength = 0.15f;    
    const GLfloat arrowHeadRadius = 0.027f;  
    const GLfloat arrowShaftRadius = 0.009f; 
    
    
    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    
    glPushMatrix();
    

    glTranslatef(ballX, ballY, ballZ);
    
    GLfloat zAxis[3] = {0, 0, 1};
    GLfloat rotAxis[3] = {
        zAxis[1] * vz - zAxis[2] * vy,
        zAxis[2] * vx - zAxis[0] * vz,
        zAxis[0] * vy - zAxis[1] * vx 
    };
    
    GLfloat rotAxisLength = sqrt(rotAxis[0]*rotAxis[0] + rotAxis[1]*rotAxis[1] + rotAxis[2]*rotAxis[2]);
    
    if (rotAxisLength < 0.001f) {
        if (vz < 0) {
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        }
    } else {
        rotAxis[0] /= rotAxisLength;
        rotAxis[1] /= rotAxisLength;
        rotAxis[2] /= rotAxisLength;
        
        GLfloat dot = zAxis[0]*vx + zAxis[1]*vy + zAxis[2]*vz; 
        GLfloat angle = acos(dot) * 180.0f / M_PI;  
        
        glRotatef(angle, rotAxis[0], rotAxis[1], rotAxis[2]);
    }
    
    glColor3f(1.0f, 1.0f, 0.0f);  
    gluCylinder(quadric, arrowShaftRadius, arrowShaftRadius, arrowLength - arrowHeadLength, 12, 1);
    
    glTranslatef(0, 0, arrowLength - arrowHeadLength);
    
    
    glColor3f(1.0f, 0.5f, 0.0f); 
    gluDisk(quadric, arrowShaftRadius, arrowHeadRadius, 16, 1);
    
    gluCylinder(quadric, arrowHeadRadius, 0, arrowHeadLength, 16, 1);
    
    glPopMatrix();
    gluDeleteQuadric(quadric);
}

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    resetBall();
    simulationRunning = GL_FALSE;
    
    printf("--- 3D Bouncing Ball Controls ---\n");
    printf("Space: Toggle simulation on/off\n");
    printf("v: Toggle velocity arrow display\n");
    printf("+/-: Increase/decrease ball speed (when paused)\n");
    printf("r: Reset ball to random position (when paused)\n\n");
    
    printf("--- Camera Controls ---\n");
    printf("Arrow keys: Move camera left/right/forward/backward\n");
    printf("Page Up/Down: Move camera up/down\n");
    printf("w/s: Move camera up/down (keeping reference point)\n");
    printf("1/2: Look left/right\n");
    printf("3/4: Look up/down\n");
    printf("5/6: Tilt camera clockwise/counterclockwise\n");
}


void drawCube(){
	glBegin(GL_QUADS);
	{
		// Top face (y = 1.0f) - Magenta
		glColor3f(0.5f, 0.5f, 1.0f);
		glVertex3f(1.5f, 1.5f, -1.5f);
		glVertex3f(-1.5f, 1.5f, -1.5f);
		glVertex3f(-1.5f, 1.5f, 1.5f);
		glVertex3f(1.5f, 1.5f, 1.5f);
	
		// Bottom face (y = -1.0f) - CheckerBoard
		GLfloat x=-1.5f,z=-1.5f,a=1.0f;
		GLfloat tilesize=0.15f;
		for(int i=0;i<20;i++){
			z = -1.5f;
			for(int j=0;j<20;j++){
				glColor3f(a, a, a);
				glVertex3f(x, -1.5f, z);
				glVertex3f(x+tilesize, -1.5f, z);
				glVertex3f(x+tilesize, -1.5f, z+tilesize);
				glVertex3f(x, -1.5f, z+tilesize);
				z = z + tilesize;
				a = a==1.0f ? 0.0f : 1.0f;
			}
			x = x + tilesize;
		 	a = a==1.0f ? 0.0f : 1.0f;
		}
	
		// Front face  (z = 1.5f) - Teal
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(1.5f, 1.5f, 1.5f);
		glVertex3f(-1.5f, 1.5f, 1.5f);
		glVertex3f(-1.5f, -1.5f, 1.5f);
		glVertex3f(1.5f, -1.5f, 1.5f);
	
		// Back face (z = -1.5f) - Yellow
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(1.5f, -1.5f, -1.5f);
		glVertex3f(-1.5f, -1.5f, -1.5f);
		glVertex3f(-1.5f, 1.5f, -1.5f);
		glVertex3f(1.5f, 1.5f, -1.5f);
	
		// Left face (x = -1.5f) -Red
		glColor3f(1.0f, 0.0f, 0.0f); 
		glVertex3f(-1.5f, 1.5f, 1.5f);
		glVertex3f(-1.5f, 1.5f, -1.5f);
		glVertex3f(-1.5f, -1.5f, -1.5f);
		glVertex3f(-1.5f, -1.5f, 1.5f);
	
		// Right face (x = 1.5f) - Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(1.5f, 1.5f, -1.5f);
		glVertex3f(1.5f, 1.5f, 1.5f);
		glVertex3f(1.5f, -1.5f, 1.5f);
		glVertex3f(1.5f, -1.5f, -1.5f);
	}
	glEnd();
}


void drawBall() {
    glPushMatrix();
    
    glTranslatef(ballX, ballY, ballZ);
    
    glRotatef(ballRotX, 1.0, 0.0, 0.0);
    glRotatef(ballRotY, 0.0, 1.0, 0.0);
    glRotatef(ballRotZ, 0.0, 0.0, 1.0);
    
    const int stacks = 20;
    const int slices = 20;
    const float PI = 3.14159265358979323846f;
    
    float stackStep = PI / stacks;
    float sliceStep = 2.0f * PI / slices;
    float stackAngle, sliceAngle;
    float x, y, z;
    
    int middleStack = stacks / 2;
    

    for (int i = 0; i < stacks; i++) {
        stackAngle = PI / 2.0f - i * stackStep;
        float nextStackAngle = PI / 2.0f - (i + 1) * stackStep;
        
        bool isTopHalf = (i < middleStack);
        
        glBegin(GL_QUAD_STRIP);
        
        for (int j = 0; j <= slices; j++) {
            sliceAngle = j * sliceStep;  
            
            x = ballRadius * cosf(nextStackAngle) * cosf(sliceAngle);
            y = ballRadius * sinf(nextStackAngle);
            z = ballRadius * cosf(nextStackAngle) * sinf(sliceAngle);
            
            if (j % 2 == 0) {
                if (isTopHalf) {
                    glColor3f(1.0f, 0.0f, 0.0f);
                } else {
                    glColor3f(0.0f, 1.0f, 0.0f);
                }
            } else {
                if (isTopHalf) {
                    glColor3f(0.0f, 1.0f, 0.0f);
                } else {
                    glColor3f(1.0f, 0.0f, 0.0f);
                }
            }
            
            glVertex3f(x, y, z);
            
            x = ballRadius * cosf(stackAngle) * cosf(sliceAngle);
            y = ballRadius * sinf(stackAngle);
            z = ballRadius * cosf(stackAngle) * sinf(sliceAngle);
            
            glVertex3f(x, y, z);
        }
        
        glEnd();
    }
    
    glPopMatrix();
}


void move(struct vector vect,int sign,double v){
	eyex += sign * vect.x * v;
	eyey += sign * vect.y * v;
	eyez += sign * vect.z * v;
	centerx += sign * vect.x * v;
	centery += sign * vect.y * v;
	centerz += sign * vect.z * v;
}



void specialKeyListener(int key, int x, int y){
	double v = 0.05;

	struct vector look;
	struct vector up;
	struct vector right;
	
    look.x = centerx - eyex;
    look.y = centery - eyey;
    look.z = centerz - eyez;
    
    look.len = sqrt(look.x*look.x + look.y*look.y + look.z*look.z);
    look.x /= look.len;
    look.y /= look.len;
    look.z /= look.len;
    
    right.x = upy*look.z - upz*look.y;
    right.y = upz*look.x - upx*look.z;
    right.z = upx*look.y - upy*look.x;
    
    right.len = sqrt(right.x*right.x + right.y*right.y + right.z*right.z);
    right.x /= right.len;
    right.y /= right.len;
    right.z /= right.len;

	up.x = upx;
	up.y = upy;
	up.z = upz;
	up.len = sqrt(up.x*up.x + up.y*up.y + up.z*up.z);
	switch (key)
	{
		case GLUT_KEY_RIGHT:{
			move(right,-1,v);
			break;
		}
		case GLUT_KEY_LEFT:{
			move(right,1,v);
			break;
		}
		case GLUT_KEY_UP:{
			move(look,1,v);
			break;
		}
		case GLUT_KEY_DOWN:{
			move(look,-1,v);
			break;
		}
		case GLUT_KEY_PAGE_UP:{
			eyex += upx * v;
			eyey += upy * v;
			eyez += upz * v;
			centerx += upx * v;
			centery += upy * v;
			centerz += upz * v;
			break;
		}
		case GLUT_KEY_PAGE_DOWN:{
			eyex -= upx * v;
			eyey -= upy * v;
			eyez -= upz * v;
			centerx -= upx * v;
			centery -= upy * v;
			centerz -= upz * v;
			break;
		}
		default:{
			printf("Unknown key pressed\n");
        	break;
		}
	}

	glutPostRedisplay();
}


void roll(struct vector look,double angle){
	double nx = upx;
	double ny = upy;
	double nz = upz;
	double dot = look.x*upx + look.y*upy + look.z*upz;
	

	double cx = look.y*upz - look.z*upy;
	double cy = look.z*upx - look.x*upz;
	double cz = look.x*upy - look.y*upx;
	
	// Rodrigues rotation formula
	upx = nx*cos(angle) + cx*sin(angle) + look.x*dot*(1-cos(angle));
	upy = ny*cos(angle) + cy*sin(angle) + look.y*dot*(1-cos(angle));
	upz = nz*cos(angle) + cz*sin(angle) + look.z*dot*(1-cos(angle));
	
	double uplen = sqrt(upx*upx + upy*upy + upz*upz);
	upx /= uplen;
	upy /= uplen;
	upz /= uplen;
}

void pitch(struct vector look,struct vector right,double angle){
	double c = cos(angle);
	double s = sin(angle);

	double oldLx = look.x;
	double oldLy = look.y;
	double oldLz = look.z;
	
	double dotProd = right.x*look.x + right.y*look.y + right.z*look.z; 

	// Rodrigues rotation formula
	// l' = l*cos(θ) + (r×l)*sin(θ) + r*(r·l)*(1-cos(θ))
	look.x = oldLx*c + (right.y*oldLz - right.z*oldLy)*s + right.x*dotProd*(1-c);
	look.y = oldLy*c + (right.z*oldLx - right.x*oldLz)*s + right.y*dotProd*(1-c);
	look.z = oldLz*c + (right.x*oldLy - right.y*oldLx)*s + right.z*dotProd*(1-c);
	

	double newLen = sqrt(look.x*look.x + look.y*look.y + look.z*look.z);
	look.x /= newLen;
	look.y /= newLen;
	look.z /= newLen;
	
	right.x = upy*look.z - upz*look.y;
	right.y = upz*look.x - upx*look.z;
	right.z = upx*look.y - upy*look.x;
	
	right.len = sqrt(right.x*right.x + right.y*right.y + right.z*right.z);
	right.x /= right.len;
	right.y /= right.len;
	right.z /= right.len;
	
	upx = look.y*right.z - look.z*right.y;
	upy = look.z*right.x - look.x*right.z;
	upz = look.x*right.y - look.y*right.x;
	
	double uplen = sqrt(upx*upx + upy*upy + upz*upz);
	upx /= uplen;
	upy /= uplen;
	upz /= uplen;
	
	centerx = eyex + look.x*look.len;
	centery = eyey + look.y*look.len;
	centerz = eyez + look.z*look.len;
}

void yaw(struct vector look,double angle){
	double temp = look.x*cos(angle) - look.z*sin(angle);
	look.z = look.x*sin(angle) + look.z*cos(angle);
	look.x = temp;

	centerx = eyex + look.x*look.len;
	centery = eyey + look.y*look.len;
	centerz = eyez + look.z*look.len;
}

void moveWS(){
	double lx = centerx - eyex;
	double ly = centery - eyey;
	double lz = centerz - eyez;

	double len = sqrt(lx*lx + ly*ly + lz*lz);

	lx /= len;
	ly /= len;
	lz /= len;
	
	double rx = upy*lz - upz*ly;
	double ry = upz*lx - upx*lz;
	double rz = upx*ly - upy*lx;

	double rlen = sqrt(rx*rx + ry*ry + rz*rz);

	rx /= rlen;
	ry /= rlen;
	rz /= rlen;
	
	upx = ly*rz - lz*ry;
	upy = lz*rx - lx*rz;
	upz = lx*ry - ly*rx;

	double uplen = sqrt(upx*upx + upy*upy + upz*upz);

	upx /= uplen;
	upy /= uplen;
	upz /= uplen;
}


void keyboardListener(unsigned char key, int x,int y){
	double v= 0.05;
	double angle=0.05;

	struct vector look;

	 look.x = centerx - eyex;
	 look.y = centery - eyey;
	 look.z = centerz - eyez;

	look.len = sqrt(look.x*look.x+look.y*look.y+look.z*look.z);

	look.x /= look.len;
	look.y /= look.len;
	look.z /= look.len;

	struct vector right;
	

	right.x = upy*look.z - upz*look.y;
	right.y = upz*look.x - upx*look.z;
	right.z = upx*look.y - upy*look.x;

	right.len= sqrt(right.x*right.x + right.y*right.y + right.z*right.z);
	right.x /= right.len;
	right.y /= right.len;
	right.z /= right.len;

	double temp, nx , ny , nz;
	 
	switch (key)
	{
	case '1':
		yaw(look,-angle);
		break;
	case '2':
		yaw(look,angle);
		break;
	case '3':{
		pitch(look,right,-angle);
		break;
	}
	case '4':{
		pitch(look,right,angle);
		break;
	}
	case '5':{
		roll(look,angle);
		break;
	}
	case '6':{	
		roll(look,-angle);
		break;
	}
	case 'w':{
		eyey += v;
		moveWS();
		break;
	}

	case 's':{
		eyey -= v;
		moveWS();
		break;
	}

	case 'r':
		resetBall();
		break;
	case 'v':
		showVelocityArrow = ! showVelocityArrow;
		printf("Toggle velocity arrow\n");
		break;
	case ' ':
		simulationRunning = ! simulationRunning;
		break;
	case '+':
        if (!simulationRunning) {
            initialSpeed += 0.1f;
            if (initialSpeed > 10.0f) initialSpeed = 10.0f;
            printf("Initial speed increased to %.1f\n", initialSpeed);
        }
        break;
    case '-':
        if (!simulationRunning) {
            initialSpeed -= 0.1f;
            if (initialSpeed < 0.1f) initialSpeed = 0.1f;
            printf("Initial speed decreased to %.1f\n", initialSpeed);
        } else
        break;	
	default:
		printf("Unknown key pressed\n");
		break;
	}

	glutPostRedisplay();
} 

void reshapeListener(GLsizei width, GLsizei height){
	if(height==0){
		height=1;
	}

	GLfloat aspect = (GLfloat)width / (GLfloat)height;

	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}


void timer(int value) {
    updateBall();
    
    glutPostRedisplay();

    glutTimerFunc(16, timer, 0);
}



void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
		eyex , eyey , eyez,
		centerx , centery , centerz,
		upx , upy , upz
	);

	drawCube();

	drawBall();

	drawVelocityArrow();

	glutSwapBuffers();
}


int main(int argc, char **argv){
	glutInit(&argc,argv);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1760,1060);
	glutInitWindowPosition(25,25);
	glutCreateWindow("3D Bouncing Ball Simulation");

	glutDisplayFunc(display);
	glutReshapeFunc(reshapeListener);
	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutTimerFunc(16,timer,0);

	initGL();

	glutMainLoop();
	return 0;
}
