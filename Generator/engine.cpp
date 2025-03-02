#include <GL/glut.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

std::vector<float> vertices;
std::vector<std::string> modelos;

struct Camera {
    float posX, posY, posZ;
    float lookAtX, lookAtY, lookAtZ;
    float upX, upY, upZ;
    float fov, near, far;
} camera;

int largura;
int altura;


void loadModel(const char* filename);

void parsexml(const char* file) {
    XMLDocument doc;
    if (doc.LoadFile(file) != XML_SUCCESS) {
        std::cerr << "Erro no XML" << std::endl;
        return;
    }
    XMLElement* first = doc.FirstChildElement("world");
    if (!first) return;

    XMLElement* windowElem = first->FirstChildElement("window");
    if (windowElem) {
        windowElem->QueryIntAttribute("width", &largura);
        windowElem->QueryIntAttribute("height", &altura);
    }
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
            proj->QueryFloatAttribute("near", &camera.near);
            proj->QueryFloatAttribute("far", &camera.far);
        }
    }

    XMLElement* groupElem = first->FirstChildElement("group");
    if (groupElem) {
        XMLElement* modelsElem = groupElem->FirstChildElement("models");
        if (modelsElem) {
            XMLElement* model = modelsElem->FirstChildElement("model");
            while (model) {
                const char* modelFile = model->Attribute("file");
                if (modelFile) {
                    modelos.push_back(modelFile);
                    loadModel(modelFile);
                }
                model = model->NextSiblingElement("model");
            }
        }
    }
}

void loadModel(const char* filename) {
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cerr << "Erro ao abrir o ficheiro " << filename << endl;
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
    gluPerspective(camera.fov, ratio, camera.near, camera.far);
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camera.posX, camera.posY, camera.posZ,
        camera.lookAtX, camera.lookAtY, camera.lookAtZ,
        camera.upX, camera.upY, camera.upZ);

    glDisable(GL_DEPTH_TEST);
    drawAxes();
    glEnable(GL_DEPTH_TEST);

    glColor3f(1.0f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < vertices.size(); i += 3) {
        glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    parsexml("config.xml");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(largura, altura);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Figuras 3D");

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glutDisplayFunc(display);
    glutReshapeFunc(changeSize);

    glutMainLoop();
    return 0;
}
