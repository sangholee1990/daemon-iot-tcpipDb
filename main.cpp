/*
 *	TCP Manager
 */
#include "Configure.h"
#include "MySQLService.h"
#include "TCPService.h"
#include "/SYSTEMS/IOT/Roverdyn/PROJ_TCP_DB/nlohmann/json.hpp"

struct MSG
{
	uint16_t msg_id = 0;
	uint8_t msg_length = 0;
	uint8_t payload[255] = {0};
};

// 네임스페이스
using json = nlohmann::json;
using std::cerr;
using std::cout;
using std::endl;
using std::exit;
using std::string;
using std::uint16_t;
using std::uint32_t;
using namespace std;

void *thread_main(void *socket_info);
void startDaemon();
const char *getDateTime();
tm *getDateTimeStruct();

MySQLService DB;

MSG parseMsg(uint8_t *data, int32_t data_length);


int main(int argc, char **argv)
{
	std::string DB_HOST;
    std::string DB_USER;
    std::string DB_PASSWD;
    std::string DB_TABLE;
    uint16_t DB_PORT;
    uint16_t SERVICE_PORT;
    uint32_t MAX_CONNECTION_LIMIT;
	uint32_t CONNECTION_TIMEOUT;
	// const char* filename = "./appConfig.json";
	std::string filename = "/SYSTEMS/IOT/Roverdyn/PROJ_TCP_DB/appConfig.json";
	// std::string filename = "./appConfig.json";

	std::cout << "***** SOLARMY TCP-DB Agent *****" << std::endl;
	// App Config 파일로부터 설정 파일 읽어옴
	printf(">> 설정파일 읽기 : %s\n", filename.c_str());
    std::ifstream configFile(filename);

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

	  printf(">> 설정파일 출력\n");
	  std::cout << "[CHECK] App Version : " << APP_VER_MAJOR << "." << APP_VER_MINOR << "." << APP_VER_PATCH << endl;
	  printf("[CHECK] DB_HOST : %s\n", DB_HOST.c_str());
	  printf("[CHECK] DB_USER : %s\n", DB_USER.c_str());
	  printf("[CHECK] DB_PASSWD : %s\n", DB_PASSWD.c_str());
	  printf("[CHECK] DB_TABLE : %s\n", DB_TABLE.c_str());

	  printf("[CHECK] DB_PORT : %u\n", DB_PORT);
	  printf("[CHECK] SERVICE_PORT : %u\n", SERVICE_PORT);
	  printf("[CHECK] MAX_CONNECTION_LIMIT : %u\n", MAX_CONNECTION_LIMIT);
	  printf("[CHECK] CONNECTION_TIMEOUT : %u\n", CONNECTION_TIMEOUT);

	  // 데몬 시작
	  // startDaemon();

	  /*
	   * 		MySQL 라이브러리 초기화
	   */
	  MYSQL_CONFIG config = {DB_HOST, DB_USER, DB_PASSWD, DB_TABLE, DB_PORT};
	  DB.Init(config);

	  /*
	   * 		TCP Service 초기화
	   */
	  TCPService SERVER;
	  SOCKET_INFO info = SERVER.Init(SERVICE_PORT);

	  while (true)
	  {
		  // 소켓을 생성한 후 클라이언트 연결을 대기한다.
		  SOCKET_INFO_CLIENT client_info = SERVER.Listen(info.server_fd, info.sock_len);

		  // 클라이언트가 Accept 되면 쓰레드 생성 후 분리한다.
		  pthread_t threads;
		  SOCKET_INFO_CLIENT *socket_info;
		  socket_info = (SOCKET_INFO_CLIENT *)malloc(sizeof(SOCKET_INFO_CLIENT));
		  socket_info->client_fd = client_info.client_fd;
		  socket_info->client_addr = client_info.client_addr;
		  pthread_create(&threads, NULL, thread_main, (void *)socket_info);
		  pthread_detach(threads); // Thread 분리
	  }

	return 0;
}

