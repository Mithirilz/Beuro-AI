#include <nlohmann/json.hpp>
#include "Beuro/BeuroAI.h"
#include <unordered_map>
#include <dpp/dpp.h>
#include <fstream>
#include <string>
#include <mutex>
using json = nlohmann::json;

dpp::job BeuroAI::writeBeuro_ChatHistory(const std::string beuro_chat, const std::string user, const std::string user_message){
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

dpp::task<void> BeuroAI::Beuro_Commands(const std::string& DECISION, const std::string& content_message){
    if (DECISION == "NOTHING"){
        std::cout << "Beuro did NOTHING" << std::endl;
        co_return;
    }

    else if(DECISION == "RETRIEVE MEMORY"){
        std::cout << "Beuro ENGAGED RAG (RETRIEVAL)" << std::endl;

        sqlexec.GetIDTargets(
            chromaexec.SearchThroughVDB({content_message})
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
        chromaexec.store_message(content_message);
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

dpp::task<std::string> BeuroAI::Decision_Maker(const std::string& user_message, const dpp::message_create_t& event, dpp::cluster& Beuro){    
    std::vector<std::unordered_map<std::string, std::string>> command_set;
    
    std::unordered_map<std::string, std::string> command;
    command["role"] = "system";
    command["content"] = "You can only respond with these 3 answers(RETRIEVE MEMORY, NOTHING)\n"
                         "If the user isn't mentioning new information, always pick NOTHING.\n"
                         "If there is information said that wasn't mentioned througho   ut the conversation, use RETRIEVE MEMORY";
    command_set.push_back(command);


    std::unordered_map<std::string, std::string> user_chat;
    user_chat["role"] = "user";
    user_chat["content"] = user_message;
    command_set.push_back(user_chat);
    
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

    co_return message.at("message").at("content").get<std::string>();
}

dpp::task<void> BeuroAI::Beuro_Response(std::string user_message, const dpp::message_create_t& event, dpp::cluster& Beuro){
    dpp::async typing_status = Beuro.co_channel_typing(event.msg.channel_id);
    dpp::task<std::string> decision_task;
    
    if(!event.msg.is_dm()){
        std::string BOT_ID = ("<@" + std::to_string(Beuro.me.id) + ">");
        auto BOT_ID_index = user_message.find(BOT_ID);
        
        user_message.erase(BOT_ID_index, BOT_ID.length());
    }

    const std::string content_message = "[" + event.msg.author.username + "] : " + user_message;

    if(this->decider){
        decision_task = this->decider(user_message, event, Beuro);
    }

    if (this->chat_history.size() > 250){
        {
            std::lock_guard<std::mutex> lock(this->chat_history_lock);
            chat_history.erase(this->chat_history.begin(), this->chat_history.begin()+150);
        }    
    }

    if (this->decider){
        co_await Beuro_Commands(
            co_await decision_task,
            content_message
        );
    }

    std::unordered_map<std::string, std::string> user_chat;
    user_chat["role"] = "user";
    user_chat["content"] = content_message;    
    {
        std::lock_guard<std::mutex> lock(this->chat_history_lock);
        this->chat_history.push_back(user_chat);    
    }
    
    json message_to_send;
    {
        std::lock_guard<std::mutex> lock(this->chat_history_lock);
        message_to_send["model"] = "Beuro";
        message_to_send["messages"] = chat_history;
        message_to_send["stream"] = false;
    }

    const std::string http_message = message_to_send.dump();
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

    const std::string beuro_response = json::parse(response.body).at("message").at("content").get<std::string>();

    co_await typing_status;
    event.co_reply(beuro_response);

    std::unordered_map<std::string, std::string> beuro_chat;
    beuro_chat["role"] = "assistant";
    beuro_chat["content"] = beuro_response;
    {
        std::lock_guard<std::mutex> lock(this->chat_history_lock);
        this->chat_history.push_back(beuro_chat);

        for (int i = 0; i < this->chat_history.size(); i++){
            std::cout << this->chat_history[i].at("content") << std::endl;

            if (i == this->chat_history.size()-1){
                std::cout << "---------------------RENEWED RECORD HISTORY--------------------" << std::endl;
            }
        }
    }
    
    writeBeuro_ChatHistory(beuro_response, event.msg.author.username, user_message);
    co_return;
}