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
    std::string caminhoCSV = "spotify-tracks-dataset.csv";
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
    
    // Ignora a linha do cabeçalho mapeando pela string "track_id"
    if (campos.empty() || campos[2] == "track_id") continue;
    if (campos.size() < 22) continue; // Garante que a linha está completa até o gênero

    MusicRecord record;
    std::memset(&record, 0, sizeof(MusicRecord));

    // Mapeamento exato baseado no arquivo carregado:
    std::strncpy(record.singer, campos[3].c_str(), 149); // artists -> campos[3]
    std::strncpy(record.name, campos[5].c_str(), 149);   // track_name -> campos[5]
    
    // Mantém o prefixo "Album " antes do nome do álbum
    std::string album_name = campos[4];       // album_name -> campos[4]
    std::strncpy(record.album_name, album_name.c_str(), 149);
    
    // Monta a URL usando o "track_id" real da coluna 2 do dataset
    std::string track_id = campos[2];                    // track_id -> campos[2]
    std::string url_final = "https://open.spotify.com/intl-pt/track/" + track_id;
    std::strncpy(record.url, url_final.c_str(), 199);
    
    std::strncpy(record.genre, campos[21].c_str(), 49);  // track_genre -> campos[21]

    // Duração (duration_ms está na coluna 7)
    record.duration_ms = 180000.0f;
    try { 
        record.duration_ms = std::stof(campos[7]); 
    } catch(...) {} 
    
    // Popularidade (popularity está na coluna 6)
    record.popularity = 50.0f;
    try { 
        record.popularity = std::stof(campos[6]); 
    } catch(...) {}  
    
    // Mantém o padrão do seu código (já que o dataset não possui essa propriedade)
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