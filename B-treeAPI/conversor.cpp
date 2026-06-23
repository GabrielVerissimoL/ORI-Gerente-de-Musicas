// ============================================================================
// conversor.cpp - Conversão de CSV para Binário de Tamanho Fixo (724 Bytes)
// ============================================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include "../MusicClass/music.hpp" // Altere o caminho caso os arquivos estejam em pastas diferentes

#define CSV_FILE "dataset.csv"                  // Nome do seu arquivo CSV de entrada
#define BINARY_FILE "arquivo_de_dados_binario"  // Nome do binário que será gerado

using namespace std;

// Layout físico obrigatório para gravação exata de 724 bytes em disco
struct FileRecordLayout {
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

// Função auxiliar para limpar as aspas que o formato CSV costuma colocar nas strings
string limparAspas(string str) {
    if (!str.empty() && str.front() == '"') str.erase(0, 1);
    if (!str.empty() && str.back() == '"') str.pop_back();
    return str;
}

int main() {
    // 1. Abre o arquivo CSV para leitura
    ifstream csv_in(CSV_FILE);
    if (!csv_in.is_open()) {
        cerr << "[ERRO] Nao foi possivel abrir o arquivo CSV '" << CSV_FILE << "'" << endl;
        return 1;
    }

    // 2. Abre o arquivo binário para escrita limpa
    FILE* bin_out = fopen(BINARY_FILE, "wb");
    if (!bin_out) {
        cerr << "[ERRO] Nao foi possivel criar o arquivo binario!" << endl;
        csv_in.close();
        return 1;
    }

    string linha;
    // Pula a primeira linha do CSV (Cabeçalho com os nomes das colunas)
    getline(csv_in, linha);

    cout << "====================================================" << endl;
    cout << "Iniciando conversao do CSV para Binario de Tamanho Fixo..." << endl;
    cout << "====================================================" << endl;
    
    int processados = 0;

    // Loop de leitura linha por linha
    while (getline(csv_in, linha)) {
        if (linha.empty()) continue;

        stringstream ss(linha);
        string campo;
        vector<string> campos;

        // Divide a linha do CSV usando a vírgula como delimitador
        while (getline(ss, campo, ',')) {
            campos.push_back(campo);
        }

        // Verifica se a linha tem o número mínimo de colunas necessárias do seu dataset
        if (campos.size() < 8) continue; 

        // Extrai e limpa os campos de texto
        string v_name       = limparAspas(campos[0]);
        string v_singer     = limparAspas(campos[1]);
        string v_album_name = limparAspas(campos[2]);
        string v_url        = limparAspas(campos[3]);
        string v_genre      = limparAspas(campos[4]);
        
        // Conversão dos tipos numéricos (com tratamento básico de erro de conversão)
        float v_duration = 0.0f;
        float v_popularity = 0.0f;
        int v_album_id = -1;
        
        try {
            v_duration   = stof(campos[5]);
            v_popularity = stof(campos[6]);
            v_album_id   = stoi(campos[7]);
        } catch (...) {
            // Se der erro de parsing em alguma linha corrompida do CSV, apenas pula ela
            continue; 
        }

        // 3. Instancia o objeto 'music' usando o novo construtor
        // O ID incremental e o RRN em bytes (724 * id) são calculados automaticamente aqui dentro!
        music m(v_name, v_singer, v_album_name, v_url, v_genre, v_duration, v_popularity, v_album_id);

        // 4. Monta a estrutura física limpa para salvar no disco de forma estática
        FileRecordLayout registro_disco;
        memset(&registro_disco, 0, sizeof(registro_disco)); // Garante que não vá lixo de memória para o arquivo

        // Copia as std::string com segurança respeitando o limite dos arrays de char do arquivo
        strncpy(registro_disco.name, m.getName().c_str(), 149);
        strncpy(registro_disco.singer, m.getSinger().c_str(), 149);
        strncpy(registro_disco.album_name, m.getalbum_name().c_str(), 149);
        strncpy(registro_disco.url, m.getUrl().c_str(), 199); 
        strncpy(registro_disco.genre, m.getGenre().c_str(), 49);

        // Atribui os dados numéricos e de indexação gerados pela classe
        registro_disco.duration_ms = m.getDuration();
        registro_disco.popularity  = m.getPopularity();
        registro_disco.album_id    = v_album_id;
        registro_disco.id          = m.getid();   // Captura o idaux automático
        registro_disco.rrn         = m.getrrn();  // Captura o Byte Offset exato (724 * id)

        // 5. Escreve o bloco fixo de 724 bytes direto no arquivo binário
        fwrite(&registro_disco, sizeof(FileRecordLayout), 1, bin_out);
        processados++;
    }

    // Fecha os descritores de arquivo abertos
    fclose(bin_out);
    csv_in.close();

    cout << "====================================================" << endl;
    cout << "[SUCESSO] Conversao concluida!" << endl;
    cout << "Total de musicas gravadas: " << processados << " registros." << endl;
    cout << "Tamanho de cada registro em disco: " << sizeof(FileRecordLayout) << " bytes." << endl;
    cout << "====================================================" << endl;

    return 0;
}