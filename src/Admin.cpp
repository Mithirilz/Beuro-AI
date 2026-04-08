#include "Beuro/BeuroRAG.h"
#include <memory>
#include <string>

void Actual::ChromaDB_Execs::format_message(const std::string& collection_name){
    if(this->chat.empty()){
        std::cout << "There is no chat to format, please prepare the chat data first." << std::endl;
        return;
    }

    if(!BeuroVDB.CollectionExists(collection_name)){
        std::cout << "Collection doesn't exist." << std::endl;
        return;
    }
    
    std::cout << "Beuro's Memory Heartbeat: "<< BeuroVDB.GetHeartbeat() << std::endl;

    chromadb::Collection ChatHistory = BeuroVDB.GetCollection(collection_name);
    auto results = BeuroVDB.GetEmbeddings(ChatHistory);
    
    std::string STR_lastID = results.back().id;

    STR_lastID.erase(STR_lastID.find("ID"), 2);
    int INT_lastID = std::stoi(STR_lastID);
    int counter = INT_lastID + 1;

    for(int i = 0; i < this->chat.size(); i+=2){
        this->chat_set[counter] = chat[i] + "\n"+ chat[i+1];        
        ++counter;
    }    
}

std::unordered_map<int, std::string> Actual::ChromaDB_Execs::Get_Chat_Data(){
    std::unordered_map<int, std::string> chat_set_copy = this->chat_set; 
    this->chat_set.clear(); //Clears back memory area

    return chat_set_copy;
}

void Actual::ChromaDB_Execs::display_messages(const std::string& collection_name){
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

    for(int i = 0; i < this->chat.size(); i+=2){
        std::string ID = "ID" + std::to_string(counter);
        std::string display_msg = this->chat[i] + "\n" + this->chat[i+1];
        
        std::cout << ID << std::endl;
        std::cout << display_msg << std::endl << std::endl;
        
        ++counter;
    }
}

void Actual::ChromaDB_Execs::store_message(const std::string message){
    this->chat.emplace_back(message);
}

void Actual::ChromaDB_Execs::inject_into_VDB(const std::string& collection_name){
    std::vector<std::string> IDs = {};
    std::vector<std::string> Chats = {};

    std::shared_ptr<chromadb::LocalOllamaEmbeddingFunction> OllamaEmbedder = std::make_shared<chromadb::LocalOllamaEmbeddingFunction>("127.0.0.1:11434", "nomic-embed-text");

    auto collection = BeuroVDB.GetCollection(collection_name, OllamaEmbedder);

    for (auto& [ID, chat] : this->chat_set){
        IDs.push_back("ID" + std::to_string(ID));
        Chats.push_back(chat);
    }

    auto embeddings = OllamaEmbedder->Generate(Chats);
    this->BeuroVDB.AddEmbeddings(collection, IDs, embeddings);    
    return;
}

void Actual::ChromaDB_Execs::DoesCollectionExist(){
    std::cout << BeuroVDB.GetHeartbeat() << std::endl;

    if(!BeuroVDB.CollectionExists("ChatHistory")) {
        std::cout << "This collection does not exist" << std::endl;
    }
    
    std::cout << "This collection exists!" << std::endl;
}

void Actual::ChromaDB_Execs::GetAllInfoFromCollection(const std::string& collection_name){
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

std::vector<std::string> Actual::ChromaDB_Execs::SearchThroughVDB(const std::vector<std::string>& query_data){
    auto timer_start = std::chrono::high_resolution_clock::now();
    std::shared_ptr<chromadb::LocalOllamaEmbeddingFunction> OllamaEmbeddingFunction = std::make_shared<chromadb::LocalOllamaEmbeddingFunction>("127.0.0.1:11434", "nomic-embed-text");

    chromadb::Collection collection = BeuroVDB.GetCollection("ChatHistory", OllamaEmbeddingFunction);

    auto embeds = OllamaEmbeddingFunction->Generate(query_data);
    auto results = BeuroVDB.Query(collection, {}, embeds, 1);

    auto timer_end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> timed = timer_end - timer_start;
    std::cout << timed << std::endl;

    return results[0].ids;
}

void Actual::ChromaDB_Execs::hard_reset(){
    BeuroVDB.Reset();
}
