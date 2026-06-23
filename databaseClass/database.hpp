#pragma once

#include <string>
#include <map>
#include <vector>
#include "../B-treeClass/btree.hpp" 
#include "../MusicClass/music.hpp"

class database {
private:
    // Índice Primário (id)
    BTree btree; 

    //indices secundarios
    std::map<std::string, std::vector<int>> indiceNomes;
    std::map<std::string, std::vector<int>> indicecantores;
    std::map<std::string, std::vector<int>> indiceAlbuns;
    std::map<std::string, std::vector<int>> indiceGeneros;

    void removerIdDoVetor(std::vector<int>& vetor, int id);
public:

    void insert(music m);

    void load_from_disk();

    //busca por ID
    BTreeNode* IdSearch(int id);

    //Buscas indice secundario
    std::vector<music> SingerSearch(std::string singer);
    std::vector<music> NameSearch(std::string Name);
    std::vector<music> Album_nameSearch(std::string album);
    std::vector<music> GenreSearch(std::string genre);

    void removeMusic(int id);
   
};