#include<iostream>
#include<stdlib.h>
#include<math.h>
#include<vector>
#include "bitmap_image.hpp"


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include<GL/glut.h>
#endif


using namespace std;

#define INF 999999999
#define AMBIENT 0
#define DIFFUSE 1
#define SPECULAR 2
#define REFLECTION 3


double cameraHeight;
double cameraAngle;
int drawGrid;
int drawAxis;
double angle;
int drawTexture;

int levelsOfRecursion = 0;
int pixelsAlongBothAxis = 0;
int numberOfObjects = 0;
int numberOfPointLights = 0;
int numberOfSpotLights = 0;
int numberOfCapturedImages = 11;


unsigned char* textureData = nullptr;
int textureWidth = 0;
int textureHeight = 0;
int textureChannels = 0;



bool verbose = false;

double deg2rad(double deg){
    return deg * M_PI / 180.0;
}

double rad2deg(double rad){
    return (rad * 180.0) / M_PI;
}

class Vector3D{


	public:
		double x, y, z;
		Vector3D(){
			x = 0.0;
			y = 0.0;
			z = 0.0;
		}

		Vector3D(double x, double y, double z){
			this->x = x;
			this->y = y;
			this->z = z;
		}

		Vector3D(double x, double y){
			this->x = x;
			this->y = y;
			this->z = 0.0;
		}

		Vector3D operator+(const Vector3D &p){
			Vector3D p1;
			p1.x = x + p.x;
			p1.y = y + p.y;
			p1.z = z + p.z;
			return p1;
		}


		Vector3D operator-(const Vector3D &p){
			Vector3D p1;
			p1.x = x - p.x;
			p1.y = y - p.y;
			p1.z = z - p.z;
			return p1;
		}


		Vector3D operator*(const double &d){
			Vector3D p1;
			p1.x = x * d * 1.0;
			p1.y = y * d * 1.0;
			p1.z = z * d * 1.0;
			return p1;
		}


		Vector3D operator/(const double &d){
			Vector3D p1;
			p1.x = x / d * 1.0;
			p1.y = y / d * 1.0;
			p1.z = z / d * 1.0;
			return p1;
		}


		Vector3D operator=(const Vector3D &p){
			x = p.x;
			y = p.y;
			z = p.z;
			return *this;
		}


		Vector3D normalize(){
			double norm = sqrt(x*x + y*y + z*z) * 1.0;
			x /= norm;
			y /= norm;
			z /= norm;
			return *this;
		}


		Vector3D operator-(){
			x = -x;
			y = -y;
			z = -z;
			return *this;
		}


		double distance(Vector3D p){
			return sqrt((x-p.x)*(x-p.x) *1.0 + (y-p.y)*(y-p.y)*1.0 + (z-p.z)*(z-p.z)*1.0);
		}


		double dot(const Vector3D &p){
			return (x*p.x + y*p.y + z*p.z) * 1.0;
		}


		Vector3D cross(const Vector3D &p){
			Vector3D p1;
			p1.x = y*p.z - z*p.y;
			p1.y = z*p.x - x*p.z;
			p1.z = x*p.y - y*p.x;
			return p1;
		}

		double length(){
			return sqrt(x*x + y*y + z*z);
		}


		friend ostream &operator<<( ostream &output, const Vector3D &p ) { 
			output<< p.x << " " << p.y << " " << p.z << endl;
			return output;            
		}


		friend istream &operator>>( istream  &input, Vector3D &p ) { 
			input >> p.x >> p.y >> p.z;
			return input;            
		}

		static Vector3D rotateVector(Vector3D p, Vector3D ax, double angle){
			Vector3D r;
			Vector3D c = ax.cross(p);
			r = p * cos(angle) + c * sin(angle);
			return r;
		}

		static Vector3D RodriguesFormula(Vector3D x, Vector3D a, double angle){
			angle = deg2rad(angle);
			Vector3D p1 = x * cos(angle);
			Vector3D p2 = a * a.dot(x) * (1 - cos(angle));
			Vector3D p3 = x.cross(a) * sin(angle);
			return p1 + p2 + p3;
		}

		void print(){
			cout<<"( x: "<<x<<", y: "<<y<<", z: "<<z<<")"<<endl;
		}


		~Vector3D(){
			x = 0.0;
			y = 0.0;
			z = 0.0;
		}
};


Vector3D position, up, right1, look;




class Color{
    public:
        double r, g, b; 

        Color(int r, int g, int b){
            this->r = r;
            this->g = g;
            this->b = b;
        }
		
        Color(){
            r = 0.0;
            g = 0.0;
            b = 0.0;
        }

        Color clip(){
            if(r > 1.0) r = 1.0;
            if(g > 1.0) g = 1.0;
            if(b > 1.0) b = 1.0;
            if(r < 0.0) r = 0.0;
            if(g < 0.0) g = 0.0;
            if(b < 0.0) b = 0.0;
            return *this;
        }
        
        Color operator+(const Color &c){
            Color c1;
            c1.r = r + c.r;
            c1.g = g + c.g;
            c1.b = b + c.b;
            return c1;
        }

