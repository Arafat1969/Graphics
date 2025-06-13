#include<iostream>
#include<stdlib.h>
#include<algorithm>
#include<math.h>
#include<fstream>
#include<sstream>
#include<string>
#include<stack>
#include<iomanip>
#include<vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include<GL/glut.h>
#endif

#include "Matrix.cpp"
#include "bitmap_image.hpp"

using namespace std;

class Rasterization {
public:
    Point eye, look, up;
    GLdouble fovY, aspectRatio, near, far;
    GLint screenWidth, screenHeight;
    GLdouble leftLimit, rightLimit, bottomLimit, topLimit, frontLimit, rearLimit;
    GLdouble dx, dy, leftX, topY, zMax;
    ifstream infile1, infile2;
    ofstream outfile;
    Triangle triangle;
    static unsigned long int g_seed;

    Rasterization() {
        cout << "hiii" << endl;
    }

    void printTriangle() {
        outfile << fixed << setprecision(7);
        outfile << triangle.getPoint(0).getX() << " " << triangle.getPoint(0).getY() << " " << triangle.getPoint(0).getZ() << endl;
        outfile << triangle.getPoint(1).getX() << " " << triangle.getPoint(1).getY() << " " << triangle.getPoint(1).getZ() << endl;
        outfile << triangle.getPoint(2).getX() << " " << triangle.getPoint(2).getY() << " " << triangle.getPoint(2).getZ() << endl;
        outfile << endl;
    }

    void setTrainglePoints() {
        GLdouble x, y, z;
        infile1 >> x >> y >> z;
        triangle.setPoint(Point(x, y, z), 0);

        infile1 >> x >> y >> z;
        triangle.setPoint(Point(x, y, z), 1);

        infile1 >> x >> y >> z;
        triangle.setPoint(Point(x, y, z), 2);
    }

    void setGluLookAtParameters() {
        GLdouble x, y, z;

        infile1 >> x >> y >> z;
        eye.setX(x);
        eye.setY(y);
        eye.setZ(z);

        infile1 >> x >> y >> z;
        look.setX(x);
        look.setY(y);
        look.setZ(z);

        infile1 >> x >> y >> z;
        up.setX(x);
        up.setY(y);
        up.setZ(z);
    }

    void stage1() {
        infile1.open("scene.txt");
        outfile.open("stage1.txt");

        setGluLookAtParameters();

        infile1 >> fovY >> aspectRatio >> near >> far;

        stack<Matrix> stack;
        Matrix I(4, 4);
        stack.push(I.identity());

        string command;

        while (infile1 >> command) {
            if (command == "triangle") {

                setTrainglePoints();

                triangle.setPoint(stack.top() * triangle.getPoint(0), 0);
                triangle.setPoint(stack.top() * triangle.getPoint(1), 1);
                triangle.setPoint(stack.top() * triangle.getPoint(2), 2);

                printTriangle();

            } else if (command == "translate") {

                GLdouble tx, ty, tz;
                infile1 >> tx >> ty >> tz;
                Matrix matrix(4, 4);
                matrix.translationMatrix(tx, ty, tz);
                stack.top() = stack.top() * matrix;

            } else if (command == "scale") {

                GLdouble sx, sy, sz;
                infile1 >> sx >> sy >> sz;
                Matrix matrix(4, 4);
                matrix.scalingMatrix(sx, sy, sz);
                stack.top() = stack.top() * matrix;

            } else if (command == "rotate") {

                GLdouble angle;
                GLdouble rx, ry, rz;
                infile1 >> angle >> rx >> ry >> rz;
                Point p(rx, ry, rz);
                Matrix matrix(4, 4);
                matrix.rotationMatrix(angle, p);
                stack.top() = stack.top() * matrix;

            } else if (command == "push") {

                stack.push(stack.top());

            } else if (command == "pop") {

                stack.pop();

            } else if (command == "end") {
                break;
            }

        }
        infile1.close();
        outfile.close();
    }

    void stage2() {
        infile1.open("stage1.txt");
        outfile.open("stage2.txt");

        Point lookVector, rightVector, upVector;
        lookVector = look - eye;
        lookVector = lookVector.normalize();
        rightVector = lookVector * up;
        rightVector = rightVector.normalize();
        upVector = rightVector * lookVector;

        Matrix translationMatrix(4, 4);
        translationMatrix.translationMatrix(-eye.getX(), -eye.getY(), -eye.getZ());

        Matrix rotationMatrix(4, 4);
        rotationMatrix.rotationMatrix(rightVector, upVector, lookVector);

        Matrix viewMatrix = rotationMatrix * translationMatrix;

        while (true) {
            setTrainglePoints();

            if (infile1.eof()) {
                break;
            }

            triangle.setPoint(viewMatrix * triangle.getPoint(0), 0);
            triangle.setPoint(viewMatrix * triangle.getPoint(1), 1);
            triangle.setPoint(viewMatrix * triangle.getPoint(2), 2);

            printTriangle();
        }

        infile1.close();
        outfile.close();
    }

