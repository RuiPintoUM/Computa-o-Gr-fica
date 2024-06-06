#ifndef STRUCTURES
#define STRUCTURES

#include <IL/il.h>
#include <GL/glew.h>
#include "GL/glut.h"
#include <sstream>
#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include "tinyxml/tinyxml2.hpp"

using namespace std;

class Point {
    private:
        float x, y, z;
    
    public:
        float getX() const {return x;}
        float getY() const {return y;}
        float getZ() const {return z;}
        
        void setX(float x) {this->x = x;}
        void setY(float y) {this->y = y;}
        void setZ(float z) {this->z = z;}

        float* pointToVector() {
            return new float[3]{x, y, z};
        }
        
        string toString() {
            ostringstream point;
            point << x << ' ' << y << ' ' << z;
            return point.str();
        }

        void printPoint() {
            cout << "x: " << x << " y: " << y << " z: " << z << endl;
        }

        Point() {
            x = 0;
            y = 0;
            z = 0;
        }

        Point(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }
};


class Point2D {
    private:
        float x, y;

    public:
        float getX() const {return x;}
        float getY() const {return y;}

        void printPoint() {
            cout << "x:" << x << " y:" << y << endl;
        }

        Point2D(){
            x = 0;
            y = 0;
        }

        Point2D(float x, float y) {
            this->x = x;
            this->y = y;
        }
};

class Camera {
    private:
        Point position, up, lookAt, projection;
        float alpha, betha, radius; 

    public:
        Point getPosition() const {return position;}
        Point getLookAt() const {return lookAt;}
        Point getUp() const {return up;}
        Point getProjection() const {return projection;} 
        float getAlpha() const {return alpha;}
        float getBetha() const {return betha;}
        float getRadius() const {return radius;}
        
        void setAlpha(float a) { alpha = a; }
        void setBetha(float b) { betha = b; }
        void setRadius(float r) { radius = r; }

        void moveCamera(){
            position.setX(radius * cos(betha) * sin(alpha));
            position.setY(radius * sin(betha));
            position.setZ(radius * cos(betha) * cos(alpha));  
        }

        void convertToSpherical() {

            if (position.getX() == 0) position.setX(0.0000001);
            if (position.getY() == 0) position.setY(0.0000001);
            if (position.getZ() == 0) position.setZ(0.0000001);

            radius = sqrt(pow(position.getX() - lookAt.getX(), 2) + pow(position.getY() - lookAt.getY(), 2) + pow(position.getZ() - lookAt.getZ(), 2));
            alpha = atan2(position.getX(), position.getZ());
            betha = asin((position.getY() - lookAt.getY()) / radius);

            if (betha >= M_PI / 2) betha -= M_PI;
            if (betha <= -M_PI / 2) betha += M_PI;
        }
        
        void printCamera() {
            cout << "Camera position: " << getPosition().getX() << " " << getPosition().getY() << " " << getPosition().getZ() << endl;
            cout << "Camera lookAt: " << getLookAt().getX() << " " << getLookAt().getY() << " " << getLookAt().getZ() << endl;
            cout << "Camera up: " << getUp().getX() << " " << getUp().getY() << " " << getUp().getZ() << endl;
            cout << "Camera projection: " << getProjection().getX() << " " << getProjection().getY() << " " << getProjection().getZ() << endl;
            cout << "Camera alpha: " << alpha << endl;
            cout << "Camera betha: " << betha << endl;
            cout << "Camera radius: " << radius << endl;
        }

        Camera() {
            position = Point();
            up = Point();
            lookAt = Point();
            projection = Point();
            alpha = 0;
            betha = 0;
            radius = 0;
        }

        Camera(Point position, Point up, Point lookAt, Point projection, float alpha, float betha, float radius) {
            this->position = position;
            this->up = up;
            this->lookAt = lookAt;
            this->projection = projection;
            this->alpha = alpha;
            this->betha = betha;
            this->radius = radius;
        }
};

class aux {
    public:
        static void multMatrixVector(float *m, float *v, float* res) {
            for (int j = 0; j < 4; ++j) {
                res[j] = 0;
                for (int k = 0; k < 4; ++k) {
                    res[j] += v[k] * m[j * 4 + k];
                }
            }
        }

        static void multVectorMatrix(float *v, float *m, float* res) {
            for (int j = 0; j < 4; j++) {
                res[j] = 0;
                for (int k = 0; k < 4; k++) {
                    res[j] += v[k] * m[k * 4 + j];
                }
            }
        }

