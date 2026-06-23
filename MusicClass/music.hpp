#pragma once

#include <string>

extern int idaux;

// Estrutura espelho com tamanho fixo para salvar em disco com segurança
struct MusicRecord {
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

class music {
private:
    std::string name;
    std::string singer;
    std::string album_name;
    std::string url;
    std::string genre;
    float duration_ms;
    float popularity;
    int album_id;
    int id;
    int rrn;

public:
    music(const std::string& name, const std::string& singer, const std::string& album_name,
          const std::string& url, const std::string& genre, float duration_ms, 
          float popularity, int album_id);
    
    music ();

    ~music();


    void print() const;

    static bool nameCompare(const music& a, const music& b);
    static bool genreCompare(const music& a, const music& b);
    static bool singerCompare(const music& a, const music& b);
    static bool popularityCompare(const music& a, const music& b);
    static bool alphabeticalnamecompare(const music& a, const music& b);


    std::string getName() const { return name;}
    std::string getSinger() const { return singer;}
    std::string getGenre() const {return genre;}
    std::string getalbum_name() const {return album_name;}
    std::string getUrl() const {return url;}
    float getPopularity() const {return popularity;}
    float getDuration() const {return duration_ms;}
    int getalbum_id() const {return album_id;}
    int getid() const {return id;}
    int getrrn() const {return rrn;}
    
    void setrrn(int nrrn) {this->rrn = nrrn; return;}
};