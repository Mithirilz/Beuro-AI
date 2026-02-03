#include <SQLiteCpp/SQLiteCpp.h>
#include <dotenv.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class SQL_Execs{
    private:
        std::vector<int> IDs = {};
        std::vector<std::string> chat_content = {};

    public:
        void CreateTable(SQLite::Database& beuroDB){
            beuroDB.exec("CREATE TABLE IF NOT EXISTS ChatHistory (ID INTEGER, CONTENT TEXT);");
        }

        void push_into_IDs(int& ID){
            this->IDs.push_back(ID);
        }

        void push_into_chatcontent(std::string& chat){
            this->chat_content.push_back(chat);
        }

        void InsertDataintoTable(SQLite::Database& beuroDB){
            SQLite::Statement execution(beuroDB, "INSERT INTO ChatHistory (ID, CONTENT) VALUES (?, ?);");
            int counter = 1;

            for (const auto& chats : this->chat_content){
                execution.bind(1, counter);
                execution.bind(2, chats);

                execution.exec();

                std::cout << counter << "|" << chats << std::endl;
                execution.reset();

                counter = counter + 1;
            }

            std::cout << "Last ID entered (Or amount of messages in the database): " << counter << std::endl;
        }

        void QueryAllInformation(SQLite::Database& beuroDB){
            SQLite::Statement execution(beuroDB, "SELECT * FROM ChatHistory");
            while(execution.executeStep()){
                auto ID = execution.getColumn(1);
                auto result = execution.getColumn(2);
                
                std::cout << ID << "|" << result << std::endl;
            }
        }

        void QuerySpecificInformation(SQLite::Database& beuroDB){
            SQLite::Statement execution(beuroDB, "SELECT * FROM ChatHistory WHERE ID = ?");

            for (const auto& ID : this->IDs){
                execution.bind(1, ID);
                execution.exec();

                auto ID_result = execution.getColumn(1);
                auto result = execution.getColumn(2);
                
                std::cout << ID_result << "|" << result << std::endl;

                execution.reset();
            }
        }
};

int main(){
    dotenv::init();
    const std::string FILEPATH = dotenv::getenv("FILEPATH", "None");
    
    SQLite::Database BeuroDB(FILEPATH, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    SQL_Execs DB;

    int decision = 0;

    enum decisions{
        Create_Table = 1,
        Insert_Data = 2,
        Get_all_data = 3,
        Search_data = 4
    };

    std::unordered_map<int, std::function<void()>> actions;
    actions[Create_Table] = [&BeuroDB, &DB](){DB.CreateTable(BeuroDB);}; 
    actions[Insert_Data] = [&BeuroDB, &DB](){DB.InsertDataintoTable(BeuroDB);}; 
    actions[Get_all_data] = [&BeuroDB, &DB](){DB.QueryAllInformation(BeuroDB);}; 
    actions[Search_data] = [&BeuroDB, &DB](){DB.QuerySpecificInformation(BeuroDB);}; 

    std::cout << "---- USER INTERFACE ----" << std::endl;
    std::cout << "1. Create Table" << std::endl;
    std::cout << "2. Insert Data" << std::endl;
    std::cout << "3. Get all data in table" << std::endl;
    std::cout << "4. Search data in table" << std::endl;
    std::cout << "What would you like to do? (1-4)" << std::endl;

    std::cin >> decision;

    actions[decision]();
}