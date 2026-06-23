#include <drogon/HttpController.h>
#include <map>
#include <string>
#include <vector>
#include "../B-treeClass/btree.hpp"
#include "../MusicClass/music.hpp"
#include "../databaseClass/database.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace drogon;

// INSTÂNCIA GLOBAL ÚNICA E UNIFICADA
database dbGlobal;

void populate_json(Json::Value *ret, music current);

class MusicController : public HttpController<MusicController> {
public:

    // Definição das rotas usando macros de alto nível
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(MusicController::SEARCH_BY_ID, "/music/{id}", Get);
        ADD_METHOD_TO(MusicController::SEARCH_BY_NAME, "/music/name/{name}", Get);      
        ADD_METHOD_TO(MusicController::SEARCH_BY_SINGER, "/music/singer/{singer}", Get); 
        ADD_METHOD_TO(MusicController::SEARCH_BY_ALBUM, "/music/album/{album}", Get);   
        ADD_METHOD_TO(MusicController::UPLOAD, "/music", Post);
    METHOD_LIST_END
    
    void SEARCH_BY_ID(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id) 
    {
        Json::Value ERR;
        ERR["Erro"] = "Essa musica nao existe.";
        auto res_ERR = HttpResponse::newHttpJsonResponse(ERR);
        res_ERR->setStatusCode(k404NotFound);

        // Busca usando o método correto da dbGlobal unificada
        BTreeNode *found = dbGlobal.IdSearch(id);

        if (found == nullptr)
        {
            callback(res_ERR);
            return;
        }
    
        // Descobre em qual índice do nó está a chave do ID procurado
        int idx = -1;
        for (int i = 0; i < found->n; i++)
        {
            if (found->keys[i].getid() == id)
            {
                idx = i;
                break;
            }
        } 

        if (idx == -1)
        {
            callback(res_ERR);
            return;
        }
    
        int rrn = found->keys[idx].getrrn();
        if (rrn < 0) 
        {
            ERR["Erro"] = "RRN invalido.";
            res_ERR = HttpResponse::newHttpJsonResponse(ERR);
            callback(res_ERR);
            return;
        }

        // Abre o arquivo binário com segurança
        FILE *dataout = fopen(FILE_NAME, "rb");
        if (dataout == nullptr)
        {
            ERR["Erro"] = "Nao foi possivel abrir o arquivo de dados.";
            res_ERR = HttpResponse::newHttpJsonResponse(ERR);
            callback(res_ERR);
            return;
        }

        // CORREÇÃO CRÍTICA: Lendo para uma struct intermediária (Evita std::bad_alloc)
        MusicRecord record;
        long long byteOffset = static_cast<long long>(rrn) * sizeof(MusicRecord);
        fseek(dataout, byteOffset, SEEK_SET);

        if (fread(&record, sizeof(MusicRecord), 1, dataout) == 0)
        {
            fclose(dataout);
            ERR["Erro"] = "Nao foi possivel ler do arquivo de dados.";
            res_ERR = HttpResponse::newHttpJsonResponse(ERR);
            callback(res_ERR);
            return;
        }
        fclose(dataout);

        // Monta o objeto real music alocado na stack de forma segura
        music m(record.name, record.singer, record.album_name, record.url, record.genre, record.duration_ms, record.popularity, record.album_id);
        m.setrrn(record.rrn);

        Json::Value Answer;
        populate_json(&Answer, m);

        auto res = HttpResponse::newHttpJsonResponse(Answer);
        callback(res);
        return;
    }

    void SEARCH_BY_NAME(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string name) 
    {
        // CORREÇÃO: Buscando na dbGlobal compartilhada
        std::vector<music> msc = dbGlobal.NameSearch(name);
        Json::Value Answer(Json::arrayValue);
        Json::Value Musics;
    
        for (size_t i = 0; i < msc.size(); i++)
        {
            populate_json(&Musics, msc[i]);
            Answer.append(Musics);
        }

        auto res = HttpResponse::newHttpJsonResponse(Answer); 
        callback(res);
        return;
    }

    void SEARCH_BY_SINGER(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string singer) 
    {
        // CORREÇÃO: Buscando na dbGlobal compartilhada
        std::vector<music> msc = dbGlobal.SingerSearch(singer);
        Json::Value Answer(Json::arrayValue);
        Json::Value Musics;
    
        for (size_t i = 0; i < msc.size(); i++)
        {
            populate_json(&Musics, msc[i]);
            Answer.append(Musics);
        }

        auto res = HttpResponse::newHttpJsonResponse(Answer); 
        callback(res);
        return;
    }

    void SEARCH_BY_ALBUM(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string album) 
    {
        // CORREÇÃO: Buscando na dbGlobal compartilhada
        std::vector<music> msc = dbGlobal.Album_nameSearch(album);
        Json::Value Answer(Json::arrayValue);
        Json::Value Musics;
    
        for (size_t i = 0; i < msc.size(); i++)
        {
            populate_json(&Musics, msc[i]);
            Answer.append(Musics);
        }

        auto res = HttpResponse::newHttpJsonResponse(Answer); 
        callback(res);
        return;
    }

    void UPLOAD(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) 
    {
        auto res_ERR = HttpResponse::newHttpResponse();
        res_ERR->setStatusCode(k400BadRequest);

        auto Music_json = req->getJsonObject();
        if (!Music_json) 
        {
            callback(res_ERR);
            return;
        }

        std::string name    = (*Music_json)["name"].asString();
        std::string singer  = (*Music_json)["singer"].asString();
        std::string album   = (*Music_json)["album_name"].asString();
        std::string url     = (*Music_json)["url"].asString();
        std::string genre   = (*Music_json)["genre"].asString();
        float duration      = (*Music_json)["duration_ms"].asFloat();
        float popularity    = (*Music_json)["popularity"].asFloat();
        int album_id        = (*Music_json)["album_id"].asInt();
    
        // Cria o objeto music de forma limpa
        music m(name, singer, album, url, genre, duration, popularity, album_id);

        // Insere na dbGlobal unificada
        dbGlobal.insert(m);

        std::cout << "[SERVER] Insercao Realizada com sucesso. ID: " << m.getid() << std::endl;

        Json::Value OK;
        OK["Status"] = "Musica adicionada com sucesso!";
        OK["id"] = m.getid();
        OK["rrn"] = m.getrrn();
        
        auto res = HttpResponse::newHttpJsonResponse(OK);
        res->setStatusCode(k201Created);
        
        callback(res);
        return;
    }
};

void populate_json(Json::Value *ret, music current)
{   
    (*ret)["Name"]       = current.getName();
    (*ret)["Singer"]     = current.getSinger();
    (*ret)["Album"]      = current.getalbum_name();
    (*ret)["Genre"]      = current.getGenre();
    (*ret)["URL"]        = current.getUrl();
    (*ret)["Popularity"] = current.getPopularity();
    (*ret)["Duration"]   = current.getDuration();
}

int main() {
    // 1. Recupera tudo o que já existe no arquivo binário para a RAM antes de ligar o servidor
    dbGlobal.load_from_disk();

    // 2. Inicializa o Framework Drogon
    app()
        .addListener("0.0.0.0", 5000)
        .setThreadNum(1) 
        .run();
    return 0;
}