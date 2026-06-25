#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include "../MusicClass/music.hpp"
#include "../B-treeClass/btree.hpp"


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
    std::string caminhoCSV = "B-treeAPI/tcc_ceds_music.csv";
    std::ifstream file(caminhoCSV);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o CSV." << std::endl;
        return 1;
    }

    std::string line;
    if (!std::getline(file, line)) return 1;

    FILE *datain = fopen(FILE_NAME, "wb");
    if (!datain) return 1;

    std::cout << "Convertendo CSV para registros de tamanho fixo..." << std::endl;

    int contador_id = 0;

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
        std::string url = "https://spotify.example.com/" + std::to_string(idaux);
        std::strncpy(record.url, url.c_str(), 199);
        std::strncpy(record.genre, campos[4].c_str(), 49);

        record.duration_ms = 180000.0f;
        try { record.duration_ms = std::stof(campos[6]) * 1000.0f; } catch(...) {}
        record.popularity = 50.0f;
        record.album_id = 0;
        record.id = idaux;
        record.rrn = idaux;

        fwrite(&record, sizeof(MusicRecord), 1, datain);
        idaux++;
        contador_id++;
    }

    file.close();
    fclose(datain);
    std::cout << "Sucesso! Criado arquivo binario estável com " << contador_id << " musicas." << std::endl;
    return 0;
}
//     g++ -Wall -Wextra -g3 -I"s:\Codigos\ORI-Gerente-de-Musicas\MusicClass" "s:\Codigos\ORI-Gerente-de-Musicas\B-treeAPI\conversor.cpp" "s:\Codigos\ORI-Gerente-de-Musicas\MusicClass\music.cpp" "s:\Codigos\ORI-Gerente-de-Musicas\databaseClass\database.cpp" -o "s:\Codigos\ORI-Gerente-de-Musicas\B-treeAPI\conversor.exe"