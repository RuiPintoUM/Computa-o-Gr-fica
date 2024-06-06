#include <GL/glew.h>
#include <GL/glut.h>
#include "structures.hpp"
#include <cmath>

static int eixos = 1;
double timebase, elapsed = 0;
Camera cam;

using namespace std;

World WorldXML = World();


void drawTriangle(const Point& p1, const Point& p2, const Point& p3)
{
    printf("Drawing triangle at the following points:\n");
    glBegin(GL_TRIANGLES);
    glVertex3f(p1.getX(), p1.getY(), p1.getZ());
    glVertex3f(p2.getX(), p2.getY(), p2.getZ());
    glVertex3f(p3.getX(), p3.getY(), p3.getZ());
    glEnd();
}


int calculateFigCount(Group& group) {
    int count = 0;
    
    for (Model& model : group.getModels().getModels()) {
        count += 1;
    }
    for (Group& nestedGroup : group.getGroups()) {
        count += calculateFigCount(nestedGroup);
    }
    
    return count;
}

int calculateFigCount() {
    Group& rootGroup = WorldXML.getGroup();
    return calculateFigCount(rootGroup);
}

void processGroup(Group& group, GLuint* bufferIds, int& index) {
    int i = 0;
    for (Group& subgroup : group.getGroups()) {
        i++;
        processGroup(subgroup, bufferIds, index);
    }

    for (Model& model : group.getModels().getModels()) {
        vector<Point>* points = model.getPoints();
        if (points->empty()) {
            cerr << "Model has no points. Skipping model." << endl;
            continue;
        }
        vector<float> vertices;
        for (const Point& p : *points) {
            vertices.push_back(p.getX());
            vertices.push_back(p.getY());
            vertices.push_back(p.getZ());
        }

        glBindBuffer(GL_ARRAY_BUFFER, bufferIds[index]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        model.setBufferID(bufferIds[index]);
        model.setPointCount(vertices.size() / 3);
        //cout << "Buffer ID[" << index << "]: " << bufferIds[index] << endl;
        index++;
    }
}

void initVBOs() {
    int figCount = calculateFigCount();
    //printf("Calculating fig count: %d\n", figCount);
    GLuint* bufferIds = new GLuint[figCount];
    glGenBuffers(figCount, bufferIds);

    //cout << "Generated Buffer IDs:" << endl;
    int index = 0;

    Group& rootGroup = WorldXML.getGroup();
    processGroup(rootGroup, bufferIds, index);

    delete[] bufferIds;
}

void drawModel(const Model& model) {
    glBindBuffer(GL_ARRAY_BUFFER, model.getBufferID());
    glVertexPointer(3, GL_FLOAT, 0, NULL);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_TRIANGLES, 0, model.getPointCount());
    glDisableClientState(GL_VERTEX_ARRAY); 
}

void drawGroup(Group& group){
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);

    for(Transform* transform : group.getTransforms().getTransforms()) {
        transform->execute(elapsed);
    }

    for (Model& model : group.getModels().getModels()) {
        drawModel(model);
    }

    for(Group& subGroup : group.getGroups()){
        drawGroup(subGroup);  
    }

    glPopMatrix();
}

void draw_axis() {
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f); // X axis in red
    glVertex3f(-1500.0f, 0.0f, 0.0f);
    glVertex3f(1500.0f, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f); // Y axis in green
    glVertex3f(0.0f, -1500.0f, 0.0f);
    glVertex3f(0.0f, 1500.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f); // Z axis in blue
    glVertex3f(0.0f, 0.0f, -1500.0f);
    glVertex3f(0.0f, 0.0f, 1500.0f);

    glColor3f(255.0f, 255.0f, 255.0f);    
    glEnd();
}

void changeSize(int w, int h) {
    if (h == 0) h = 1; // Prevent a divide by zero
    float ratio = w * 1.0 / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);

    Point projection = WorldXML.getCamera().getProjection();
    gluPerspective(projection.getX(), ratio, projection.getY(), projection.getZ());
    glMatrixMode(GL_MODELVIEW);
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //Camera cam = WorldXML.getCamera();
    cam.moveCamera();
    Point position = cam.getPosition();
    Point lookAt = cam.getLookAt();
    Point up = cam.getUp();

    gluLookAt(position.getX(), position.getY(), position.getZ(), 
              lookAt.getX(), lookAt.getY(), lookAt.getZ(),
              up.getX(), up.getY(), up.getZ());

    if (eixos) draw_axis();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Group& rootGroup = WorldXML.getGroup();
    drawGroup(rootGroup);

    double time = glutGet(GLUT_ELAPSED_TIME);
    elapsed = (time - timebase) / 1000.0;

    glutSwapBuffers();
    glutPostRedisplay();
}

void processKeys(unsigned char c, int xx, int yy) {
    switch (c) {
        case '+': cam.setRadius(cam.getRadius() - 2); break;
        case '-': cam.setRadius(cam.getRadius() + 2); break;
        case '.': eixos = 1 - eixos; break;
        default: break;
    }

    if (cam.getRadius() <= 0) cam.setRadius(1);
    if (cam.getRadius() >= 500) cam.setRadius(500);
}

void processSpecialKeys(int key, int xx, int yy) {
    switch (key) {
        case GLUT_KEY_UP: cam.setBetha(cam.getBetha() + 0.1); break;
        case GLUT_KEY_DOWN: cam.setBetha(cam.getBetha() - 0.1); break;
        case GLUT_KEY_LEFT: cam.setAlpha(cam.getAlpha() - 0.1); break;
        case GLUT_KEY_RIGHT: cam.setAlpha(cam.getAlpha() + 0.1); break;
        default: break;
    }

    if (cam.getBetha() >= 1.5) cam.setBetha(1.5);
    if (cam.getBetha() <= -1.5) cam.setBetha(-1.5);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <XML configuration file>" << endl;
        return 0;
    }

    string filename = argv[1];
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG@DI-UM");

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        cerr << "Error:" << glewGetErrorString(err) << endl;
        return EXIT_FAILURE;
    }

    WorldXML = loadXML(filename.c_str());

    initVBOs();

    cam = WorldXML.getCamera();
    cam.convertToSpherical(); 

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    timebase = glutGet(GLUT_ELAPSED_TIME);

    glutMainLoop();

    return 1;
}

