#include <SQLiteCpp/SQLiteCpp.h>
#include <dotenv.h>
#include <string>
#include <vector>

class SQL_Execs{
    private:
        std::vector<int> ID_targets = {1, 12};
        std::vector<std::string> chat_content = {};

    public:
        void CreateTable(SQLite::Database& beuroDB){
            beuroDB.exec("CREATE TABLE IF NOT EXISTS ChatHistory (ID INTEGER, CONTENT TEXT);");
        }

        void ReadFromFile(){
        std::string message;
            std::ifstream memory("Memory.txt");
            
            if(!memory.is_open()){
                std::cout << "Unable to open the file" << std::endl;
            }
            
            while(std::getline(memory, message)){
                if(message == ""){
                    continue;
                }
                
                this->chat_content.emplace_back(message);
                std::cout << message << std::endl;        
            }

            memory.close();
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
                auto ID = execution.getColumn(0);
                auto result = execution.getColumn(1);
                
                std::cout << ID << "|" << result << std::endl << std::endl;
            }
        }

        void QueryTargettedData(SQLite::Database& beuroDB){
            SQLite::Statement execution(beuroDB, "SELECT * FROM ChatHistory WHERE ID = ?");

            for (const auto& ID : this->ID_targets){
                execution.bind(1, ID);
                
                while(execution.executeStep()){
                    auto ID_result = execution.getColumn(0);
                    auto result = execution.getColumn(1);
                    
                    std::cout << ID_result << "|" << result << std::endl << std::endl;
                }

                execution.reset();
            }
        }

        void ChangeDataType(std::string& ID){
            auto search_result = ID.find("ID");

            std::string cleaned_copy = ID.erase(search_result, 2);

            int ID_number = std::stoi(cleaned_copy);
            std::cout << ID_number << std::endl;
        }
 
};