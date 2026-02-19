#include <nlohmann/json.hpp>
#include "Beuro/BeuroAI.h"
#include <unordered_map>
#include <dpp/dpp.h>
#include <fstream>
#include <string>
#include <mutex>
using json = nlohmann::json;

dpp::job BeuroAI::writeBeuro_ChatHistory(std::string beuro_chat, std::string user, std::string user_message){
    std::ofstream RecordHistory;

    RecordHistory.open("History.txt", std::ofstream::app);

    if(!RecordHistory.is_open()){
        std::cout << "File could not be opened";
        co_return;
    }

    RecordHistory << user + ": " + user_message << std::endl;
    RecordHistory << "Beuro: " + beuro_chat << std::endl << std::endl;
    
    RecordHistory.close();
}

dpp::task<void> BeuroAI::Decision_Maker(const std::string& DECISION, const std::string& user_message){
    if (DECISION == "NOTHING"){
        std::cout << "Beuro did NOTHING" << std::endl;
        co_return;
    }

    else if(DECISION == "RETRIEVE MEMORY"){
        std::cout << "Beuro ENGAGED RAG (RETRIEVAL)" << std::endl;
        co_await sqlexec.GetIDTargets(
            co_await chromaexec.SearchThroughVDB({user_message})
        );

        std::unordered_map<std::string, std::string> system_chat;
        system_chat["role"] = "system";
        system_chat["content"] = sqlexec.GetInformationFromIDTargets();

        {
            std::lock_guard<std::mutex> lock(this->chat_history_lock);
            this->chat_history.push_back(system_chat);
        }

        co_return;
    }

    else if(DECISION == "REMEMBER FACT"){
        std::cout << "Beuro ENGAGED RAG (STORE)" << std::endl;
        
        //VECTOR DATABASE RAG PIPELINE
        chromaexec.store_message(user_message);
        chromaexec.format_message("ChatHistory");
        chromaexec.display_messages("ChatHistory");
        
        chromaexec.inject_into_VDB("ChatHistory");

        sqlexec.InsertDataintoTable(
            chromaexec.Get_Chat_Data()
        );

        co_return;
    }

    else{
        std::cout << "Beuro made an INVALID DECISION" << std::endl;
        co_return;
    }
}


dpp::task<std::string> BeuroAI::Agentic_AI(const std::string& user_message, const dpp::message_create_t& event, dpp::cluster& Beuro){    
    std::vector<std::unordered_map<std::string, std::string>> command_set;
    std::unordered_map<std::string, std::string> command;
    
    command["role"] = "system";
    command["content"] = "Deduce what the user wants you to do here and ONLY say one of these 3 depending on the answer(RETRIEVE MEMORY, REMEMBER FACT, NOTHING)";
    command_set.push_back(command);

    command["role"] = "user";
    command["content"] = user_message;
    command_set.push_back(command);
    
    json message_to_send;
    message_to_send["model"] = "Beuro-proto";
    message_to_send["messages"] = command_set;
    message_to_send["stream"] = false;
    
    std::string http_message = message_to_send.dump();

    auto result = co_await Beuro.co_request(
        "http://127.0.0.1:11434/api/chat",
        dpp::m_post,
        http_message,
        "application/json"
    );

    auto message = json::parse(result.body);

    co_return message.at("message").at("content");
}

dpp::task<void> BeuroAI::Beuro_Response(std::string user_message, const dpp::message_create_t& event, dpp::cluster& Beuro){
    dpp::async typing_status = Beuro.co_channel_typing(event.msg.channel_id);
    auto decision_result = Agentic_AI(user_message, event, Beuro);


    std::string username = event.msg.author.username;
    std::string content_message = "[User " + username + "]: " + user_message;

    if (this->chat_history.size() > 250){
        {
            std::lock_guard<std::mutex> lock(this->chat_history_lock);
            chat_history.erase(this->chat_history.begin(), this->chat_history.begin()+150);
        }    
    }
    
    if(!event.msg.is_dm()){
        std::string BOT_ID = ("<@" + std::to_string(Beuro.me.id) + ">");
        auto BOT_ID_index = user_message.find(BOT_ID);
        
        user_message.erase(BOT_ID_index, BOT_ID.length());
    }
    
    
    std::unordered_map<std::string, std::string> user_chat;
    user_chat["role"] = "user";
    user_chat["content"] = content_message;
    
    {
        std::lock_guard<std::mutex> lock(this->chat_history_lock);
        this->chat_history.push_back(user_chat);    
    }

    co_await Decision_Maker(
        co_await decision_result,
        user_message
    );
    
    json message_to_send;
    message_to_send["model"] = "Beuro-proto";
    message_to_send["messages"] = chat_history;
    message_to_send["stream"] = false;
    
    std::string http_message = message_to_send.dump();

    auto response = co_await Beuro.co_request(
        "http://127.0.0.1:11434/api/chat", 
        dpp::m_post, 
        http_message, 
        "application/json"
    );
    
    if(response.status != 200){
        event.co_reply("ZzzZZzz...");
        std::cout << "Self-debug: Bot is either not on or faces a different error (" << response.status << ")";
        {
            std::lock_guard<std::mutex> lock(chat_history_lock);
            this->chat_history.pop_back();
            this->chat_history.pop_back();
        }
        
        co_return;
    }

    auto json_Beuro = json::parse(response.body);
    std::string beuro_response = json_Beuro.at("message").at("content").get<std::string>();

    co_await typing_status;
    event.co_reply(beuro_response);


    std::unordered_map<std::string, std::string> beuro_chat;
    beuro_chat["role"] = "assistant";
    beuro_chat["content"] = beuro_response;

    {
        std::lock_guard<std::mutex> lock(this->chat_history_lock);
        this->chat_history.push_back(beuro_chat);
    }
    
    {
        std::lock_guard<std::mutex> lock(this->chat_history_lock);
        
        for (int i = 0; i < this->chat_history.size(); i++){
            std::cout << this->chat_history[i].at("content") << std::endl;
            
            if (i == this->chat_history.size()-1){
                std::cout << "--------------------------------" << std::endl;
            }
        }
    }
    
    writeBeuro_ChatHistory(json_Beuro.at("message").at("content").get<std::string>(), username, user_message);
    co_return;
}