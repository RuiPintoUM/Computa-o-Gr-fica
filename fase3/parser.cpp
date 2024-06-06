#include "structures.hpp"
#include "aux.hpp"
#include <string.h>

using namespace std;
using namespace tinyxml2;


void Translation::getCatmullRomPoint(float t, Point p1, Point p2, Point p3, Point p4, Point *pos, Point *deriv) {
    // catmull-rom matriz 
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

    // catmull-rom function
    pos->setX(powf(t,3.0) * ax[0] + powf(t, 2.0) * ax[1] + t * ax[2] + ax[3]);
    pos->setY(powf(t,3.0) * ay[0] + powf(t, 2.0) * ay[1] + t * ay[2] + ay[3]);
    pos->setZ(powf(t,3.0) * az[0] + powf(t, 2.0) * az[1] + t * az[2] + az[3]);

    // catmull-rom derivative
    deriv->setX(3 * powf(t, 2.0) * ax[0] + 2 * t * ax[1] + ax[2]);
    deriv->setY(3 * powf(t, 2.0) * ay[0] + 2 * t * ay[1] + ay[2]);
    deriv->setZ(3 * powf(t, 2.0) * az[0] + 2 * t * az[1] + az[2]);
    
}

void Translation::getGlobalCatmullRomPoint(float gt, Point *pos, Point *deriv) {
    int size = points.size();
    float tCatmull = gt * size;
    int index = floor(tCatmull); // segmento
    tCatmull -= index; // aonde, dentro do segmento

    int ind[4];
    ind[0] = (index + size-1) % size;
    ind[1] = (ind[0]+1) % size;
    ind[2] = (ind[1]+1) % size;
    ind[3] = (ind[2]+1) % size;

    getCatmullRomPoint(tCatmull, points[ind[0]], points[ind[1]], points[ind[2]], points[ind[3]], pos, deriv);
}

void Translation::renderCatmullRoomCurve() {
    float gt = 0;
    Point pos = Point();
    Point deriv = Point();

    glBegin(GL_LINE_LOOP);
    while(gt < 1) {
        getGlobalCatmullRomPoint(gt, &pos, &deriv);
        glVertex3f(pos.getX(), pos.getY(), pos.getZ());
        gt += 0.001;
    }
    glEnd();
}

void Translation::alignModel(Point *d) {
    Point * der_cross = new Point();
    Point * up = new Point(0, 1, 0);

    aux::normalize(d);

    aux::cross(d, up, der_cross);

    aux::normalize(der_cross);

    aux::cross(der_cross, d, up);

    aux::normalize(up);

    float m[4][4] = {{d->getX(), d->getY(), d->getZ(), 0},
                     {up->getX(), up->getY(), up->getZ(), 0},
                     {der_cross->getX(), der_cross->getY(), der_cross->getZ(), 0},
                     {0, 0, 0, 1}};
    
    glMultMatrixf((float *) m);

}

Window readWindow(XMLNode *world)
{
    XMLElement* windowElement = world->FirstChildElement("window");

    if(!windowElement) {
        return Window();
    }

    int width = windowElement->IntAttribute("width");
    int height = windowElement->IntAttribute("height");

    return Window(width, height);
}

Camera readCamera(XMLNode *world)
{
    XMLElement* cameraElement = world->FirstChildElement("camera");

    if(!cameraElement) {
        return Camera();
    }

    Point position(stof(cameraElement->FirstChildElement("position")->Attribute("x")),
                   stof(cameraElement->FirstChildElement("position")->Attribute("y")),
                   stof(cameraElement->FirstChildElement("position")->Attribute("z")));

    Point lookAt(stof(cameraElement->FirstChildElement("lookAt")->Attribute("x")),
                 stof(cameraElement->FirstChildElement("lookAt")->Attribute("y")),
                 stof(cameraElement->FirstChildElement("lookAt")->Attribute("z")));

    Point up(stof(cameraElement->FirstChildElement("up")->Attribute("x")),
             stof(cameraElement->FirstChildElement("up")->Attribute("y")),
             stof(cameraElement->FirstChildElement("up")->Attribute("z")));

    Point projection(stof(cameraElement->FirstChildElement("projection")->Attribute("fov")),
                     stof(cameraElement->FirstChildElement("projection")->Attribute("near")),
                     stof(cameraElement->FirstChildElement("projection")->Attribute("far")));

    return Camera(position, up, lookAt, projection);
}

