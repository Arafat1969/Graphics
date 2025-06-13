#include<iostream>
#include<stdlib.h>
#include<math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include<GL/glut.h>
#endif

#include "Color.cpp"
#include "Point.cpp"

using namespace std;


class Triangle{
	Point points[3]; 
	Color color;

	public :

	void setPoint(Point a , int index){
		this->points[index] = a;
	}

	Point getPoint(int index){
		return points[index];
	}

	void setColor(Color color){
		this->color = color;
	}

	Color getColor(){
		return color;
	}


	GLdouble getMaxX(){
		GLdouble maxValue = points[0].getX();
		for (int i = 1; i < 3; i++) {
			if (points[i].getX() > maxValue) {
				maxValue = points[i].getX();
			}
		}
		return maxValue;
	}

	GLdouble getMinX(){
		GLdouble minValue = points[0].getX();
		for (int i = 1; i < 3; i++) {
			if (points[i].getX() < minValue) {
				minValue = points[i].getX();
			}
		}
		return minValue;
	}

	GLdouble getMaxY(){
		GLdouble maxValue = points[0].getY();
		for (int i = 1; i < 3; i++) {
			if (points[i].getY() > maxValue) {
				maxValue = points[i].getY();
			}
		}
		return maxValue;
	}

	GLdouble getMinY(){
		GLdouble minValue = points[0].getY();
		for (int i = 1; i < 3; i++) {
			if (points[i].getY() < minValue) {
				minValue = points[i].getY();
			}
		}
		return minValue;
	}

	GLdouble getMaxZ(){
		GLdouble maxValue = points[0].getZ();
		for (int i = 1; i < 3; i++) {
			if (points[i].getZ() > maxValue) {
				maxValue = points[i].getZ();
			}
		}
		return maxValue;
	}

	GLdouble getMinZ(){
		GLdouble minValue = points[0].getZ();
		for (int i = 1; i < 3; i++) {
			if (points[i].getZ() < minValue) {
				minValue = points[i].getZ();
			}
		}
		return minValue;
	}


};