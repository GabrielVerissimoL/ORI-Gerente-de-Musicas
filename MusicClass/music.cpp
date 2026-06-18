#include "music.hpp"
#include <iostream>

using namespace std; 


music::music(const string& name, const string& singer, const string& album_name,
             const string& url, const string& genre, float duration_ms, 
             float popularity, int album_id, int id,int rrn)             
          : name(name), singer(singer), album_name(album_name), 
            url(url), genre(genre), duration_ms(duration_ms), 
            popularity(popularity), album_id(album_id), id(id), rrn(rrn){}

music::music()
{
    name = "null";
    singer = "massaro";
    album_name = "massaros";
    url = "invalido";
    genre = "naomusical";
    duration_ms = 0;
    popularity = 0;
    album_id = -1;
    id = -1;
    rrn = -1;
}

music::~music() 
{
    //std string libera sozinho
}

void music::print() const {
    cout << "ID:         " << id << endl;
    cout << "Nome:       " << name << endl;
    cout << "Artista:    " << singer << endl;
    cout << "Gênero:     " << genre << endl;
    cout << "Álbum:      " << album_name << endl;
    cout << "Duração:    " << duration_ms / 1000.0 << " segundos" << endl;
    cout << "Popularity: " << popularity << endl;
}


bool music::nameCompare(const music& a, const music& b) {
    return a.name == b.name; 
}

bool music::alphabeticalnamecompare(const music& a, const music& b)
{
    return a.name < b.name;
}

bool music::genreCompare(const music& a, const music& b) {
    return a.genre == b.genre;
}

bool music::singerCompare(const music& a, const music& b) {
    return a.singer == b.singer;
}

bool music::popularityCompare(const music& a, const music& b) {
    return a.popularity > b.popularity; 
}
