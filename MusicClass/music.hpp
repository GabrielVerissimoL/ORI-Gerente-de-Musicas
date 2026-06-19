#pragma once

#include <string>

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
          float popularity, int album_id, int id, int rrn);
    
    music ();

    ~music();


    void print() const;

    static bool nameCompare(const music& a, const music& b);
    static bool genreCompare(const music& a, const music& b);
    static bool singerCompare(const music& a, const music& b);
    static bool popularityCompare(const music& a, const music& b);
    static bool alphabeticalnamecompare(const music& a, const music& b);


    std::string getName() const { return name; }
    std::string getSinger() const { return singer; }
    std::string getGenre() const {return genre;}
    std::string getalbum_name() const {return album_name;}
    std::string getUrl() const {return url;}
    int getid() const {return id;}
};