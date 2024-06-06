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
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }
    void setX(float x) { this->x = x; }
    void setY(float y) { this->y = y; }
    void setZ(float z) { this->z = z; }

    string toString() {
        ostringstream point;
        point << x << ' ' << y << ' ' << z;
        return point.str();
    }

    Point() : x(0), y(0), z(0) {}
    Point(float x, float y, float z) : x(x), y(y), z(z) {}
};

class Window {
private:
    int width, height;

public:
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void printWindow() {
        cout << "Width: " << getWidth() << endl;
        cout << "Height: " << getHeight() << endl;
    }

    Window() : width(0), height(0) {}
    Window(int width, int height) : width(width), height(height) {}
};

class Camera {
private:
    Point position, up, lookAt, projection;
    float alpha, betha, radius;

public:
    Point getPosition() { return position; }
    Point getLookAt() { return lookAt; }
    Point getUp() { return up; }
    Point getProjection() { return projection; }

    float getAlpha() const { return alpha; }
    float getBetha() const { return betha; }
    float getRadius() const { return radius; }

    void setAlpha(float a) { alpha = a; }
    void setBetha(float b) { betha = b; }
    void setRadius(float r) { radius = r; }

    void moveCamera() {
        position.setX(radius * cos(betha) * sin(alpha));
        position.setY(radius * sin(betha));
        position.setZ(radius * cos(betha) * cos(alpha));
    }

    void convertToSpherical() {
        if(position.getX() == 0) position.setX(0.0000001);
        if(position.getY() == 0) position.setY(0.0000001);
        if(position.getZ() == 0) position.setZ(0.0000001);

        radius = sqrt(pow(position.getX() - lookAt.getX(), 2) + pow(position.getY() - lookAt.getY(), 2) + pow(position.getZ() - lookAt.getZ(), 2));
        alpha = atan2(position.getX(), position.getZ());
        betha = asin((position.getY() - lookAt.getY()) / radius);

        if(betha >= M_PI / 2) betha -= M_PI;
        if(betha <= -M_PI / 2) betha += M_PI;
    }

    void printCamera() {
        cout << "Camera position: " << position.getX() << " " << position.getY() << " " << position.getZ() << endl;
        cout << "Camera lookAt: " << lookAt.getX() << " " << lookAt.getY() << " " << lookAt.getZ() << endl;
        cout << "Camera up: " << up.getX() << " " << up.getY() << " " << up.getZ() << endl;
        cout << "Camera projection: " << projection.getX() << " " << projection.getY() << " " << projection.getZ() << endl;
        cout << "Camera alpha: " << alpha << endl;
        cout << "Camera betha: " << betha << endl;
        cout << "Camera radius: " << radius << endl;
    }

    Camera() : position(Point()), up(Point()), lookAt(Point()), projection(Point()), alpha(0), betha(0), radius(0) {}
    Camera(Point position, Point up, Point lookAt, Point projection) : position(position), up(up), lookAt(lookAt), projection(projection), alpha(0), betha(0), radius(0) {}
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

    Transform() : point(Point(0, 0, 0)) {}
    Transform(float x, float y, float z) : point(Point(x, y, z)) {}
};

class Translation : public Transform {
private:
    float time;
    bool align;
    vector<Point> points;

public:
    float getTime() const { return time; }
    bool getAlign() const { return align; }
    vector<Point>& getPoints() { return points; }

    void getCatmullRomPoint(float t, Point p1, Point p2, Point p3, Point p4, Point *pos, Point *deriv);
    void getGlobalCatmullRomPoint(float gt, Point *pos, Point *deriv);
    void renderCatmullRoomCurve();
    void alignModel(Point *deriv);

    void printTransform() {
        cout << "Translation: " << getX() << " " << getY() << " " << getZ() << endl;
        for (const Point& p : points) {
            cout << p.getX() << " " << p.getY() << " " << p.getZ() << endl;
        }
    }

    void execute(float t) {
        if (points.empty()) {
            glTranslatef(getX(), getY(), getZ());
            return;
        }

        renderCatmullRoomCurve();
        int start = t / time;
        t -= start * time;
        Point pos = Point(), deriv = Point();
        getGlobalCatmullRomPoint(t / time, &pos, &deriv);
        glTranslatef(pos.getX(), pos.getY(), pos.getZ());

        if (align) alignModel(&deriv);
    }