        Color operator=(const Color &c){
            r = c.r;
            g = c.g;
            b = c.b;
            return *this;
        }

        Color operator*(const double &d){
            Color c;
            c.r = r * d * 1.0;
            c.g = g * d * 1.0;
            c.b = b * d * 1.0;
            return c;
        }

        Color operator*(const Color &c){
            Color c1;
            c1.r = r * c.r * 1.0;
            c1.g = g * c.g * 1.0;
            c1.b = b * c.b * 1.0;
            return c1;
        }
        
        friend istream &operator>>(istream  &input, Color &c ) { 
            input >> c.r >> c.g >> c.b;
            return input;            
        }

        friend ostream& operator<<(ostream& os, const Color& c){
            os<<c.r<<" "<<c.g<<" "<<c.b<<endl;
            return os;
        }

        ~Color(){
            r = 0.0;
            g = 0.0;
            b = 0.0;
        }
};


Color sampleTexture(double u, double v) {
    if (!textureData || textureWidth <= 0 || textureHeight <= 0) {
        return Color(0.5, 0.5, 0.5);
    }

    u = std::max(0.0, std::min(1.0, u));
    v = std::max(0.0, std::min(1.0, v));

    int pixel_x = static_cast<int>(u * (textureWidth - 1));
    int pixel_y = static_cast<int>((1.0 - v) * (textureHeight - 1));

    pixel_x = std::max(0, std::min(textureWidth - 1, pixel_x));
    pixel_y = std::max(0, std::min(textureHeight - 1, pixel_y));

    int index = (pixel_y * textureWidth + pixel_x) * textureChannels;

    Color color;
    color.b = static_cast<double>(textureData[index]) / 255.0;
    
    if (textureChannels >= 3) {
        color.g = static_cast<double>(textureData[index + 1]) / 255.0;
        color.r = static_cast<double>(textureData[index + 2]) / 255.0;
    } else {
        color.g = color.b;
        color.r = color.b;
    }
    
    return color;
}

class PointLight{
	public:
		Vector3D light_pos;
 		Color color;

		PointLight(Vector3D light_pos, Color color){
			this->light_pos = light_pos;
			this->color = color;
		}

		PointLight(){
			light_pos = Vector3D();
			color = Color();
		}

		
		virtual Color getColor(Vector3D intersectionPoint){
			return color;
		}

		virtual void draw(){
			glPushMatrix();
			glTranslatef(light_pos.x, light_pos.y, light_pos.z);
			glColor3f(color.r, color.g, color.b);
			glutSolidSphere(1, 10, 10);
			glPopMatrix();
		}

		
		friend ostream& operator<<(ostream &out, const PointLight &p){
			out<<"PointLight light_pos: ";
			out<<p.light_pos<<endl;
			out<<"Colors: ";
			out<<p.color<<endl;
			return out;
		}

        
		friend istream& operator>>(istream &in, PointLight &p){
			in>>p.light_pos;
			in>>p.color;
			return in;
		}

		
		~PointLight(){

		}
};



class SpotLight: public PointLight{
	public:
		Vector3D light_direction; 
		double cutoff_angle; 

		SpotLight(Vector3D position, Color color, Vector3D light_direction, double cutoff_angle):PointLight(position, color){
			this->light_direction = light_direction;
			this->cutoff_angle = cutoff_angle;
		}

		SpotLight():PointLight(){
			light_direction = Vector3D();
			cutoff_angle = 0;
		}

		
		void drawSquare(double a){
			glBegin(GL_QUADS);{
				glVertex3f( a, a,0);
				glVertex3f( a,-a,0);
				glVertex3f(-a,-a,0);
				glVertex3f(-a, a,0);
			}glEnd();
		}

		
		void draw(){
			glPushMatrix();
			glTranslatef(light_pos.x, light_pos.y, light_pos.z);
			glColor3f(color.r, color.g, color.b);
			drawSquare(10);
			glPopMatrix();
		}

		
		Color getColor(Vector3D intersectionPoint){
			light_direction.normalize();
			Vector3D light_to_intersectPoint = intersectionPoint - light_pos;
			light_to_intersectPoint.normalize();
			double angle = acos(light_direction.dot(light_to_intersectPoint));
			angle = deg2rad(angle);

			if(angle > cutoff_angle) 
				return Color(0, 0, 0);
			
			Color spotLightColor = color * pow(cos(angle), 20);
			return spotLightColor;
		}

	
		friend ostream& operator<<(ostream &out, const SpotLight &s){
			out<<"SpotLight position: ";
			out<<s.light_pos<<endl;
			out<<"Colors: ";
			out<<s.color<<endl;
			out<<"Light direction: ";
			out<<s.light_direction<<endl;
			out<<"Cutoff angle: ";
			out<<s.cutoff_angle<<endl;
			return out;
		}

		
		friend istream& operator>>(istream &in, SpotLight &s){
			in>>s.light_pos;
			in>>s.color;
			in>>s.light_direction;
			in>>s.cutoff_angle;
			return in;
		}


		~SpotLight(){

		}
};


class Ray{
	public :
		Vector3D start;
 		Vector3D dir;

