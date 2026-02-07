#include <SQLiteCpp/SQLiteCpp.h>
#include <ChromaDB/ChromaDB.h>
#include "Proto/Beuro-proto.h"
#include <Http/httplib.h>
#include <dotenv.h>

int main(){
    dotenv::init();
    
    SQL_Execs sqlexec(dotenv::getenv("FILEPATH"));
    ChromaDB_Execs chromaexec("http", "127.0.0.1", "8080"); 

    std::cout << chromaexec.GetAllEmbeddingsFromCollection("ChatHistory") << std::endl;

    return 0;
}