#include "structures.hpp"
#include <string.h>

using namespace std;
using namespace tinyxml2;

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

    float angle, x, y, z;
    while (transformation)
    {
        if (!strcmp(transformation->Name(), "translate"))
        {
            transformation->QueryFloatAttribute("x", &x);
            transformation->QueryFloatAttribute("y", &y);
            transformation->QueryFloatAttribute("z", &z);

            Translation *t = new Translation(x, y, z);
            transforms.addTransform(*t);
        }

        if (!strcmp(transformation->Name(), "rotate"))
        {
            transformation->QueryFloatAttribute("angle", &angle);
            transformation->QueryFloatAttribute("x", &x);
            transformation->QueryFloatAttribute("y", &y);
            transformation->QueryFloatAttribute("z", &z);

            Rotation *r = new Rotation(angle, x, y, z);
            transforms.addTransform(*r);
        }
        if (!strcmp(transformation->Name(), "scale"))
        {
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