		Ray(){
            start = Vector3D();
            dir = Vector3D();
        }

        Ray(Vector3D start, Vector3D direction){
            this->start = start; 
            this->dir = direction.normalize(); 
        }

        ~Ray(){

        }
};




class Object{
	public:
		Vector3D reference_point; // should have x, y, z
		double height, width, length;
		Color color;
		double coEfficients[4] ;// ambient, diffuse, specular,
								// reflection coEfficients
		int shine ;// exponent term of specular component
		Object(){
			reference_point = Vector3D(0, 0, 0);
			height = 0;
			width = 0;
			length = 0;
			color = Color();
			coEfficients[0] = 0;
			coEfficients[1] = 0;
			coEfficients[2] = 0;
			coEfficients[3] = 0;
			shine = 0;
		}


		virtual void draw(){

		}

		void setColor(Color c){
			color = c;
		}

		void setShine(int s){
			shine = s ;
		}

 		void setcoEfficients(double ambient, double diffuse, double specular, double reflection_coefficient){
			coEfficients[0] = ambient;
			coEfficients[1] = diffuse;
			coEfficients[2] = specular;
			coEfficients[3] = reflection_coefficient;
		}

		virtual double intersect(Ray ray, Color &color, int level){
			return -1.0;
		}


		

		friend ostream& operator<<(ostream& os, const Object& o){
			os<<"reference_Vector3D: "<<o.reference_point<<endl;
			os<<"height width length:"<<endl;
			os<<o.height<<" "<<o.width<<" "<<o.length<<endl;
			os<<"color: "<<o.color<<endl;
			os<<"coEfficients: ";
			os<<o.coEfficients[0]<<" , "<<o.coEfficients[1]<<" , "<<o.coEfficients[2]<<" , "<<o.coEfficients[3]<<" "<<o.shine<<endl;
			return os;
		}


		~Object(){
			length = 0;
			width = 0;
			height = 0;
			shine = 0;
			coEfficients[0] = coEfficients[1] = coEfficients[2] = coEfficients[3] = 0;
		}
};



vector<Object*> objects;
vector<PointLight*> lights;


class Sphere : public Object{
	public:
		Sphere():Object(){

		}

		Sphere(Vector3D center,double radius){
			reference_point = center;
			length = radius;
		}