Transforms readTransforms(XMLNode *group)
{
    Transforms transforms = Transforms();

    XMLElement *transform = group->FirstChildElement("transform");
    if (!transform)
    {
        return transforms;
    }

    XMLElement *transformation = transform->FirstChildElement();

    while (transformation)
    {
        if (!strcmp(transformation->Name(), "translate"))
        {
            float time = -1;
            bool align = 0;
            vector<Point> points = vector<Point>();
            
            Translation *t = new Translation();

            if(transformation->QueryFloatAttribute("time", &time));
            if(transformation->QueryBoolAttribute("align", &align));

            XMLElement *point = transformation->FirstChildElement("point");
            while(point) {
                float x, y, z;
                point->QueryFloatAttribute("x", &x);
                point->QueryFloatAttribute("y", &y);
                point->QueryFloatAttribute("z", &z);

                points.push_back(Point(x, y, z));
                point = point->NextSiblingElement("point");
            }  
            if(time == -1) {
                float x, y, z;
                transformation->QueryFloatAttribute("x", &x);
                transformation->QueryFloatAttribute("y", &y);
                transformation->QueryFloatAttribute("z", &z);

                t = new Translation(x, y, z);
            }
            else {
                t = new Translation(time, align, points);
            }

            transforms.addTransform(*t);
        }

        if (!strcmp(transformation->Name(), "rotate"))
        {
            float angle = 0, time = -1, x, y, z;

            if(transformation->QueryFloatAttribute("angle", &angle));
            if(transformation->QueryFloatAttribute("time", &time));
            
            transformation->QueryFloatAttribute("x", &x);
            transformation->QueryFloatAttribute("y", &y);
            transformation->QueryFloatAttribute("z", &z);

            Rotation *r = new Rotation(angle, time,x, y, z);
            transforms.addTransform(*r);
        }
        if (!strcmp(transformation->Name(), "scale"))
        {
            float x, y, z;
            transformation->QueryFloatAttribute("x", &x);
            transformation->QueryFloatAttribute("y", &y);
            transformation->QueryFloatAttribute("z", &z);

            Scale *s = new Scale(x, y, z);
            transforms.addTransform(*s);
        }
        transformation = transformation->NextSiblingElement();
    }

    return transforms;
}

vector<Point> *readPoints(const char *path)
{
    float x, y, z;
    //printf("entrei em readPoint %s\n", path);
    vector<Point> *points = new vector<Point>();

    ifstream file(path);

    if (!file.is_open())
    {
        cout << "Error opening file" << endl;
        return points;
    }

    while (file >> x >> y >> z)
    {
        points->push_back(Point(x, y, z));
    }

    return points;
}

Models readModels(XMLNode *group)
{
    Models models = Models();

    XMLElement *ms = group->FirstChildElement("models");
    if (!ms)
    {
        return models;
    }

    XMLElement *m = ms->FirstChildElement("model");

    while (m)
    {
        std::string file = m->ToElement()->Attribute("file");
        models.addModel(Model(file, readPoints((file).c_str())));

        m = m->NextSiblingElement("model");
    }

    return models;
}

Group readGroups(XMLNode *group) 
{
    Models m = readModels(group);
    Transforms t = readTransforms(group);
    vector<Group> groups = vector<Group>();
    
    XMLElement *subGroup = group->FirstChildElement("group");
    while (subGroup)
    {
        groups.push_back(readGroups(subGroup));
        subGroup = subGroup->NextSiblingElement("group");
    }

    return Group(t, m, groups);
}

World loadXML(const char *path)
{
    XMLDocument doc;

    doc.LoadFile(path);
    if (doc.ErrorID())
    {
        printf("%s\n", doc.ErrorStr());
        exit(0);
    }

    XMLNode *world = doc.FirstChildElement("world");
    if (world == NULL)
        exit(0);
    
    XMLElement *group = world->FirstChildElement("group");
    return World(readWindow(world), readCamera(world), readGroups(group));
}
