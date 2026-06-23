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
    
    // Variável estática para controle automático do ID incremental
    static int idaux;

public:
    // Construtor atualizado: o ID foi removido dos parâmetros!
    music(const std::string& name, const std::string& singer, const std::string& album_name,
          const std::string& url, const std::string& genre, float duration_ms, 
          float popularity, int album_id, int rrn = -1);
    
    music();

    ~music();

    void print() const;

    // Seus métodos originais de comparação preservados intactos
    static bool nameCompare(const music& a, const music& b);
    static bool genreCompare(const music& a, const music& b);
    static bool singerCompare(const music& a, const music& b);
    static bool popularityCompare(const music& a, const music& b);
    static bool alphabeticalnamecompare(const music& a, const music& b);

    // Seus getters e setters originais preservados
    std::string getName() const { return name; }
    std::string getSinger() const { return singer; }
    std::string getGenre() const { return genre; }
    std::string getalbum_name() const { return album_name; }
    std::string getUrl() const { return url; }
    float getPopularity() const { return popularity; }
    float getDuration() const { return duration_ms; }
    int getid() const { return id; }
    int getrrn() const { return rrn; }
    
    void setrrn(int nrrn) { this->rrn = nrrn; }
    
    // Método estático auxiliar caso precise resetar o contador de IDs
    static void resetCounter() { idaux = 0; }
};