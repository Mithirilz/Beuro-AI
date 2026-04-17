#pragma once
#include <dpp/dpp.h>
#include <unordered_map>
#include <SQLiteCpp/SQLiteCpp.h>
#include <ChromaDB/ChromaDB.h>

class ChromaDB_Execs{ 
    private:
        std::vector<std::string> chat;
        std::unordered_map<int, std::string> chat_set;
        chromadb::Client BeuroVDB;
        
    public:
        ChromaDB_Execs(const std::string& http, const std::string& address, const std::string& port) : BeuroVDB{http, address, port}{}
        void format_message(const std::string& collection_name);
        void display_messages(const std::string& collection_name);
        std::unordered_map<int, std::string> Get_Chat_Data();
        void inject_into_VDB(const std::string& collection_name);
        void DoesCollectionExist();
        void store_message(const std::string message);
        void GetAllInfoFromCollection(const std::string& collection_name);
        std::vector<std::string> SearchThroughVDB(const std::vector<std::string>& query_data);
        void hard_reset();
};

class SQL_Execs{
    private:
        SQLite::Database BeuroDB;
        std::vector<int> ID_targets;

    public:
        SQL_Execs(const std::string& FILEPATH) : BeuroDB{FILEPATH, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE}{}
        void CreateTable();
        dpp::job InsertDataintoTable(std::unordered_map<int, std::string> chat_set);
        void GetAllInformationFromTable();
        std::string GetInformationFromIDTargets();
        void GetIDTargets(std::vector<std::string> IDs);
};

class BeuroAI{
    private:
        std::deque<std::unordered_map<std::string, std::string>> chat_history; 
        std::mutex chat_history_lock;
        ChromaDB_Execs chromaexec;
        SQL_Execs sqlexec;

    public:
        BeuroAI(const std::string& FILEPATH, const std::string& PORT) : chromaexec("http", "127.0.0.1", PORT), sqlexec(FILEPATH){}

        dpp::task<void> Beuro_Response(std::string user_message, const dpp::message_create_t& event, dpp::cluster& Beuro);
        dpp::task<void> initiate_act(const std::string& DECISION, const std::string& user_message);
        dpp::task<void> store_memory(dpp::cluster& Beuro);
        dpp::task<std::string> make_a_decision(const std::string& user_message, const dpp::message_create_t& event, dpp::cluster& Beuro);
        dpp::job writeBeuro_ChatHistory(std::string beuro_chat, std::string user, std::string user_message);
};

