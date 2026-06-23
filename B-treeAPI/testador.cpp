// ============================================================================
// TESTADOR INTERATIVO DEFINITIVO: Busca por ID ou Artista via Terminal
// ============================================================================
#include <iostream>
#include <vector>
#include <string>
#include "../databaseClass/database.hpp"
#include "../MusicClass/music.hpp"
#include "../B-treeClass/btree.hpp"

struct MusicRecord {
    char name[150]; char singer[150]; char album_name[150]; char url[200]; char genre[50];
    float duration_ms; float popularity; int album_id; int id; int rrn;
};

int main() {
    database dbGlobal;
    FILE *datain = fopen(FILE_NAME, "rb");
    if (!datain) {
        std::cerr << "Erro: Arquivo binario nao encontrado!" << std::endl;
        return 1;
    }

    std::cout << "====================================================" << std::endl;
    std::cout << "Lendo " << FILE_NAME << "..." << std::endl;
    std::cout << "====================================================" << std::endl;
    
    MusicRecord record;
    int totalCarregado = 0;

    std::vector<music> bancoEmMemoria;
    bancoEmMemoria.reserve(28500); 

    while (fread(&record, sizeof(MusicRecord), 1, datain) == 1) {
        music m(record.name, record.singer, record.album_name, record.url, record.genre, 
                record.duration_ms, record.popularity, record.album_id, record.id, record.rrn);
        bancoEmMemoria.push_back(m);
        totalCarregado++;
    }
    fclose(datain);

    std::cout << "Inserindo " << totalCarregado << " musicas na Arvore B e Índices..." << std::endl;
    for (int i = 0; i < totalCarregado; i++) {
        dbGlobal.insert(bancoEmMemoria[i]);
    }

    std::cout << "\n[SUCESSO] Sistema pronto para consultas!" << std::endl;

    int opcao = 0;
    while (opcao != 3) {
        std::cout << "\n==============================================" << std::endl;
        std::cout << "          GERENTE DE MÚSICAS - MENU           " << std::endl;
        std::cout << "==============================================" << std::endl;
        std::cout << "1. Buscar Música por ID (Árvore B)" << std::endl;
        std::cout << "2. Buscar Músicas por Artista (Índice Secundário)" << std::endl;
        std::cout << "3. Sair do Programa" << std::endl;
        std::cout << "Escolha uma opcao: ";
        
        if (!(std::cin >> opcao)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "[ERRO] Digite apenas numeros!" << std::endl;
            continue;
        }

        std::cin.ignore(10000, '\n'); // Limpa o buffer do teclado

        if (opcao == 1) {
            int idBusca;
            std::cout << "Digite o ID da musica: ";
            if (!(std::cin >> idBusca)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "[ERRO] ID invalido!" << std::endl;
                continue;
            }
            std::cin.ignore(10000, '\n');

            std::cout << "\n--- Buscando ID " << idBusca << " na Arvore B ---" << std::endl;
            BTreeNode* noEncontrado = dbGlobal.IdSearch(idBusca);
            
            if (noEncontrado != nullptr) {
                bool achou = false;
                for (int i = 0; i < noEncontrado->n; i++) {
                    if (noEncontrado->keys[i].getid() == idBusca) {
                        std::cout << "[OK] Musica encontrada!" << std::endl;
                        noEncontrado->keys[i].print();
                        achou = true;
                        break;
                    }
                }
                if (!achou) {
                    std::cout << "[INFO] O no foi retornado, mas o ID " << idBusca << " nao estava nesta pagina." << std::endl;
                }
            } else {
                std::cout << "[ERRO] ID " << idBusca << " nao existe na Arvore B." << std::endl;
            }

        } else if (opcao == 2) {
            std::string artistaBusca;
            std::cout << "Digite o nome (ou parte do nome) do artista: ";
            std::getline(std::cin, artistaBusca);

            std::cout << "\n--- Buscando '" << artistaBusca << "' no Indice Secundario ---" << std::endl;
            std::vector<music> resultados = dbGlobal.SingerSearch(artistaBusca);
            
            if (!resultados.empty()) {
                std::cout << "[OK] Foram encontradas " << resultados.size() << " musicas!" << std::endl;
                std::cout << "----------------------------------------------" << std::endl;
                for (size_t i = 0; i < resultados.size(); i++) {
                    std::cout << "Resultado #" << i + 1 << ":" << std::endl;
                    resultados[i].print();
                    std::cout << "----------------------------------------------" << std::endl;
                    
                    // Pausa para não floodar a tela se houverem muitos resultados
                    if ((i + 1) % 5 == 0 && i + 1 < resultados.size()) {
                        std::cout << "Pressione ENTER para ver mais resultados...";
                        std::cin.get();
                    }
                }
            } else {
                std::cout << "[AVISO] Nenhuma musica encontrada para o artista '" << artistaBusca << "'." << std::endl;
            }

        } else if (opcao == 3) {
            std::cout << "Encerrando o testador interativo. Ate logo!" << std::endl;
        } else {
            std::cout << "[ERRO] Opcao invalida! Tente novamente." << std::endl;
        }
    }

    return 0;
}