// ============================================================================
// TESTADOR INTERATIVO: Busca por ID/Artista/Nome/Album/Genero e Cadastro via Terminal
// ============================================================================
#include <iostream>
#include <vector>
#include <string>
#include "../databaseClass/database.hpp"
#include "../MusicClass/music.hpp"
#include "../B-treeClass/btree.hpp"

void imprimirResultados(const std::vector<music>& resultados) {
    if (resultados.empty()) {
        std::cout << "[AVISO] Nenhum resultado encontrado." << std::endl;
        return;
    }

    std::cout << "[OK] Foram encontradas " << resultados.size() << " musicas!" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    for (size_t i = 0; i < resultados.size(); i++) {
        std::cout << "Resultado #" << i + 1 << ":" << std::endl;
        resultados[i].print();
        std::cout << "----------------------------------------------" << std::endl;
        
        if ((i + 1) % 5 == 0 && i + 1 < resultados.size()) {
            std::cout << "Pressione ENTER para ver mais resultados...";
            std::cin.get();
        }
    }
}

int main() {
    database dbGlobal;
    
    // Usa o metodo unificado de carga (reconstroi Arvore B + indices secundarios)
    dbGlobal.load_from_disk();

    std::cout << "\n[SUCESSO] Sistema pronto para consultas!" << std::endl;

    int opcao = 0;
    while (opcao != 7) {
        std::cout << "\n==============================================" << std::endl;
        std::cout << "           GERENTE DE MÚSICAS - MENU          " << std::endl;
        std::cout << "==============================================" << std::endl;
        std::cout << "1. Buscar Música por ID (Árvore B)" << std::endl;
        std::cout << "2. Buscar Músicas por Artista (Índice Secundário)" << std::endl;
        std::cout << "3. Buscar Músicas por Nome" << std::endl;
        std::cout << "4. Buscar Músicas por Álbum" << std::endl;
        std::cout << "5. Buscar Músicas por Gênero" << std::endl;
        std::cout << "6. Adicionar Nova Música" << std::endl;
        std::cout << "7. Sair do Programa" << std::endl;
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
            std::string busca;
            std::cout << "Digite o nome (ou parte do nome) do artista: ";
            std::getline(std::cin, busca);
            std::cout << "\n--- Buscando '" << busca << "' no Indice de Artistas ---" << std::endl;
            imprimirResultados(dbGlobal.SingerSearch(busca));

        } else if (opcao == 3) {
            std::string busca;
            std::cout << "Digite o nome (ou parte do nome) da musica: ";
            std::getline(std::cin, busca);
            std::cout << "\n--- Buscando '" << busca << "' no Indice de Nomes ---" << std::endl;
            imprimirResultados(dbGlobal.NameSearch(busca));

        } else if (opcao == 4) {
            std::string busca;
            std::cout << "Digite o nome (ou parte do nome) do album: ";
            std::getline(std::cin, busca);
            std::cout << "\n--- Buscando '" << busca << "' no Indice de Albums ---" << std::endl;
            imprimirResultados(dbGlobal.Album_nameSearch(busca));

        } else if (opcao == 5) {
            std::string busca;
            std::cout << "Digite o genero musical: ";
            std::getline(std::cin, busca);
            std::cout << "\n--- Buscando '" << busca << "' no Indice de Generos ---" << std::endl;
            imprimirResultados(dbGlobal.GenreSearch(busca));

        } else if (opcao == 6) {
            std::string nome, artista, album, url, genero;
            float duracao, popularidade;
            int album_id;

            std::cout << "\n--- Cadastro de Nova Musica ---" << std::endl;
            std::cout << "Nome da Musica: "; std::getline(std::cin, nome);
            std::cout << "Artista/Cantor: "; std::getline(std::cin, artista);
            std::cout << "Nome do Album:  "; std::getline(std::cin, album);
            std::cout << "URL (Link):     "; std::getline(std::cin, url);
            std::cout << "Genero Musical: "; std::getline(std::cin, genero);
            
            std::cout << "Duracao (em ms): "; 
            while (!(std::cin >> duracao)) {
                std::cin.clear(); std::cin.ignore(10000, '\n');
                std::cout << "[ERRO] Digite um valor numerico para duracao: ";
            }
            
            std::cout << "Popularidade (0 a 100): ";
            while (!(std::cin >> popularidade)) {
                std::cin.clear(); std::cin.ignore(10000, '\n');
                std::cout << "[ERRO] Digite um valor numerico para popularidade: ";
            }

            std::cout << "ID do Album (inteiro): ";
            while (!(std::cin >> album_id)) {
                std::cin.clear(); std::cin.ignore(10000, '\n');
                std::cout << "[ERRO] Digite um numero inteiro para o ID do Album: ";
            }
            std::cin.ignore(10000, '\n'); // Limpa buffer residual

            // Cria o objeto música (o construtor trata o idaux interno para ID e RRN)
            music novaMusica(nome, artista, album, url, genero, duracao, popularidade, album_id);

            // Insere na Árvore B, Mapas Secundários e disco
            if (dbGlobal.insert(novaMusica)) {
                std::cout << "[SUCESSO] Musica inserida com ID: " << novaMusica.getid() << std::endl;
            } else {
                std::cout << "[ERRO] Falha ao inserir musica no disco." << std::endl;
            }

        } else if (opcao == 7) {
            std::cout << "Encerrando o testador interativo. Ate logo!" << std::endl;
        } else {
            std::cout << "[ERRO] Opcao invalida! Tente novamente." << std::endl;
        }
    }

    return 0;
}