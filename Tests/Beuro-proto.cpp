#include <ChromaDB/ChromaDB.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include "Beuro-proto.h"

int main(){
    SQL_Execs sqlexec;
    ChromaDB_Execs chromaexec;

    chromadb::Client BeuroDB("http", "127.0.0.1", "8080");
    

    //Classes are severely hard to understand, refactor imminent   
    std::cout << chromaexec.QueryData(BeuroDB, {"Beuro you wanna talk about ready player one?"}) << std::endl;
}