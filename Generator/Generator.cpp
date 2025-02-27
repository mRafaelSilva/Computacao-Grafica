#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

void generatePlane(float length, int subdivisions, const string& filename) {
    ofstream file(filename);
    if (!file) {
        cerr << "Erro ao criar o ficheiro!" << endl;
        return;
    }

    vector<float> vertices;
    float step = length / subdivisions;  // Tamanho de cada subdivisão
    float half = length / 2;             // Para centralizar no (0,0)

    for (int i = 0; i < subdivisions; i++) {
        for (int j = 0; j < subdivisions; j++) {
            // Coordenadas dos 4 pontos da célula atual
            float x1 = -half + j * step;
            float z1 = -half + i * step;
            float x2 = x1 + step;
            float z2 = z1;
            float x3 = x1;
            float z3 = z1 + step;
            float x4 = x2;
            float z4 = z3;

            // Triângulo 1
            vertices.push_back(x1); vertices.push_back(0); vertices.push_back(z1);
            vertices.push_back(x2); vertices.push_back(0); vertices.push_back(z2);
            vertices.push_back(x3); vertices.push_back(0); vertices.push_back(z3);

            // Triângulo 2
            vertices.push_back(x2); vertices.push_back(0); vertices.push_back(z2);
            vertices.push_back(x4); vertices.push_back(0); vertices.push_back(z4);
            vertices.push_back(x3); vertices.push_back(0); vertices.push_back(z3);
        }
    }

    // Escrever os vértices no ficheiro
    file << vertices.size() / 3 << endl;
    for (size_t i = 0; i < vertices.size(); i += 3) {
        file << vertices[i] << " " << vertices[i + 1] << " " << vertices[i + 2] << endl;
    }

    file.close();
    cout << "Ficheiro " << filename << " criado com sucesso!" << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Uso: generator plane <comprimento> <subdivisoes> <ficheiro>" << endl;
        return 1;
    }

    string type = argv[1];

    if (type == "plane") {
        float length = stof(argv[2]);
        int subdivisions = stoi(argv[3]);
        string filename = argv[4];

        generatePlane(length, subdivisions, filename);
    }
    else {
        cerr << "Tipo inválido! Apenas suporta 'plane'." << endl;
    }

    return 0;
}