    Translation() : Transform(), time(-1), align(false), points(vector<Point>()) {}
    Translation(float x, float y, float z) : Transform(x, y, z), time(-1), align(false), points(vector<Point>()) {}
    Translation(float time, bool align, vector<Point> points) : Transform(), time(time), align(align), points(points) {}
};

class Rotation : public Transform {
    private:
        float angle, time;

    public:
        float getAngle() const { return angle; }
        float getTime() const { return time; }

        void printTransform() {
            cout << "Rotation: " << getAngle() << " " << getX() << " " << getY() << " " << getZ() << endl;
        }

        void execute(float t) {
            float newAngle = 0;
            if (time != -1) {
                int start = t / time;
                t -= start * time;
                newAngle = 360 * (t / time);
            }

            glRotatef(angle + newAngle, getX(), getY(), getZ());
        }

        Rotation() : Transform(), angle(0), time(-1) {}
        Rotation(float angle, float time, float x, float y, float z) : Transform(x, y, z), angle(angle), time(time) {}
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
        vector<Transform*>& getTransforms() { return transforms; }

        void addTransform(Transform& t) { transforms.push_back(&t); }

        void printTransforms() {
            for (Transform* t : transforms) {
                t->printTransform();
            }
        }

        Transforms() : transforms(vector<Transform*>()) {}
        Transforms(vector<Transform*> transforms) : transforms(transforms) {}
};

class Model {
    private:
        string file;
        vector<Point>* points;
        GLuint bufferID; // OpenGL buffer ID
        int pointCount; // Number of vertices

    public:
        string getFile() const { return file; }
        vector<Point>* getPoints() const { return points; }
        GLuint getBufferID() const { return bufferID; }
        int getPointCount() const { return pointCount; }

        void setBufferID(GLuint id) { bufferID = id; }
        void setPointCount(int count) { pointCount = count; }

        void printModel() const {
            cout << "File: " << getFile() << endl;
            cout << "Points: ";
            for (const Point& p : *points) {
                cout << p.getX() << " " << p.getY() << " " << p.getZ() << " ";
            }
            cout << endl;
        }

        Model() : file(""), points(new vector<Point>()) {}
            Model(string file, vector<Point>* points) : file(file), points(points), bufferID(0), pointCount(0) {}
};

class Models {
    private:
        vector<Model> models;

    public:
        vector<Model>& getModels() { return models; }
        void addModel(const Model& m) { models.push_back(m); }

        void printModels() {
            for (const Model& m : models) {
                m.printModel();
            }
        }

        Models() : models(vector<Model>()) {}
        Models(vector<Model> models) : models(models) {}
};

class Group {
    private:
        Transforms transforms;
        Models models;
        vector<Group> groups;

    public:
        Transforms& getTransforms() { return transforms; }
        Models& getModels() { return models; }
        vector<Group>& getGroups() { return groups; }

        void addGroup(Group& g) { groups.push_back(g); }

        void printGroups() {
            cout << "Transforms: ";
            transforms.printTransforms();
            cout << "Models: ";
            models.printModels();
            cout << "Groups: ";
            for (Group& g : groups) {
                g.printGroups();
            }
            cout << endl;
        }

        Group() : transforms(Transforms()), models(Models()), groups(vector<Group>()) {}
        Group(Transforms transforms, Models models, vector<Group> groups) : transforms(transforms), models(models), groups(groups) {}
};

class World {
    private:
        Window window;
        Camera camera;
        Group groups;

    public:
        Window& getWindow() { return window; }
        Camera getCamera() { return camera; }
        Group& getGroup() { return groups; }

        void printTags() {
            cout << "Window: ";
            window.printWindow();
            cout << "Camera: ";
            camera.printCamera();
            cout << "Groups: ";
            groups.printGroups();
        }

        World() : window(Window()), camera(Camera()), groups(Group()) {}
        World(Window window, Camera camera, Group groups) : window(window), camera(camera), groups(groups) {}
};

World loadXML(const char * path);

#endif
