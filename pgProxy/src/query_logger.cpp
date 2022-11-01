#include <string>
#include <string.h>
#include <iostream>
#include <filesystem>
#include <fstream>


const int QUERY_STRING_BYTE {5};
const int MESSAGE_TYPE_BYTE {0};
const std::string DEFAULT_LOG_PATH {"../log/"};
const std::string LOG_FILE_NAME {"db_queries.log"};
const unsigned char QUERY_MESSAGE_TYPE = 'Q';

class query_logger {

    public:
        query_logger(const std::string& log_path) {
            open_log_file(log_path);
        }
        query_logger() {
            open_log_file(DEFAULT_LOG_PATH);
        }

        ~query_logger() {
            ofs.close();
        }

        void log_query(const unsigned char* message, int size) {

            if (message[MESSAGE_TYPE_BYTE] == QUERY_MESSAGE_TYPE) {
                
                if (ofs.is_open()) {
                    for (auto i = QUERY_STRING_BYTE; i < size - 1; i++) {
                        std::cout << message[i];  
                        ofs.put(message[i]);
                    }
                    std::cout << std::endl;
                    ofs.put('\n');
                    ofs.flush();
                }
            }
        }

    private:    
        void open_log_file(const std::string& path) {
            if (!std::filesystem::exists(path)) {
                std::filesystem::create_directory(path);
            }
            ofs.open(path + LOG_FILE_NAME, std::ofstream::out | std::ofstream::app);
        }

        unsigned char message[8192];
        std::string query;
        std::ofstream ofs;
};