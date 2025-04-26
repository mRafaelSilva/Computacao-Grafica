#include <GL/glut.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

struct Model {
    string file;
    float color[3] = { 1.0f, 1.0f, 1.0f };
};

struct Rotation {
    float angle;
    float axis[3];
};

struct Transform {
    vector<Rotation> rotations;
    float translate[3] = { 0, 0, 0 };
    float scale[3] = { 1, 1, 1 };
    bool hasTranslate = false;
    bool hasScale = false;
};

struct Group {
    Transform transform;
    vector<Model> models;
    vector<Group> children;
};


std::vector<float> vertices;

struct Camera {
    float posX, posY, posZ;
    float lookAtX, lookAtY, lookAtZ;
    float upX, upY, upZ;
    float fov, nearc, farc;
} camera;


int width;
int height;

float camAngle = 0.0f;
float camDistance = 80.0f;


void loadModel(const char* filename);

Group parseGroup(XMLElement* groupElem) {
    Group group;

    XMLElement* transform = groupElem->FirstChildElement("transform");
    if (transform) {
        XMLElement* t = transform->FirstChildElement("translate");
        if (t) {
            group.transform.hasTranslate = true;
            t->QueryFloatAttribute("x", &group.transform.translate[0]);
            t->QueryFloatAttribute("y", &group.transform.translate[1]);
            t->QueryFloatAttribute("z", &group.transform.translate[2]);
        }

        XMLElement* r = transform->FirstChildElement("rotate");
        while (r) {
            Rotation rot;
            r->QueryFloatAttribute("angle", &rot.angle);
            r->QueryFloatAttribute("x", &rot.axis[0]);
            r->QueryFloatAttribute("y", &rot.axis[1]);
            r->QueryFloatAttribute("z", &rot.axis[2]);
            group.transform.rotations.push_back(rot);
            r = r->NextSiblingElement("rotate");
        }

        XMLElement* s = transform->FirstChildElement("scale");
        if (s) {
            group.transform.hasScale = true;
            s->QueryFloatAttribute("x", &group.transform.scale[0]);
            s->QueryFloatAttribute("y", &group.transform.scale[1]);
            s->QueryFloatAttribute("z", &group.transform.scale[2]);
        }
    }

    XMLElement* models = groupElem->FirstChildElement("models");
    if (models) {
        XMLElement* modelElem = models->FirstChildElement("model");
        while (modelElem) {
            const char* file = modelElem->Attribute("file");
            if (file) {
                Model model;
                model.file = file;

                XMLElement* colorElem = modelElem->FirstChildElement("color");
                if (colorElem) {
                    colorElem->QueryFloatAttribute("r", &model.color[0]);
                    colorElem->QueryFloatAttribute("g", &model.color[1]);
                    colorElem->QueryFloatAttribute("b", &model.color[2]);
                }

                group.models.push_back(model);
            }
            modelElem = modelElem->NextSiblingElement("model");
        }
    }


    XMLElement* child = groupElem->FirstChildElement("group");
    while (child) {
        group.children.push_back(parseGroup(child));
        child = child->NextSiblingElement("group");
    }

    return group;
}

Group root;


