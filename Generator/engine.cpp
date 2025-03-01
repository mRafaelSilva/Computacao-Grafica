#include <GL/glut.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<float> vertices;

void loadModel(const char* filename) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << filename << std::endl;
        return;
    }

    bool firstLine = true;
    while (std::getline(file, line)) {
        if (firstLine) {  // Ignorar primeira linha
            firstLine = false;
            continue;
        }
        std::istringstream iss(line);
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
    float ratio = w * 1.0 / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(2.5f, 2.5f, 6.0f,
              0.0f, 0.0f, 0.0f, 
              0.0f, 1.0f, 0.0f);

    // Desenha os eixos ANTES do cubo
    glDisable(GL_DEPTH_TEST); // Para garantir que os eixos são sempre visíveis
    drawAxes();
    glEnable(GL_DEPTH_TEST);  // Reativa profundidade para o cubo
    
    // Desenha o modelo carregado
    glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para as linhas
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
    loadModel("C:/Users/mrafa/OneDrive/Ambiente de Trabalho/Uni/CG/projeto/Generator/build/Release/cone.3d");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 800);
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