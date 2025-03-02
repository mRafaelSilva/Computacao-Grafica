#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>


using namespace std;


void writeVertices(const string& filename, const vector<float>& vertices) {
    ofstream file(filename);
    if (!file) {
        cerr << "Erro a criar o ficheiro" << endl;
        return;
    }

    file << vertices.size() / 3 << endl;
    for (size_t i = 0; i < vertices.size(); i += 3) {
        file << vertices[i] << " " << vertices[i + 1] << " " << vertices[i + 2] << endl;
    }

    file.close();
    cout << "O ficheiro " << filename << " foi criado" << endl;
}

void generateBox(float length, int subdivisions, const string& filename) {
    vector<float> vertices;
    float step = length / subdivisions;
    float half = length / 2;

    for (int i = 0; i < subdivisions; i++) {
        for (int j = 0; j < subdivisions; j++) {
            float x1 = -half + j * step;
            float y1 = -half + i * step;
            float x2 = x1 + step;
            float y2 = y1;
            float x3 = x1;
            float y3 = y1 + step;
            float x4 = x2;
            float y4 = y3;

           
            vertices.insert(vertices.end(), {x1, y1, half, x2, y2, half, x3, y3, half});
            vertices.insert(vertices.end(), {x3, y3, half, x2, y2, half, x4, y4, half});
            
            
            vertices.insert(vertices.end(), {x1, y1, -half, x3, y3, -half, x2, y2, -half});
            vertices.insert(vertices.end(), {x3, y3, -half, x4, y4, -half, x2, y2, -half});
            
            
            vertices.insert(vertices.end(), {-half, y1, x1, -half, y2, x2, -half, y3, x3});
            vertices.insert(vertices.end(), {-half, y3, x3, -half, y2, x2, -half, y4, x4});
            
            
            vertices.insert(vertices.end(), {half, y1, x1, half, y3, x3, half, y2, x2});
            vertices.insert(vertices.end(), {half, y3, x3, half, y4, x4, half, y2, x2});

            
            vertices.insert(vertices.end(), {x1, half, y1, x3, half, y3, x2, half, y2});
            vertices.insert(vertices.end(), {x3, half, y3, x4, half, y4, x2, half, y2});
            
            
            vertices.insert(vertices.end(), {x1, -half, y1, x2, -half, y2, x3, -half, y3});
            vertices.insert(vertices.end(), {x3, -half, y3, x2, -half, y2, x4, -half, y4});
        }
    }

    writeVertices(filename, vertices);
}


void generatePlane(float length, int subdivisions, const string& filename) {    

    vector<float> vertices;
    float step = length / subdivisions;  
    float half = length / 2;            

    for (int i = 0; i < subdivisions; i++) {
        for (int j = 0; j < subdivisions; j++) {
            
            float x1 = -half + j * step;
            float z1 = -half + i * step;
            float x2 = x1 + step;
            float z2 = z1;
            float x3 = x1;
            float z3 = z1 + step;
            float x4 = x2;
            float z4 = z3;


            
            vertices.push_back(x1); vertices.push_back(0); vertices.push_back(z1);
            vertices.push_back(x3); vertices.push_back(0); vertices.push_back(z3);
            vertices.push_back(x2); vertices.push_back(0); vertices.push_back(z2);
            
            vertices.push_back(x3); vertices.push_back(0); vertices.push_back(z3);
            vertices.push_back(x4); vertices.push_back(0); vertices.push_back(z4);
            vertices.push_back(x2); vertices.push_back(0); vertices.push_back(z2);
        }
    }

    writeVertices(filename, vertices);
}




void generateSphere(float radius, int slices, int stacks, const string& filename) {
    vector<float> vertices;
    
    for (int i = 0; i < stacks; i++) {
        float theta1 = M_PI * i / stacks;
        float theta2 = M_PI * (i + 1) / stacks;

        for (int j = 0; j < slices; j++) {
            float phi1 = 2 * M_PI * j / slices;
            float phi2 = 2 * M_PI * (j + 1) / slices;

            float x1 = radius * sin(theta1) * cos(phi1);
            float y1 = radius * cos(theta1);
            float z1 = radius * sin(theta1) * sin(phi1);

            float x2 = radius * sin(theta1) * cos(phi2);
            float y2 = radius * cos(theta1);
            float z2 = radius * sin(theta1) * sin(phi2);

            float x3 = radius * sin(theta2) * cos(phi1);
            float y3 = radius * cos(theta2);
            float z3 = radius * sin(theta2) * sin(phi1);

            float x4 = radius * sin(theta2) * cos(phi2);
            float y4 = radius * cos(theta2);
            float z4 = radius * sin(theta2) * sin(phi2);

            // Triângulo 1
            vertices.push_back(x2); vertices.push_back(y1); vertices.push_back(z2);
            vertices.push_back(x3); vertices.push_back(y3); vertices.push_back(z3);
            vertices.push_back(x1); vertices.push_back(y2); vertices.push_back(z1);

            // Triângulo 2
            vertices.push_back(x2); vertices.push_back(y2); vertices.push_back(z2);
            vertices.push_back(x4); vertices.push_back(y3); vertices.push_back(z4);
            vertices.push_back(x3); vertices.push_back(y4); vertices.push_back(z3);
        }
    }

    writeVertices(filename, vertices);
}