        static float multVectors(float *v1, float *v2) {
            float res = 0;
            for (int j = 0; j < 4; j++) {
                res += v1[j] * v2[j];
            }
            return res;
        }

        static void normalize(Point *a) {
            float norm = sqrt(a->getX() * a->getX() + a->getY() * a->getY() + a->getZ() * a->getZ());
            a->setX(a->getX() / norm);
            a->setY(a->getY() / norm);
            a->setZ(a->getZ() / norm);
        }

        static void cross(Point *a, Point *b, Point *res) {
            res->setX(a->getY() * b->getZ() - a->getZ() * b->getY());
            res->setY(a->getZ() * b->getX() - a->getX() * b->getZ());
            res->setZ(a->getX() * b->getY() - a->getY() * b->getX());
        } 

        static void dot(Point *a, Point *b, float *res) {
            *res = a->getX() * b->getX() + a->getY() * b->getY() + a->getZ() * b->getZ();
        }
};

class Transform {
    private:
        Point point;

    public:
        float getX() const { return point.getX(); }
        float getY() const { return point.getY(); }
        float getZ() const { return point.getZ(); }

        virtual void printTransform() = 0;

        virtual void execute(float t) = 0;

        Transform() {
            point.setX(0);
            point.setY(0);
            point.setZ(0);
        }

        Transform(float x, float y, float z) {
            point.setX(x);
            point.setY(y);
            point.setZ(z);
        }
};


class Translation : public Transform {
    private:
        float time;
        bool align;
        vector<Point> points;

    public:
        float getTime() const {return time;}
        float getAlign() const {return align;}
        vector<Point> getPoints() const {return points;}

        void getCatmullRomPoint(float t, Point p1, Point p2, Point p3, Point p4, Point *pos, Point *deriv) {
            
            float m[4][4] = {	{-0.5f,  1.5f, -1.5f,  0.5f},
                                { 1.0f, -2.5f,  2.0f, -0.5f},
                                {-0.5f,  0.0f,  0.5f,  0.0f},
                                { 0.0f,  1.0f,  0.0f,  0.0f}};

            float px[4] = {p1.getX(), p2.getX(), p3.getX(), p4.getX()};
            float py[4] = {p1.getY(), p2.getY(), p3.getY(), p4.getY()};
            float pz[4] = {p1.getZ(), p2.getZ(), p3.getZ(), p4.getZ()};

            float ax[4], ay[4], az[4];
            aux::multMatrixVector(*m, px, ax);
            aux::multMatrixVector(*m, py, ay);
            aux::multMatrixVector(*m, pz, az);

            pos->setX(powf(t,3.0) * ax[0] + powf(t,2.0) * ax[1] + t * ax[2] + ax[3]);
            pos->setY(powf(t,3.0) * ay[0] + powf(t,2.0) * ay[1] + t * ay[2] + ay[3]);
            pos->setZ(powf(t,3.0) * az[0] + powf(t,2.0) * az[1] + t * az[2] + az[3]);

            deriv->setX(3 * powf(t,2.0) * ax[0] + 2 * t * ax[1] + ax[2]);
            deriv->setY(3 * powf(t,2.0) * ay[0] + 2 * t * ay[1] + ay[2]);
            deriv->setZ(3 * powf(t,2.0) * az[0] + 2 * t * az[1] + az[2]);
        }

        void getGlobalCatmullRomPoint(float gt, Point *pos, Point *deriv) {
            int size = points.size();
            float tCatmull = gt * size; 
            int index = floor(tCatmull);  
            tCatmull -= index;

            int ind[4];
            ind[0] = (index + size-1) % size;	
            ind[1] = (ind[0]+1) % size;
            ind[2] = (ind[1]+1) % size; 
            ind[3] = (ind[2]+1) % size;

            getCatmullRomPoint(tCatmull, points[ind[0]], points[ind[1]], points[ind[2]], points[ind[3]], pos, deriv);
        }

        void renderCatmullRoomCurve() {
            float gt = 0;
            Point pos = Point(), deriv = Point();

            glBegin(GL_LINE_LOOP);
            while (gt < 1) {
                getGlobalCatmullRomPoint(gt, &pos, &deriv);
                glVertex3f(pos.getX(), pos.getY(), pos.getZ());
                gt += 0.001;
            }
            glEnd();
        }

