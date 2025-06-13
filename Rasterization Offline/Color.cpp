#include<iostream>
#include<stdlib.h>
#include<math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include<GL/glut.h>
#endif

using namespace std;

class Color{
	GLint red;
	GLint green;
	GLint blue;

	public:

	Color(){
		this->red = 0.0;
		this->green = 0.0;
		this->blue = 0.0; 
	}

	GLint getRed() const { 
		return red; 
	}

	void setRed(GLint red) { 
		this->red = red; 
	}

	GLint getGreen() const { 
		return green; 
	}

	void setGreen(GLint green) { 
		this->green = green; 
	}

	GLint getBlue() const { 
		return blue; 
	}

	void setBlue(GLint blue) { 
		this->blue = blue; 
	}
	
};

