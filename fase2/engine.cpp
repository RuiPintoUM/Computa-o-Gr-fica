#include <GL/glut.h>
#include "structures.hpp"
#include <math.h>

static int eixos = 1;
static float alpha, betha, radius;

World WorldXML = World();

void drawTriangle(Point p1, Point p2, Point p3)
{
    glBegin(GL_TRIANGLES);
    glVertex3f(p1.getX(), p1.getY(), p1.getZ());
    glVertex3f(p2.getX(), p2.getY(), p2.getZ());
    glVertex3f(p3.getX(), p3.getY(), p3.getZ());
    glEnd();
}

void drawModel(Model model)
{
    for (int i = 0; i < model.getPoints()->size(); i += 3) {
        drawTriangle(model.getPoints()->at(i), model.getPoints()->at(i + 1), model.getPoints()->at(i + 2));
    }
}


void drawGroup(Group group){
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);

    // Transformations
    for(int i = 0; i < group.getTransforms().getTransforms().size(); i++) {
        group.getTransforms().getTransforms().at(i)->execute();
    }

    //Models
    for (Model m : group.getModels().getModels()) {
        drawModel(m);
    }

    for(Group g: group.getGroups()){
        drawGroup(g);  
    }

    glPopMatrix();
}

void moveCamera(){
    Point position = WorldXML.getCamera().getPosition();

    position.setX(radius * cos(betha) * sin(alpha));
    position.setY(radius * sin(betha));
    position.setZ(radius * cos(betha) * cos(alpha));   
}

void convertToSpherical() {
    Point position = WorldXML.getCamera().getPosition();
    Point lookAt = WorldXML.getCamera().getLookAt();

    if (position.getX() == 0) position.setX(0.0000001);
    if (position.getY() == 0) position.setY(0.0000001);
    if (position.getZ() == 0) position.setZ(0.0000001);

    radius = sqrt(pow(position.getX() - lookAt.getX(), 2) + pow(position.getY() - lookAt.getY(), 2) + pow(position.getZ() - lookAt.getZ(), 2));
    alpha = atan2(position.getX(), position.getZ());
    betha = asin((position.getY() - lookAt.getY()) / radius);

    if (betha >= M_PI / 2) betha -= M_PI;
    if (betha <= -M_PI / 2) betha += M_PI;
}

void draw_axis(void)
{
    glBegin(GL_LINES);
    // X axis in red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-1500.0f, 0.0f, 0.0f);
    glVertex3f(1500.0f, 0.0f, 0.0f);
    
    // Y axis in green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -1500.0f, 0.0f);
    glVertex3f(0.0f, 1500.0f, 0.0f);
    
    // Z axis in blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -1500.0f);
    glVertex3f(0.0f, 0.0f, 1500.0f);

    glColor3f(255.0f, 255.0f, 255.0f);
        
    glEnd();
}

void changeSize(int w, int h) {
    Point projection = WorldXML.getCamera().getProjection();

    if (h == 0)
        h = 1;

    float ratio = w * 1.0 / h;

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glViewport(0, 0, w, h);

    gluPerspective(projection.getX(), 
                   ratio, 
                   projection.getY(), 
                   projection.getZ());

    glMatrixMode(GL_MODELVIEW);
}

void renderScene()
{
    Point position = WorldXML.getCamera().getPosition();
    Point lookAt = WorldXML.getCamera().getLookAt();
    Point up = WorldXML.getCamera().getUp();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    moveCamera();
    gluLookAt(position.getX(), position.getY(), position.getZ(), 
              lookAt.getX(), lookAt.getY(), lookAt.getZ(),
              up.getX(), up.getY(), up.getZ());

    if (eixos)
        draw_axis();
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    drawGroup(WorldXML.getGroup());

    glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {

    //em falta

    glutPostRedisplay();
}


int main(int argc, char **argv) {

    if (argc != 2) {
        return 0;
    }

    std::string filename = std::string("../test_files_phase_2/") + argv[1];

    WorldXML = loadXML(filename.c_str());
    //WorldXML.printWorld();
    
    convertToSpherical();

    // init GLUT and the window 
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG@DI-UM");

    // Required callback registry
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    // Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);

    //  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // enter GLUT's main cycle
    glutMainLoop();

    return 1;
}