		void drawSphere(double radius,int slices,int stacks){
			Vector3D points[100][100];
			int i,j;
			double h,r;
			for(i=0;i<=stacks;i++)
			{
				h=radius*sin(((double)i/(double)stacks)*(M_PI/2));
				r=radius*cos(((double)i/(double)stacks)*(M_PI/2));
				for(j=0;j<=slices;j++)
				{
					points[i][j].x=r*cos(((double)j/(double)slices)*2*M_PI);
					points[i][j].y=r*sin(((double)j/(double)slices)*2*M_PI);
					points[i][j].z=h;
				}
			}
	
			for(i=0;i<stacks;i++){
				for(j=0;j<slices;j++)
				{
					glBegin(GL_QUADS);{
						glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
						glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
						glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
						glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);

						glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
						glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
						glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
						glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
					}glEnd();
				}
			}
		}

		void draw(){
			glPushMatrix();
			glTranslatef(reference_point.x, reference_point.y, reference_point.z);
			glColor3f(color.r, color.g, color.b);
			drawSphere(length, 70, 70);
			glPopMatrix();
		}

		void computeAmbientDiffuseSpecularComponent(Ray ray, Color &color, int level, double t_neg, Vector3D normal, Vector3D intersectPoint,Color intersectionPointColor){

			color = intersectionPointColor * coEfficients[AMBIENT];
			
			for(int i=0; i< lights.size();i++){
				Vector3D lightPosition = lights[i]->light_pos; 

				Color lightColor = lights[i]->getColor(intersectPoint);
				
				Vector3D lightDir = intersectPoint - lightPosition; 
				lightDir.normalize();

				double lightDistance = lightPosition.distance(intersectPoint);
				
				Ray lightRay(lightPosition, lightDir);
				
				
				bool shadow = false;
				double eps = 0.000001;

				double tMin = INF,t;

				for(int j=0; j<objects.size(); j++){
					Color dummyColor;
					t = objects[j]->intersect(lightRay, dummyColor, 0);
					if(t>0.0 && tMin>t){
						tMin = t;
					}
				}

				if(tMin < lightDistance-eps){
					shadow = true;
				}

				if(!shadow){
					double diffuse = normal.dot(-lightRay.dir) * 1.0;

					if(diffuse>0.0){
						color = color + intersectionPointColor * lightColor * coEfficients[DIFFUSE] * diffuse;
					}

					Vector3D refRayDir = lightRay.dir - normal * ((normal.dot(lightRay.dir)) * 2.0);
					double specular = refRayDir.dot(ray.dir);
					
					if(specular>0.0){
						specular = pow(specular, shine);
						color = color + lightColor * coEfficients[SPECULAR] * specular;
					}
				}
			}
		}
		

		void recursiveReflection(Ray ray, Color &color, int level, double t_neg, Vector3D normal, Vector3D intersectPoint){
			Vector3D reflectionDir = ray.dir - normal * (ray.dir.dot(normal) * 2.0);
			reflectionDir.normalize();

			Ray reflectionRay(intersectPoint+reflectionDir, reflectionDir);

			int nearestObject = -1;
			double tMin = INF, t;

			for(int i=0; i<objects.size(); i++){
				Color tempColor;
				t = objects[i]->intersect(reflectionRay, tempColor, 0);
				if(t>0.0 && t<tMin){
					tMin = t;
					nearestObject = i;
				}
			}

			Color reflectedColor;
			if(nearestObject != -1){
				tMin = objects[nearestObject]->intersect(reflectionRay, reflectedColor, level+1);
			}
			
			color = color + reflectedColor * coEfficients[REFLECTION];
			color.clip();
		}

		double intersect(Ray ray, Color &color, int level){
			double a, b, c;
			double tPos, tNeg;

			Vector3D dir = ray.dir;
			Vector3D origin = ray.start - reference_point;
			
			a = dir.dot(dir);
			b = origin.dot(dir) * 2.0;
			c = origin.dot(origin) - length*length;
			
			double d_sq = (b*b - 4.0*a*c);

			if(d_sq<0.0){
				tNeg = INF;
			}else{
				if(d_sq>0.0){
					tPos = (-b+sqrt(d_sq))/(2.0*a);
					tNeg = (-b-sqrt(d_sq))/(2.0*a);
					if(tNeg<0.0){
						tNeg = tPos;
					}
				}else{
					tNeg = - b / (2.0*a);
				}
			}

			if (level == 0){
				return tNeg;
			}

			Vector3D intersectPoint = ray.start + ray.dir * tNeg;

			Vector3D normal = intersectPoint - reference_point;
			normal.normalize();
			
			double distance = position.distance(reference_point); 

			if(distance < length){
				normal = - normal;
				normal.normalize(); 
				// cout<<" here in -normal\n";           
			}
			Color intersectionPointColor = this->color;

			computeAmbientDiffuseSpecularComponent(ray, color, level, tNeg, normal, intersectPoint, intersectionPointColor);
			
			
		
			if(level >= levelsOfRecursion){
				return tNeg;
			}

			
			recursiveReflection(ray, color, level, tNeg, normal, intersectPoint);
			
			return tNeg;
		}


		friend istream& operator>>(istream &in, Sphere &s){
			in>>s.reference_point;
			in>>s.length;
			in>>s.color;
			in>>s.coEfficients[0]>>s.coEfficients[1]>>s.coEfficients[2]>>s.coEfficients[3]>>s.shine;
			return in;
		}

		friend ostream& operator<<(ostream &out, const Sphere &s){
			out<<"Sphere:"<<endl;
			out<<s.reference_point<<s.length<<endl;
			out<<"Colors:"<<endl;
			out<<s.color<<endl;
			out<<"coEfficients:"<<endl;
			out<<s.coEfficients[0]<<" , "<<s.coEfficients[1]<<" , "<<s.coEfficients[2]<<" , "<<s.coEfficients[3]<<endl;
			out<<"Shine:"<<endl;
			out<<s.shine<<endl;
			return out;
		}

		~Sphere(){
			
		}
};


class Triangle : public Object{
	public :
		Vector3D points[3];
		Color color; 

		Triangle():Object(){
			points[0] = Vector3D();
			points[1] = Vector3D();
			points[2] = Vector3D();
			color = Color();
		}

		Triangle(Vector3D p1, Vector3D p2, Vector3D p3, Color c){
			points[0] = p1;
			points[1] = p2;
			points[2] = p3;
			color = c;
		}

		Triangle(Vector3D p1, Vector3D p2, Vector3D p3){
			points[0] = p1;
			points[1] = p2;
			points[2] = p3;
			color = Color();
		}

		void draw(){
			glColor3f(color.r, color.g, color.b);
			glBegin(GL_TRIANGLES);{
				glVertex3f(points[0].x, points[0].y, points[0].z);
				glVertex3f(points[1].x, points[1].y, points[1].z);
				glVertex3f(points[2].x, points[2].y, points[2].z);
			}glEnd();
		}

		void computeAmbientDiffuseSpecularComponent(Ray ray, Color &color, int level, double t_neg, Vector3D normal, Vector3D intersectPoint,Color intersectionPointColor){

			color = intersectionPointColor * coEfficients[AMBIENT];
			
			for(int i=0; i< lights.size();i++){
				Vector3D lightPosition = lights[i]->light_pos; 
				
				Color lightColor = lights[i]->getColor(intersectPoint); 
				
				Vector3D lightDir = intersectPoint - lightPosition;
				lightDir.normalize();

				double lightDistance = lightPosition.distance(intersectPoint);
				
				Ray lightRay(lightPosition, lightDir);
				
				bool shadow = false;
				double eps = 0.000001;

				double tMin = INF,t;
				
				for(int j=0; j<objects.size(); j++){
					Color dummyColor;
					t = objects[j]->intersect(lightRay, dummyColor, 0);
					if(t>0.0 && tMin>t){
						tMin = t;
					}
				}

				
				if(tMin < lightDistance-eps){
					shadow = true;
				}

				if(!shadow){
					double diffuse = normal.dot(-lightRay.dir) * 1.0;

					if(diffuse>0.0){
						color = color + intersectionPointColor * lightColor * coEfficients[DIFFUSE] * diffuse;
					}

					Vector3D refRayDir = lightRay.dir - normal * ((normal.dot(lightRay.dir)) * 2.0);
					double specular = refRayDir.dot(ray.dir);
					
					if(specular>0.0){
						specular = pow(specular, shine);
						
						color = color + lightColor * coEfficients[SPECULAR] * specular;
					}
				}
			}
		}
		

