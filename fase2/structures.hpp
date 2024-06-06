#ifndef STRUCTURES
#define STRUCTURES

#include <string>
#include <sstream>
#include "GL/glut.h"
#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
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
        
        string toString() {
            ostringstream point;                
            point << x << ' ' << y << ' ' << z; 
            return point.str();                 
        }

        Point () {
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

class Window {
    private:
        int width, height;
    
    public:
        int getWidth() const {return width;}
        int getHeight() const {return height;}

        void printWindow() {
            cout << "Width: " << getWidth() << endl;
            cout << "Height: " << getHeight() << endl;
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

class Camera {
    private:
        Point position, up, lookAt, projection; 

    public:
        Point getPosition() const {return position;}
        Point getLookAt() const {return lookAt;}
        Point getUp() const {return up;}
        Point getProjection() const {return projection;} 
        
        void printCamera() {
            cout << "Position: " << getPosition().getX() << " " << getPosition().getY() << " " << getPosition().getZ() << endl;
            cout << "LookAt: " << getLookAt().getX() << " " << getLookAt().getY() << " " << getLookAt().getZ() << endl;
            cout << "Up: " << getUp().getX() << " " << getUp().getY() << " " << getUp().getZ() << endl;
            cout << "Projection: " << getProjection().getX() << " " << getProjection().getY() << " " << getProjection().getZ() << endl;
        }

        Camera() {
            position = Point();
            up = Point();
            lookAt = Point();
            projection = Point();
        }

        Camera(Point position, Point up, Point lookAt, Point projection) {
            this->position = position;
            this->up = up;
            this->lookAt = lookAt;
            this->projection = projection;
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

        virtual void execute() = 0;

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
    public:
        void printTransform() {
            cout << "Translation: " << getX() << " " << getY() << " " << getZ() << endl;
        }

        void execute() {
            glTranslatef(getX(), getY(), getZ());
        }

        Translation(float x, float y, float z) : Transform(x, y, z) {}
};

class Rotation : public Transform {
    private:
        float angle;

    public:
        float getAngle() const {return angle;}
        
        void printTransform() {
            cout << "Rotation: " << getAngle() << " " << getX() << " " << getY() << " " << getZ() << endl;
        }

        void execute() {
            glRotatef(angle, getX(), getY(), getZ());
        }

        Rotation(float angle, float x, float y, float z) : Transform(x, y, z) {this->angle = angle;}
};

class Scale : public Transform {
    public:
        void printTransform() {
            cout << "Scale: " << getX() << " " << getY() << " " << getZ() << endl;
        }
        void execute() {
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

class Model {
    private:
        std::string file;
        vector<Point>* points;
    
    public:
        std::string getFile() const {return file;}
        vector<Point>* getPoints() const {return points;}
        
        void printModel() {
            cout << "File: " << getFile() << endl;
            cout << "Points: " << endl;
            for (int i = 0; i < points->size(); i++) {
                cout << points->at(i).getX() << " " << points->at(i).getY() << " " << points->at(i).getZ() << endl;
            }
        }

        Model() {
            file = "";
            points = new vector<Point>();
        }

        Model(std::string file, vector<Point>* points) {
            this->file = file;
            this->points = points;
        }
};

class Models {
    private:
        vector<Model> models;
    
    public:
        vector<Model> getModels() const {return models;}
        
        void addModel(Model m) {models.push_back(m);}
        
        void printModels() {
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
            cout << "Transforms: " << endl;
            transforms.printTransforms();
            cout << "Models: " << endl;
            models.printModels();
            cout << "Groups: " << endl;
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

class World {
    private:
        Window window;
        Camera camera;
        Group groups;
    
    public:
        Window getWindow() const {return window;}
        Camera getCamera() const {return camera;}
        Group getGroup() const {return groups;}

        void printTags() {
            cout << "Window: " << endl;
            window.printWindow();
            cout << "Camera: " << endl;
            camera.printCamera();
            cout << "Groups: " << endl;
            groups.printGroups();
        }

        World() {
            window = Window();
            camera = Camera();
            groups = Group();
        }

        World(Window window, Camera camera, Group groups) {
            this->window = window;
            this->camera = camera;
            this->groups = groups;
        }
};

World loadXML(const char * path);

#endif