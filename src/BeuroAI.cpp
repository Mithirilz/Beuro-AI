#include <nlohmann/json.hpp>
#include "Beuro/BeuroAI.h"
#include <unordered_map>
#include <dpp/dpp.h>
#include <fstream>
#include <string>
#include <mutex>
using json = nlohmann::json;

BeuroAI::BeuroAI(const std::string& FILEPATH, const std::string& PORT) : m_chromaexec{"http", "127.0.0.1", PORT}, m_sqlexec{FILEPATH}{
    int NumberofIDs = m_sqlexec.getNumberofIDs();
    std::cout << NumberofIDs << std::endl;

    if (NumberofIDs != 0){
        m_decider = [this](const std::string &user_message, const dpp::message_create_t &event, dpp::cluster &Beuro)-> dpp::task<std::string>{
            return make_a_decision(user_message, event, Beuro);
        };
    }
}

void BeuroAI::manage_task_queue(std::string user_message, const dpp::message_create_t &event, dpp::cluster &Beuro){
    if (!m_is_processing){
        auto start_processing = Beuro_Response(user_message, event,Beuro);
        return;
    }

    std::unordered_map<std::string, std::string> user_chat;
    user_chat["role"] = "user";
    user_chat["content"] = user_message;

    m_is_processing.wait(m_is_processing);
}

dpp::task<std::string> BeuroAI::make_a_decision(const std::string user_message, const dpp::message_create_t& event, dpp::cluster& Beuro){    
    std::vector<std::unordered_map<std::string, std::string>> command_set;
    
    std::unordered_map<std::string, std::string> command;
    command["role"] = "system";
    command["content"] = "You can only respond with these 2 answers (RETRIEVE MEMORY, NOTHING)\n"
                         "If the user isn't mentioning new information, always pick NOTHING.\n"
                         "If there is information said that wasn't mentioned throughout the conversation, use RETRIEVE MEMORY";

    std::unordered_map<std::string, std::string> user_chat;
    user_chat["role"] = "user";
    user_chat["content"] = user_message;
    
    command_set.push_back(command);
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

dpp::task<std::string> BeuroAI::initiate_act(const std::string& DECISION, const std::string& content_message){
    if (DECISION == "NOTHING"){
        std::cout << "Beuro did NOTHING" << std::endl;
        co_return content_message;
    }

    else if(DECISION == "RETRIEVE MEMORY"){
        std::cout << "Beuro ENGAGED RAG (RETRIEVAL)" << std::endl;

        m_sqlexec.GetIDTargets(
            m_chromaexec.SearchThroughVDB({content_message})
        );

        const std::string final_message = "[Context/Memory]:\n" + m_sqlexec.GetInformationFromIDTargets() + "\n\n" + content_message;

        co_return final_message;
    }

    else{
        std::cout << "Beuro made an INVALID DECISION" << std::endl; 
        co_return content_message;
    }
}

dpp::task<void> BeuroAI::store_memory(dpp::cluster& Beuro){
    std::vector<std::string> summaries;
    summaries.reserve(3);
    
    json message_to_send;
    std::unordered_map<std::string, std::string> user_chat;
    user_chat["role"] = "user";
    user_chat["content"] = "END OF CONVERSATION";
    {
        std::lock_guard<std::mutex> lock(m_chat_history_lock);
        m_chat_history.push_back(user_chat);
        
        message_to_send["model"] = "Summariser";
        message_to_send["messages"] = m_chat_history;
        message_to_send["stream"] = false;
    }
    
    const int NUMBER_OF_SUMMARIES = 3;
    std::string summarised_text;
    std::string next_prompt;

    //Refine summary 3 times before completely shutting down
    for (int i = 0; i < NUMBER_OF_SUMMARIES; i++) {
        auto result = co_await Beuro.co_request(
        "http://127.0.0.1:11434/api/chat",
        dpp::m_post,
        message_to_send.dump(),
        "application/json");

        if (result.status != 200){
            std::cout << "Memory storage failed" << std::endl;
            break;
        }

        if (result.body.find("NULL") != std::string::npos){
            break;
        }

        summarised_text = json::parse(result.body).at("message").at("content").get<std::string>();
        summaries.emplace_back(summarised_text);

        next_prompt = "Cross check this summary for missing or incorrect details send the refined version:\n\n" + summaries[i] + "\n\nIf no changes are needed, return NULL";
        user_chat["content"] = next_prompt;
        {
            std::lock_guard<std::mutex> lock(m_chat_history_lock);
            
            m_chat_history.push_back(user_chat);
            message_to_send["messages"] = m_chat_history;
        }
        
        std::cout << summarised_text << std::endl;        
    }

    m_chromaexec.store_message(summarised_text);
    m_chromaexec.format_message("ChatHistory");
    m_chromaexec.display_messages("ChatHistory");
    m_chromaexec.inject_into_VDB("ChatHistory");

    m_sqlexec.InsertDataintoTable(
        m_chromaexec.Get_Chat_Data()
    );

    co_return;
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

    if(m_is_decider_active){
        decision_task = m_is_decider_active(user_message, event, Beuro);
    }

    if(m_chat_history.size() > 250){
        {
            std::lock_guard<std::mutex> lock(m_chat_history_lock);
            m_chat_history.erase(m_chat_history.begin(), m_chat_history.begin()+150);
        }    
    }

    std::string final_message;

    if (m_is_decider_active){
        final_message = co_await initiate_act(
            co_await decision_task,
            content_message
        );
    }

    std::unordered_map<std::string, std::string> user_chat;
    user_chat["role"] = "user";
    user_chat["content"] = final_message;    
    {
        std::lock_guard<std::mutex> lock(m_chat_history_lock);
        m_chat_history.push_back(user_chat);    
    }
    
    json message_to_send;
    {
        std::lock_guard<std::mutex> lock(m_chat_history_lock);
        message_to_send["model"] = "Beuro-proto";
        message_to_send["messages"] = m_chat_history;
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
            std::lock_guard<std::mutex> lock(m_chat_history_lock);
            m_chat_history.pop_back();
            m_chat_history.pop_back();
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
        std::lock_guard<std::mutex> lock(m_chat_history_lock);
        m_chat_history.push_back(beuro_chat);
    }

    std::cout << final_message << std::endl;
    std::cout << "Beuro: " + beuro_response << std::endl;
    std::cout << "__________________________________" << std::endl;
    
    savetxtfile_chat_history(beuro_response, event.msg.author.username, user_message);
    co_return;
}

dpp::job BeuroAI::savetxtfile_chat_history(std::string beuro_chat, std::string user, std::string user_message){
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