		void recursiveReflection(Ray ray, Color &color, int level, double t_neg, Vector3D normal, Vector3D intersectPoint){
			Vector3D reflectionDir = ray.dir - normal * (ray.dir.dot(normal) * 2.0);
			reflectionDir.normalize();

			Ray reflectionRay(intersectPoint+reflectionDir, reflectionDir);

			int nearestObject = -1;
			double tMin = INF, t;

			for(int i=0; i<objects.size(); i++){
				Color tempColor;
				t = objects[i]->intersect(reflectionRay, tempColor, 0);
				if(t>0.0 && t<tMin){
					tMin = t;
					nearestObject = i;
				}
			}

			Color reflectedColor;
			if(nearestObject != -1){
				tMin = objects[nearestObject]->intersect(reflectionRay, reflectedColor, level+1);
			}

			color = color + reflectedColor * coEfficients[REFLECTION];

			color.clip();
		}

		double intersect(Ray ray, Color &color, int level){
			double determinantA, determinantBeta, determinantGamma, determinantT;
			double beta, gamma, t;

			Vector3D a,b,c;
			a = points[0];
			b = points[1];
			c = points[2];


			determinantA = (a.x-b.x) * ( (a.y-c.y)*ray.dir.z - (a.z-c.z)*ray.dir.y);
			determinantA += (a.y-b.y) * ( (a.z-c.z)*ray.dir.x - (a.x-c.x)*ray.dir.z);
			determinantA += (a.z-b.z) * ( (a.x-c.x)*ray.dir.y - (a.y-c.y)*ray.dir.x);

			determinantBeta = (a.x-ray.start.x) * ( (a.y-c.y)*ray.dir.z - (a.z-c.z)*ray.dir.y);
			determinantBeta += (a.y-ray.start.y) * ( (a.z-c.z)*ray.dir.x - (a.x-c.x)*ray.dir.z);
			determinantBeta += (a.z-ray.start.z) * ( (a.x-c.x)*ray.dir.y - (a.y-c.y)*ray.dir.x);

			determinantGamma = (a.x-b.x) * ( (a.y-ray.start.y)*ray.dir.z - (a.z-ray.start.z)*ray.dir.y);
			determinantGamma += (a.y-b.y) * ( (a.z-ray.start.z)*ray.dir.x - (a.x-ray.start.x)*ray.dir.z);
			determinantGamma += (a.z-b.z) * ( (a.x-ray.start.x)*ray.dir.y - (a.y-ray.start.y)*ray.dir.x);

			determinantT = (a.x-b.x) * ( (a.y-c.y)*(a.z-ray.start.z) - (a.z-c.z)*(a.y-ray.start.y));
			determinantT += (a.y-b.y) * ( (a.z-c.z)*(a.x-ray.start.x) - (a.x-c.x)*(a.z-ray.start.z));
			determinantT += (a.z-b.z) * ( (a.x-c.x)*(a.y-ray.start.y) - (a.y-c.y)*(a.x-ray.start.x));

			if(determinantA == 0.0){
				t = INF;
			}
			else{
				beta = determinantBeta/determinantA;
				gamma = determinantGamma/determinantA;

				if(beta < 0 || gamma < 0 || beta+gamma > 1){
					t = INF;
				}
				else{
					t = determinantT/determinantA;
				}
			}

			if(level == 0){
				return t;
			}
			
			Vector3D interestionPoint = ray.start + ray.dir*t;
			Color interestionPointColor = this->color;

			Vector3D normal = (b-a).cross(c-a);
			normal.normalize(); 

			
			if(normal.dot(ray.dir) > 0){
				normal = -normal;
			}
			
			
			computeAmbientDiffuseSpecularComponent(ray, color, level, t, normal, interestionPoint, interestionPointColor);

			
			if(level >= levelsOfRecursion){
				return t;
			}
			
			recursiveReflection(ray, color, level, t, normal, interestionPoint);

			return t;
		}

		friend ostream& operator<<(ostream &out, const Triangle &t){
			out<<"Triangle: ";
			out<<t.points[0]<<" ; "<<t.points[1]<<" ; "<<t.points[2]<<endl;
			out<<"Colors: ";
			out<<t.color<<endl;
			out<<"Co-Efficients: ";
			out<<t.coEfficients[0]<<" , "<<t.coEfficients[1]<<" , "<<t.coEfficients[2]<<" , "<<t.coEfficients[3]<<endl;
			out<<"Shine: ";
			out<<t.shine<<endl;
			return out;
		}

