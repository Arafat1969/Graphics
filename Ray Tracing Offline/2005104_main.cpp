#include "2005104_classes.hpp"
#include "stb_image.h"
#include<sstream>

string inputFileName = "scene.txt";
int windowWidth = 500;
int windowHeight = 500;
double fovY = 80.0;
double moveAmount = 5.0;

void drawAxes(){
	if(drawAxis==1){
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


void drawGrids(){
	int i;
	if(drawGrid==1){
		glColor3f(0.6, 0.6, 0.6);
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){
				if(i==0)
					continue;

				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}



void capture(){
	cout <<"capturing image\n";
	bitmap_image image(pixelsAlongBothAxis,pixelsAlongBothAxis);
	
	for(int i=0;i<pixelsAlongBothAxis;i++){
		for(int j=0;j<pixelsAlongBothAxis;j++){
			image.set_pixel(i,j, 0, 0, 0);
		}
	}

	double planeDistance = (windowHeight/2.0) / tan(deg2rad(fovY)/2.0);
	
	Vector3D topLeft = position + look * planeDistance - right1 * (windowWidth/2.0) + up * (windowHeight/2.0);
	
	double du = windowWidth / (pixelsAlongBothAxis * 1.0);
	double dv = windowHeight / (pixelsAlongBothAxis * 1.0);

	topLeft = topLeft + right1 * (du*0.5) - up * (dv*0.5);

	for (int i=0;i<pixelsAlongBothAxis;i++){

		for(int j=0;j<pixelsAlongBothAxis;j++){

			Vector3D curPixel = topLeft + right1 * (i*du) - up * (j*dv);
			Ray ray(position, curPixel - position);
			Color color = Color(0,0,0);
			int nearest = -1;
			double t, tMin = INF;
			for(int k=0;k<objects.size();k++){
				t = objects[k]->intersect(ray, color, 0);
				if(t>0.0 && t<tMin){
					tMin = t;
					nearest = k;
				}
			}
			if(nearest != -1){
				Color color;
				tMin = objects[nearest]->intersect(ray, color, 1);
				image.set_pixel(i,j,(int) round(color.r * 255.0),(int) round(color.g * 255.0), (int)round(color.b * 255.0));
			}
		}
	}

	stringstream stream;
    stream <<  numberOfCapturedImages++;

	string imageName = "Output_" + stream.str() + ".bmp";
	image.save_image("Output/" + imageName);

	cout<<"saving img: "<<imageName<<"\n";
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){
		case '0':
			capture();
			break;

		case '1':
			right1 = Vector3D::rotateVector(right1, up, deg2rad(moveAmount));
			look = Vector3D::rotateVector(look, up, deg2rad(moveAmount));
			
			break;
		case '2':
			right1 = Vector3D::rotateVector(right1, up, deg2rad(-moveAmount));
			look = Vector3D::rotateVector(look, up, deg2rad(-moveAmount));
			
			break;
		case '3':
			look = Vector3D::rotateVector(look, right1, deg2rad(-moveAmount));
			up = Vector3D::rotateVector(up, right1, deg2rad(-moveAmount));
			 
			break;
		case '4':
			look = Vector3D::rotateVector(look, right1, deg2rad(moveAmount));
			up = Vector3D::rotateVector(up, right1, deg2rad(moveAmount));
			 
			break;
		case '5':
			right1 = Vector3D::rotateVector(right1, look, deg2rad(moveAmount));
			up = Vector3D::rotateVector(up, look, deg2rad(moveAmount));
			
			break;
		case '6':
			right1 = Vector3D::rotateVector(right1, look, deg2rad(-moveAmount));
			up = Vector3D::rotateVector(up, look, deg2rad(-moveAmount));

			break;
		case 't':
			drawTexture = 1 - drawTexture;

			break;
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:
			cameraHeight -= 3.0;
			position = position - (look * moveAmount);
			break;
		case GLUT_KEY_UP:
			cameraHeight += 3.0;
			position = position + (look * moveAmount);
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
			position = position - (right1 * moveAmount);
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			position = position + (right1 * moveAmount);
			break;

		case GLUT_KEY_PAGE_UP:
			position = position + (up * moveAmount);
			break;
		case GLUT_KEY_PAGE_DOWN:
			position = position - (up * moveAmount);
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


void mouseListener(int button, int state, int x, int y){	
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){
				drawAxis=1-drawAxis;
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


void loadTexture(const char* filename) {
    cout << "Loading texture: " << filename << endl;
    bitmap_image texture(filename);

    if (!texture) {
        cerr << "Error: Could not open texture file " << filename << endl;
        return;
    }

    textureWidth = texture.width();
    textureHeight = texture.height();
    textureChannels = texture.bytes_per_pixel();
    
    textureData = texture.data();

    static bitmap_image persistentTexture(filename);
    if (!persistentTexture) {
         cerr << "Error: Could not create persistent texture " << filename << endl;
         return;
    }
    textureData = persistentTexture.data();
    textureWidth = persistentTexture.width();
    textureHeight = persistentTexture.height();
    textureChannels = persistentTexture.bytes_per_pixel();

    cout << "Texture loaded successfully: " 
         << textureWidth << "x" << textureHeight 
         << " Channels: " << textureChannels << endl;
}

void loadData(){
	ifstream inputFile(inputFileName);

	inputFile >> levelsOfRecursion >> pixelsAlongBothAxis;
	inputFile >> numberOfObjects;

	for(int i=0;i<numberOfObjects;i++){
		string objectType;
		inputFile >> objectType;
		if(objectType == "sphere"){
			Sphere *s = new Sphere();
			inputFile >> (*s);
			objects.push_back(s);
		}
		else if(objectType == "triangle"){
			Triangle *t = new Triangle();
			inputFile >> *t;
			objects.push_back(t);
		}
		else if(objectType == "general"){
			General *g = new General();
			inputFile >> *g;
			objects.push_back(g);
		}
		else{
			cout<<"Invalid Object Type : "<< objectType<<endl;
		}
	}
	
	inputFile >> numberOfPointLights;
	for(int i=0;i<numberOfPointLights;i++){
		PointLight *pl = new PointLight();
		inputFile >> *pl;
		lights.push_back(pl);
	}

	inputFile >> numberOfSpotLights;
	for(int i=0;i<numberOfSpotLights;i++){
		SpotLight *sl = new SpotLight();
		inputFile >> *sl;
		lights.push_back(sl);
	}

	inputFile.close();

	Object *floor = new Floor(1000, 20);
	floor->setColor(Color(1,1,1));
	floor->setShine(10);
	floor->setcoEfficients(0.4, 0.2, 0.2, 0.2);
	objects.push_back(floor);

	loadTexture("texture3.bmp");
}

void showData(){
	cout << "Levels of Recursion: " << levelsOfRecursion << endl;
	cout << "Pixels along both axis: " << pixelsAlongBothAxis << endl;
	cout << "Number of Objects: " << numberOfObjects << endl;

	for(int i=0;i<objects.size();i++){
		cout << "Object " << i+1 << ": " << endl;
		cout << *objects[i] << endl;
	}

	cout << "Number of Point Lights: " << numberOfPointLights << endl;
	for(int i=0;i<numberOfPointLights;i++){
		cout << "Point Light " << i+1 << ": " << endl;
		cout << *lights[i] << endl;
	}

	cout << "Number of Spot Lights: " << numberOfSpotLights << endl;
	for(int i=numberOfPointLights;i<numberOfPointLights+numberOfSpotLights;i++){
		cout << "Spot Light " << i+1 << ": " << endl;
		cout << *lights[i] << endl;
	}
}


void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(position.x,position.y,position.z, position.x+look.x,position.y+look.y,position.z+look.z, up.x,up.y,up.z);

	glMatrixMode(GL_MODELVIEW);

	drawAxes();
	drawGrids();

	for(int i=0;i<objects.size();i++){
		objects[i]->draw();
	}
	
	for(int i=0;i<lights.size();i++){
		lights[i]->draw();
	}
	glutSwapBuffers();
	
}


void animate(){
	angle += 0.05;
	glutPostRedisplay();	
}

void init(){
	drawGrid=0;
	drawAxis=0;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;
	drawTexture = 0;

	double root2 = 1.0/sqrt(2);

	position = Vector3D(100,100,50);
	up = Vector3D(0,0,1);
	right1 = Vector3D(-root2,root2,0);
	look = Vector3D(-root2,-root2,0);

	glClearColor(0,0,0,0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovY,	1,	1,	1000.0);
}

int main(int argc, char **argv){
	srand(time(NULL));
	glutInit(&argc,argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);

	glutCreateWindow("Ray Tracing");

	init();
	loadData();
	// showData();

	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(display);
	glutIdleFunc(animate);

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();
	
	vector<Object*>().swap(objects);
	vector<PointLight*>().swap(lights);
	
	return 0;
}
