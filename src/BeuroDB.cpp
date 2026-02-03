#include <SQLiteCpp/SQLiteCpp.h>
#include <fstream>
#include <dotenv.h>
#include <iostream>

std::vector<std::string> ReadFromFile();

int main(){
    dotenv::init();
    const std::string FILEPATH = dotenv::getenv("FILEPATH", "None");
    SQLite::Database BeuroDB(FILEPATH, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
}

std::vector<std::string> ReadFromFile(){
    std::string message;
    std::ifstream memory("Memory.txt");
    
    if(!memory.is_open()){
        std::cout << "Unable to open the file" << std::endl;
    }
    
    std::vector<std::string> chats = {};
    
    while(std::getline(memory, message)){
        if(message == ""){
            continue;
        }
        
        chats.emplace_back(message);
        std::cout << message << std::endl;        
    }

    memory.close();

    return chats;
}

/*
1. Set up database *
2. Send some test data *
3. Try to query the data *
4. Prepare test data for co-query with ChromaDB *
5. Test data query from ChromaDB
6. Try to query data via chromaDB
    - Get data to be organised correctly into blocks first
*/