		friend istream& operator>>(istream &in, Triangle &t){
			in>>t.points[0]>>t.points[1]>>t.points[2];
			in>>t.color;
			in>>t.coEfficients[0]>>t.coEfficients[1]>>t.coEfficients[2]>>t.coEfficients[3]>>t.shine;
			return in;
		}

		~Triangle(){

		} 
};


class Floor : public Object{
    public:
		double floorWidth, tileWidth;
		Color tileColors[2];
		
		Floor():Object(){
			floorWidth = 0;
			tileWidth = 0;
			tileColors[0] = Color(0, 0, 0);
			tileColors[1] = Color(1, 1, 1);
		}

		Floor(double floorWidth, double tileWidth){
			reference_point = Vector3D(-floorWidth/2.0, -floorWidth/2.0, 0);
			length = tileWidth;
			this->floorWidth = floorWidth;
			this->tileWidth = tileWidth;
			tileColors[0] = Color(0, 0, 0);
			tileColors[1] = Color(1, 1, 1);
		}


		void draw(){
			glPushMatrix();
			glTranslatef(reference_point.x, reference_point.y, reference_point.z); 
			for(int i=0; i<floorWidth/tileWidth; i++){
				for(int j=0; j<floorWidth/tileWidth; j++){
					if((i+j)%2 == 0){
						glColor3f(tileColors[0].r, tileColors[0].g, tileColors[0].b);
					}
					else{
						glColor3f(tileColors[1].r, tileColors[1].g, tileColors[1].b);
					}
					glBegin(GL_QUADS);{
						glVertex3f(i*tileWidth, j*tileWidth, 0);
						glVertex3f(i*tileWidth, (j+1)*tileWidth, 0);
						glVertex3f((i+1)*tileWidth, (j+1)*tileWidth, 0);
						glVertex3f((i+1)*tileWidth, j*tileWidth, 0);
					}
					glEnd();
				}
			}

			glPopMatrix();
		}

		void computeAmbientDiffuseSpecularComponent(Ray ray, Color &color, int level, double t_neg, Vector3D normal, Vector3D intersectPoint,Color intersectionPointColor){

			color = intersectionPointColor * coEfficients[AMBIENT];
			
			for(int i=0; i< lights.size();i++){
				Vector3D lightPosition = lights[i]->light_pos; 
				
				Color lightColor = lights[i]->getColor(intersectPoint); 
				
				Vector3D lightDir = intersectPoint - lightPosition;
				lightDir.normalize();

				
				double lightDistance = lightPosition.distance(intersectPoint);
				
				Ray lightRay(lightPosition, lightDir);
				
				bool shadow = false;
				double eps = 0.000001;

				double tMin = INF,t;
				
				for(int j=0; j<objects.size(); j++){
					Color dummyColor;
					t = objects[j]->intersect(lightRay, dummyColor, 0);
					if(t>0.0 && tMin>t){
						tMin = t;
					}
				}

				if(tMin < lightDistance-eps){
					shadow = true;
				}

				
				if(!shadow){
					double diffuse = normal.dot(-lightRay.dir) * 1.0;

					if(diffuse>0.0){
						color = color + intersectionPointColor * lightColor * coEfficients[DIFFUSE] * diffuse;
					}

					Vector3D refRayDir = lightRay.dir - normal * ((normal.dot(lightRay.dir)) * 2.0);
					double specular = refRayDir.dot(ray.dir);
					
					if(specular>0.0){
						specular = pow(specular, shine);
						color = color + lightColor * coEfficients[SPECULAR] * specular;
					}
				}
			}
		}
		

