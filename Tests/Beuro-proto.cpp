#include <iostream> 
#include <ChromaDB/ChromaDB.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include "Beuro-proto.h"

int main(){
    SQL_Execs sqlexec;
    ChromaDB_Execs chromaexec;

    chromadb::Client BeuroDB("http", "127.0.0.1", "8080");

    chromaexec.GetEmbeddingFromCollection(BeuroDB);
}