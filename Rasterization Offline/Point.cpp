#include<iostream>
#include<stdlib.h>
#include<math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include<GL/glut.h>
#endif



using namespace std;

class Point{
	
	GLdouble x,y,z;
	
	public:
	
	Point(){
		this->x=0.0;
		this->y=0.0;
		this->z=0.0;
	}

	Point(GLdouble x,GLdouble y, GLdouble z){
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Point(const Point &ob){
		this->x = ob.x;
		this->y = ob.y;
		this->z = ob.z;
	}

	void setX(GLdouble x){
		this->x = x;
	}

	void setY(GLdouble y){
		this->y = y;
	}

	void setZ(GLdouble z){
		this->z = z;
	}

	GLdouble getX(){
		return x;
	}

	GLdouble getY(){
		return y;
	}

	GLdouble getZ(){
		return z;
	}

	Point operator+(const Point &ob){
		Point result;
		result.x = this->x + ob.x;
		result.y = this->y + ob.y;
		result.z = this->z + ob.z;
		return result;
	}

	Point operator-(const Point &ob){
		Point result;
		result.x = this->x - ob.x;
		result.y = this->y - ob.y;
		result.z = this->z - ob.z;
		return result;
	}

	Point operator*(const GLdouble &m){
		Point result;
		result.x = this->x * m;
		result.y = this->y * m;
		result.z = this->z * m;
		return result;
	}

	Point operator*(const Point &ob){
		Point result;
		result.x = this->y * ob.z - this->z * ob.y;
        result.y = this->z * ob.x - this->x * ob.z;
        result.z = this->x * ob.y - this->y * ob.x;
		return result;
	}

	Point operator/(const GLdouble &m){
		Point result;
		result.x = this->x / m;
		result.y = this->y / m;
		result.z = this->z / m;
		return result;
	}

	Point operator=(const Point &ob){
		this->x = ob.x;
		this->y = ob.y;
		this->z = ob.z;
		return *this;
	}


	GLdouble dotProduct(const Point & ob){
		GLdouble result = this->x * ob.x + this->y * ob.y + this->z * ob.z;
		return result;
	}


	GLdouble magnitude(){
		return sqrt(x*x + y*y + z*z);
	}



	Point normalize(){
		Point result;
		GLdouble length = magnitude();
		result.x = this->x / length; 
		result.y = this->y / length;
		result.z = this->z / length;
		return result;
	}

};
