#include <GL/glut.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

// Vetores globais para os vértices e nomes dos arquivos dos modelos
std::vector<float> vertices;
std::vector<std::string> modelFiles;

// Estrutura para os parâmetros da câmera
struct Camera {
    float posX, posY, posZ;
    float lookAtX, lookAtY, lookAtZ;
    float upX, upY, upZ;
    float fov, nearc, farc;
} camera;

// Variáveis globais para a janela
int width;
int height;

// Declaração antecipada para que parsexml possa chamar loadModel
void loadModel(const char* filename);

void parsexml(const char* file) {
    XMLDocument doc;
    if (doc.LoadFile(file) != XML_SUCCESS) {
        std::cerr << "Error loading XML file!" << std::endl;
        return;
    }
    XMLElement* first = doc.FirstChildElement("world");
    if (!first) return;

    XMLElement* windowElem = first->FirstChildElement("window");
    if (windowElem) {
        windowElem->QueryIntAttribute("width", &width);
        windowElem->QueryIntAttribute("height", &height);
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
            proj->QueryFloatAttribute("near", &camera.nearc);
            proj->QueryFloatAttribute("far", &camera.farc);
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
                    modelFiles.push_back(modelFile);
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
        cerr << "Erro ao abrir o arquivo: " << filename << endl;
        return;
    }

    bool firstLine = true;
    while (getline(file, line)) {
        if (firstLine) {  // Ignorar a primeira linha
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
    glLineWidth(2.0f);  // Aumenta a espessura dos eixos

    glBegin(GL_LINES);
    // Eixo X - Vermelho
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-50.0f, 0.0f, 0.0f);
    glVertex3f(50.0f, 0.0f, 0.0f);

    // Eixo Y - Verde
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -50.0f, 0.0f);
    glVertex3f(0.0f, 50.0f, 0.0f);

    // Eixo Z - Azul
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

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camera.posX, camera.posY, camera.posZ,
        camera.lookAtX, camera.lookAtY, camera.lookAtZ,
        camera.upX, camera.upY, camera.upZ);

    // Desenha os eixos (garante visibilidade dos eixos)
    glDisable(GL_DEPTH_TEST);
    drawAxes();
    glEnable(GL_DEPTH_TEST);

    // Desenha o modelo carregado (modo linha)
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
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3D Engine");

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