		void recursiveReflection(Ray ray, Color &color, int level, double t_neg, Vector3D normal, Vector3D intersectPoint){

			Vector3D reflectionDir = ray.dir - normal * (ray.dir.dot(normal) * 2.0);
			reflectionDir.normalize();

			Ray reflectionRay(intersectPoint+reflectionDir, reflectionDir);

			int nearestObject = -1;
			double tMin = INF, t;

			for(int i=0; i<objects.size(); i++){
				Color tempColor;
				t = objects[i]->intersect(reflectionRay, tempColor, 0);
				if(t>0.0 && t<tMin){
					tMin = t;
					nearestObject = i;
				}
			}

			Color reflectedColor;
			if(nearestObject != -1){
				tMin = objects[nearestObject]->intersect(reflectionRay, reflectedColor, level+1);
			}
			
			color = color + reflectedColor * coEfficients[REFLECTION];
			color.clip();
		}

	
		double intersect(Ray ray, Color &color, int level){
			Vector3D normal = Vector3D(0,0,1.0);

			if (position.dot(normal) < 0.0){
				normal = -normal;
			}

			double tMin = INF;

			if(ray.dir.dot(normal)!=0.0){
				tMin = - (normal.dot(ray.start)/ray.dir.dot(normal));
			}

			if(tMin < INF && tMin > 0.0){
				Vector3D intersectPoint = ray.start + ray.dir*tMin;
				if(intersectPoint.x >= reference_point.x && intersectPoint.x <= reference_point.x+floorWidth && intersectPoint.y >= reference_point.y && intersectPoint.y <= reference_point.y+floorWidth){
					// intersection point is within the floor
				}
				else{
					tMin = INF;
				}
			}

			if(level == 0){
				return tMin;
			}

			Vector3D intersectPoint = ray.start + ray.dir*tMin;
			Color intersectionPointColor;
			if (textureData && drawTexture == 1) { 
				cout<<"inside if"<<endl;
                double tileOriginX = floor((intersectPoint.x - reference_point.x) / tileWidth) * tileWidth + reference_point.x;
                double tileOriginY = floor((intersectPoint.y - reference_point.y) / tileWidth) * tileWidth + reference_point.y;

                double u = (intersectPoint.x - tileOriginX) / tileWidth;
                double v = (intersectPoint.y - tileOriginY) / tileWidth;

                intersectionPointColor = sampleTexture(u, v);
            } else {
                Vector3D floorPoint = intersectPoint - reference_point;
				int m = floor(floorPoint.x/tileWidth);
				int n = floor(floorPoint.y/tileWidth);

				if((m+n)%2 == 0){
					intersectionPointColor = tileColors[0];
				}else{
					intersectionPointColor = tileColors[1];
				}
            }
			
			computeAmbientDiffuseSpecularComponent(ray, color, level, tMin, normal, intersectPoint, intersectionPointColor);

			if(level >= levelsOfRecursion){
				return tMin;
			}

			recursiveReflection(ray, color, level, tMin, normal, intersectPoint);

			return tMin;
		}

		
		friend istream& operator>>(istream &in, Floor &f){
			in>>f.reference_point;
			in>>f.length;
			in>>f.color;
			in>>f.coEfficients[0]>>f.coEfficients[1]>>f.coEfficients[2]>>f.coEfficients[3]>>f.shine;
			return in;
		}

		friend ostream& operator<<(ostream &out, const Floor &f){
			out<<"Floor: ";
			out<<f.reference_point<<f.length<<endl;
			out<<"Colors: ";
			out<<f.color<<endl;
			out<<"Coefficients: ";
			out<<f.coEfficients[0]<<" , "<<f.coEfficients[1]<<" , "<<f.coEfficients[2]<<" , "<<f.coEfficients[3]<<endl;
			out<<"Shine: ";
			out<<f.shine<<endl;
			return out;
		}

		~Floor(){
			floorWidth = tileWidth = 0.0;
		}
};


class General : public Object{

	public:
		double A , B , C , D , E , F , G , H , I , J;

		General(double A ,double B ,double C ,double D ,double E ,double F ,double G ,double H ,double I ,double J):Object(){
			this->A = A;
			this->A = A;
			this->B = B;
			this->C = C;
			this->D = D;
			this->E = E;
			this->F = F;
			this->G = G;
			this->H = H;
			this->I = I;
			this->J = J;
		}


		General():Object(){
			A = B = C = D = E = F = G = H = I = J = 0.0;
		}

		void draw(){}


		void computeAmbientDiffuseSpecularComponent(Ray ray, Color &color, int level, double tNeg, Vector3D normal, Vector3D intersectPoint,Color intersectionPointColor){

			color = intersectionPointColor * coEfficients[AMBIENT];
			
			for(int i=0; i< lights.size();i++){
				Vector3D lightPosition = lights[i]->light_pos; 

				Color lightColor = lights[i]->getColor(intersectPoint);
				
				Vector3D lightDir = intersectPoint - lightPosition;
				lightDir.normalize();
				double lightDistance = lightPosition.distance(intersectPoint);

				Ray lightRay(lightPosition, lightDir);
				
				bool shadow = false;
				double eps = 0.000001;

				double tMin = INF,t;
				
				for(int j=0; j<objects.size(); j++){
					Color dummyColor;
					t = objects[j]->intersect(lightRay, dummyColor, 0);
					if(t>0.0 && tMin>t){
						tMin = t;
					}
				}

				if(tMin < lightDistance-eps){
					shadow = true;
				}

				if(!shadow){
					double diffuse = normal.dot(-lightRay.dir) * 1.0;

					if(diffuse>0.0){
						color = color + intersectionPointColor * lightColor * coEfficients[DIFFUSE] * diffuse;
					}

					Vector3D refRayDir = lightRay.dir - normal * ((normal.dot(lightRay.dir)) * 2.0);
					double specular = refRayDir.dot(ray.dir);

					if(specular>0.0){
						specular = pow(specular, shine);
						
						color = color + lightColor * coEfficients[SPECULAR] * specular;
					}
				}
			}
		}
		

		void recursiveReflection(Ray ray, Color &color, int level, double tNeg, Vector3D normal, Vector3D intersectPoint){

			Vector3D reflectionDir = ray.dir - normal * (ray.dir.dot(normal) * 2.0);
			reflectionDir.normalize();
			
			Ray reflectionRay(intersectPoint+reflectionDir, reflectionDir);

			int nearestObject = -1;
			double tMin = INF, t;

			for(int i=0; i<objects.size(); i++){
				Color tempColor;
				t = objects[i]->intersect(reflectionRay, tempColor, 0);
				if(t>0.0 && t<tMin){
					tMin = t;
					nearestObject = i;
				}
			}

			Color reflectedColor;
			if(nearestObject != -1){
				tMin = objects[nearestObject]->intersect(reflectionRay, reflectedColor, level+1);
			}
			
			color = color + reflectedColor * coEfficients[REFLECTION];
			color.clip();
		}


