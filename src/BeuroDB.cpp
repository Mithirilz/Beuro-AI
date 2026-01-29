#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>

int main(){
    const std::string FILEPATH = "C:/Users/ASUS/Documents/Visual Studio Code/Discord Bot/DBBeuro/Beuro_ChatHistory.db"; 
    SQLite::Database BeuroDB(FILEPATH, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    BeuroDB.exec("CREATE TABLE IF NOT EXISTS ChatHistory (MessageID INTEGER PRIMARY KEY, Message TEXT); ");

    SQLite::Statement insert(BeuroDB, "INSERT Into ChatHistory (Message) VALUES(?);");

    std::vector<std::string> msg = {"Hi there Beuro", "How you doing?", "Yep, been doing fine", "I got a question for ya"};

    for (const std::string& messages : msg){
        insert.bind(1, messages);
        insert.exec();
        insert.reset();
    }
        

    SQLite::Statement select(BeuroDB, "SELECT MessageID, Message FROM ChatHistory;");

    while(select.executeStep()){
        int ID = select.getColumn(0);
        std::string message = select.getColumn(1);

        std::cout << ID << " : " << message << std::endl;
    }
}

/*
1. Set up database *
2. Send some test data *
3. Try to query the data *
4. Prepare test data for co-query with ChromaDB
5. Try to query data via chromaDB
*/