    void stage3() {
        infile1.open("stage2.txt");
        outfile.open("stage3.txt");

        GLdouble fovX = fovY * aspectRatio;
        GLdouble top = near * tan((fovY / 2) * (M_PI / 180));
        GLdouble right = near * tan((fovX / 2) * (M_PI / 180));

        Matrix projectionMatrix(4, 4);
        projectionMatrix.projectionMatrix(top, right, near, far);

        while (true) {
            setTrainglePoints();

            if (infile1.eof()) {
                break;
            }

            triangle.setPoint(projectionMatrix * triangle.getPoint(0), 0);
            triangle.setPoint(projectionMatrix * triangle.getPoint(1), 1);
            triangle.setPoint(projectionMatrix * triangle.getPoint(2), 2);

            printTriangle();
        }

        infile1.close();
        outfile.close();
    }

    inline int Random() {
        g_seed = (214013 * g_seed + 2531011);
        return (g_seed >> 16) & 0x7FFF;
    }

    GLdouble getZ(Point temp, GLdouble D, GLdouble x, GLdouble y) {
        GLdouble z = -(temp.getX() * x + temp.getY() * y + D) / temp.getZ();
        return z;
    }

    GLdouble getArea(Point a, Point b, Point c) {
        Point result = (b - a) * (c - a);
        GLdouble area = result.magnitude() * 0.5;
        return area;
    }

    bool isInside(Point p, Triangle tri) {
        Point a = tri.getPoint(0);
        Point b = tri.getPoint(1);
        Point c = tri.getPoint(2);

        if (getArea(p, a, b) + getArea(p, b, c) + getArea(p, c, a) - getArea(a, b, c) < 0.0006) {
            return true;
        }
        return false;
    }

    void stage4() {
        infile1.open("stage3.txt");
        infile2.open("config.txt");
        outfile.open("z_buffer.txt");

        infile2 >> screenWidth >> screenHeight;
        infile2 >> leftLimit;
        rightLimit = -leftLimit;

        infile2 >> bottomLimit;
        topLimit = -bottomLimit;

        infile2 >> frontLimit >> rearLimit;
        zMax = rearLimit;

        vector<Triangle> triangles;

        while (true) {
            setTrainglePoints();

            if (infile1.eof()) {
                break;
            }

            GLdouble red, green, blue;

            red = Random();
            green = Random();
            blue = Random();

            Color color;
            color.setRed(red);
            color.setGreen(green);
            color.setBlue(blue);

            triangle.setColor(color);

            triangles.push_back(triangle);
        }

        dx = (rightLimit - leftLimit) / screenWidth;
        dy = (topLimit - bottomLimit) / screenHeight;

        topY = topLimit - dy / 2.0;
        leftX = leftLimit + dx / 2.0;

        vector<vector<GLdouble>> zBuffer(screenHeight, vector<GLdouble>(screenWidth, zMax));

        bitmap_image image(screenWidth, screenHeight);
        image.set_all_channels(0, 0, 0);

        for (Triangle tri : triangles) {

            GLdouble minY = max(bottomLimit, tri.getMinY());
            GLdouble maxY = min(topLimit, tri.getMaxY());
            GLdouble minX = max(leftLimit, tri.getMinX());
            GLdouble maxX = min(rightLimit, tri.getMaxX());

            Point temp1 = tri.getPoint(0) - tri.getPoint(1);
            Point temp2 = tri.getPoint(0) - tri.getPoint(2);
            Point temp3 = temp1 * temp2;

            GLdouble D = -tri.getPoint(0).dotProduct(temp3);

            for (GLdouble i = maxY; i >= minY ; i -= dy) {
                for (GLdouble j = minX ; j < maxX; j += dx) {
                    GLdouble z = getZ(temp3, D, j, i);

                    if (isInside(Point(j, i, z), tri)) {

                        int x = round((j - leftX) / dx);
                        int y = round((topY - i) / dy);
						if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
							if (z < zBuffer[y][x] && z > frontLimit && z < rearLimit) {

								zBuffer[y][x] = z;
								image.set_pixel(x, y, tri.getColor().getRed(),
													tri.getColor().getGreen(),
													tri.getColor().getBlue());
							}
						}
                    }
                }
            }
        }

		image.save_image("out.bmp");

		for(int i = 0 ; i <screenHeight ; i++){
			for(int j = 0 ; j < screenWidth ; j++){
				if(zBuffer[i][j] < zMax){
					outfile << fixed << setprecision(6);
					outfile << zBuffer[i][j] << "\t";
				}
			
			}
			outfile << endl;
		}

        infile1.close();
		infile2.close();
        outfile.close();
		zBuffer.clear();
    	zBuffer.shrink_to_fit();
    }

};

unsigned long int Rasterization::g_seed = 1;

int main() {
    Rasterization raster;
    raster.stage1();
    raster.stage2();
    raster.stage3();
    raster.stage4();

    return 0;
}