        void alignModel(Point *deriv) {
            Point * up = new Point(0,1,0);
            Point * deriv_cross = new Point(); 
            
            aux::normalize(deriv);
            aux::cross(deriv, up, deriv_cross);
            aux::normalize(deriv_cross);
            aux::cross(deriv_cross, deriv, up);
            aux::normalize(up);
            
            // Rotation matrix
            float m[4][4] = {{deriv->getX(), deriv->getY(), deriv->getZ(), 0},
                            {up->getX(), up->getY(), up->getZ(), 0},
                            {deriv_cross->getX(), deriv_cross->getY(), deriv_cross->getZ(), 0},
                            {0, 0, 0, 1}};

            glMultMatrixf((float *) m);
        }


        void printTransform() {
            cout << "Translation: " << time << " " << align << endl;
            for (int i = 0; i < points.size(); i++) {
                cout << points[i].getX() << " " << points[i].getY() << " " << points[i].getZ() << endl;
            }
        }

        void execute(float t) {
            if(points.empty()) {
                glTranslatef(getX(), getY(), getZ());
                return;
            }
            
            //renderCatmullRoomCurve();
            int start = t/time;
            t-= start * time;
            Point pos = Point(), deriv = Point();
            getGlobalCatmullRomPoint(t/time, &pos, &deriv);
            glTranslatef(pos.getX(), pos.getY(), pos.getZ());

            if(align) alignModel(&deriv);
        }

        Translation() : Transform() {
            this->time = -1;
            this->align = false;
            this->points = vector<Point>();

        }

        Translation(float x, float y, float z) : Transform(x, y, z) {
            this->time = -1;
            this->align = false;
            this->points = vector<Point>();
        }

        Translation(float time, bool align, vector<Point> points) : Transform() {
            this->time = time;
            this->align = align;
            this->points = points;
        }
};

class Rotation : public Transform {
    private:
        float angle, time;

    public:
        float getAngle() const {return angle;}
        float getTime() const {return time;}
        
        void printTransform() {
            cout << "Rotation: " << angle << " " << time << " " << getX() << " " << getY() << " " << getZ() << endl;
        } 

        void execute(float t) {
            float newAngle = 0;

            if(time != -1) {
                int start = t/time;
                t-= start * time;
                newAngle = 360 * (t / time);
            }


            glRotatef(angle + newAngle, getX(), getY(), getZ());
        }

        Rotation() : Transform() {
            this->angle = 0;
            this->time = -1;
        }

        Rotation(float angle, float time, float x, float y, float z) : Transform(x, y, z) {
            this->angle = angle;
            this->time = time;
        }
};

class Scale : public Transform {
    public:

        void printTransform() {
            cout << "Scale: " << getX() << " " << getY() << " " << getZ() << endl;
        }

        void execute(float t) {
            glScalef(getX(), getY(), getZ());
        }

        Scale(float x, float y, float z) : Transform(x, y, z) {}
};

class Transforms {
    private:
        vector<Transform*> transforms;

    public:
        vector<Transform*> getTransforms() const {return transforms;}
        
        void addTransform(Transform &t) {transforms.push_back(&t);}

        void printTransforms() {
            for (int i = 0; i < transforms.size(); i++) {
                transforms.at(i)->printTransform();
            }
        }

        Transforms() {
            transforms = vector<Transform*>();
        }

        Transforms(vector<Transform*> transforms) {
            this->transforms = transforms;
        }
};

class Color {
    private:
        Point diffuse, ambient, specular, emissive;
        float shininess;
    
    public:
        Point getDiffuse() const {return diffuse;}
        Point getAmbient() const {return ambient;}
        Point getSpecular() const {return specular;}
        Point getEmissive() const {return emissive;}
        float getShininess() const {return shininess;}

        void setDiffuse(float r, float g, float b) {diffuse = Point(r/255, g/255, b/255);}
        void setAmbient(float r, float g, float b) {ambient = Point(r/255, g/255, b/255);}
        void setSpecular(float r, float g, float b) {specular = Point(r/255, g/255, b/255);}
        void setEmissive(float r, float g, float b) {emissive = Point(r/255, g/255, b/255);}
        void setShininess(float shininess) {this->shininess = shininess;}

