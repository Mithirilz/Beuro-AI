#pragma once
#include "Beuro/BeuroRAG.h" 
#include <dpp/dpp.h>

class BeuroAI{
    private:
        std::deque<std::unordered_map<std::string, std::string>> chat_history; 
        std::mutex chat_history_lock;
        Actual::ChromaDB_Execs chromaexec;
        Actual::SQL_Execs sqlexec;

    public:
        BeuroAI(const std::string& FILEPATH, const std::string& PORT) : chromaexec("http", "127.0.0.1", PORT), sqlexec(FILEPATH){}

        dpp::task<void> Beuro_Response(std::string user_message, const dpp::message_create_t& event, dpp::cluster& Beuro);
        dpp::task<void> Beuro_Commands(const std::string& DECISION, const std::string& user_message);
        dpp::task<std::string> Decision_Maker(const std::string& user_message, const dpp::message_create_t& event, dpp::cluster& Beuro);
        dpp::job writeBeuro_ChatHistory(std::string beuro_chat, std::string user, std::string user_message);
};

