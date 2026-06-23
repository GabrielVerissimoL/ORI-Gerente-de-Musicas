#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <algorithm> 
#include <cctype>
#include "../B-treeClass/btree.hpp" 
#include "../MusicClass/music.hpp"
#include "database.hpp"

std::string tolowercase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

void database::removerIdDoVetor(std::vector<int>& vetor, int id) 
{
    vetor.erase(std::remove(vetor.begin(), vetor.end(), id), vetor.end());
}

void database::load_from_disk() 
{
    FILE* datafile = fopen(FILE_NAME, "rb");
    if (!datafile) {
        std::cout << "[INFO] Nenhum arquivo de dados encontrado para carregar. Iniciando banco vazio." << std::endl;
        idaux = 0; // Se não tem arquivo, começamos do ID 0
        return;
    }

    std::cout << "[CARREGANDO] Lendo registros do arquivo binario..." << std::endl;
    
    MusicRecord record;
    int contador = 0;

    // Lê struct por struct sequencialmente até o fim do arquivo
    while (fread(&record, sizeof(MusicRecord), 1, datafile) == 1) {
        
        // Se no seu projeto você implementou "remoção lógica" (ex: ID -1 para deletados),
        // adicione um IF aqui para pular registros excluídos.
        
        // 1. Reconstrói o objeto music
        music m(record.name, record.singer, record.album_name, record.url, record.genre, record.duration_ms, record.popularity, record.album_id);

        // 2. Insere na Árvore B na memória RAM
        btree.insert(m);

        // 3. Insere nos mapas de índices secundários
        int id_musica = m.getid(); // ou m.getrrn(), dependendo de como você mapeou as buscas
        indiceNomes[tolowercase(m.getName())].push_back(id_musica);
        indicecantores[tolowercase(m.getSinger())].push_back(id_musica);
        indiceAlbuns[tolowercase(m.getalbum_name())].push_back(id_musica);
        indiceGeneros[tolowercase(m.getGenre())].push_back(id_musica);

        idaux++;
        contador++;
        
        // Garante que o idaux global fique sempre à frente do maior ID lido
        
    }

    fclose(datafile);
    std::cout << "[SUCESSO] Carregamento concluido! " << contador << " musicas indexadas na RAM. Proximo ID automatico: " << idaux << std::endl;
}

void database::insert(music m) 
{
    
    int id = m.getrrn();

    if(id == idaux){
       
        FILE* dataout = fopen(FILE_NAME, "rb+");
    
        if (!dataout) {
            return;
        }

        MusicRecord record;

        std::memset(&record, 0, sizeof(MusicRecord));

        // Copia limitando o tamanho para não estourar os arrays
        std::strncpy(record.singer, m.getSinger().c_str() , 149);
        std::strncpy(record.name, m.getName().c_str(), 149);
        std::strncpy(record.album_name, m.getalbum_name().c_str(), 149);
        std::strncpy(record.url, m.getUrl().c_str(), 199);
        std::strncpy(record.genre, m.getGenre().c_str(), 49);

        record.duration_ms = m.getDuration(); 
        record.popularity  = m.getPopularity();
        record.album_id    = m.getalbum_id();
        record.id = m.getid();
        record.rrn = m.getrrn();

        long long byteOffset = static_cast<long long>(m.getrrn()) * sizeof(MusicRecord);
            
            // Move o ponteiro do arquivo para o lugar certo e grava
            fseek(dataout, byteOffset, SEEK_SET);
            fwrite(&record, sizeof(MusicRecord), 1, dataout);

        // Não esqueça de fechar no final se abrir com sucesso
        fclose(dataout);
    }

    btree.insert(m);

    //indices para nome da musica, cantor, nome do album e genero (Respectivamente)
    indiceNomes[tolowercase(m.getName())].push_back(id);
    indicecantores[tolowercase(m.getSinger())].push_back(id);
    indiceAlbuns[tolowercase(m.getalbum_name())].push_back(id);
    indiceGeneros[tolowercase(m.getGenre())].push_back(id);

     idaux++;

}

BTreeNode* database::IdSearch(int id)
{
    return btree.search(id);
}

