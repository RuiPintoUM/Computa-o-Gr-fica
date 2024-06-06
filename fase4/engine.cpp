#include "structures.hpp"

static int eixos = 1, normals = 0;
double timebase, elapsed = 0;
Camera cam;

World worldXML = World();

void drawNormals(const Model& model) {
    vector<Point> vertices = model.getPoints();
    vector<Point> normals = model.getNormals();

    vector<float> data;
    float scale = 0.1f;

    for (size_t i = 0; i < vertices.size(); ++i) {
        const Point& vertex = vertices[i];
        const Point& normal = normals[i];

        data.push_back(vertex.getX());
        data.push_back(vertex.getY());
        data.push_back(vertex.getZ());

        data.push_back(vertex.getX() + normal.getX() * scale);
        data.push_back(vertex.getY() + normal.getY() * scale);
        data.push_back(vertex.getZ() + normal.getZ() * scale);
    }

    GLuint normalsVBO;
    glGenBuffers(1, &normalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    GLint previousVBO;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousVBO);

    GLboolean previousVertexArrayState;
    glGetBooleanv(GL_VERTEX_ARRAY, &previousVertexArrayState);
    if (!previousVertexArrayState) {
        glEnableClientState(GL_VERTEX_ARRAY);
    }

    glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glColor3f(1.0f, 0.0f, 0.0f); 

    glDrawArrays(GL_LINES, 0, vertices.size() * 2);

    glBindBuffer(GL_ARRAY_BUFFER, previousVBO);
    if (!previousVertexArrayState) {
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    glDeleteBuffers(1, &normalsVBO);
}

void drawModel(const Model& model) {
    // Color
    model.getColor().paint();

    // VBOs
    glBindBuffer(GL_ARRAY_BUFFER, model.getBufferID());
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, model.getBufferNormalID());
    glNormalPointer(GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, model.getBufferTextureID());
    glTexCoordPointer(2, GL_FLOAT, 0, 0);

    glBindTexture(GL_TEXTURE_2D, model.getTextureID());

    glEnable(GL_LIGHTING);
    glDrawArrays(GL_TRIANGLES, 0, model.getPoints().size());
    glDisable(GL_LIGHTING);
    
    glBindTexture(GL_TEXTURE_2D, 0);

    if (normals)
        drawNormals(model);

}

void drawGroup(Group group){
    float white[4] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
    glPushMatrix();

    // Transformations
    for(int i = 0; i < group.getTransforms().getTransforms().size(); i++) {        
        group.getTransforms().getTransforms().at(i)->execute(elapsed);
    }

    //Models
    for (Model model : group.getModels().getModels()) {
        drawModel(model);
    }    

    for(Group g: group.getGroups()){
        drawGroup(g);  
    }

    glPopMatrix();
    
}

void draw_axis(void) {
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
    if (h == 0)
        h = 1;

    float ratio = w * 1.0 / h;

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glViewport(0, 0, w, h);

    gluPerspective(cam.getProjection().getX(), 
                   ratio, 
                   cam.getProjection().getY(), 
                   cam.getProjection().getZ());

    glMatrixMode(GL_MODELVIEW);
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (eixos)
        draw_axis();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    cam.moveCamera();

    gluLookAt(cam.getPosition().getX(), cam.getPosition().getY(), cam.getPosition().getZ(), 
              cam.getLookAt().getX(), cam.getLookAt().getY(), cam.getLookAt().getZ(),
              cam.getUp().getX(), cam.getUp().getY(), cam.getUp().getZ());

    for(auto light : worldXML.getLights().getLights()) {
        light->execute();
    }

    drawGroup(worldXML.getGroup());

    double time = glutGet(GLUT_ELAPSED_TIME);
    elapsed = (time - timebase) / 1000.0;

    glutSwapBuffers();

    glutPostRedisplay();
}

void renderLights() {
    int lightSize = (int) worldXML.getLights().getLights().size();

    float dark[4] = {0.2, 0.2, 0.2, 1.0};
    float white[4] = {1.0, 1.0, 1.0, 1.0};

    for(int i=0; i<lightSize; i++) {
        glEnable(GL_LIGHT0 + i);
        glLightfv(GL_LIGHT0 + i, GL_AMBIENT, dark);
        glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, white);
        glLightfv(GL_LIGHT0 + i, GL_SPECULAR, white);
    }
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, white);
}

void processKeys(unsigned char c, int xx, int yy) {
    switch (c) {
        case '+':
            cam.setRadius(cam.getRadius() - 2);
            break;
        case '-':
            cam.setRadius(cam.getRadius() + 2);
            break;
        case '.':
            eixos = 1 - eixos;
            break;
        case ',':
            normals = 1 - normals;
            break;
        default:
            break;
    }
    
    if (cam.getRadius() <= 0) cam.setRadius(1);
    if (cam.getRadius() >= 500) cam.setRadius(500);
}

void processSpecialKeys(int key, int xx, int yy) {
    switch (key) {
        case GLUT_KEY_UP:
            cam.setBetha(cam.getBetha() + 0.1);
            break;
        case GLUT_KEY_DOWN:
            cam.setBetha(cam.getBetha() - 0.1);
            break;
        case GLUT_KEY_LEFT:
            cam.setAlpha(cam.getAlpha() - 0.1);
            break;
        case GLUT_KEY_RIGHT:
            cam.setAlpha(cam.getAlpha() + 0.1);
            break;
        default:
            break;
    }
    
    if (cam.getBetha() >= 1.5) cam.setBetha(1.5);
    if (cam.getBetha() <= -1.5) cam.setBetha(-1.5);
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Not enough arguments\n");
        return 0;
    }
    
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
    glutSpecialFunc(processSpecialKeys);
    
    // Init glew
    glewInit();
    ilInit();

    //  OpenGL settings
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_TEXTURE_2D);
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

    string filename = argv[1];
    worldXML = readXML(filename.c_str());

    cam = worldXML.getCamera();
    cam.convertToSpherical();

    renderLights();

    timebase = glutGet(GLUT_ELAPSED_TIME);

    glutMainLoop();

    return 1;
}