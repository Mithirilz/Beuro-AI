#include "SQLiteCpp/Database.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <ChromaDB/ChromaDB.h>
#include <dotenv.h> 

int main(){ 
    SQLite::Database BeuroDB(dotenv::getenv("FILEPATH"));
    

    return 0;
}