#include "Proto/Beuro-proto.h"
#include <ChromaDB/ChromaDB.h>
#include <Http/httplib.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

void Proto::ChromaDB_Execs::PrepareMessagesForStoring(const std::string& collection_name){
    if(this->chat.empty()){
        std::cout << "There is no chat to format, please prepare the chat data first." << std::endl;
        return;
    }

    if(!BeuroVDB.CollectionExists(collection_name)){
        std::cout << "Collection doesn't exist." << std::endl;
        return;
    }
    
    std::cout << BeuroVDB.GetHeartbeat() << std::endl;

    chromadb::Collection message_history = BeuroVDB.GetCollection(collection_name);
    auto results = BeuroVDB.GetEmbeddings(message_history);
    
    std::string STR_lastID = results.back().id;

    STR_lastID.erase(STR_lastID.find("ID"), 2);
    int INT_lastID = std::stoi(STR_lastID);
    int counter = INT_lastID + 1;

    for(int i = 0; i < this->chat.size(); i+=2){
        this->chat_set[counter] = chat[i] + "\n"+ chat[i+1];        
        ++counter;
    }    
}

std::unordered_map<int, std::string> Proto::ChromaDB_Execs::Get_Chat_Data(){
    return this->chat_set;
}

void Proto::ChromaDB_Execs::DisplayMessagesForStoring(const std::string& collection_name){
    if(this->chat.empty()){
        std::cout << "The chat array for storage is empty, set the data first." << std::endl;
        return;
    }

    chromadb::Collection message_history = BeuroVDB.GetCollection(collection_name);
    auto results = BeuroVDB.GetEmbeddings(message_history);
    
    std::string STR_lastID = results.back().id;

    STR_lastID.erase(STR_lastID.find("ID"), 2);
    int INT_lastID = std::stoi(STR_lastID);
    int counter = INT_lastID + 1;

    for(int i = 0; i < this->chat.size();i+=2){
        std::string ID = "ID" + std::to_string(counter);
        std::string display_msg = this->chat[i] + "\n" + this->chat[i+1];
        
        std::cout << ID << std::endl;
        std::cout << display_msg << std::endl << std::endl;
        
        ++counter;
    }
}

void Proto::ChromaDB_Execs::Get_Chat_From_Chat_File(){
    std::string message;
    std::ifstream memory("Memory.txt");
    
    if(!memory.is_open()){
        std::cout << "Unable to open the file" << std::endl;
    }
    
    while(std::getline(memory, message)){
        if(message == "") {
            continue;
        }
        
        std::cout << message << std::endl;
        this->chat.emplace_back(message);
    }

    memory.close();
}

void Proto::ChromaDB_Execs::DoesCollectionExist(){
    std::cout << BeuroVDB.GetHeartbeat() << std::endl;

    if(!BeuroVDB.CollectionExists("ChatHistory")) {
        std::cout << "This collection does not exist" << std::endl;
    }
    
    std::cout << "This collection exists!" << std::endl;
}

void Proto::ChromaDB_Execs::Get_All_IDs_and_Embeddings_from_Collection(const std::string& collection_name){
    if(!BeuroVDB.CollectionExists(collection_name)){
        std::cout << "Collection doesn't exist." << std::endl;
        return;
    }

    chromadb::Collection message_history = BeuroVDB.GetCollection(collection_name);
    auto results = BeuroVDB.GetEmbeddings(message_history);
    
    for(const auto& returned_data : results){
        std::cout << returned_data.id << std::endl;
    }
}

std::vector<std::string> Proto::ChromaDB_Execs::SearchThroughChromaDB(const std::vector<std::string>& query_data){
    auto timer_start = std::chrono::high_resolution_clock::now();
    std::shared_ptr<chromadb::LocalOllamaEmbeddingFunction> OllamaEmbeddingFunction = std::make_shared<chromadb::LocalOllamaEmbeddingFunction>("127.0.0.1:11434", "nomic-embed-text");

    chromadb::Collection collection = BeuroVDB.GetCollection("ChatHistory", OllamaEmbeddingFunction);

    auto embeds = OllamaEmbeddingFunction->Generate(query_data);
    auto results = BeuroVDB.Query(collection, {}, embeds, 3);

    auto timer_end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> timed = timer_end - timer_start;
    std::cout << timed << std::endl;

    return results[0].ids;
}

void Proto::ChromaDB_Execs::hard_reset(){
    BeuroVDB.Reset();
}
