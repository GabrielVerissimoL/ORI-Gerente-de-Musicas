#include <iostream>
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

void database::insert(music m) 
{
    int id = m.getid();
        
    btree.insert(m);
        
    //indices para nome da musica, cantor, nome do album e genero (Respectivamente)
    indiceNomes[tolowercase(m.getName())].push_back(id);
    indicecantores[tolowercase(m.getSinger())].push_back(id);
    indiceAlbuns[tolowercase(m.getalbum_name())].push_back(id);
    indiceGeneros[tolowercase(m.getGenre())].push_back(id);
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