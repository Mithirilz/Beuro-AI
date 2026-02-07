#include <Http/httplib.h>
#include <ChromaDB/ChromaDB.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Proto/Beuro-proto.h"

std::string ChromaDB_Execs::InsertEmbedIntoCollection(){
    std::cout << BeuroVDB.GetHeartbeat() << std::endl;

    if (this->chats.empty()){
        return "The chat array is empty";
    }

    int counter = 1;

    std::vector<std::string> chat_set = {};
    std::vector<std::string> IDs = {};
    
    for(int i = 0; i < this->chats.size(); i+=2){
        std::string ID = "ID" + std::to_string(counter);
        std::string temp_msg = this->chats[i] + "\n" + this->chats[i+1];
        
        std::cout << ID << std::endl;
        std::cout << temp_msg << std::endl << std::endl;
        
        IDs.emplace_back(ID);
        chat_set.emplace_back(temp_msg);
        
        counter = counter + 1;
    }

    if (!BeuroVDB.CollectionExists("ChatHistory")){
        BeuroVDB.CreateCollection("ChatHistory");
    }

    chromadb::Collection message_history = BeuroVDB.GetCollection("ChatHistory");
    std::shared_ptr<chromadb::LocalOllamaEmbeddingFunction> OllamaEmbeddingFunction = std::make_shared<chromadb::LocalOllamaEmbeddingFunction>("127.0.0.1:11434", "nomic-embed-text");
    
    auto embeds = OllamaEmbeddingFunction->Generate(chat_set);
    BeuroVDB.AddEmbeddings(message_history, IDs, embeds);
    
    return "This function successfully executed.";
}

std::string ChromaDB_Execs::OnlyDisplayMessagesForStoring(){
    int counter = 1;

    for(int i = 0; i < this->chats.size(); i+=2){
        std::string ID = "ID" + std::to_string(counter);
        std::string temp_msg = this->chats[i] + "\n" + this->chats[i+1];
        
        std::cout << ID << std::endl;
        std::cout << temp_msg << std::endl << std::endl;
        
        counter = counter + 1;
    }

    return "Function executed successfully.";
}

void ChromaDB_Execs::InsertMessagesForStoring(){
    std::string message;
    std::ifstream memory("Memory.txt");
    
    if(!memory.is_open()){
        std::cout << "Unable to open the file" << std::endl;
    }
    
    while(std::getline(memory, message)){
        if(message == "") {
            continue;
        }
        
        this->chats.emplace_back(message);
        std::cout << message << std::endl;        
    }

    memory.close();
}

std::string ChromaDB_Execs::DoesCollectionExist(){
    std::cout << BeuroVDB.GetHeartbeat() << std::endl;

    if(!BeuroVDB.CollectionExists("ChatHistory")) {
        return "This collection does not exist";
    }
    
    return "This collection exists!";
}

std::string ChromaDB_Execs::GetAllEmbeddingsFromCollection(const std::string& collection_name){
    chromadb::Collection message_history = BeuroVDB.GetCollection(collection_name);

    auto results = BeuroVDB.GetEmbeddings(message_history);
    
    for(int i = 0; i < results[0].embeddings->size(); i++){
        std::cout << results[i].embeddings->at(i) << std::endl << std::endl;
    }

    return "This function successfully executed.";
}

std::string ChromaDB_Execs::GetIDFromQueryData(const std::vector<std::string>& query_data){
    std::vector<std::chrono::duration<double>> calculated_time;

    auto timer_start = std::chrono::high_resolution_clock::now();
    std::shared_ptr<chromadb::LocalOllamaEmbeddingFunction> OllamaEmbeddingFunction = std::make_shared<chromadb::LocalOllamaEmbeddingFunction>("127.0.0.1:11434", "nomic-embed-text");

    chromadb::Collection collection = BeuroVDB.GetCollection("ChatHistory", OllamaEmbeddingFunction);

    auto embeds = OllamaEmbeddingFunction->Generate(query_data);
    auto results = BeuroVDB.Query(collection, {}, embeds, 1);

    for (int i = 0; i < results.size(); i++){
        for (int j = 0; j < results[i].ids.size(); j++){
            std::cout << "IDs: " << results[i].ids.at(j) << std::endl;
        }
    }

    auto timer_end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> timed = timer_end - timer_start;
    std::cout << timed << std::endl;
    
    return "Function has executed successfully.";
}
