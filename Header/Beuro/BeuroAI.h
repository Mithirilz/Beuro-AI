#pragma once
#include <atomic>
#include <mutex>
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
        void is_collection_exist();
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
        int getNumberofIDs();
        std::string GetInformationFromIDTargets();
        void GetIDTargets(std::vector<std::string> IDs);
};

class BeuroAI{
    private:
        std::deque<dpp::task<std::string>> m_Priority_Queue;
        std::vector<dpp::task<void>> m_User_Queue;
        std::deque<std::unordered_map<std::string, std::string>> m_chat_history; 
        std::mutex m_chat_history_lock;
        ChromaDB_Execs m_chromaexec;
        std::function<dpp::task<std::string>(const std::string& user_message, const dpp::message_create_t& event, dpp::cluster& Beuro)> m_is_decider_active = nullptr;
        SQL_Execs m_sqlexec;
        std::atomic<bool> m_is_processing = false;

    public:
        BeuroAI(const std::string& FILEPATH, const std::string& PORT);
        
        void manage_task_queue(std::string user_message, const dpp::message_create_t &event, dpp::cluster &Beuro);
        dpp::task<void> Beuro_Response(std::string user_message, const dpp::message_create_t& event, dpp::cluster& Beuro);
        dpp::task<std::string> initiate_act(const std::string& DECISION, const std::string& content_message);
        dpp::task<void> store_memory(dpp::cluster& Beuro);
        dpp::task<std::string> make_a_decision(const std::string user_message, const dpp::message_create_t& event, dpp::cluster& Beuro);
        dpp::job savetxtfile_chat_history(std::string beuro_chat, std::string user, std::string user_message);
};

