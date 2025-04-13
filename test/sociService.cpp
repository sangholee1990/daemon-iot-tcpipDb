// g++ sociService.cpp -o sociService -std=c++11 && ./sociService
// cd /SYSTEMS/IOT/Roverdyn/PROJ_TCP_DB/test/build && cmake .. && make && ./sociService

// cmake -S <소스_디렉토리_경로> -B <빌드_디렉토리_경로> [기타_옵션]

#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h> // MySQL 백엔드 헤더
#include <iostream>
#include <string>
#include <exception>
#include <vector> // 여러 행 처리 시 필요
#include <ctime> 
#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>


const std::string connectString = "db=DMS02 user=your_user password=your_password host=127.0.0.1 port=3306";
const size_t poolSize = 5; // 풀 크기

int main() {
    try {
        soci::connection_pool pool(poolSize);

        // 풀 초기화
        for (size_t i = 0; i != poolSize; ++i) {
            soci::session& sql_init = pool.at(i);
            sql_init.open(soci::mysql, connectString);
        }
        std::cout << "Connection pool initialized." << std::endl;

        // 실제 작업 시 풀에서 세션 빌려오기 (RAII 방식)
        // 함수나 스레드 내에서 아래와 같이 사용
        { // 스코프 생성 (스코프 벗어나면 자동 반납)
            soci::session sql(pool); // 풀에서 세션 가져오기
            std::cout << "Got session from pool." << std::endl;
            // ... 이 sql 객체로 DB 작업 수행 ...

        } // sql 객체 소멸 -> 세션이 풀에 반납됨
        std::cout << "Session returned to pool." << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}