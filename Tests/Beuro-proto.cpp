#include <SQLiteCpp/SQLiteCpp.h>
#include <ChromaDB/ChromaDB.h>
#include "Proto/Beuro-proto.h"
#include <Http/httplib.h>
#include <dotenv.h>
#include <string>

int main(){
    dotenv::init();
    ChromaDB_Execs chromaexec("http", "127.0.0.1", "8080"); 
    SQL_Execs sqlexec(dotenv::getenv("FILEPATH"));

    sqlexec.GetIDTargets(
        chromaexec.SearchThroughChromaDB({"Beuro, you remember about that book ready player one?"})
    );

    std::cout << sqlexec.GetInformationFromIDTargets() << std::endl;


    return 0; 
}

/*
Database currently stores user response and Beuro's answers in seperate rows, that is unwanted — we must make both sqlite class store the data THE SAME way as the chromadb class does

1. Make a chromadb function that returns chat data so that sqlite can read it
2. Ensure chromadb doesnt inject said data into chromadb to avoid duplicating data
3. Once that is done, make the sqlite iterate their rows to check if it worked (also check if chromadb didnt duplicate, just incase)
4. I had to break the chromadb ID counter function for a bit, go fix it once everything works (next time use comments btw)
*/
