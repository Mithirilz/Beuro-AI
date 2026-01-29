#include <Http/httplib.h>
#include <ChromaDB/ChromaDB.h>
#include <functional>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::string AddEmbedIntoCollection(chromadb::Client& beuroDB);
std::vector<std::string> ReadFromFile();
std::string DoesCollectionExist(chromadb::Client& beuroDB);
std::string UpdateCollection(chromadb::Client& beuroDB);
std::string GetEmbeddingFromCollection(chromadb::Client& beuroDB);

int main(){
    chromadb::Client beuroDB("http", "127.0.0.1", "8080"); 
    
    std::unordered_map<int, std::function<std::string()>> function;
    
    function[1] = [&beuroDB](){return AddEmbedIntoCollection(beuroDB);};
    function[2] = [&beuroDB](){return DoesCollectionExist(beuroDB);};
    function[3] = [&beuroDB](){return UpdateCollection(beuroDB);};
    function[4] = [&beuroDB](){return GetEmbeddingFromCollection(beuroDB);};
    
    int decision = 0;

    std::cout << "--- USER INTERFACE ---" << std::endl;
    std::cout << "1. Add embed into Collection" << std::endl;
    std::cout << "2. Does collection exist?" << std::endl;
    std::cout << "3. Update collection" << std::endl;
    std::cout << "4. Get embedding from collection" << std::endl;
    std::cout << "What would you like to do: ";
    std::cin >> decision;

    std::cout << function[decision]() << std::endl;
    
    return 0;
}    

std::string AddEmbedIntoCollection(chromadb::Client& beuroDB){
    std::cout << beuroDB.GetHeartbeat() << std::endl;

    int counter = 1;
    
    std::vector<std::string> chats = ReadFromFile();

    std::vector<std::string> chat_set = {};
    std::vector<std::string> IDs = {};
    
    for(int i = 0; i < chats.size(); i+=2){
        std::string ID = "ID" + std::to_string(counter);
        std::string temp_msg = chats[i] + "\n" + chats[i+1];
        
        std::cout << ID << std::endl;
        std::cout << temp_msg << std::endl << std::endl;
        
        IDs.emplace_back(ID);
        chat_set.emplace_back(temp_msg);
        
        counter = counter + 1;
    }

    std::shared_ptr<chromadb::LocalOllamaEmbeddingFunction> OllamaEmbeddingFunction = std::make_shared<chromadb::LocalOllamaEmbeddingFunction>("127.0.0.1:11434", "nomic-embed-text");
   
    chromadb::Collection message_history = beuroDB.CreateCollection("ChatHistory");
    
    auto embeds = OllamaEmbeddingFunction->Generate(chat_set);
    beuroDB.AddEmbeddings(message_history, IDs, embeds);
    
    return "This function successfully executed.";
}

std::vector<std::string> ReadFromFile(){
    std::string message;
    std::ifstream memory("Memory.txt");
    
    if(!memory.is_open()){
        std::cout << "Unable to open the file" << std::endl;
    }
    
    std::vector<std::string> chats = {};
    
    while(std::getline(memory, message)){
        if(message == "") {continue;}
        
        chats.emplace_back(message);
        std::cout << message << std::endl;        
    }

    memory.close();

    return chats;
}

std::string DoesCollectionExist(chromadb::Client& beuroDB){
    std::cout << beuroDB.GetHeartbeat() << std::endl;
    
    bool exists = beuroDB.CollectionExists("ChatHistory");

    if(exists != true) {return "This collection does not exists";}
    
    return "This collection exists!";
}

std::string UpdateCollection(chromadb::Client& beuroDB){
    beuroDB.UpdateCollection("ChatHistory", "NewName");

    return "This function successfully executed.";
}

std::string GetEmbeddingFromCollection(chromadb::Client& beuroDB){
    chromadb::Collection message_history = beuroDB.GetCollection("ChatHistory");

    auto results = beuroDB.GetEmbeddings(message_history);
    
    for(int i = 0; i < results[0].embeddings->size(); i++){
        std::cout << results[i].embeddings->at(i) << std::endl << std::endl;
    }

    return "This function successfully executed.";
}