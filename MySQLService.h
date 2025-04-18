#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "/usr/include/mysql/mysql.h"
#include <string>
#include <cstdint>
#include "spdlog/spdlog.h"

/*
	이벤트 정의
*/
#define MYSQL_EVENTS_CONNECTED (const char *)"CONNECTED"
#define MYSQL_EVENTS_DISCONNECTED (const char *)"DISCONNECTED"
#define MYSQL_EVENTS_ERROR (const char *)"ERROR"

struct MYSQL_CONFIG
{
	// const char *host;
	// const char *user;
	// const char *password;
	// const char *table;
	std::string host;
    std::string user;
    std::string password;
    std::string table;
	uint16_t port;
};

// 데이터 구조체 정의
struct CONN_LOG
{
	int32_t id;
	char device[45];
	char eventTime[45];
	char eventType[45];
	char address[45];
};

struct INPUT_DATA
{
	char product_serial_number[60];
	// char product_serial_number[12];
	char date_time[30];
	float temp;
	float hmdty;
	float pm25;
	float pm10;
	char mvmnt[20];
	float tvoc;
	float hcho;
	float co2;
	float co;
	float benzo;
	float radon;
	char mod_date[30];
	char reg_date[30];
	float tmp;
	float tmp2;
	float tmp3;
	float tmp4;
	float tmp5;
	char tmp6[20];
	char tmp7[20];
	char tmp8[20];
	char tmp9[20];
	char tmp10[20];
};

struct FOTA_INFO
{
	int32_t id;
	int32_t version_major;
	int32_t version_minor;
	int32_t version_patch;
	char firm_url[45];
	char contents[200];
	char reg_data[30];
};

struct IOT_FIRM
{
	int32_t id;
	char type[200];
	char ver[200];
	char host[200];
	char port[200];
	char bin[200];
	char reg_date[30];
};

struct IOT_PRDCT_AUTH
{
	char product_serial_number[63];
	bool auth_yn;
	bool use_yn;
	char mod_date[30];
	char reg_date[30];
};

struct KEPCO_API_AUTH
{
	char kepco_api_key[63];
	bool auth_yn;
	bool use_yn;
	char mod_date[30];
	char reg_date[30];
};

struct MEMBER
{
	char product_serial_number[63];
	int32_t customer_link_number;
	char kepco_api_key[63];
	char sns_id[100];
	char sns_pwd[100];
	char sns_key[100];
	char sns_type[100];
	char user_name[100];
	char phone_number[20];
	char zip_number[100];
	char addr[200];
	char addr_dtl[200];
	double lon;
	double lat;
	double house_area;
	int32_t male_cnt;
	int32_t female_cnt;
	int32_t room_cnt;
	int32_t tv_cnt;
	int32_t air_conditioner_cnt;
	int32_t washing_machine_cnt;
	int32_t dryer_cnt;
	int32_t rice_cooker_cnt;
	int32_t fan_cnt;
	int32_t disk_washer_cnt;
	int32_t computer_cnt;
	char rate_plan[100];
	bool use_yn;
	char mod_date[30];
	char reg_date[30];
};

struct MSG_INFO
{
	char msg_code[63];
	char msg_version[20];
	char msg_cont[2000];
	bool use_yn;
	char mod_date[30];
	char reg_date[30];
};

struct OUTPUT_DATA
{
	uint32_t customer_link_number;
	char date_time[30];
	float temp;
	float hmdty;
	float pm25;
	float pm10;
	float dust;
	float co2;
	float pwr;
	float gas;
	float water;
	float prd_pwr;
	float prd_gas;
	float prd_water;
	char vis_sync[5];
	char mod_date[30];
	char reg_date[30];
};

struct OUTPUT_STAT_DATA
{
	uint32_t customer_link_number;
	char date_time[30];
	float prv_pwr;
	float prv_gas;
	float prv_water;
	float prd_prv_pwr;
	float prd_prv_gas;
	float prd_prv_water;
	float pre_pwr;
	float pre_gas;
	float pre_water;
	float prd_pre_pwr;
	float prd_pre_gas;
	float prd_pre_water;
	char mod_date[30];
	char reg_date[30];
};

