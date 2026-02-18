#include "Beuro/BeuroRAG.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <unordered_map>
#include <dotenv.h>
#include <dpp/dpp.h>
#include <utility>
#include <string>

void Actual::SQL_Execs::CreateTable(){
    BeuroDB.exec("CREATE TABLE IF NOT EXISTS ChatHistory (ID INTEGER, CONTENT TEXT);");
}

void Actual::SQL_Execs::InsertDataintoTable(std::unordered_map<int, std::string> chat_set){
    SQLite::Statement execution(BeuroDB, "INSERT INTO ChatHistory (ID, CONTENT) VALUES (?, ?);");
    if(chat_set.empty()){
        std::cout << "SQL_Execs(InsertDataintoTable): The data given in the parameter is empty." << std::endl;
    }

    for (const auto&[ChatID, content] : chat_set){
        execution.bind(1, ChatID);
        execution.bind(2, content);
        execution.exec();

        std::cout << "ID" << ChatID << " | " << content << std::endl << std::endl;
        execution.reset();
    }

    //std::cout << "Last ID entered (Or amount of messages in the database): " << counter << std::endl;
}

void Actual::SQL_Execs::GetAllInformationFromAllColumns(){
    SQLite::Statement execution(BeuroDB, "SELECT * FROM ChatHistory");
    
    while(execution.executeStep()){
        auto ID = execution.getColumn(0);
        auto result = execution.getColumn(1);
        
        std::cout << ID << "|" << result << std::endl << std::endl;
    }
}

std::string Actual::SQL_Execs::GetInformationFromIDTargets(){
    SQLite::Statement execution(BeuroDB, "SELECT * FROM ChatHistory WHERE ID = ?");
    std::string chat = "Possibly-related past memories:\n";

    for (const auto& ID : this->ID_targets){
        execution.bind(1, ID);
        
        while(execution.executeStep()){
            auto ID_result = execution.getColumn(0);
            std::string result = execution.getColumn(1);
            
            std::cout << "ID" << ID_result << std::endl;
            chat = chat + result + "\n";
        }

        execution.reset();
    }

    return chat;
}

dpp::task<void> Actual::SQL_Execs::GetIDTargets(std::vector<std::string> IDs){
    auto ID_list = std::move(IDs);

    for (std::string& ID : ID_list){
        ID.erase(ID.find("ID"), 2);
        int ID_target = std::stoi(ID);
        this->ID_targets.push_back(ID_target);
    }

    co_return;
}
