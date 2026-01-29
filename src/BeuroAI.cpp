#include <dpp/dpp.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <unordered_map>
using json = nlohmann::json;

struct ServerHistories{}; 
std::deque<std::unordered_map<std::string, std::string>> chat_history; 
std::mutex chat_history_lock;

void writeBeuro_ChatHistory(std::string beuro_chat, std::string user, std::string user_message){
    std::ofstream RecordHistory;

    RecordHistory.open("History.txt", std::ofstream::app);

    if(!RecordHistory.is_open()){
        std::cout << "File could not be opened";
        return;
    }

    RecordHistory << user + ": " + user_message << std::endl;
    RecordHistory << "Beuro: " + beuro_chat << std::endl << std::endl;
    
    RecordHistory.close();
}

dpp::task<void> Beuro_Response(std::string user_message, const dpp::message_create_t& event, dpp::cluster& Beuro){
    if (chat_history.size() > 250){
        {
            std::lock_guard<std::mutex> lock(chat_history_lock);
            chat_history.erase(chat_history.begin(), chat_history.begin()+150);
        }    
    }

    if(!event.msg.is_dm()){
        std::string BOT_ID = ("<@" + std::to_string(Beuro.me.id) + ">");
        auto BOT_ID_index = user_message.find(BOT_ID);
        
        user_message.erase(BOT_ID_index, BOT_ID.length());
    }
    std::string user = event.msg.author.username;
    std::string content_message = "[User " + user + "]: " + user_message;
    
    {
        std::lock_guard<std::mutex> lock(chat_history_lock);
        
        std::unordered_map<std::string, std::string> user_chat;
        user_chat["role"] = "user";
        user_chat["content"] = content_message;
        chat_history.push_back(user_chat);
    }

    json message_to_send;
    message_to_send["model"] = "Beuro-proto";
    message_to_send["messages"] = chat_history;
    message_to_send["stream"] = false;

    std::string http_message = message_to_send.dump();

    dpp::http_request_completion_t response = co_await Beuro.co_request(
        "http://127.0.0.1:11434/api/chat", 
        dpp::m_post, 
        http_message, 
        "application/json"
    );

    if(response.status != 200){
        event.reply("ZzzZZzz...");
        std::cout << "Self-debug: Bot is either not on or faces a different error (" << response.status << ")";
        {
            std::lock_guard<std::mutex> lock(chat_history_lock);
            chat_history.pop_back();
        }
        co_return;
    }

    auto json_Beuro = json::parse(response.body);
    std::string beuro_response = json_Beuro.at("message").at("content").get<std::string>();

    Beuro.channel_typing(event.msg.channel_id);
    event.co_reply(beuro_response);

    {
        std::lock_guard<std::mutex> lock(chat_history_lock);
        
        std::unordered_map<std::string, std::string> beuro_chat;
        beuro_chat["role"] = "assistant";
        beuro_chat["content"] = beuro_response;
        chat_history.push_back(beuro_chat);
    }

    for (int i = 0; i < chat_history.size(); i++){
        std::cout << chat_history[i].at("role") << ": " << chat_history[i].at("content") << std::endl;

        if (i == chat_history.size()-1){
            std::cout << "--------" << std::endl;
        }
    }

    writeBeuro_ChatHistory(json_Beuro.at("message").at("content").get<std::string>(), user, user_message);
}