#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include <ChromaDB/ChromaDB.h>
#include <unordered_map>
#include <vector>

class ChromaDB_Execs {
    private:
        chromadb::Client BeuroVDB;
        std::vector<std::string> chat;
        std::unordered_map<int, std::string> chat_set;
        
    public:
        ChromaDB_Execs(const std::string& http, const std::string& address, const std::string& port) : BeuroVDB{http, address, port}{}
        void PrepareMessagesForStoring(const std::string& collection_name);
        void DisplayMessagesForStoring(const std::string& collection_name);
        std::unordered_map<int, std::string> Get_Chat_Data();
        void InsertMessagesForStoringIntoVDB(const std::string& collection_name);
        void DoesCollectionExist();
        void Get_Chat_From_Chat_File();
        void Get_All_IDs_and_Embeddings_from_Collection(const std::string& collection_name);
        std::vector<std::string> SearchThroughChromaDB(const std::vector<std::string>& query_data);
        void hard_reset();
};

class SQL_Execs{
    private:
        SQLite::Database BeuroDB;
        std::vector<int> ID_targets;

    public:
        SQL_Execs(const std::string& FILEPATH) : BeuroDB{FILEPATH, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE}{}
        void CreateTable();
        void InsertDataintoTable(std::unordered_map<int, std::string> chat_set);
        void GetAllInformationFromAllColumns();
        std::string GetInformationFromIDTargets();
        void GetIDTargets(std::vector<std::string> IDs);
};