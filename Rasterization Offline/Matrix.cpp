#include<iostream>
#include<stdlib.h>
#include<math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include<GL/glut.h>
#endif

#include "Triangle.cpp"

using namespace std;

class Matrix{
	GLint row, column;
	GLdouble** matrix;

	public :

	Matrix(GLint row,GLint column){
		this->row = row;
		this->column = column;
		matrix=new GLdouble*[row];
        for(int i=0;i<row;i++)
        {
            matrix[i]=new GLdouble[column];
        }
	}

	Matrix identity(){
		for(int i =0 ; i < this->row ; i++){
			for(int j=0 ; j < this->column ; j++){
				if( i == j){
					this->matrix[i][j] = 1.0;
				}else{
					this->matrix[i][j] = 0.0;
				}
			}
		}

		return *this;
	}

	Matrix translationMatrix(GLdouble tx, GLdouble ty, GLdouble tz){
		identity();

		matrix[0][3] = tx;
		matrix[1][3] = ty;
		matrix[2][3] = tz;
		
		return *this;
	}

	Matrix scalingMatrix(GLdouble sx, GLdouble sy, GLdouble sz){
		identity();

		matrix[0][0] = sx;
		matrix[1][1] = sy;
		matrix[2][2] = sz;
		
		return *this;
	}

	Point rodriguesFormula(Point rotationAxis , Point a , GLdouble radian){
        Point result ;
        result = rotationAxis * cos(radian) + a * (a.dotProduct(rotationAxis)) * (1-cos(radian)) + (a * rotationAxis) * sin(radian);
        return result ;
    }

    Matrix rotationMatrix(double angle , Point a){
        double radian = angle * M_PI / 180;
        a.normalize();
  		identity();

        Point c1 = rodriguesFormula(Point(1 , 0 , 0) , a , radian);
        Point c2 = rodriguesFormula(Point(0 , 1 , 0) , a , radian);
        Point c3 = rodriguesFormula(Point(0 , 0 , 1) , a , radian);

		matrix[0][0] = c1.getX();
		matrix[1][0] = c1.getY();
		matrix[2][0] = c1.getZ();
		matrix[0][1] = c2.getX();
		matrix[1][1] = c2.getY();
		matrix[2][1] = c2.getZ();
		matrix[0][2] = c3.getX();
		matrix[1][2] = c3.getY();
		matrix[2][2] = c3.getZ();

        return *this;
    }

	Matrix rotationMatrix(Point right , Point up , Point look){
		identity();

		matrix[0][0] = right.getX();
		matrix[0][1] = right.getY();
		matrix[0][2] = right.getZ();
		matrix[1][0] = up.getX();
		matrix[1][1] = up.getY();
		matrix[1][2] = up.getZ();
		matrix[2][0] = -look.getX();
		matrix[2][1] = -look.getY();
		matrix[2][2] = -look.getZ();

		return *this;
	}

	Matrix projectionMatrix(GLdouble top , GLdouble right , GLdouble near , GLdouble far){
		identity();
		
		matrix[0][0] = near / right;
		matrix[1][1] = near / top;
		matrix[2][2] = -(far + near) / (far - near);
		matrix[2][3] = -(2.0 * far * near) / (far - near);
		matrix[3][2] = -1.0;
		matrix[3][3] = 0.0;

		return *this;
	}

	Matrix operator*(const Matrix &mat){
		Matrix result(this->row , mat.column);
		for(int i = 0 ; i < this->row ; i++){
            for(int j = 0 ; j < this->column ; j++){
                result.matrix[i][j] = 0.0;
                for(int k = 0 ; k < this->column ; k++){
                    result.matrix[i][j] += this->matrix[i][k] * mat.matrix[k][j];
                }
            }
        }

        return result;
	}


	Point operator*(Point p){
        GLdouble output[4] = {0.0};
        GLdouble temp[4] = {p.getX() , p.getY() , p.getZ() , 1};

        for(int i = 0 ; i < 4 ; i++){
            for(int j = 0 ; j < 4 ; j++){
                output[i] += matrix[i][j] * temp[j];
            }
        }

        output[0] /= output[3];
        output[1] /= output[3];
        output[2] /= output[3];
        return Point(output[0],output[1],output[2]);
    }

	

};