#include <IL/il.h>
#include <string.h>
#include <GL/glew.h>
#include "GL/glut.h"
#include "structures.hpp"
#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

using namespace std;
using namespace tinyxml2;

Camera readCamera(XMLNode *world)
{
    if(!world->FirstChildElement("camera")) {
        return Camera();
    }

    Point position(stof(world->FirstChildElement("camera")->FirstChildElement("position")->Attribute("x")),
                   stof(world->FirstChildElement("camera")->FirstChildElement("position")->Attribute("y")),
                   stof(world->FirstChildElement("camera")->FirstChildElement("position")->Attribute("z")));

    Point lookAt(stof(world->FirstChildElement("camera")->FirstChildElement("lookAt")->Attribute("x")),
                 stof(world->FirstChildElement("camera")->FirstChildElement("lookAt")->Attribute("y")),
                 stof(world->FirstChildElement("camera")->FirstChildElement("lookAt")->Attribute("z")));

    Point up(stof(world->FirstChildElement("camera")->FirstChildElement("up")->Attribute("x")),
             stof(world->FirstChildElement("camera")->FirstChildElement("up")->Attribute("y")),
             stof(world->FirstChildElement("camera")->FirstChildElement("up")->Attribute("z")));

    Point projection(stof(world->FirstChildElement("camera")->FirstChildElement("projection")->Attribute("fov")),
                     stof(world->FirstChildElement("camera")->FirstChildElement("projection")->Attribute("near")),
                     stof(world->FirstChildElement("camera")->FirstChildElement("projection")->Attribute("far")));

    return Camera(position, up, lookAt, projection, 0, 0, 0);
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
            while(point)
            {
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

            Rotation *r = new Rotation(angle, time, x, y, z);
            transforms.addTransform(*r);
        }

        if (strcmp(transformation->Name(), "scale") == 0)
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

unsigned loadTexture(string filename) {
    unsigned int t, tw, th;
    unsigned char *texData;
    unsigned int texID;

    ilGenImages(1, &t);
    ilBindImage(t);
    ilLoadImage((ILstring)  (string("../test_files_phase_4/") + filename).c_str());
    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);

    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    texData = ilGetData();
    
    glGenTextures(1, &texID);

    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);     

    return texID; 
}

Models readModels(XMLNode* group) {
    Models models = Models();

    XMLElement* ms = group->FirstChildElement("models");
    if (!ms) {
        return models;
    }

    XMLElement* m = ms->FirstChildElement("model");

    while (m) {
        string filename = m->ToElement()->Attribute("file");
        ifstream file(filename);

        if (file.fail()) {
            string s = "File does not exist: " + filename;
            throw runtime_error(s);
        }
        int nrVertices;
        file >> nrVertices;
        file.ignore();

        vector<Point> points;
        vector<Point> normals;
        vector<Point2D> textures;
        

        for (int i = 0; i < nrVertices; i++) {
            string line;
            getline(file, line);

            istringstream ss(line);
            string pointStr, normalStr, textureStr;

            getline(ss, pointStr, ';');
            getline(ss, normalStr, ';');
            getline(ss, textureStr, ';');

            istringstream pointSS(pointStr);
            istringstream normalSS(normalStr);
            istringstream textureSS(textureStr);

            float px, py, pz, nx, ny, nz, tx, ty;

            pointSS >> px >> py >> pz;
            normalSS >> nx >> ny >> nz;
            textureSS >> tx >> ty;

            points.push_back(Point(px, py, pz));
            normals.push_back(Point(nx, ny, nz));
            textures.push_back(Point2D(tx, ty));
        }

        XMLElement* texture = m->FirstChildElement("texture");
        string t_filename = "";
        unsigned int textureID = 0;
        if (texture) {
            t_filename = texture->ToElement()->Attribute("file");

            textureID = loadTexture(t_filename);
        }

        file.close();

        Color c = Color();

        XMLElement* color = m->FirstChildElement("color");
        if (color) {
            XMLElement* type = color->FirstChildElement();
            while (type) {
                float r = 0, g = 0, b = 0;
                if (!strcmp(type->Name(), "diffuse")) {
                    type->QueryFloatAttribute("R", &r);
                    type->QueryFloatAttribute("G", &g);
                    type->QueryFloatAttribute("B", &b);
                    c.setDiffuse(r, g, b);
                } else if (!strcmp(type->Name(), "ambient")) {
                    type->QueryFloatAttribute("R", &r);
                    type->QueryFloatAttribute("G", &g);
                    type->QueryFloatAttribute("B", &b);
                    c.setAmbient(r, g, b);
                } else if (!strcmp(type->Name(), "specular")) {
                    type->QueryFloatAttribute("R", &r);
                    type->QueryFloatAttribute("G", &g);
                    type->QueryFloatAttribute("B", &b);
                    c.setSpecular(r, g, b);
                } else if (!strcmp(type->Name(), "emissive")) {
                    type->QueryFloatAttribute("R", &r);
                    type->QueryFloatAttribute("G", &g);
                    type->QueryFloatAttribute("B", &b);
                    c.setEmissive(r, g, b);
                } else if (!strcmp(type->Name(), "shininess")) {
                    float s = 0;
                    type->QueryFloatAttribute("value", &s);
                    c.setShininess(s);
                }
                type = type->NextSiblingElement();
            }
        }

        GLuint pointsVBO, normalsVBO = 0, texturesVBO = 0;
        
        // Create and bind VBO for points
        glGenBuffers(1, &pointsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_STATIC_DRAW);

        // Create and bind VBO for normals if they exist
        glGenBuffers(1, &normalsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Point), normals.data(), GL_STATIC_DRAW);

        // Create and bind VBO for textures if they exist
        glGenBuffers(1, &texturesVBO);
        glBindBuffer(GL_ARRAY_BUFFER, texturesVBO);
        glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(Point2D), textures.data(), GL_STATIC_DRAW);

        // Add the model to the models container
        models.addModel(Model(filename, points, normals, pointsVBO, normalsVBO, texturesVBO, textureID, c));

        m = m->NextSiblingElement("model");
    }

    return models;
}

