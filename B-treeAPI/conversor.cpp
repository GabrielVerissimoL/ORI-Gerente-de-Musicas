#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include "../MusicClass/music.hpp"
#include "../B-treeClass/btree.hpp"

// Estrutura espelho com tamanho fixo para salvar em disco com segurança
struct MusicRecord {
    char name[150];
    char singer[150];
    char album_name[150];
    char url[200];
    char genre[50];
    float duration_ms;
    float popularity;
    int album_id;
    int id;
    int rrn;
};

std::vector<std::string> parseCSVLine(const std::string& line) {
    std::vector<std::string> result;
    std::string current;
    bool inQuotes = false;
    for (char c : line) {
        if (c == '"') inQuotes = !inQuotes;
        else if (c == ',' && !inQuotes) {
            result.push_back(current);
            current.clear();
        } else {
            current.push_back(c);
        }
    }
    result.push_back(current);
    return result;
}

int main() {
    std::string caminhoCSV = "tcc_ceds_music.csv";
    std::ifstream file(caminhoCSV);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o CSV." << std::endl;
        return 1;
    }

    std::string line;
    if (!std::getline(file, line)) return 1;

    FILE *datain = fopen(FILE_NAME, "wb");
    if (!datain) return 1;

    int contador_id = 0;
    std::cout << "Convertendo CSV para registros de tamanho fixo..." << std::endl;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::vector<std::string> campos = parseCSVLine(line);
        if (campos.size() < 6) continue;

        MusicRecord record;
        std::memset(&record, 0, sizeof(MusicRecord));

        // Copia limitando o tamanho para não estourar os arrays
        std::strncpy(record.singer, campos[1].c_str(), 149);
        std::strncpy(record.name, campos[2].c_str(), 149);
        std::string album_name = "Album " + campos[3];
        std::strncpy(record.album_name, album_name.c_str(), 149);
        std::string url = "https://spotify.example.com/" + std::to_string(contador_id);
        std::strncpy(record.url, url.c_str(), 199);
        std::strncpy(record.genre, campos[4].c_str(), 49);

        record.duration_ms = 180000.0f;
        try { record.duration_ms = std::stof(campos[6]) * 1000.0f; } catch(...) {}
        record.popularity = 50.0f;
        record.album_id = 0;
        record.id = contador_id;
        record.rrn = contador_id;

        fwrite(&record, sizeof(MusicRecord), 1, datain);
        contador_id++;
    }

    file.close();
    fclose(datain);
    std::cout << "Sucesso! Criado arquivo binario estável com " << contador_id << " musicas." << std::endl;
    return 0;
}