        void paint() {
            float diffuse[4] = {this->diffuse.getX(), this->diffuse.getY(), this->diffuse.getZ(), 1},
                  ambient[4] = {this->ambient.getX(), this->ambient.getY(), this->ambient.getZ(), 1},
                  specular[4] = {this->specular.getX(), this->specular.getY(), this->specular.getZ(), 1},
                  emissive[4] = {this->emissive.getX(), this->emissive.getY(), this->emissive.getZ(), 1};
            
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
        }

        void printColor() {
            cout << "Diffuse: " << diffuse.getX() << " " << diffuse.getY() << " " << diffuse.getZ() << endl;
            cout << "Ambient: " << ambient.getX() << " " << ambient.getY() << " " << ambient.getZ() << endl;
            cout << "Specular: " << specular.getX() << " " << specular.getY() << " " << specular.getZ() << endl;
            cout << "Emission: " << emissive.getX() << " " << emissive.getY() << " " << emissive.getZ() << endl;
            cout << "Shininess: " << shininess << endl;
        }

        Color() {
            diffuse = Point(200, 200, 200);
            ambient = Point(50, 50, 50);
            specular = Point(0, 0, 0);
            emissive = Point(0, 0, 0);
            shininess = 0;
        }

        Color(Point diffuse, Point ambient, Point specular, Point emission, float shininess) {
            this->diffuse = diffuse;
            this->ambient = ambient;
            this->specular = specular;
            this->emissive = emission;
            this->shininess = shininess;
        }
};

class Model {
    private:
        string file;
        vector<Point> points;
        vector<Point> normals;
        GLuint bufferID; 
        GLuint bufferNormalID; 
        GLuint bufferTextureID; 
        GLuint textureID;
        Color color;

    public:
        string getFileName() const {return file;}
        vector<Point> getPoints() const {return points;}
        vector<Point> getNormals() const {return normals;}
        GLuint getBufferID() const { return bufferID; }
        GLuint getBufferNormalID() const { return bufferNormalID; }
        GLuint getBufferTextureID() const { return bufferTextureID; }
        GLuint getTextureID() const { return textureID; }
        Color getColor() const {return color;}

        void printModel() {
            cout << "File: " << file << endl;
            cout << "Points: " << endl;
            for (int i = 0; i < points.size(); i++) {
                points.at(i).printPoint();
            }
            cout << "Buffers VBOs: " << endl;
            cout << "Points Buffer ID: " << bufferID << endl;
            cout << "Normals Buffer ID: " << bufferNormalID << endl;
            cout << "Textures Buffer ID: " << bufferTextureID << endl;
            cout << "Color: " << endl;
            color.printColor();
        }

        Model() {
            this->file = "";
            this->points = vector<Point>();
            this->normals = vector<Point>();
            this->bufferID = 0;
            this->bufferNormalID = 0;
            this->bufferTextureID = 0;
            this->color = Color();
        }

        Model(string file, vector<Point> points, vector<Point> normals, GLuint bufferID, GLuint bufferNormalID, GLuint bufferTextureID, GLuint textureID, Color color) {
            this->file = file;
            this->points = points;
            this->normals = normals;
            this->bufferID = bufferID;
            this->bufferNormalID = bufferNormalID;
            this->bufferTextureID = bufferTextureID;
            this->textureID = textureID;
            this->color = color;
        }
};

class Models {
    private:
        vector<Model> models;
    
    public:
        vector<Model> getModels() const {return models;}
        
        void addModel(Model m) {models.push_back(m);}
        
        void printModels() {
            if (models.empty()) {
                cout << "Models vector is empty" << endl;
                return;
            }

            for (int i = 0; i < models.size(); i++) {
                models.at(i).printModel();
            }
        }

        Models() {
            this->models = vector<Model>();
        }

        Models(vector<Model> models) {
            this->models = models;
        }
};

class Group {
    private: 
        Transforms transforms;
        Models models;
        vector<Group> groups;
    
    public:
        Transforms getTransforms() const {return transforms;}
        Models getModels() const {return models;}
        vector<Group> getGroups() const {return groups;}
        
        void addGroup(Group g) {groups.push_back(g);}
        
        void printGroups() {
            cout << "Group transforms: " << endl;
            transforms.printTransforms();
            cout << "Group models: " << endl;
            models.printModels();
            cout << "Group groups: " << endl;
            for (int i = 0; i < groups.size(); i++) {
                groups.at(i).printGroups();
            }
        }

        Group() {
            transforms = Transforms();
            models = Models();
            groups = vector<Group>();
        }