		double intersect(Ray ray, Color &color, int level){
			double a, b, c;
			double tPos, tNeg;
			
			Vector3D dir = ray.dir;
			dir.normalize();

			Vector3D origin = ray.start;
			
			a = A*dir.x*dir.x + B*dir.y*dir.y + C*dir.z*dir.z + D*dir.x*dir.y + E*dir.y*dir.z + F*dir.x*dir.z;
			b = 2.0*(A*origin.x*dir.x + B*origin.y*dir.y + C*origin.z*dir.z + D*origin.x*dir.y + D*origin.y*dir.x + E*origin.y*dir.z + E*origin.z*dir.y + F*origin.x*dir.z + F*origin.z*dir.x + G*dir.x + H*dir.y + I*dir.z);
			c = A*origin.x*origin.x + B*origin.y*origin.y + C*origin.z*origin.z + D*origin.x*origin.y + E*origin.y*origin.z + F*origin.x*origin.z + G*origin.x + H*origin.y + I*origin.z + J;

			if(a == 0.0){
				if(b!=0.0){
					tNeg = (-c)/b;
				}
				else{
					tNeg = INF;
				}
				tPos = INF;
			}else{
				double d_sq = (b*b - 4.0*a*c); // b^2 - 4ac
				if(d_sq<0.0){
					tNeg = INF;
					tPos = INF;
				}
				else{
					if(d_sq>0.0){
						// cout<<"d_sq: "<<d_sq<<endl;
						tPos = (-b+sqrt(d_sq))/(2.0*a);
						tNeg = (-b-sqrt(d_sq))/(2.0*a);
						if(tNeg<0.0){
							tNeg = tPos;
						}
					}
					else{
						tNeg = - b / (2.0*a);
						tPos = INF;
					}
				}
			}

			Vector3D intersectPoint = ray.start + ray.dir * tNeg;
			intersectPoint = intersectPoint - reference_point;
			double x = intersectPoint.x;
			double y = intersectPoint.y;
			double z = intersectPoint.z;

			if(abs(x)>length || abs(y)>width || abs(z)>height){
				tNeg = INF;
			}
			
			if(tNeg == INF){
				intersectPoint = ray.start + ray.dir * tPos;
				intersectPoint = intersectPoint - reference_point;
				x = intersectPoint.x;
				y = intersectPoint.y;
				z = intersectPoint.z;

				if(abs(x)>length || abs(y)>width || abs(z)>height){
					tPos = INF;
				}
				else{
					tNeg = tPos;
				}
			}

			if (level == 0){
				return tNeg;
			}

			Vector3D normal = Vector3D(2.0*A*x + D*y + F*z + G, 2.0*B*y + D*x + E*z + H, 2.0*C*z + E*y + F*x + I);
			normal.normalize(); 

			Color intersectionPointColor = this->color; 

			computeAmbientDiffuseSpecularComponent(ray, color, level, tNeg, normal, intersectPoint, intersectionPointColor);
			
			if(level >= levelsOfRecursion){
				return tNeg;
			}
			recursiveReflection(ray, color, level, tNeg, normal, intersectPoint);

			return tNeg;
		}

		friend istream& operator>>(istream &in, General &g){
			in>>g.A>>g.B>>g.C>>g.D>>g.E>>g.F>>g.G>>g.H>>g.I>>g.J;
			in>> g.reference_point;
			in >> g.length >> g.width >> g.height;
			in>>g.color;
			in>>g.coEfficients[0]>>g.coEfficients[1]>>g.coEfficients[2]>>g.coEfficients[3]>>g.shine;
			if(g.length==0.0) g.length = INF; 
			if(g.width==0.0) g.width = INF; 
			if(g.height==0.0) g.height = INF; 
			return in;
		}

		

		friend ostream& operator<<(ostream &out, const General &g){
			out<<"General:  "<<endl;
			out<<g.A<<" , "<<g.B<<" , "<<g.C<<" , "<<g.D<<" , "<<g.E<<" , "<<g.F<<" , "<<g.G<<" , "<<g.H<<" , "<<g.I<<" , "<<g.J<<endl;
			out<<g.reference_point<<g.length<<" "<<g.width<<" "<<g.height<<endl;
			out<<"Colors: "<<endl;
			out<<g.color<<endl;
			out<<"Coefficients: "<<endl;
			out<<g.coEfficients[0]<<" , "<<g.coEfficients[1]<<" , "<<g.coEfficients[2]<<" , "<<g.coEfficients[3]<<endl;
			out<<"Shine: "<<endl;
			out<<g.shine<<endl;
			return out;
		}
		


		~General(){
			A = B = C = D = E = F = G = H = I = J = 0.0;
		}

};