struct SYS_INFO
{
	uint32_t customer_link_number;
	int32_t pwr_svn_trg;
	int32_t gas_svn_trg;
	int32_t wtr_svn_trg;
	bool alarm_use_yn;
	bool alarm_sound_yn;
	bool alarm_vbrtn_yn;
	bool alarm_optm_yn;
	bool use_yn;
	char reg_date[30];
	char mod_date[30];
	float tmp;
	float tmp2;
	float tmp3;
	float tmp4;
	float tmp5;
	char tmp6[20];
	char tmp7[20];
	char tmp8[20];
	char tmp9[20];
	char tmp10[20];
};

struct TERMS_COND
{
	int32_t customer_link_number;
	char terms_code[63];
	char terms_version[20];
	char terms_cont[100];
	char agrtr_cndtn_yn[1];
	bool use_yn;
	char mod_date[30];
	char reg_date[30];
};

class MySQLService
{
private:
	MYSQL *conn, *result;
	MYSQL_CONFIG conf;
	const char *getDateTime();
	char DB_NAME[20] = "DMS02";

public:
	void Init(MYSQL_CONFIG config);
	/*
		추가(Creation)
	*/
	int32_t addEvent(const char *device_id, const char *evt_type, const char *addr);
	int32_t addInputData(uint16_t years, INPUT_DATA inputData);
	int32_t addIotFirm(IOT_FIRM iotFirm);
	int32_t addIotPrdctAuth(IOT_PRDCT_AUTH iotPrdctAuth);
	int32_t addKepcoApiAuth(KEPCO_API_AUTH kepcoApiAuth);
	int32_t addMember(MEMBER member);
	int32_t addMsgInfo(MSG_INFO msgInfo);
	int32_t addOutputData(uint16_t years, OUTPUT_DATA outputData);
	int32_t addOutputStatData(uint16_t years, OUTPUT_STAT_DATA outputStatData);
	int32_t addSysInfo(SYS_INFO sysInfo);
	int32_t addTermsCond(TERMS_COND termsCond);
	int32_t addFotaInfo();

	/*
		읽기(Read)
	*/
	void getEvent();
	INPUT_DATA getInputData(uint16_t years, const char *PRODUCT_SERIAL_NUMBER, const char *DATE_TIME);
	IOT_FIRM getIotFirm();
	IOT_PRDCT_AUTH getIotPrdctAuth(const char *serial_number);
	KEPCO_API_AUTH getKepcoApiAuth(const char *api_key);
	MEMBER getMember(uint32_t CUSTOMER_LINK_NUMBER, const char *PRODUCT_SERIAL_NUMBER);
	MSG_INFO getMsgInfo(const char *msg_code);
	OUTPUT_DATA getOutputData(uint16_t years, uint32_t CUSTOMER_LINK_NUMBER, const char *dateTime);
	OUTPUT_STAT_DATA getOutputStatData(uint16_t years, uint32_t CUSTOMER_LINK_NUMBER, const char *dateTime);
	SYS_INFO getSysInfo(uint32_t CUSTOMER_LINK_NUMBER);
	TERMS_COND getTermsCond(uint32_t CUSTOMER_LINK_NUMBER);
	void getFotaInfo();

	/*
		갱신(Update)
	*/
	void updateEvent();
	void updateInputData(uint16_t years);
	void updateIotFirm();
	void updateIotPrdctAuth();
	void updateKepcoApiAuth();
	void updateMember();
	void updateMsgInfo();
	void updateOutputData(uint16_t years);
	void updateOutputStatData(uint16_t years);
	void updateSysInfo();
	void updateTermsCond();
	void updateFotaInfo();

	/*
		삭제(Delete)
	*/
	void deleteEvent();
	void deleteInputData();
	void deleteIotFirm();
	void deleteIotPrdctAuth();
	void deleteKepcoApiAuth();
	void deleteMember();
	void deleteMsgInfo();
	void deleteOutputData();
	void deleteOutputStatData();
	void deleteSysInfo();
	void deleteTermsCond();
	void deleteFotaInfo();
};
