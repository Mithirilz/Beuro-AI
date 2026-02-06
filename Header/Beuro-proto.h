#pragma once
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>
#include <ChromaDB/ChromaDB.h>
#include <Http/httplib.h>

class ChromaDB_Execs {
    private:
        std::vector<std::string> chats;

    public:
        std::string AddEmbedIntoCollection(chromadb::Client& beuroDB);
        std::string DisplayMemory_to_Store();
        std::vector<std::string> ReadFromFile();
        std::string DoesCollectionExist(chromadb::Client& beuroDB);
        std::string UpdateCollection(chromadb::Client& beuroDB);
        std::string GetEmbeddingFromCollection(chromadb::Client& beuroDB);
        std::string QueryData(chromadb::Client& beuroDB, const std::vector<std::string>& query_data);
};

class SQL_Execs{
    private:
        std::vector<int> ID_targets;
        std::vector<std::string> chat_content;    

    public:
        void CreateTable(SQLite::Database& beuroDB);
        void ReadFromFile();
        void InsertDataintoTable(SQLite::Database& beuroDB);
        void QueryAllInformation(SQLite::Database& beuroDB);
        void QueryTargettedData(SQLite::Database& beuroDB);
        void ChangeDataType(std::string& ID);
};