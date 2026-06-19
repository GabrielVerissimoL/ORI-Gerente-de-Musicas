#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "../B-treeClass/btree.hpp" 
#include "../MusicClass/music.hpp"

class database {
private:
    // Índice Primário (id)
    BTree arvorePrincipal; 

    //indices secundarios
    std::unordered_map<std::string, std::vector<int>> indiceNomes;
    std::unordered_map<std::string, std::vector<int>> indiceCantores;
    std::unordered_map<std::string, std::vector<int>> indiceAlbuns;
    std::unordered_map<std::string, std::vector<int>> indiceGeneros;

    void removerIdDoVetor(std::vector<int>& vetor, int id);
public:

    void insert(music m);

    //busca por ID
    BTreeNode* IdSearch(int id);

    //Buscas indice secundario
    std::vector<BTreeNode*> SingerSearch(std::string singer);
    std::vector<BTreeNode*> NameSearch(std::string Name);
    std::vector<BTreeNode*> Album_nameSearch(std::string album);
    std::vector<BTreeNode*> GenreSearch(std::string genre);

    void removeMusic(int id);
   
};