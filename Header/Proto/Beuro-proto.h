#pragma once
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>
#include <ChromaDB/ChromaDB.h>

class ChromaDB_Execs {
    private:
        chromadb::Client BeuroVDB;
        std::vector<std::string> chats;

    public:
        ChromaDB_Execs(const std::string& http, const std::string& address, const std::string& port) : BeuroVDB{http, address, port}{}
        std::string InsertEmbedIntoCollection();
        std::string OnlyDisplayMessagesForStoring();
        void InsertMessagesForStoring();
        std::string DoesCollectionExist();
        std::string GetAllEmbeddingsFromCollection(const std::string& collection_name);
        std::string GetIDFromQueryData(const std::vector<std::string>& query_data);
};

class SQL_Execs{
    private:
        SQLite::Database BeuroDB;
        std::vector<int> ID_targets;
        std::vector<std::string> chat_content;    

    public:
        SQL_Execs(const std::string& FILEPATH) : BeuroDB{FILEPATH, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE}{}
        void CreateTable();
        void ReadFromFile();
        void InsertDataintoTable();
        void QueryAllInformation();
        void QueryTargettedData();
        void ChangeDataType(std::string& ID);
};