void generateCone(float radius, float height, int slices, int stacks, const string& filename) {
    vector<float> vertices;
    float angleStep = 2 * M_PI / slices;
    float stackHeight = height / stacks;

    // Base 
    for (int i = 0; i < slices; i++) {
        float theta1 = i * angleStep;
        float theta2 = (i + 1) * angleStep;

        float x1 = radius * cos(theta1);
        float z1 = radius * sin(theta1);
        float x2 = radius * cos(theta2);
        float z2 = radius * sin(theta2);

        
        vertices.push_back(0);  vertices.push_back(0);  vertices.push_back(0);
        vertices.push_back(x1); vertices.push_back(0);  vertices.push_back(z1);
        vertices.push_back(x2); vertices.push_back(0);  vertices.push_back(z2);
    }

    // Laterais do cone 
    for (int i = 0; i < stacks; i++) {
        float r1 = radius * (1 - (float)i / stacks);
        float r2 = radius * (1 - (float)(i + 1) / stacks);
        float y1 = i * stackHeight;
        float y2 = (i + 1) * stackHeight;

        for (int j = 0; j < slices; j++) {
            float theta1 = j * angleStep;
            float theta2 = (j + 1) * angleStep;

            float x1 = r1 * cos(theta1);
            float z1 = r1 * sin(theta1);
            float x2 = r1 * cos(theta2);
            float z2 = r1 * sin(theta2);
            float x3 = r2 * cos(theta1);
            float z3 = r2 * sin(theta1);
            float x4 = r2 * cos(theta2);
            float z4 = r2 * sin(theta2);

            // Triângulo inferior 
            vertices.push_back(x1); vertices.push_back(y1); vertices.push_back(z1);
            vertices.push_back(x3); vertices.push_back(y2); vertices.push_back(z3);
            vertices.push_back(x2); vertices.push_back(y1); vertices.push_back(z2);

            // Triângulo superior 
            vertices.push_back(x2); vertices.push_back(y1); vertices.push_back(z2);
            vertices.push_back(x3); vertices.push_back(y2); vertices.push_back(z3);
            vertices.push_back(x4); vertices.push_back(y2); vertices.push_back(z4);
        }
    }

    // Ligar a última camada ao topo
    float yTop = height;
    float lastRadius = radius * (1 - (float)(stacks - 1) / stacks); 

    for (int i = 0; i < slices; i++) {
        float theta1 = i * angleStep;
        float theta2 = (i + 1) * angleStep;

        float x1 = lastRadius * cos(theta1);
        float z1 = lastRadius * sin(theta1);
        float x2 = lastRadius * cos(theta2);
        float z2 = lastRadius * sin(theta2);

        vertices.push_back(0);  vertices.push_back(yTop); vertices.push_back(0);
        vertices.push_back(x2); vertices.push_back(height - stackHeight); vertices.push_back(z2);
        vertices.push_back(x1); vertices.push_back(height - stackHeight); vertices.push_back(z1);
    }

    writeVertices(filename, vertices);
}



int main(int argc, char* argv[]) {
    if ((argc != 5) && (argc != 6) && (argc != 7)) {
        cerr << "erro no input" << endl;
        return 1;
    }

    string type = argv[1];

    if (type == "plane") {
        float length = stof(argv[2]);
        int subdivisions = stoi(argv[3]);
        string filename = argv[4];

        generatePlane(length, subdivisions, filename);
    }

    else if (type == "box") {
        float length = stof(argv[2]);
        int subdivisions = stoi(argv[3]);
        string filename = argv[4];

        generateBox(length,subdivisions,filename);
    }

    else if (type == "sphere") {
        float raio = stof(argv[2]);
        int fatias = stoi(argv[3]);
        int stacks = stoi(argv[4]);
        string filename = argv[5];

        generateSphere(raio,fatias,stacks,filename);
    }

    else if (type == "cone") {
        float radius = stof(argv[2]);
        float height = stof(argv[3]);
        int slices = stoi(argv[4]);
        int stacks = stoi(argv[5]);
        string filename = argv[6];
    
        generateCone(radius, height, slices, stacks, filename);
    }
    
    else {
        cerr << "Tipo de figura inválido" << endl;
    }

    return 0;
}   
