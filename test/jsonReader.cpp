// g++ jsonReader.cpp -o jsonReader -std=c++11 && ./jsonReader
// cd /SYSTEMS/IOT/Roverdyn/PROJ_TCP_DB/test/build && cmake .. && make && ./jsonReader

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
// #include <nlohmann/json.hpp>
#include "/SYSTEMS/IOT/Roverdyn/PROJ_TCP_DB/nlohmann/json.hpp"
#include <unistd.h>
#include <limits.h>
#include <cstdio>
#include <iostream>
#include <string>

// 네임스페이스 사용 (편의를 위해)
using json = nlohmann::json;
using std::cerr;
using std::cout;
using std::endl;
using std::exit;
using std::string;
using std::uint16_t;
using std::uint32_t;

int main()
{
    // std::cout << "Working Directory: " << std::filesystem::current_path() << std::endl;
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("[CHECK] cwd : %s\n", cwd);


    // TEST_PATH
    
    // const char* filename = "appConfig.json";
    std::string filename = "appConfig.json";

    // std::string(text);
    char buffer[PATH_MAX];
    // snprintf(buffer, sizeof(buffer), "%s/../%s", cwd, filename.c_str());
    // snprintf(buffer, sizeof(buffer), "%s/%s", cwd, filename.c_str());
    // std::string fileInfo = std::string(buffer);

    std::string fileInfo = std::string(PRJ_PATH) + "/appConfig.json";

    // --- 1. 설정값을 저장할 변수 선언 (기본값 포함) ---
    std::string DB_HOST;
    std::string DB_USER;
    std::string DB_PASSWD;
    std::string DB_TABLE;
    uint16_t DB_PORT;
    uint16_t SERVICE_PORT;
    uint32_t MAX_CONNECTION_LIMIT;
    uint32_t CONNECTION_TIMEOUT;

    printf(">> 설정파일 읽기 : %s\n", fileInfo.c_str());
    // printf(">> 설정파일 읽기 : %s\n", fileInfo);
    std::ifstream configFile(fileInfo);

    // --- 2. 파일 읽기 및 JSON 파싱 ---
    if (!configFile.is_open())
    {
        fprintf(stderr, "설정 파일읽기 실패 : %s을 열 수 없습니다. 프로그램 비정상 종료\n", filename.c_str());
        std::exit(EXIT_FAILURE);
    }

    printf(">> 설정파일 파싱\n");
    json configJson;
    try
    {
        configJson = json::parse(configFile);
        configFile.close();

        DB_HOST = configJson.value("DB_HOST", DB_HOST);
        DB_USER = configJson.value("DB_USER", DB_USER);
        DB_PASSWD = configJson.value("DB_PASSWD", DB_PASSWD);
        DB_TABLE = configJson.value("DB_TABLE", DB_TABLE);
        DB_PORT = configJson.value("DB_PORT", DB_PORT);
        SERVICE_PORT = configJson.value("SERVICE_PORT", SERVICE_PORT);
        MAX_CONNECTION_LIMIT = configJson.value("MAX_CONNECTION_LIMIT", MAX_CONNECTION_LIMIT);
        CONNECTION_TIMEOUT = configJson.value("CONNECTION_TIMEOUT", CONNECTION_TIMEOUT);
    }
    catch (const std::exception &e)
    {
        fprintf(stderr, "설정 파일읽기 오류 : %s을 열 수 없습니다. 프로그램 비정상 종료\n", filename.c_str());
        std::exit(EXIT_FAILURE);
    }

    // --- 4. 읽어온 설정값 출력 ---
    printf(">> 설정파일 출력\n");
    printf("[CHECK] DB_HOST : %s\n", DB_HOST.c_str());
    printf("[CHECK] DB_USER : %s\n", DB_USER.c_str());
    printf("[CHECK] DB_PASSWD : %s\n", DB_PASSWD.c_str());
    printf("[CHECK] DB_TABLE : %s\n", DB_TABLE.c_str());

    printf("[CHECK] DB_PORT : %u\n", DB_PORT);
    printf("[CHECK] SERVICE_PORT : %u\n", SERVICE_PORT);
    printf("[CHECK] MAX_CONNECTION_LIMIT : %u\n", MAX_CONNECTION_LIMIT);
    printf("[CHECK] CONNECTION_TIMEOUT : %u\n", CONNECTION_TIMEOUT);

    return 0;
}