// 쓰레드 변수
void *thread_main(void *socket_info)
{
	// 로컬 쓰레드
	TCPService TCPClient;

	// 서버 소켓 읽기
	SOCKET_INFO_CLIENT *client_info = (SOCKET_INFO_CLIENT *)socket_info;

	// 소켓 타임아웃 설정
	struct timeval timeout;
	timeout.tv_sec = SOCKET_READ_TIMEOUT;
	timeout.tv_usec = 0;
	setsockopt(client_info->client_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

	// 연결 시 업데이트
	DB.addEvent("ID_TEST", MYSQL_EVENTS_CONNECTED, inet_ntoa(client_info->client_addr.sin_addr));
	cout << getDateTime() << " : Client Connected. IP Address is " << inet_ntoa(client_info->client_addr.sin_addr) << endl;

	// 데이터 구조체 정의
	INPUT_DATA inputData;
	FOTA_INFO fotaInfo;
	IOT_FIRM iotFirm;
	IOT_PRDCT_AUTH iotPrdctAuth;
	KEPCO_API_AUTH kepcoApiAuth;
	MEMBER member;
	MSG_INFO msgInfo;
	OUTPUT_DATA outputData;
	OUTPUT_STAT_DATA outputStatData;
	SYS_INFO sysInfo;
	TERMS_COND termsCond;

	// 메시지 구조체 정의
	MSG msg;
	memset(&msg, 0x00, sizeof(msg));

	// 메인 루프
	while (true)
	{
		uint8_t recv_buffer[RECV_BUFFER_SIZE] = {0}, write_buffer[RECV_BUFFER_SIZE] = {0};
		int32_t recv_length = TCPClient.receive(client_info->client_fd, recv_buffer);

		// 클라이언트의 연결 해제 시 루프 종료
		if (recv_length == 0)
		{
			cout << ">> Client에서 연결 해제 요청" << endl;
			break;
		}

		// Timeout 시 루프 종료
		if (recv_length == -1)
		{
			cout << getDateTime() << " : TCP Timeout!" << endl;
			break;
		}

		// 접속 단말 IP 주소 확인
		/*
		cout<<"RECV("<<inet_ntoa(client_info->client_addr.sin_addr)<<") : ";
		cout<<recv_buffer<<endl;
		*/

		// 명령 수신 후 파싱 검사
		if (recv_buffer[0] != 0xFF)
		{
			sprintf((char *)write_buffer, "SoF 실패\r\n");
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			printf("SoF 실패\n");
			// break;
			continue;
		}

		msg = parseMsg(recv_buffer, recv_length);
		printf("MSG ID(%s) : %02X length %d\r\n", inet_ntoa(client_info->client_addr.sin_addr), msg.msg_id, msg.msg_length);

		// Msg length
		msg.msg_length = recv_buffer[3];

		// Payload Length 검사
		// if (msg.msg_length < 1 || msg.msg_length == '0')
		// {
		// 	// sprintf((char *)write_buffer, "msg_length 실패\r\n");
		// 	// TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
		// 	// printf("msg_length 실패\n");
		// 	// continue;
		// }

		// Payload Length가 0이상일 때 payload 배열에 복사
		memcpy(msg.payload, &recv_buffer[4], msg.msg_length);

		printf("[CHECK] Header Payload : ");
		for (int i = 0; i < 4; i++)
		{
			printf("%02X", recv_buffer[i]);
		}
		printf(" ");
		for (int i = 0; i < msg.msg_length; i++)
		{
			printf("%02X", msg.payload[i]);
		}
		printf("\n");

		// 서버 날짜/시간을 받아옴
		tm *dateTime = getDateTimeStruct();

		uint16_t years = 0;
		uint8_t temp[4] = {0};
		int32_t results = 0;
		size_t offset = 0;

		// printf("[START] msg.msg_id : %s\n", msg.msg_id);
		// printf("[START] msg.msg_id : %02X\n", msg.msg_id);

		switch (msg.msg_id)
		{
		case 0x00:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;

		// ******************************************
		// DB 조회
		// ******************************************
		case CTRL_REQUEST_FIRMVER:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_REQUEST_FIRMURL:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case GET_SYSTEM_TIME:
			// 서버 날짜-시간을 리턴하는 함수
			// 메시지 헤더 정의
			write_buffer[0] = 0xFF;
			write_buffer[1] = 0x00;
			write_buffer[2] = 0x03;
			write_buffer[3] = 0x07;

			// 페이로드 정의
			uint32_t years;
			years = dateTime->tm_year + 1900;
			write_buffer[4] = (years >> 8) & 0xFF;	// Year_H
			write_buffer[5] = years & 0xFF;			// Year_L
			write_buffer[6] = dateTime->tm_mon + 1; // Month
			write_buffer[7] = dateTime->tm_mday;	// Day
			write_buffer[8] = dateTime->tm_hour;	// Hours
			write_buffer[9] = dateTime->tm_min;		// Minutes
			write_buffer[10] = dateTime->tm_sec;	// Seconds

			// 메시지 리턴
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, write_buffer[3] + 4);
			printf("[CHECK] dateTime : %d-%02d-%02d %02d:%02d:%02d\n", dateTime->tm_year + 1900, dateTime->tm_mon + 1, dateTime->tm_mday, dateTime->tm_hour, dateTime->tm_min, dateTime->tm_sec);

			break;
		case CTRL_REQUEST_INPUT_DATA:
			memset(&inputData, 0x00, sizeof(inputData));
			if (msg.msg_length < 5)
			{
				break;
			}
			years = (msg.payload[0] << 8) | msg.payload[1];
			memcpy(inputData.date_time, &msg.payload[2], 19);
			memcpy(inputData.product_serial_number, &msg.payload[21], msg.msg_length - 21);
			inputData = DB.getInputData(years, inputData.product_serial_number, inputData.date_time);
			sprintf((char *)write_buffer, "%s,%s,%f,%f,%f,%f,%s,%f,%f,%f,%f,%f,%f,%s,%s,%f,%f,%f,%f,%f,%s,%s,%s,%s,%s",
					inputData.product_serial_number, inputData.date_time, inputData.temp, inputData.hmdty, inputData.pm25, inputData.pm10, inputData.mvmnt,
					inputData.tvoc, inputData.hcho, inputData.co2, inputData.co, inputData.benzo, inputData.radon, inputData.mod_date, inputData.reg_date,
					inputData.tmp, inputData.tmp2, inputData.tmp3, inputData.tmp4, inputData.tmp5, inputData.tmp6, inputData.tmp7, inputData.tmp8,
					inputData.tmp9, inputData.tmp10);
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_REQUEST_IOT_FIRM:

			break;
		case CTRL_REQUEST_IOT_PRDCT_AUTH:
			memset(&iotPrdctAuth, 0x00, sizeof(iotPrdctAuth));
			if (msg.msg_length < 5)
			{
				break;
			}
			iotPrdctAuth = DB.getIotPrdctAuth((const char *)msg.payload);
			sprintf((char *)write_buffer, "%s,%d,%d,%s,%s", iotPrdctAuth.product_serial_number,
					iotPrdctAuth.auth_yn, iotPrdctAuth.use_yn, iotPrdctAuth.mod_date, iotPrdctAuth.reg_date);
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_REQUEST_KEPCO_API_AUTH:
			memset(&kepcoApiAuth, 0x00, sizeof(kepcoApiAuth));
			if (msg.msg_length < 5)
			{
				break;
			}
			kepcoApiAuth = DB.getKepcoApiAuth((const char *)msg.payload);
			sprintf((char *)write_buffer, "%s,%d,%d,%s,%s", kepcoApiAuth.kepco_api_key,
					kepcoApiAuth.auth_yn, kepcoApiAuth.use_yn, kepcoApiAuth.mod_date, kepcoApiAuth.reg_date);
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_REQUEST_MEMBER:
			memset(&member, 0x00, sizeof(member));
			if (msg.msg_length < 4)
			{
				break;
			}
			member.customer_link_number = (msg.payload[0] << 24) | (msg.payload[1] << 16) | (msg.payload[2] << 8) | msg.payload[3];
			memcpy(member.product_serial_number, &msg.payload[4], msg.msg_length - 4);
			member = DB.getMember(member.customer_link_number, member.product_serial_number);
			sprintf((char *)write_buffer, "%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s,%d,%s,%s",
					member.product_serial_number, member.customer_link_number, member.kepco_api_key, member.sns_id, member.sns_pwd, member.sns_key, member.sns_type, member.user_name,
					member.phone_number, member.zip_number, member.addr, member.addr_dtl, member.lon, member.lat, member.house_area, member.male_cnt, member.female_cnt, member.room_cnt,
					member.tv_cnt, member.air_conditioner_cnt, member.washing_machine_cnt, member.dryer_cnt, member.rice_cooker_cnt, member.fan_cnt, member.disk_washer_cnt, member.computer_cnt,
					member.rate_plan, member.use_yn, member.mod_date, member.reg_date);
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_REQUEST_MSG_INFO:
			memset(&msgInfo, 0x00, sizeof(msgInfo));
			if (msg.msg_length < 5)
			{
				break;
			}
			msgInfo = DB.getMsgInfo((char *)msg.payload);
			sprintf((char *)write_buffer, "%s,%s,%s,%d,%s,%s", msgInfo.msg_code, msgInfo.msg_version, msgInfo.msg_cont, msgInfo.use_yn, msgInfo.mod_date, msgInfo.reg_date);
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_REQUEST_OUTPUT_DATA:
			memset(&outputData, 0x00, sizeof(outputData));
			if (msg.msg_length < 25)
			{
				break;
			}
			years = (msg.payload[0] << 8) | msg.payload[1];
			if (years < 2022)
			{
				break;
			}
			outputData.customer_link_number = (msg.payload[2] << 24) | (msg.payload[3] << 16) | (msg.payload[4] << 8) | msg.payload[5];
			memcpy(outputData.date_time, &msg.payload[6], 19);
			outputData = DB.getOutputData(years, outputData.customer_link_number, outputData.date_time);
			sprintf((char *)write_buffer, "%d,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%s,%s",
					outputData.customer_link_number, outputData.date_time,
					outputData.temp, outputData.hmdty, outputData.pm25, outputData.pm10,
					outputData.dust, outputData.co2, outputData.pwr, outputData.gas, outputData.water,
					outputData.prd_pwr, outputData.prd_gas, outputData.prd_water,
					outputData.mod_date, outputData.reg_date);
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_REQUEST_OUTPUT_STAT_DATA:
			memset(&outputStatData, 0x00, sizeof(outputStatData));
			if (msg.msg_length < 25)
			{
				break;
			}
			years = (msg.payload[0] << 8) | msg.payload[1];
			if (years < 2022)
			{
				break;
			}
			outputStatData.customer_link_number = (msg.payload[2] << 24) | (msg.payload[3] << 16) | (msg.payload[4] << 8) | msg.payload[5];
			memcpy(outputStatData.date_time, &msg.payload[6], 19);
			outputStatData = DB.getOutputStatData(years, outputStatData.customer_link_number, outputStatData.date_time);
			sprintf((char *)write_buffer, "%d,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%s,%s",
					outputStatData.customer_link_number, outputStatData.date_time,
					outputStatData.prv_pwr, outputStatData.prv_gas, outputStatData.prv_water,
					outputStatData.prd_prv_pwr, outputStatData.prd_prv_gas, outputStatData.prd_prv_water,
					outputStatData.pre_pwr, outputStatData.pre_gas, outputStatData.pre_water,
					outputStatData.prd_pre_pwr, outputStatData.prd_pre_gas, outputStatData.prd_pre_water,
					outputStatData.mod_date, outputStatData.reg_date);
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_REQUEST_SYS_INFO:
			memset(&sysInfo, 0x00, sizeof(sysInfo));
			if (msg.msg_length < 4)
			{
				break;
			}
			sysInfo.customer_link_number = (msg.payload[0] << 24) | (msg.payload[1] << 16) | (msg.payload[2] << 8) | msg.payload[3];
			sysInfo = DB.getSysInfo(sysInfo.customer_link_number);
			sprintf((char *)write_buffer, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%s,%s,%f,%f,%f,%f,%f,%s,%s,%s,%s,%s", sysInfo.customer_link_number,
					sysInfo.pwr_svn_trg, sysInfo.gas_svn_trg, sysInfo.wtr_svn_trg, sysInfo.alarm_use_yn, sysInfo.alarm_sound_yn, sysInfo.alarm_vbrtn_yn, sysInfo.alarm_optm_yn, sysInfo.use_yn,
					sysInfo.reg_date, sysInfo.mod_date, sysInfo.tmp, sysInfo.tmp2, sysInfo.tmp3, sysInfo.tmp4, sysInfo.tmp5, sysInfo.tmp6, sysInfo.tmp7, sysInfo.tmp8, sysInfo.tmp9, sysInfo.tmp10);
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_REQUEST_TERMS_COND:
			memset(&termsCond, 0x00, sizeof(termsCond));
			if (msg.msg_length < 4)
			{
				break;
			}
			termsCond.customer_link_number = (msg.payload[0] << 24) | (msg.payload[1] << 16) | (msg.payload[2] << 8) | msg.payload[3];
			termsCond = DB.getTermsCond(termsCond.customer_link_number);
			sprintf((char *)write_buffer, "%d,%s,%s,%s,%s,%d,%s,%s", termsCond.customer_link_number, termsCond.terms_code, termsCond.terms_version, termsCond.terms_cont, termsCond.agrtr_cndtn_yn, termsCond.use_yn, termsCond.mod_date, termsCond.reg_date);
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;

		// ******************************************
		// DB 삽입
		// ******************************************
		case CTRL_CREATE_INPUT:

			memset(&inputData, 0x00, sizeof(inputData));
			strcpy(inputData.reg_date, getDateTime());
			strcpy(inputData.mod_date, getDateTime());

			// 메모리 (41 B8 00 00) -> 리틀 엔디안 정수값 (47169.0f)
			// uint32_t tmpInt;
			// tmpInt = (msg.payload[73] << 24) | (msg.payload[72] << 16) | (msg.payload[71] << 8) | (msg.payload[70] & 0xFF);

			// 메모리 (41 B8 00 00) -> 빅 엔디안 정수값 (22.0f)
			// uint32_t tmpInt;
			// tmpInt = (msg.payload[70] << 24) | (msg.payload[71] << 16) | (msg.payload[72] << 8) | (msg.payload[73] & 0xFF);
			// memcpy(&inputData.temp, &tmpInt, 4);
			// printf("[CHECK] inputData.temp : %.1f\n", inputData.temp);

			years = (msg.payload[0] << 8) | msg.payload[1];
			// printf("[CHECK] years : %u\n", years);

			memcpy(inputData.product_serial_number, &msg.payload[2], 49);
			// printf("[CHECK] inputData.product_serial_number : %s\n", inputData.product_serial_number);

			memcpy(inputData.date_time, &msg.payload[51], 19);
			// printf("[CHECK] inputData.date_time : %s\n", inputData.date_time);

			// 동적 처리
			uint32_t tmpInt;
			offset = 70;
			tmpInt = (msg.payload[offset] << 24) | (msg.payload[offset + 1] << 16) | (msg.payload[offset + 2] << 8) | (msg.payload[offset + 3] & 0xFF);
			memcpy(&inputData.temp, &tmpInt, 4);
			// printf("[CHECK] inputData.temp : %.1f\n", inputData.temp);

			offset += 4;
			tmpInt = (msg.payload[offset] << 24) | (msg.payload[offset + 1] << 16) | (msg.payload[offset + 2] << 8) | (msg.payload[offset + 3] & 0xFF);
			memcpy(&inputData.hmdty, &tmpInt, 4);
			// printf("[CHECK] inputData.hmdty : %.1f\n", inputData.hmdty);

			// inputData.pm25 = 0;
			offset += 4;
			tmpInt = (msg.payload[offset] << 24) | (msg.payload[offset + 1] << 16) | (msg.payload[offset + 2] << 8) | (msg.payload[offset + 3] & 0xFF);
			memcpy(&inputData.pm25, &tmpInt, 4);
			// printf("[CHECK] inputData.pm25 : %.1f\n", inputData.pm25);

			// inputData.pm10 = 0;
			offset += 4;
			tmpInt = (msg.payload[offset] << 24) | (msg.payload[offset + 1] << 16) | (msg.payload[offset + 2] << 8) | (msg.payload[offset + 3] & 0xFF);
			memcpy(&inputData.pm10, &tmpInt, 4);
			// printf("[CHECK] inputData.pm10 : %.1f\n", inputData.pm10);

			// strcpy(inputData.mvmnt, "TEST");
			offset += 4;
			memcpy(inputData.mvmnt, &msg.payload[offset], 20);
			// printf("[CHECK] inputData.mvmnt : %s\n", inputData.mvmnt);

			// inputData.tvoc = 0;
			offset += 20;
			tmpInt = (msg.payload[offset] << 24) | (msg.payload[offset + 1] << 16) | (msg.payload[offset + 2] << 8) | (msg.payload[offset + 3] & 0xFF);
			memcpy(&inputData.tvoc, &tmpInt, 4);
			// printf("[CHECK] inputData.tvoc : %.1f\n", inputData.tvoc);

			// inputData.hcho = 0;
			offset += 4;
			tmpInt = (msg.payload[offset] << 24) | (msg.payload[offset + 1] << 16) | (msg.payload[offset + 2] << 8) | (msg.payload[offset + 3] & 0xFF);
			memcpy(&inputData.hcho, &tmpInt, 4);
			// printf("[CHECK] inputData.hcho : %.1f\n", inputData.hcho);

			// inputData.co2 = 0;
			offset += 4;
			tmpInt = (msg.payload[offset] << 24) | (msg.payload[offset + 1] << 16) | (msg.payload[offset + 2] << 8) | (msg.payload[offset + 3] & 0xFF);
			memcpy(&inputData.co2, &tmpInt, 4);
			// printf("[CHECK] inputData.co2 : %.1f\n", inputData.co2);

			// inputData.co = 0;
			offset += 4;
			tmpInt = (msg.payload[offset] << 24) | (msg.payload[offset + 1] << 16) | (msg.payload[offset + 2] << 8) | (msg.payload[offset + 3] & 0xFF);
			memcpy(&inputData.co, &tmpInt, 4);
			// printf("[CHECK] inputData.co : %.1f\n", inputData.co);

			// inputData.benzo = 0;
			offset += 4;
			tmpInt = (msg.payload[offset] << 24) | (msg.payload[offset + 1] << 16) | (msg.payload[offset + 2] << 8) | (msg.payload[offset + 3] & 0xFF);
			memcpy(&inputData.benzo, &tmpInt, 4);
			// printf("[CHECK] inputData.benzo : %.1f\n", inputData.benzo);

			// inputData.radon = 0;
			offset += 4;
			tmpInt = (msg.payload[offset] << 24) | (msg.payload[offset + 1] << 16) | (msg.payload[offset + 2] << 8) | (msg.payload[offset + 3] & 0xFF);
			memcpy(&inputData.radon, &tmpInt, 4);
			// printf("[CHECK] inputData.radon : %.1f\n", inputData.radon);

			inputData.tmp = 0;
			inputData.tmp2 = 0;
			inputData.tmp3 = 0;
			inputData.tmp4 = 0;
			inputData.tmp5 = 0;
			strcpy(inputData.tmp6, "");
			strcpy(inputData.tmp7, "");
			strcpy(inputData.tmp8, "");
			strcpy(inputData.tmp9, "");
			strcpy(inputData.tmp10, "");

			printf("[CHECK] product_serial_number=%s, date_time=%s, temp=%.1f, hmdty=%.1f, pm25=%.1f, pm10=%.1f, mvmnt=%s, tvoc=%.1f, hcho=%.1f, co2=%.1f, co=%.1f, benzo=%.1f, radon=%.1f, mod_date=%s, reg_date=%s, tmp=%.1f, tmp2=%.1f, tmp3=%.1f, tmp4=%.1f, tmp5=%.1f, tmp6=%s, tmp7=%s, tmp8=%s, tmp9=%s, tmp10=%s\n", inputData.product_serial_number, inputData.date_time, inputData.temp, inputData.hmdty, inputData.pm25, inputData.pm10, inputData.mvmnt, inputData.tvoc, inputData.hcho, inputData.co2, inputData.co, inputData.benzo, inputData.radon, inputData.mod_date, inputData.reg_date, inputData.tmp, inputData.tmp2, inputData.tmp3, inputData.tmp4, inputData.tmp5, inputData.tmp6, inputData.tmp7, inputData.tmp8, inputData.tmp9, inputData.tmp10);

			// results = DB.addInputData(years, outputData);
			results = DB.addInputData(years, inputData);

			char tempOutputInput[10];
			if (results == 0)
			{
				strcpy(tempOutputInput, "200");
			}
			else if (results == 1)
			{
				// 업데이트 실패
				strcpy(tempOutputInput, "400");
			}
			printf("[CHECK] DB results : %d\n", results);

			TCPClient.write(client_info->client_fd, (uint8_t *)tempOutputInput, strlen((char *)tempOutputInput));
			break;
		case CTRL_CREATE_MEMBER:
			memset(&member, 0x00, sizeof(member));
			DB.addMember(member);
			break;
		case CTRL_CREATE_IOT_PRDCT_AUTH:
			memset(&iotPrdctAuth, 0x00, sizeof(iotPrdctAuth));
			DB.addIotPrdctAuth(iotPrdctAuth);
			break;
		case CTRL_CREATE_KEPCO_API_AUTH:
			memset(&kepcoApiAuth, 0x00, sizeof(kepcoApiAuth));
			DB.addKepcoApiAuth(kepcoApiAuth);
			break;
		case CTRL_CREATE_MSG_INFO:
			memset(&msgInfo, 0x00, sizeof(msgInfo));
			break;
		case CTRL_CREATE_OUTPUT_DATA:
			memset(&outputData, 0x00, sizeof(outputData));
			years = msg.payload[0] << 8 | msg.payload[1];
			if (years < 2022)
			{
				break;
			}
			outputData.customer_link_number = (msg.payload[2] << 24) | (msg.payload[3] << 16) | (msg.payload[4] << 8) | msg.payload[5];
			memcpy(outputData.date_time, &msg.payload[6], 19);
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[28 - i];
			}
			memcpy(&outputData.temp, temp, 4);
			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[32 - i];
			}
			memcpy(&outputData.hmdty, temp, 4);
			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[36 - i];
			}
			memcpy(&outputData.pm25, temp, 4);
			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[40 - i];
			}
			memcpy(&outputData.pm10, temp, 4);
			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[44 - i];
			}
			memcpy(&outputData.dust, temp, 4);
			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[48 - i];
			}
			memcpy(&outputData.co2, temp, 4);
			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[52 - i];
			}
			memcpy(&outputData.pwr, temp, 4);
			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[56 - i];
			}
			memcpy(&outputData.gas, temp, 4);
			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[60 - i];
			}
			memcpy(&outputData.water, temp, 4);
			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[64 - i];
			}
			memcpy(&outputData.prd_pwr, temp, 4);
			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[68 - i];
			}
			memcpy(&outputData.prd_gas, temp, 4);
			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[72 - i];
			}
			memcpy(&outputData.prd_water, temp, 4);
			memset(temp, 0x00, sizeof(temp));
			strcpy(outputData.vis_sync, "blue");
			strcpy(outputData.mod_date, getDateTime());
			strcpy(outputData.reg_date, getDateTime());
			results = DB.addOutputData(years, outputData);
			char tempOutput[10];
			if (results == 0)
			{
				strcpy(tempOutput, "200");
			}
			else if (results == 1)
			{
				// 업데이트 실패
				strcpy(tempOutput, "400");
			}
			TCPClient.write(client_info->client_fd, (uint8_t *)tempOutput, strlen((char *)tempOutput));
			break;
		case CTRL_CREATE_OUTPUT_STAT_DATA:
			memset(&outputStatData, 0x00, sizeof(outputStatData));
			years = msg.payload[0] << 8 | msg.payload[1];
			if (years < 2022)
			{
				break;
			}
			outputStatData.customer_link_number = (msg.payload[2] << 24) | (msg.payload[3] << 16) | (msg.payload[4] << 8) | msg.payload[5];
			memcpy(outputStatData.date_time, &msg.payload[6], 19);

			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[28 - i];
			}
			memcpy(&outputStatData.prv_pwr, temp, 4);

			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[32 - i];
			}
			memcpy(&outputStatData.prv_gas, temp, 4);

			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[36 - i];
			}
			memcpy(&outputStatData.prv_water, temp, 4);

			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[40 - i];
			}
			memcpy(&outputStatData.prd_prv_pwr, temp, 4);

			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[44 - i];
			}
			memcpy(&outputStatData.prd_prv_gas, temp, 4);

			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[48 - i];
			}
			memcpy(&outputStatData.prd_prv_water, temp, 4);

			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[52 - i];
			}
			memcpy(&outputStatData.pre_pwr, temp, 4);

			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[56 - i];
			}
			memcpy(&outputStatData.pre_gas, temp, 4);

			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[60 - i];
			}
			memcpy(&outputStatData.pre_water, temp, 4);

			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[64 - i];
			}
			memcpy(&outputStatData.prd_pre_pwr, temp, 4);

			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[68 - i];
			}
			memcpy(&outputStatData.prd_pre_gas, temp, 4);
			memset(temp, 0x00, sizeof(temp));
			for (uint8_t i = 0; i < 4; i++)
			{
				temp[i] = msg.payload[72 - i];
			}
			memcpy(&outputStatData.prd_pre_water, temp, 4);
			strcpy(outputStatData.mod_date, getDateTime());
			strcpy(outputStatData.reg_date, getDateTime());
			results = DB.addOutputStatData(years, outputStatData);
			char tempOutputStat[10];
			if (results == 0)
			{
				strcpy(tempOutputStat, "200");
			}
			else if (results == 1)
			{
				// 업데이트 실패
				strcpy(tempOutputStat, "400");
			}
			TCPClient.write(client_info->client_fd, (uint8_t *)tempOutputStat, strlen((char *)tempOutputStat));
			break;
		case CTRL_CREATE_SYS_INFO:
			memset(&sysInfo, 0x00, sizeof(sysInfo));
			break;
		case CTRL_CREATE_TERMS_COND:
			memset(&termsCond, 0x00, sizeof(termsCond));
			break;
		case CTRL_UPDATE_MEMBER:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_UPDATE_IOT_PRDCT_AUTH:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_UPDATE_KEPCO_API_AUTH:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_UPDATE_MSG_INFO:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_UPDATE_OUTPUT_DATE:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_UPDATE_OUTPUT_STAT_DATA:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_UPDATE_SYS_INFO:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_DELETE_MEMBER:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_DELETE_IOT_PRDCT_AUTH:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_DELETE_KEPCO_API_AUTH:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_DELETE_MSG_INFO:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_DELETE_OUTPUT_DATA:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_DELETE_OUTPUT_STAT_DATA:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		case CTRL_DELETE_SYS_INFO:
			sprintf((char *)write_buffer, "%s", inet_ntoa(client_info->client_addr.sin_addr));
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		default:
			// WRONG MSG ID
			sprintf((char *)write_buffer, "Wrong command(%04X) received.", msg.msg_id);
			TCPClient.write(client_info->client_fd, (uint8_t *)write_buffer, strlen((char *)write_buffer));
			break;
		}

		// printf("[END] msg.msg_id : %s\n", msg.msg_id);
		// printf("[END] msg.msg_id : %u\n", msg.msg_id);
	}

	// 연결 해제 시 업데이트
	DB.addEvent("ID_TEST", MYSQL_EVENTS_DISCONNECTED, inet_ntoa(client_info->client_addr.sin_addr));
	cout << getDateTime() << " : Disconnected. IP Address is " << inet_ntoa(client_info->client_addr.sin_addr) << endl;

	// Client FD 닫음
	if (client_info->client_fd > 0)
	{
		close(client_info->client_fd);
	}

	return NULL;
}

void startDaemon()
{

	pid_t pid;
	if ((pid = fork()) < 0)
	{
		exit(0);
	}
	else if (pid == 0)
	{
		// signal(SIGHUP, SIG_IGN);
		setsid();
	}
	else
	{
		exit(0);
	}
}

const char *getDateTime()
{
	// 현재 시간 가져오는 함수
	static char dateTime[100] = {0};
	time_t timeNow;
	struct tm *t;
	time(&timeNow);
	t = (struct tm *)localtime(&timeNow);
	sprintf(dateTime, "%04d-%02d-%02d %02d:%02d:%02d",
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
			t->tm_hour, t->tm_min, t->tm_sec);
	return dateTime;
}

tm *getDateTimeStruct()
{
	struct tm *t;
	time_t timeNow;
	time(&timeNow);
	t = (struct tm *)localtime(&timeNow);
	return t;
}

MSG parseMsg(uint8_t *data, int32_t data_length)
{
	static MSG msg;
	msg.msg_id = ((uint16_t)data[1] << 8) | data[2];
	msg.msg_length = data[3];
	if (data_length > 0)
	{
		// memcpy(msg.payload, &data[4], data_length - 4);
	}

	return msg;
}