Group readGroups(XMLNode *group)
{
    Transforms t = readTransforms(group);
    Models m = readModels(group);
    vector<Group> groups = vector<Group>();
    
    XMLElement *subGroup = group->FirstChildElement("group");
    while (subGroup)
    {
        groups.push_back(readGroups(subGroup));
        subGroup = subGroup->NextSiblingElement("group");
    }

    return Group(t, m, groups);
}

Lights readLights(XMLNode *root) {

    Lights ls = Lights();

    XMLElement *lights = root->FirstChildElement("lights");
    if (!lights) return ls;

    XMLElement *light = lights->FirstChildElement("light");

    while(light) {
        int id = ls.getLights().size();

        if(!strcmp(light->Attribute("type"), "point")) {
            float posx, posy, posz;
            light->QueryFloatAttribute("posx", &posx);
            light->QueryFloatAttribute("posy", &posy);
            light->QueryFloatAttribute("posz", &posz);

            PointLight *pl = new PointLight(id, Point(posx, posy, posz));
            ls.addLight(pl);
        }
        
        if(!strcmp(light->Attribute("type"), "directional")) {
            float dirx, diry, dirz;
            light->QueryFloatAttribute("dirx", &dirx);
            light->QueryFloatAttribute("diry", &diry);
            light->QueryFloatAttribute("dirz", &dirz);
       
            DirectionalLight *dl = new DirectionalLight(id, Point(dirx, diry, dirz));
            ls.addLight(dl);
        }
        
        if(!strcmp(light->Attribute("type"), "spot")) {
            float posx, posy, posz, dirx, diry, dirz, cutoff;
            light->QueryFloatAttribute("posx", &posx);
            light->QueryFloatAttribute("posy", &posy);
            light->QueryFloatAttribute("posz", &posz);
            light->QueryFloatAttribute("dirx", &dirx);
            light->QueryFloatAttribute("diry", &diry);
            light->QueryFloatAttribute("dirz", &dirz);
            light->QueryFloatAttribute("cutoff", &cutoff);

            SpotLight *sl = new SpotLight(id, Point(posx, posy, posz), Point(dirx, diry, dirz), cutoff);
            ls.addLight(sl);
        }

        light = light->NextSiblingElement();
    }

    return ls;
}

Window readWindow(XMLNode *world)
{
    if(!world->FirstChildElement("window")) {
        return Window();
    }

    int width = world->FirstChildElement("window")->IntAttribute("width");
    int height = world->FirstChildElement("window")->IntAttribute("height");

    return Window(width, height);
}

World readXML(const char *path)
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

    Window window = readWindow(world);
    Camera camera = readCamera(world);
    Lights lights = readLights(world);
    Group g = readGroups(group); 


    return World(window, camera, lights, g);
}