        Group(Transforms transforms, Models models, vector<Group> groups) {
            this->transforms = transforms;
            this->models = models;
            this->groups = groups;
        }
};

class Light {
    private:
        int id;

    public:
        int getId() const {return id;}

        virtual void execute() = 0;

        virtual void printLight() = 0;

        Light(int id) {
            this->id = GL_LIGHT0 + id;
        }
};

class PointLight : public Light {
    private:
        Point position;

    public:
        Point getPosition() const {return position;}

        void execute() {
            float pos[4] = {position.getX(), position.getY(), position.getZ(), 1.0};
            glLightfv(getId(), GL_POSITION, pos);
        }

        void printLight() {
            cout << "Point Light" << endl;
            cout << "Position: " << position.getX() << " " << position.getY() << " " << position.getZ() << endl;
        }

        PointLight(int id, Point position) : Light(id) {
            this->position = position;
        }
};

class DirectionalLight : public Light {
    private:
        Point direction;

    public:
        Point getDirection() const {return direction;}

        void execute() {
            float dir[4] = {direction.getX(), direction.getY(), direction.getZ(), 0.0};         
            glLightfv(getId(), GL_POSITION , dir);
        }
        void printLight() {
            cout << "Id: " << getId() << endl; 
            cout << "Directional Light" << endl;
            cout << "Direction: " << direction.getX() << " " << direction.getY() << " " << direction.getZ() << endl;
        }

        DirectionalLight(int id, Point direction) : Light(id) {
            this->direction = direction;
        }
};

class SpotLight : public Light {
    private:
        Point position, direction;
        float cutoff;

    public:
        Point getPosition() const {return position;}
        Point getDirection() const {return direction;}
        float getCutoff() const {return cutoff;}

        void execute() {
            float pos[4] = {position.getX(), position.getY(), position.getZ(), 1.0};
            float dir[4] = {direction.getX(), direction.getY(), direction.getZ(), 0.0};
            glLightfv(getId(), GL_POSITION, pos);
            glLightfv(getId(), GL_SPOT_DIRECTION, dir);
            glLightf(getId(), GL_SPOT_CUTOFF, cutoff);
        }

        void printLight() {
            cout << "Spot Light" << endl;
            cout << "Position: " << position.getX() << " " << position.getY() << " " << position.getZ() << endl;
            cout << "Direction: " << direction.getX() << " " << direction.getY() << " " << direction.getZ() << endl;
            cout << "Cutoff: " << cutoff << endl;
        }

        SpotLight(int id, Point position, Point direction, float cutoff) : Light(id) {
            this->position = position;
            this->direction = direction;
            this->cutoff = cutoff;
        }
};

class Lights {
    private:
        vector<Light*> lights;
    
    public:
        vector<Light*> getLights() const {return lights;}
        
        void addLight(Light *light) {
            lights.push_back(light);
        }

        void execute() {
            for (int i = 0; i < lights.size(); i++) {
                lights[i]->execute();
            }
        }

        void printLights() {
            for (int i = 0; i < lights.size(); i++) {
                lights[i]->printLight();
            }
        }

        Lights() {
            this->lights = vector<Light*>();
        }

        Lights(vector<Light*> lights) {
            this->lights = lights;
        }
};

class Window {
    private:
        int width, height;
    
    public:
        int getWidth() const {return width;}
        int getHeight() const {return height;}

        void printWindow() {
            cout << "Window width: " << getWidth() << endl;
            cout << "Window height: " << getHeight() << endl;
        }

        Window() {
            width = 0;
            height = 0;
        }

        Window(int width, int height) {
            this->width = width;
            this->height = height;
        }
};

class World {
    private:
        Window window;
        Camera camera;
        Lights lights;
        Group groups;
    
    public:
        Window getWindow() const {return window;}
        Camera getCamera() const {return camera;}
        Lights getLights() const {return lights;}
        Group getGroup() const {return groups;}

        void printWorld() {
            cout << "Window: " << endl;
            window.printWindow();
            cout << "Camera: " << endl;
            camera.printCamera();
            lights.printLights();
            cout << "Groups: " << endl;
            groups.printGroups();
        }

        World() {
            window = Window();
            camera = Camera();
            lights = Lights();
            groups = Group();
        }

        World(Window window, Camera camera, Lights lights, Group groups) {
            this->window = window;
            this->camera = camera;
            this->lights = lights;
            this->groups = groups;
        }
};

World readXML(const char * path);

#endif