void parsexml(const char* file) {
    XMLDocument doc;
    if (doc.LoadFile(file) != XML_SUCCESS) {
        std::cerr << "Erro ao abrir o ficheiro XML" << std::endl;
        return;
    }

    XMLElement* first = doc.FirstChildElement("world");
    if (!first) return;

    // Ler dimensões da janela
    XMLElement* windowElem = first->FirstChildElement("window");
    if (windowElem) {
        windowElem->QueryIntAttribute("width", &width);
        windowElem->QueryIntAttribute("height", &height);
    }

    // Ler definições da câmara
    XMLElement* camElem = first->FirstChildElement("camera");
    if (camElem) {
        XMLElement* pos = camElem->FirstChildElement("position");
        if (pos) {
            pos->QueryFloatAttribute("x", &camera.posX);
            pos->QueryFloatAttribute("y", &camera.posY);
            pos->QueryFloatAttribute("z", &camera.posZ);
        }
        XMLElement* lookAt = camElem->FirstChildElement("lookAt");
        if (lookAt) {
            lookAt->QueryFloatAttribute("x", &camera.lookAtX);
            lookAt->QueryFloatAttribute("y", &camera.lookAtY);
            lookAt->QueryFloatAttribute("z", &camera.lookAtZ);
        }
        XMLElement* up = camElem->FirstChildElement("up");
        if (up) {
            up->QueryFloatAttribute("x", &camera.upX);
            up->QueryFloatAttribute("y", &camera.upY);
            up->QueryFloatAttribute("z", &camera.upZ);
        }
        XMLElement* proj = camElem->FirstChildElement("projection");
        if (proj) {
            proj->QueryFloatAttribute("fov", &camera.fov);
            proj->QueryFloatAttribute("near", &camera.nearc);
            proj->QueryFloatAttribute("far", &camera.farc);
        }
    }

    XMLElement* groupElem = first->FirstChildElement("group");
    if (groupElem) {
        root = parseGroup(groupElem);
    }
}

void loadModel(const char* filename) {
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << filename << endl;
        return;
    }

    bool firstLine = true;
    while (getline(file, line)) {
        if (firstLine) {  
            firstLine = false;
            continue;
        }
        istringstream iss(line);
        float x, y, z;
        if (iss >> x >> y >> z) {
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    file.close();
}

void drawAxes() {
    glLineWidth(2.0f);  

    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-50.0f, 0.0f, 0.0f);
    glVertex3f(50.0f, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -50.0f, 0.0f);
    glVertex3f(0.0f, 50.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -50.0f);
    glVertex3f(0.0f, 0.0f, 50.0f);
    glEnd();
}

void changeSize(int w, int h) {
    if (h == 0) h = 1;
    float ratio = w * 1.0f / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(camera.fov, ratio, camera.nearc, camera.farc);
    glMatrixMode(GL_MODELVIEW);
}

void drawGroup(const Group& group) {
    glPushMatrix();

    // 1. Órbita

    if (!group.transform.rotations.empty())
        glRotatef(group.transform.rotations[0].angle,
            group.transform.rotations[0].axis[0],
            group.transform.rotations[0].axis[1],
            group.transform.rotations[0].axis[2]);


    // 2. Translação
    if (group.transform.hasTranslate)
        glTranslatef(group.transform.translate[0], 0.0f, group.transform.translate[2]);

    // 3. Inclinação 
    for (size_t i = 1; i < group.transform.rotations.size(); ++i)
        glRotatef(group.transform.rotations[i].angle,
            group.transform.rotations[i].axis[0],
            group.transform.rotations[i].axis[1],
            group.transform.rotations[i].axis[2]);

    // 4. Escala
    if (group.transform.hasScale)
        glScalef(group.transform.scale[0], group.transform.scale[1], group.transform.scale[2]);

    for (const Model& m : group.models) {
        loadModel(m.file.c_str());
        glColor3f(m.color[0], m.color[1], m.color[2]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < vertices.size(); i += 3)
            glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
        glEnd();
        vertices.clear();
    }

    for (const Group& child : group.children)
        drawGroup(child);

    glPopMatrix();


}


void processSpecialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        camAngle -= 0.15f;
        break;
    case GLUT_KEY_RIGHT:
        camAngle += 0.15f;
        break;
    case GLUT_KEY_UP:
        camDistance -= 20.0f;
        if (camDistance < 20.0f) camDistance = 10.0f;
        break;
    case GLUT_KEY_DOWN:
        camDistance += 20.0f;
        break;
    }
    glutPostRedisplay();
}



void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float camX = camDistance * sin(camAngle);
    float camZ = camDistance * cos(camAngle);
    gluLookAt(camX, camera.posY, camZ,
        camera.lookAtX, camera.lookAtY, camera.lookAtZ,
        camera.upX, camera.upY, camera.upZ);

    //drawAxes();

    drawGroup(root); 

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    parsexml("config.xml");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3D Engine");

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glutDisplayFunc(display);
    glutSpecialFunc(processSpecialKeys);
    glutReshapeFunc(changeSize);

    glutMainLoop();
    return 0;
}
