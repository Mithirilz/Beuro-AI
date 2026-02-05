#include <SQLiteCpp/SQLiteCpp.h>
#include <dotenv.h>
#include <functional>
#include <string>
#include <unordered_map>
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

int main(){
    dotenv::init();
    const std::string FILEPATH = dotenv::getenv("FILEPATH", "None");
    
    SQLite::Database BeuroDB(FILEPATH, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    SQL_Execs DB;
    
    int decision = 0;
    
    enum decisions{
        Create_Table = 1,
        Prepare_Data = 2,
        Insert_Data = 3,
        Get_all_data = 4,
        Search_data = 5,
        Exit_Program = 6,
        Check_Data_Type = 7
    };

    bool is_on = true; 
    
    std::unordered_map<int, std::function<void()>> actions;
    actions[Create_Table] = [&BeuroDB, &DB](){DB.CreateTable(BeuroDB);};
    actions[Prepare_Data] = [&DB](){DB.ReadFromFile();};
    actions[Insert_Data] = [&BeuroDB, &DB](){DB.InsertDataintoTable(BeuroDB);};
    actions[Get_all_data] = [&BeuroDB, &DB](){DB.QueryAllInformation(BeuroDB);};
    actions[Search_data] = [&BeuroDB, &DB](){DB.QueryTargettedData(BeuroDB);}; 
    actions[Exit_Program] = [&is_on](){is_on = false;};
    actions[Check_Data_Type] = [&DB](){
        std::string ID_string = "ID12";
        DB.ChangeDataType(ID_string);
    };
    

    while(is_on){
        std::cout << "---- USER INTERFACE ----" << std::endl;
        std::cout << "1. Create Table" << std::endl;
        std::cout << "2. Prepare Data" << std::endl;
        std::cout << "3. Insert Data" << std::endl;
        std::cout << "4. Get all data in table" << std::endl;
        std::cout << "5. Search data in table" << std::endl;
        std::cout << "6. Exit the program" << std::endl;
        std::cout << "7. Check data type" << std::endl;
        
        std::cout << "What would you like to do? (1-7): ";
        std::cin >> decision;
        
        actions[decision]();
        std::cout << std::endl << std::endl;
    }
}