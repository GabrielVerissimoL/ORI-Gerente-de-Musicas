#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm> 
#include "../B-treeClass/btree.hpp" 
#include "../MusicClass/music.hpp"
#include "database.hpp"


void database::removerIdDoVetor(std::vector<int>& vetor, int id) 
{
    vetor.erase(std::remove(vetor.begin(), vetor.end(), id), vetor.end());
}

void database::insert(music m) 
{
    int id = m.getid();
        
    arvorePrincipal.insert(m);
        
    //indices para nome da musica, cantor, nome do album e genero (Respectivamente)
    indiceNomes[m.getName()].push_back(id);
    indiceCantores[m.getSinger()].push_back(id);
    indiceAlbuns[m.getalbum_name()].push_back(id);
    indiceGeneros[m.getGenre()].push_back(id);
}

BTreeNode* database::IdSearch(int id)
{
    return arvorePrincipal.search(id);
}

std::vector<BTreeNode*> database::SingerSearch(std::string singer) 
{
    std::vector<BTreeNode*> answer;
        
    if (indiceCantores.find(singer) != indiceCantores.end()) 
    {

        std::vector<int>& ids = indiceCantores[singer];
            
        for (int id : ids) 
        {
            BTreeNode* no = arvorePrincipal.search(id);
            if (no != nullptr) answer.push_back(no);
        }
         
    }

    else std::cout << singer << " Not Found\n";
        
    return answer;
}

std::vector<BTreeNode*> database::NameSearch(std::string Name) 
{
    std::vector<BTreeNode*> answer;
        
    if (indiceNomes.find(Name) != indiceNomes.end()) 
    {
        std::vector<int>& ids = indiceNomes[Name];
            
        for (int id : ids) 
        {
            BTreeNode* no = arvorePrincipal.search(id);
            if (no != nullptr) answer.push_back(no);
        }

    } 
    
    else std::cout << "Music with name: "<< Name << " Not Found\n";
        
    return answer;
}

 std::vector<BTreeNode*> database::Album_nameSearch(std::string album) 
 {
    std::vector<BTreeNode*> answer;
        
    if (indiceAlbuns.find(album) != indiceAlbuns.end()) 
    {
        std::vector<int>& ids = indiceAlbuns[album];
            
        for (int id : ids) 
        {
            BTreeNode* no = arvorePrincipal.search(id);
            if (no != nullptr) answer.push_back(no);
        }

    } 

    else std::cout << album << " Not Found\n";
        
    return answer;
}

 std::vector<BTreeNode*> database::GenreSearch(std::string genre) 
 {
    std::vector<BTreeNode*> answer;
        
    if (indiceGeneros.find(genre) != indiceGeneros.end()) {

        std::vector<int>& ids = indiceGeneros[genre];
            
        for (int id : ids) 
        {
            BTreeNode* no = arvorePrincipal.search(id);
            if (no != nullptr) answer.push_back(no);
        }
    } 
    
    else std::cout << genre << " Not Found\n";
        
    return answer;
}

void database::removeMusic(int id) 
{
    BTreeNode* node = arvorePrincipal.search(id);
    if (node != nullptr) 
    {
        for (int i = 0; i < node->n; i++) 
        {
            if (node->keys[i].getid() == id) 
            {
                music m = node->keys[i]; 
                    
                removerIdDoVetor(indiceNomes[m.getName()], id);
                removerIdDoVetor(indiceCantores[m.getSinger()], id);
                removerIdDoVetor(indiceAlbuns[m.getalbum_name()], id);
                removerIdDoVetor(indiceGeneros[m.getGenre()], id);   
                break;
            }
        }

        arvorePrincipal.remove(id);
    } 
    else std::cout << "Fail To remove music with ID: " << id << "\n"; 
}