std::vector<music> database::SingerSearch(std::string singer) 
{
    std::vector<music> answer;
    std::string singerlower = tolowercase(singer);

    auto iterador = indicecantores.lower_bound(singerlower);
    bool find = false;
    while(iterador != indicecantores.end() && iterador->first.compare(0,singerlower.size(), singerlower) == 0)
    {
        find = true;
        //lista de ids que o singer aponta
        std::vector<int>& ids = iterador->second;
        
        for(int id: ids)
        {
            BTreeNode *node = btree.search(id);
            if(node != nullptr)
            {
                for(int i = 0;i < node->n; i++)
                {
                    if(node->keys[i].getid() == id)
                    {
                        answer.push_back(node->keys[i]);
                        break;
                    }
                }
            }
        }

        iterador++;
    }

    if (!find) std::cout << "No singer starting with " << singer << " was found.\n";
    return answer;
}
std::vector<music> database::NameSearch(std::string name) 
{
    std::vector<music> answer;
    std::string namelower = tolowercase(name);

    auto iterador = indiceNomes.lower_bound(namelower);
    bool find = false;
    while(iterador != indiceNomes.end() && iterador->first.compare(0,namelower.size(), namelower) == 0)
    {
        find = true;
        //lista de ids que o nome aponta
        std::vector<int>& ids = iterador->second;
        
        for(int id: ids)
        {
            BTreeNode *node = btree.search(id);
            if(node != nullptr)
            {
                for(int i = 0;i < node->n; i++)
                {
                    if(node->keys[i].getid() == id)
                    {
                        answer.push_back(node->keys[i]);
                        break;
                    }
                }
            }
        }

        iterador++;
    }

    if (!find) std::cout << "No music starting with " << name << " was found.\n";
    return answer;
}

 std::vector<music> database::Album_nameSearch(std::string album) 
 {
     std::vector<music> answer;
    std::string albumlower = tolowercase(album);

    auto iterador = indiceAlbuns.lower_bound(albumlower);
    bool find = false;
    while(iterador != indiceAlbuns.end() && iterador->first.compare(0,albumlower.size(), albumlower) == 0)
    {
        find = true;
        //lista de ids que o album aponta
        std::vector<int>& ids = iterador->second;
        
        for(int id: ids)
        {
            BTreeNode *node = btree.search(id);
            if(node != nullptr)
            {
                for(int i = 0;i < node->n; i++)
                {
                    if(node->keys[i].getid() == id)
                    {
                        answer.push_back(node->keys[i]);
                        break;
                    }
                }
            }
        }

        iterador++;
    }

    if (!find) std::cout << "No album starting with " << album << " was found.\n";
    return answer;
}

 std::vector<music> database::GenreSearch(std::string genre) 
 {
    std::vector<music> answer;
    std::string genrelower = tolowercase(genre);

    auto iterador = indiceGeneros.lower_bound(genrelower);
    bool find = false;
    while(iterador != indiceGeneros.end() && iterador->first.compare(0,genrelower.size(), genrelower) == 0)
    {
        find = true;
        //lista de ids que o genero aponta
        std::vector<int>& ids = iterador->second;
        
        for(int id: ids)
        {
            BTreeNode *node = btree.search(id);
            if(node != nullptr)
            {
                for(int i = 0;i < node->n; i++)
                {
                    if(node->keys[i].getid() == id)
                    {
                        answer.push_back(node->keys[i]);
                        break;
                    }
                }
            }
        }

        iterador++;
    }

    if (!find) std::cout << "No genre starting with " << genre << " was found.\n";
    return answer;
}

void database::removeMusic(int id) 
{
    BTreeNode* node = btree.search(id);
    if (node != nullptr) 
    {
        for (int i = 0; i < node->n; i++) 
        {
            if (node->keys[i].getid() == id) 
            {
                music m = node->keys[i]; 
                    
                removerIdDoVetor(indiceNomes[tolowercase(m.getName())], id);
                removerIdDoVetor(indicecantores[tolowercase(m.getSinger())], id);
                removerIdDoVetor(indiceAlbuns[tolowercase(m.getalbum_name())], id);
                removerIdDoVetor(indiceGeneros[tolowercase(m.getGenre())], id);   
                break;
            }
        }

        btree.remove(id);
    } 
    else std::cout << "Fail To remove music with ID: " << id << "\n"; 
}