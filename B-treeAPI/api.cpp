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

BTree mainTree;
database dbGlobal;

void populate_json(Json::Value *ret, music current);

class MusicController : public HttpController<MusicController> {
public:

    // Definição das rotas usando macros de alto nível
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(MusicController::SEARCH_BY_ID, "/music/{id}", Get);
	ADD_METHOD_TO(MusicController::SEARCH_BY_NAME, "/music/{name}", Get);
	ADD_METHOD_TO(MusicController::SEARCH_BY_SINGER, "/music/{singer}", Get);
	ADD_METHOD_TO(MusicController::SEARCH_BY_ALBUM, "/music/{album}", Get);
//	ADD_METHOD_TO(MusicController::SEARCH_BY_POPUL, "/music/{popularity}", Get);
//	ADD_METHOD_TO(MusicController::DELETE, "/music/{id}", Delete);
        ADD_METHOD_TO(MusicController::UPLOAD, "/music", Post);
    METHOD_LIST_END
	
    	void SEARCH_BY_ID(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id) 
    	{

		Json::Value ERR;
		ERR["Erro"] = "Essa musica não existe.";
		auto res_ERR = HttpResponse::newHttpJsonResponse(ERR);
		res_ERR->setStatusCode(k404NotFound);

		BTree bt;
		BTreeNode *found = bt.search(id);

		printf("%p\n", found);
		if (found == nullptr)
		{
			callback(res_ERR);
			return ;
		}
	
		for (int i = 0; i < found->n; i++)
		{
			if (ORDER > i && &found->keys[i] == nullptr)
				continue ;

			std::cout << found->keys[i].getName() << std::endl;
		}	

		if (found == NULL || &found->keys[id] == NULL)
		{
			callback(res_ERR);
			return ;
		}
	
		int rrn = found->keys[id].getrrn();
	        if (rrn < 0) 
		{
			ERR["Erro"] = "RRN invalido.";
			res_ERR = HttpResponse::newHttpJsonResponse(ERR);
        		callback(res_ERR);
        	   	return;
        	}

		FILE *dataout = fopen(FILE_NAME, "rb");
		if (dataout == nullptr)
		{
			ERR["Erro"] = "Não foi possivel abrir o arquivo de dados.";
			res_ERR = HttpResponse::newHttpJsonResponse(ERR);
			callback(res_ERR);
			return ;
		}

		music *ptr;
		ptr->setrrn(rrn);

		fseek(dataout, ptr->getrrn(), SEEK_SET);

		if (fread(ptr, sizeof(music), 1, dataout) == 0)
		{
			fclose(dataout);
			ERR["Erro"] = "Não foi possivel ler do arquivo de dados.";
			res_ERR = HttpResponse::newHttpJsonResponse(ERR);
			callback(res_ERR);
			return ;
		}
		



        	Json::Value Answer;
		populate_json(&Answer, *ptr);

		/*
		Answer["Name"] 		= ptr->getName();
		Answer["Singer"] 	= ptr->getSinger();
		Answer["Album Name"] 	= ptr->getalbum_name();
		Answer["URL"] 		= ptr->getUrl();
		Answer["Genre"]		= ptr->getGenre();
		Answer["Duration"]	= ptr->getDuration();
		Answer["Popularity"]	= ptr->getPopularity();
		*/

		fclose(dataout);

       	 	auto res = HttpResponse::newHttpJsonResponse(Answer);
	        callback(res);

		return ;
    }


    	void SEARCH_BY_NAME(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string name) 
	{
	
		database db;
		std::vector<music> msc = db.NameSearch(name);
		Json::Value Answer(Json::arrayValue);
		Json::Value Musics;
	

		for (int i = 0; i < msc.size(); i++)
		{
			populate_json(&Musics, msc[i]);
			Answer.append(Musics);
		}

		auto res = HttpResponse::newHttpJsonResponse(Answer); 
		callback(res);
		return ;
	}


	void SEARCH_BY_SINGER(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string singer) 
	{

		database db;
		std::vector<music> msc = db.SingerSearch(singer);
		Json::Value Answer(Json::arrayValue);
		Json::Value Musics;
	
		for (int i = 0; i < msc.size(); i++)
		{
			populate_json(&Musics, msc[i]);
			Answer.append(Musics);
		}

	
		auto res = HttpResponse::newHttpJsonResponse(Answer); 
		callback(res);
		return ;
    	}

    	void SEARCH_BY_GENRE(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string genre) 
	{
		
		database db;
		std::vector<music> msc = db.GenreSearch(genre);
		Json::Value Answer(Json::arrayValue);
		Json::Value Musics;
	
		for (int i = 0; i < msc.size(); i++)
		{
			populate_json(&Musics, msc[i]);
			Answer.append(Musics);
		}

	
		auto res = HttpResponse::newHttpJsonResponse(Answer); 
		callback(res);
		return ;
	}
	

    	void SEARCH_BY_ALBUM(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string album) 
	{
		
		database db;
		std::vector<music> msc = db.Album_nameSearch(album);
		Json::Value Answer(Json::arrayValue);
		Json::Value Musics;
	
		for (int i = 0; i < msc.size(); i++)
		{
			populate_json(&Musics, msc[i]);
			Answer.append(Musics);
		}

	
		auto res = HttpResponse::newHttpJsonResponse(Answer); 
		callback(res);
		return ;
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


		std::string name 	= (*Music_json)["name"].asString();
		std::string singer 	= (*Music_json)["singer"].asString();
		std::string album 	= (*Music_json)["album_name"].asString();
		std::string url		= (*Music_json)["url"].asString();
		std::string genre	= (*Music_json)["genre"].asString();
		float duration		= (*Music_json)["duration_ms"].asFloat();
		float popularity	= (*Music_json)["popularity"].asFloat();
		int album_id		= (*Music_json)["album_id"].asInt();
	
		// objeto music criado
		music m(name, singer, album, url, genre, duration, popularity, album_id);

		dbGlobal.insert(m);

		return ;
	    }
};

void populate_json(Json::Value *ret, music current)
{	

	(*ret)["Name"] 		= current.getName();
	(*ret)["Singer"] 	= current.getSinger();
	(*ret)["Album"]		= current.getalbum_name();
	(*ret)["Genre"]		= current.getGenre();
	(*ret)["URL"]		= current.getUrl();
	(*ret)["Popularity"]	= current.getPopularity();
	(*ret)["Duration"]	= current.getDuration();

	return ;
}

int main() {

	app()
        .addListener("0.0.0.0", 5000)
        .setThreadNum(1) 
        .run();
    return 0;
}
