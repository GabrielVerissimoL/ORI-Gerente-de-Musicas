#include <drogon/HttpController.h>
#include <map>
#include <string>
#include "../MusicClass/music.hpp"
#include "../B-treeClass/btree.hpp"

using namespace drogon;

// Controller de alto nível para gerenciar produtos
class MusicController : public HttpController<MusicController> {
public:

    // Definição das rotas usando macros de alto nível
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(MusicController::SEARCH_BY_ID, "/music/{id}", Get);
        ADD_METHOD_TO(MusicController::UPLOAD, "/music", Post);
    METHOD_LIST_END

    void SEARCH_BY_ID(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id) {

	Json::Value ERR;
	ERR["Erro"] = "Essa musica não existe.";
	auto res_ERR = HttpResponse::newHttpJsonResponse(ERR);
	res_ERR->setStatusCode(k404NotFound);
	
	BTree bt;
	BTreeNode *found = bt.search(id);	

	if (found == NULL || &found->keys[id] == NULL)
	{
		callback(res_ERR);
	}
	
	int rrn = found->keys[id].getrrn();
        if (rrn < 0) {
        	callback(res_ERR);
           	return;
        }

	FILE *dataout = fopen(FILE_NAME, "rb");
	if (dataout == nullptr)
	{
		callback(res_ERR);
	}

	music *ptr;
	ptr->setrrn(rrn);

	fseek(dataout, ptr->getrrn(), SEEK_SET);

	if (fread(ptr, sizeof(music), 1, dataout) == 0)
	{
		callback(res_ERR);
	}

        Json::Value Answer;
	Answer["Name"] 		= ptr->getName();
	Answer["Singer"] 	= ptr->getSinger();
	Answer["Album Name"] 	= ptr->getalbum_name();
	Answer["URL"] 		= ptr->getUrl();
	Answer["Genre"]		= ptr->getGenre();
	Answer["Duration"]	= ptr->getDuration();
	Answer["Popularity"]	= ptr->getPopularity();

	fclose(dataout);

        auto res = HttpResponse::newHttpJsonResponse(Answer);
        callback(res);
    }

    // 3. POST - Criar produto
    void UPLOAD(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
        auto json_corpo = req->getJsonObject();
        if (!json_corpo) {
            auto res = HttpResponse::newHttpResponse();
            res->setStatusCode(k400BadRequest);
            callback(res);
            return;
        }

        int id = (*json_corpo)["id"].asInt();
        std::string nome = (*json_corpo)["nome"].asString();

        Json::Value sucesso;
        sucesso["mensagem"] = "Criado com sucesso";
        
        auto res = HttpResponse::newHttpJsonResponse(sucesso);
        res->setStatusCode(k201Created);
        callback(res);
    }
};

int main() {
    // Inicializa o servidor de forma totalmente abstrata
    app()
        .addListener("0.0.0.0", 5000)
        .setThreadNum(4) // Gerenciamento automático de threads
        .run();
    return 0;
}
