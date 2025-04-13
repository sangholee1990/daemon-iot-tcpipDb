#include "MySQLService.h"

void MySQLService::Init(MYSQL_CONFIG config)
{
	// MySQL 초기화
	conn = mysql_init((MYSQL *)NULL);
	result = mysql_real_connect(conn, config.host.c_str(), config.user.c_str(), config.password.c_str(), NULL, config.port, NULL, 0);
	if (result == nullptr)
	{
		spdlog::error("DB 접속 실패 : 비정상 종료");
		std::exit(EXIT_FAILURE);
	}

	spdlog::info("[CHECK] DB 접속 성공 : {} ({})", config.host, config.port);

	// config 복사
	memcpy(&conf, &config, sizeof(config));
}

const char *MySQLService::getDateTime()
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

INPUT_DATA MySQLService::getInputData(uint16_t years, const char *PRODUCT_SERIAL_NUMBER, const char *DATE_TIME)
{
	char query[200] = {0};
	MYSQL_RES *result;
	MYSQL_ROW rows;
	sprintf(query, "SELECT * FROM %s.TB_INPUT_DATA_%d WHERE PRODUCT_SERIAL_NUMBER = '%s' AND DATE_TIME='%s'", DB_NAME, years, PRODUCT_SERIAL_NUMBER, DATE_TIME);
	printf("%s\n", query);
	mysql_query(conn, query);
	result = mysql_store_result(conn);
	uint8_t fields_count = mysql_num_fields(result);
	uint16_t counter = 0;
	INPUT_DATA output;
	memset(&output, 0x00, sizeof(output));
	while (rows = mysql_fetch_row(result))
	{
		if (rows[0] != nullptr)
		{
			strcpy(output.product_serial_number, rows[0]);
		}
		if (rows[1] != nullptr)
		{
			strcpy(output.date_time, rows[1]);
		}
		if (rows[2] != nullptr)
		{
			output.temp = atof(rows[2]);
		}
		if (rows[3] != nullptr)
		{
			output.hmdty = atof(rows[3]);
		}
		if (rows[4] != nullptr)
		{
			output.pm25 = atof(rows[4]);
		}
		if (rows[5] != nullptr)
		{
			output.pm10 = atof(rows[5]);
		}
		if (rows[6] != nullptr)
		{
			strcpy(output.mvmnt, rows[6]);
		}
		if (rows[7] != nullptr)
		{
			output.tvoc = atof(rows[7]);
		}
		if (rows[8] != nullptr)
		{
			output.hcho = atof(rows[8]);
		}
		if (rows[9] != nullptr)
		{
			output.co2 = atof(rows[9]);
		}
		if (rows[10] != nullptr)
		{
			output.co = atof(rows[10]);
		}
		if (rows[11] != nullptr)
		{
			output.benzo = atof(rows[11]);
		}
		if (rows[12] != nullptr)
		{
			output.radon = atof(rows[12]);
		}
		if (rows[13] != nullptr)
		{
			strcpy(output.mod_date, rows[13]);
		}
		if (rows[14] != nullptr)
		{
			strcpy(output.reg_date, rows[14]);
		}
		if (rows[15] != nullptr)
		{
			output.tmp = atof(rows[15]);
		}
		if (rows[16] != nullptr)
		{
			output.tmp2 = atof(rows[16]);
		}
		if (rows[17] != nullptr)
		{
			output.tmp3 = atof(rows[17]);
		}
		if (rows[18] != nullptr)
		{
			output.tmp4 = atof(rows[18]);
		}
		if (rows[19] != nullptr)
		{
			output.tmp5 = atof(rows[19]);
		}
		if (rows[20] != nullptr)
		{
			strcpy(output.tmp6, rows[20]);
		}
		if (rows[21] != nullptr)
		{
			strcpy(output.tmp7, rows[21]);
		}
		if (rows[22] != nullptr)
		{
			strcpy(output.tmp8, rows[22]);
		}
		if (rows[23] != nullptr)
		{
			strcpy(output.tmp9, rows[23]);
		}
		if (rows[24] != nullptr)
		{
			strcpy(output.tmp10, rows[24]);
		}
	}
	mysql_free_result(result);

	return output;
}

IOT_FIRM MySQLService::getIotFirm()
{
	IOT_FIRM iot_firm;
	return iot_firm;
}

IOT_PRDCT_AUTH MySQLService::getIotPrdctAuth(const char *serial_number)
{
	char query[200] = {0};
	MYSQL_RES *res;
	MYSQL_ROW rows;
	sprintf(query, "SELECT * FROM %s.TB_IOT_PRDCT_AUTH WHERE PRODUCT_SERIAL_NUMBER = '%s'", DB_NAME, serial_number);
	int32_t status = mysql_query(conn, query);
	res = mysql_store_result(conn);
	uint8_t fieldsCounts = mysql_num_fields(res);
	IOT_PRDCT_AUTH iot_prdct_auth;
	while (rows = mysql_fetch_row(res))
	{
		if (rows[0] != nullptr)
		{
			strcpy(iot_prdct_auth.product_serial_number, rows[0]);
		}
		if (rows[1] != nullptr)
		{
			iot_prdct_auth.auth_yn = strcmp(rows[1], "Y") ? 0 : 1;
		}
		if (rows[2] != nullptr)
		{
			iot_prdct_auth.use_yn = strcmp(rows[2], "Y") ? 0 : 1;
		}
		if (rows[3] != nullptr)
		{
			strcpy(iot_prdct_auth.mod_date, rows[3]);
		}
		if (rows[4] != nullptr)
		{
			strcpy(iot_prdct_auth.reg_date, rows[4]);
		}
	}
	mysql_free_result(res);
	return iot_prdct_auth;
}

KEPCO_API_AUTH MySQLService::getKepcoApiAuth(const char *api_key)
{
	char query[200] = {0};
	MYSQL_RES *result;
	MYSQL_ROW rows;
	sprintf(query, "SELECT * FROM %s.TB_KEPCO_API_AUTH WHERE KEPCO_API_KEY = '%s'", DB_NAME, api_key);
	mysql_query(conn, query);
	result = mysql_store_result(conn);
	uint8_t fieldsCounts = mysql_num_fields(result);
	KEPCO_API_AUTH kepco_api_auth;
	while (rows = mysql_fetch_row(result))
	{
		if (rows[0] != nullptr)
		{
			memcpy(kepco_api_auth.kepco_api_key, rows[0], strlen(rows[0]));
		}
		if (rows[1] != nullptr)
		{
			kepco_api_auth.auth_yn = strcmp(rows[1], "Y") ? 0 : 1;
		}
		if (rows[2] != nullptr)
		{
			kepco_api_auth.use_yn = strcmp(rows[2], "Y") ? 0 : 1;
		}
		if (rows[3] != nullptr)
		{
			memcpy(kepco_api_auth.mod_date, rows[3], strlen(rows[3]));
		}
		if (rows[4] != nullptr)
		{
			memcpy(kepco_api_auth.reg_date, rows[4], strlen(rows[4]));
		}
	}
	mysql_free_result(result);

	return kepco_api_auth;
}

MEMBER MySQLService::getMember(uint32_t CUSTOMER_LINK_NUMBER, const char *PRODUCT_SERIAL_NUMBER)
{
	char query[200] = {0};
	MYSQL_RES *result;
	MYSQL_ROW rows;
	sprintf(query, "SELECT * FROM %s.TB_MEMBER WHERE PRODUCT_SERIAL_NUMBER='%s' AND CUSTOMER_LINK_NUMBER=%d", DB_NAME, PRODUCT_SERIAL_NUMBER, CUSTOMER_LINK_NUMBER);
	mysql_query(conn, query);
	result = mysql_store_result(conn);
	uint8_t fieldsCounts = mysql_num_fields(result);
	MEMBER output;
	memset(&output, 0x00, sizeof(output));
	while (rows = mysql_fetch_row(result))
	{
		if (rows[0] != nullptr)
		{
			strcpy(output.product_serial_number, rows[0]);
		}
		if (rows[1] != nullptr)
		{
			output.customer_link_number = atoi(rows[1]);
		}
		if (rows[2] != nullptr)
		{
			strcpy(output.kepco_api_key, rows[2]);
		}
		if (rows[3] != nullptr)
		{
			strcpy(output.sns_id, rows[3]);
		}
		if (rows[4] != nullptr)
		{
			strcpy(output.sns_pwd, rows[4]);
		}
		if (rows[5] != nullptr)
		{
			strcpy(output.sns_key, rows[5]);
		}
		if (rows[6] != nullptr)
		{
			strcpy(output.sns_type, rows[6]);
		}
		if (rows[7] != nullptr)
		{
			strcpy(output.user_name, rows[7]);
		}
		if (rows[8] != nullptr)
		{
			strcpy(output.phone_number, rows[8]);
		}
		if (rows[9] != nullptr)
		{
			strcpy(output.zip_number, rows[9]);
		}
		if (rows[10] != nullptr)
		{
			strcpy(output.addr, rows[10]);
		}
		if (rows[11] != nullptr)
		{
			strcpy(output.addr_dtl, rows[11]);
		}
		if (rows[12] != nullptr)
		{
			output.lon = atof(rows[12]);
		}
		if (rows[13] != nullptr)
		{
			output.lat = atof(rows[13]);
		}
		if (rows[14] != nullptr)
		{
			output.house_area = atof(rows[14]);
		}
		if (rows[15] != nullptr)
		{
			output.male_cnt = atoi(rows[15]);
		}
		if (rows[16] != nullptr)
		{
			output.female_cnt = atoi(rows[16]);
		}
		if (rows[17] != nullptr)
		{
			output.room_cnt = atoi(rows[17]);
		}
		if (rows[18] != nullptr)
		{
			output.tv_cnt = atoi(rows[18]);
		}
		if (rows[19] != nullptr)
		{
			output.air_conditioner_cnt = atoi(rows[19]);
		}
		if (rows[20] != nullptr)
		{
			output.washing_machine_cnt = atoi(rows[20]);
		}
		if (rows[21] != nullptr)
		{
			output.dryer_cnt = atoi(rows[21]);
		}
		if (rows[22] != nullptr)
		{
			output.rice_cooker_cnt = atoi(rows[22]);
		}
		if (rows[23] != nullptr)
		{
			output.fan_cnt = atoi(rows[23]);
		}
		if (rows[24] != nullptr)
		{
			output.disk_washer_cnt = atoi(rows[24]);
		}
		if (rows[25] != nullptr)
		{
			output.computer_cnt = atoi(rows[25]);
		}
		if (rows[26] != nullptr)
		{
			strcpy(output.rate_plan, rows[26]);
		}
		if (rows[27] != nullptr)
		{
			output.use_yn = strcmp(rows[27], "Y") ? 0 : 1;
		}
		if (rows[28] != nullptr)
		{
			strcpy(output.mod_date, rows[28]);
		}
		if (rows[29] != nullptr)
		{
			strcpy(output.reg_date, rows[29]);
		}
	}
	mysql_free_result(result);

	return output;
}

MSG_INFO MySQLService::getMsgInfo(const char *msg_code)
{
	char query[200] = {0};
	MYSQL_RES *result;
	MYSQL_ROW rows;
	sprintf(query, "SELECT * FROM %s.TB_MSG_INFO WHERE MSG_CODE='%s'", DB_NAME, msg_code);
	mysql_query(conn, query);
	result = mysql_store_result(conn);
	uint8_t fieldsCounts = mysql_num_fields(result);
	MSG_INFO output;
	memset(&output, 0x00, sizeof(output));
	while (rows = mysql_fetch_row(result))
	{
		if (rows[0] != nullptr)
		{
			strcpy(output.msg_code, rows[0]);
		}
		if (rows[1] != nullptr)
		{
			strcpy(output.msg_version, rows[1]);
		}
		if (rows[2] != nullptr)
		{
			strcpy(output.msg_cont, rows[2]);
		}
		if (rows[3] != nullptr)
		{
			output.use_yn = strcmp(rows[3], "Y") ? 0 : 1;
		}
		if (rows[4] != nullptr)
		{
			strcpy(output.mod_date, rows[4]);
		}
		if (rows[5] != nullptr)
		{
			strcpy(output.reg_date, rows[5]);
		}
	}
	mysql_free_result(result);

	return output;
}

OUTPUT_DATA MySQLService::getOutputData(uint16_t years, uint32_t CUSTOMER_LINK_NUMBER, const char *dateTime)
{
	char query[200] = {0};
	MYSQL_RES *result;
	MYSQL_ROW rows;
	sprintf(query, "SELECT * FROM %s.TB_OUTPUT_DATA_%d WHERE CUSTOMER_LINK_NUMBER=%d AND DATE_TIME='%s'", DB_NAME, years, CUSTOMER_LINK_NUMBER, dateTime);
	mysql_query(conn, query);
	result = mysql_store_result(conn);
	uint8_t fieldsCounts = mysql_num_fields(result);
	static OUTPUT_DATA output;
	memset(&output, 0x00, sizeof(output));
	while (rows = mysql_fetch_row(result))
	{
		if (rows[0] != nullptr)
		{
			output.customer_link_number = atoi(rows[0]);
		}
		if (rows[1] != nullptr)
		{
			memcpy(output.date_time, rows[1], strlen(rows[1]));
		}
		if (rows[2] != nullptr)
		{
			output.temp = atof(rows[2]);
		}
		if (rows[3] != nullptr)
		{
			output.hmdty = atof(rows[3]);
		}
		if (rows[4] != nullptr)
		{
			output.pm25 = atof(rows[4]);
		}
		if (rows[5] != nullptr)
		{
			output.pm10 = atof(rows[5]);
		}
		if (rows[6] != nullptr)
		{
			output.dust = atof(rows[6]);
		}
		if (rows[7] != nullptr)
		{
			output.co2 = atof(rows[7]);
		}
		if (rows[8] != nullptr)
		{
			output.pwr = atof(rows[8]);
		}
		if (rows[9] != nullptr)
		{
			output.gas = atof(rows[9]);
		}
		if (rows[10] != nullptr)
		{
			output.water = atof(rows[10]);
		}
		if (rows[11] != nullptr)
		{
			output.prd_pwr = atof(rows[11]);
		}
		if (rows[12] != nullptr)
		{
			output.prd_gas = atof(rows[12]);
		}
		if (rows[13] != nullptr)
		{
			output.prd_water = atof(rows[13]);
		}
		if (rows[14] != nullptr)
		{
			memcpy(output.mod_date, rows[14], strlen(rows[14]));
		}
		if (rows[15] != nullptr)
		{
			memcpy(output.reg_date, rows[15], strlen(rows[15]));
		}
	}
	mysql_free_result(result);

	return output;
}

OUTPUT_STAT_DATA MySQLService::getOutputStatData(uint16_t years, uint32_t CUSTOMER_LINK_NUMBER, const char *dateTime)
{
	char query[200] = {0};
	MYSQL_RES *result;
	MYSQL_ROW rows;
	sprintf(query, "SELECT * FROM %s.TB_OUTPUT_STAT_DATA_%d WHERE CUSTOMER_LINK_NUMBER = %d AND DATE_TIME='%s'", DB_NAME, years, CUSTOMER_LINK_NUMBER, dateTime);
	mysql_query(conn, query);
	result = mysql_store_result(conn);
	uint8_t fieldsCounts = mysql_num_fields(result);
	uint16_t counter = 0;
	OUTPUT_STAT_DATA output;
	memset(&output, 0x00, sizeof(output));
	while (rows = mysql_fetch_row(result))
	{
		if (rows[0] != nullptr)
		{
			output.customer_link_number = atoi(rows[0]);
		}
		if (rows[1] != nullptr)
		{
			memcpy(output.date_time, rows[1], strlen(rows[1]));
		}
		if (rows[2] != nullptr)
		{
			output.prv_pwr = atof(rows[2]);
		}
		if (rows[3] != nullptr)
		{
			output.prv_gas = atof(rows[3]);
		}
		if (rows[4] != nullptr)
		{
			output.prv_water = atof(rows[4]);
		}
		if (rows[5] != nullptr)
		{
			output.prd_prv_pwr = atof(rows[5]);
		}
		if (rows[6] != nullptr)
		{
			output.prd_prv_gas = atof(rows[6]);
		}
		if (rows[7] != nullptr)
		{
			output.prd_prv_water = atof(rows[7]);
		}
		if (rows[8] != nullptr)
		{
			output.pre_pwr = atof(rows[8]);
		}
		if (rows[9] != nullptr)
		{
			output.pre_gas = atof(rows[9]);
		}
		if (rows[10] != nullptr)
		{
			output.pre_water = atof(rows[10]);
		}
		if (rows[11] != nullptr)
		{
			output.prd_pre_pwr = atof(rows[11]);
		}
		if (rows[12] != nullptr)
		{
			output.prd_pre_gas = atof(rows[12]);
		}
		if (rows[13] != nullptr)
		{
			output.prd_pre_water = atof(rows[13]);
		}
		if (rows[14] != nullptr)
		{
			memcpy(output.mod_date, rows[14], strlen(rows[14]));
		}
		if (rows[15] != nullptr)
		{
			memcpy(output.reg_date, rows[15], strlen(rows[15]));
		}
		counter++;
	}
	mysql_free_result(result);
	return output;
}

SYS_INFO MySQLService::getSysInfo(uint32_t CUSTOMER_LINK_NUMBER)
{
	char query[200] = {0};
	MYSQL_RES *result;
	MYSQL_ROW rows;
	sprintf(query, "SELECT * FROM %s.TB_SYS_INFO WHERE CUSTOMER_LINK_NUMBER = %d", DB_NAME, CUSTOMER_LINK_NUMBER);
	mysql_query(conn, query);
	result = mysql_store_result(conn);
	uint8_t fieldsCounts = mysql_num_fields(result);
	uint16_t counter = 0;
	SYS_INFO output;
	memset(&output, 0x00, sizeof(output));
	while (rows = mysql_fetch_row(result))
	{
		if (rows[0] != nullptr)
		{
			output.customer_link_number = atoi(rows[0]);
		}
		if (rows[1] != nullptr)
		{
			output.pwr_svn_trg = atoi(rows[1]);
		}
		if (rows[2] != nullptr)
		{
			output.gas_svn_trg = atoi(rows[2]);
		}
		if (rows[3] != nullptr)
		{
			output.wtr_svn_trg = atoi(rows[3]);
		}
		if (rows[4] != nullptr)
		{
			output.alarm_use_yn = strcmp(rows[4], "Y") ? 0 : 1;
		}
		if (rows[5] != nullptr)
		{
			output.alarm_sound_yn = strcmp(rows[5], "Y") ? 0 : 1;
		}
		if (rows[6] != nullptr)
		{
			output.alarm_vbrtn_yn = strcmp(rows[6], "Y") ? 0 : 1;
		}
		if (rows[7] != nullptr)
		{
			output.alarm_optm_yn = strcmp(rows[7], "Y") ? 0 : 1;
		}
		if (rows[8] != nullptr)
		{
			output.use_yn = strcmp(rows[8], "Y") ? 0 : 1;
		}
		if (rows[9] != nullptr)
		{
			strcpy(output.reg_date, rows[9]);
		}
		if (rows[10] != nullptr)
		{
			strcpy(output.mod_date, rows[10]);
		}
		if (rows[11] != nullptr)
		{
			output.tmp = atof(rows[11]);
		}
		if (rows[12] != nullptr)
		{
			output.tmp2 = atof(rows[12]);
		}
		if (rows[13] != nullptr)
		{
			output.tmp3 = atof(rows[13]);
		}
		if (rows[14] != nullptr)
		{
			output.tmp4 = atof(rows[14]);
		}
		if (rows[15] != nullptr)
		{
			output.tmp5 = atof(rows[15]);
		}
		if (rows[16] != nullptr)
		{
			strcpy(output.tmp6, rows[16]);
		}
		if (rows[17] != nullptr)
		{
			strcpy(output.tmp7, rows[17]);
		}
		if (rows[18] != nullptr)
		{
			strcpy(output.tmp8, rows[18]);
		}
		if (rows[19] != nullptr)
		{
			strcpy(output.tmp9, rows[19]);
		}
		if (rows[20] != nullptr)
		{
			strcpy(output.tmp10, rows[20]);
		}
		counter++;
	}
	mysql_free_result(result);

	return output;
}

TERMS_COND MySQLService::getTermsCond(uint32_t CUSTOMER_LINK_NUMBER)
{
	char query[200] = {0};
	MYSQL_RES *result;
	MYSQL_ROW rows;
	sprintf(query, "SELECT * FROM %s.TB_TERMS_COND WHERE CUSTOMER_LINK_NUMBER = %d", DB_NAME, CUSTOMER_LINK_NUMBER);
	mysql_query(conn, query);
	result = mysql_store_result(conn);
	uint8_t fieldsCounts = mysql_num_fields(result);
	uint16_t counter = 0;
	TERMS_COND output;
	memset(&output, 0x00, sizeof(output));
	while (rows = mysql_fetch_row(result))
	{
		if (rows[0] != nullptr)
		{
			output.customer_link_number = atoi(rows[0]);
		}
		if (rows[1] != nullptr)
		{
			strcpy(output.terms_code, rows[1]);
		}
		if (rows[2] != nullptr)
		{
			strcpy(output.terms_version, rows[2]);
		}
		if (rows[3] != nullptr)
		{
			strcpy(output.terms_cont, rows[3]);
		}
		if (rows[4] != nullptr)
		{
			strcpy(output.agrtr_cndtn_yn, rows[4]);
		}
		if (rows[5] != nullptr)
		{
			output.use_yn = strcmp(rows[5], "Y") ? 0 : 1;
		}
		if (rows[6] != nullptr)
		{
			strcpy(output.reg_date, rows[6]);
		}
		if (rows[7] != nullptr)
		{
			strcpy(output.mod_date, rows[7]);
		}
		counter++;
	}
	mysql_free_result(result);

	return output;
}

void MySQLService::getFotaInfo()
{
}

int32_t MySQLService::addEvent(const char *device_id, const char *evt_type, const char *addr)
{
	// 이벤트 추가
	char query[200] = {0};
	sprintf(query, "INSERT INTO %s.TB_CONN_LOG VALUES (NULL, '%s', '%s', '%s', '%s');", conf.table.c_str(), device_id, getDateTime(), evt_type, addr);

	int32_t status = mysql_query(conn, query);
	my_ulonglong statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	return status;
}

int32_t MySQLService::addInputData(uint16_t years, INPUT_DATA inputData)
{
	char query[1024] = {0};
	sprintf(query, "INSERT INTO %s.TB_INPUT_DATA_%d VALUES ('%s', '%s', %f, %f, %f, %f, '%s', %f, %f, %f, %f, %f, %f, '%s', '%s', %f, %f, %f, %f, %f, '%s', '%s', '%s', '%s', '%s')",
			DB_NAME, years, inputData.product_serial_number, inputData.date_time, inputData.temp, inputData.hmdty, inputData.pm25, inputData.pm10,
			inputData.mvmnt, inputData.tvoc, inputData.hcho, inputData.co2, inputData.co, inputData.benzo, inputData.radon, inputData.mod_date, inputData.reg_date,
			inputData.tmp, inputData.tmp2, inputData.tmp3, inputData.tmp4, inputData.tmp5,
			inputData.tmp6, inputData.tmp7, inputData.tmp8, inputData.tmp9, inputData.tmp10);

	int32_t status = mysql_query(conn, query);
	my_ulonglong statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	return status;
}

int32_t MySQLService::addIotFirm(IOT_FIRM iotFirm)
{
	char query[1200] = {0};
	sprintf(query, "INSERT INTO %s.TB_IOT_FIRM VALUES (NULL, '%s', '%s', '%s', '%s', '%s', '%s')", DB_NAME, iotFirm.type, iotFirm.ver, iotFirm.host, iotFirm.port, iotFirm.bin, getDateTime());

	int32_t status = mysql_query(conn, query);
	my_ulonglong statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	return status;
}

int32_t MySQLService::addIotPrdctAuth(IOT_PRDCT_AUTH iotPrdctAuth)
{
	char query[200] = {0};
	sprintf(query, "INSERT INTO %s.TB_IOT_PRDCT_AUTH VALUES ('%s', %d, %d, '%s', '%s')", DB_NAME, iotPrdctAuth.product_serial_number, iotPrdctAuth.auth_yn, iotPrdctAuth.use_yn, getDateTime(), getDateTime());

	int32_t status = mysql_query(conn, query);
	my_ulonglong statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	return status;
}

int32_t MySQLService::addKepcoApiAuth(KEPCO_API_AUTH kepcoApiAuth)
{
	char query[200] = {0};
	sprintf(query, "INSERT INTO %s.TB_KEPCO_API_AUTH VALUES ('%s', %d, %d, '%s', '%s')", DB_NAME, kepcoApiAuth.kepco_api_key, kepcoApiAuth.auth_yn, kepcoApiAuth.use_yn, getDateTime(), getDateTime());

	int32_t status = mysql_query(conn, query);
	my_ulonglong statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	return status;
}

int32_t MySQLService::addMember(MEMBER member)
{
	char query[2048] = {0};
	sprintf(query, "INSERT INTO %s.TB_MEMBER VALUES ('%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %f, %f, %f, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s', '%s', '%s')", DB_NAME,
			member.product_serial_number, member.customer_link_number, member.kepco_api_key, member.sns_id, member.sns_pwd, member.sns_key, member.sns_type, member.user_name, member.phone_number, member.zip_number, member.addr, member.addr_dtl,
			member.lon, member.lat, member.house_area, member.male_cnt, member.female_cnt, member.room_cnt, member.tv_cnt, member.air_conditioner_cnt, member.washing_machine_cnt,
			member.dryer_cnt, member.rice_cooker_cnt, member.fan_cnt, member.disk_washer_cnt, member.computer_cnt, member.rate_plan, member.use_yn ? "Y" : "N", getDateTime(), getDateTime());

	int32_t status = mysql_query(conn, query);
	my_ulonglong statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	return status;
}

int32_t MySQLService::addMsgInfo(MSG_INFO msgInfo)
{
	char query[2196] = {0};
	sprintf(query, "SET foreign_key_checks = 0");

	int32_t status = mysql_query(conn, query);
	my_ulonglong statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	memset(query, 0x00, sizeof(query));
	sprintf(query, "INSERT INTO %s.TB_MSG_INFO VALUES ('%s', '%s', '%s', '%s', '%s', '%s')", DB_NAME, msgInfo.msg_code, msgInfo.msg_version, msgInfo.msg_cont, msgInfo.use_yn ? "Y" : "N", getDateTime(), getDateTime());

	status = mysql_query(conn, query);
	statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	sprintf(query, "SET foreign_key_checks = 1");

	status = mysql_query(conn, query);
	statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	memset(query, 0x00, sizeof(query));
	return status;
}

int32_t MySQLService::addOutputData(uint16_t years, OUTPUT_DATA outputData)
{
	char query[300] = {0};
	sprintf(query, "SET foreign_key_checks = 0");

	int32_t status = mysql_query(conn, query);
	my_ulonglong statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	memset(query, 0x00, sizeof(query));
	sprintf(query, "INSERT INTO %s.TB_OUTPUT_DATA_%d VALUES (%d, '%s', %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, '%s', '%s', '%s')", DB_NAME, years,
			outputData.customer_link_number, outputData.date_time, outputData.temp, outputData.hmdty, outputData.pm25, outputData.pm10,
			outputData.dust, outputData.co2, outputData.pwr, outputData.gas, outputData.water, outputData.prd_pwr, outputData.prd_gas, outputData.prd_water, outputData.vis_sync, outputData.mod_date, outputData.reg_date);

	status = mysql_query(conn, query);
	statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	memset(query, 0x00, sizeof(query));
	sprintf(query, "SET foreign_key_checks = 1");

	status = mysql_query(conn, query);
	statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	memset(query, 0x00, sizeof(query));
	return status;
}

int32_t MySQLService::addOutputStatData(uint16_t years, OUTPUT_STAT_DATA outputStatData)
{
	char query[300] = {0};
	sprintf(query, "SET foreign_key_checks = 0");

	int32_t status = mysql_query(conn, query);
	my_ulonglong statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	memset(query, 0x00, sizeof(query));
	sprintf(query, "INSERT INTO %s.TB_OUTPUT_STAT_DATA_%d VALUES (%d, '%s', %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, '%s', '%s')", DB_NAME, years,
			outputStatData.customer_link_number, outputStatData.date_time, outputStatData.prv_pwr, outputStatData.prv_gas, outputStatData.prv_water, outputStatData.prd_prv_pwr, outputStatData.prd_prv_gas, outputStatData.prd_prv_water,
			outputStatData.pre_pwr, outputStatData.pre_gas, outputStatData.pre_water, outputStatData.prd_pre_pwr, outputStatData.prd_pre_gas, outputStatData.prd_pre_water, outputStatData.mod_date, outputStatData.reg_date);

	status = mysql_query(conn, query);
	statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	memset(query, 0x00, sizeof(query));
	sprintf(query, "SET foreign_key_checks = 1");

	status = mysql_query(conn, query);
	statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	memset(query, 0x00, sizeof(query));
	return status;
}

int32_t MySQLService::addSysInfo(SYS_INFO sysInfo)
{
	char query[400] = {0};
	sprintf(query, "INSERT INTO %s.TB_SYS_INFO VALUES (%d, %d, %d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', %f, %f, %f, %f, %f, '%s', '%s', '%s', '%s', '%s')", DB_NAME,
			sysInfo.customer_link_number, sysInfo.pwr_svn_trg, sysInfo.gas_svn_trg, sysInfo.wtr_svn_trg, sysInfo.alarm_use_yn ? "Y" : "N", sysInfo.alarm_sound_yn ? "Y" : "N", sysInfo.alarm_vbrtn_yn ? "Y" : "N", sysInfo.alarm_optm_yn ? "Y" : "N", sysInfo.use_yn ? "Y" : "N",
			sysInfo.reg_date, sysInfo.mod_date, sysInfo.tmp, sysInfo.tmp2, sysInfo.tmp3, sysInfo.tmp4, sysInfo.tmp5, sysInfo.tmp6, sysInfo.tmp7, sysInfo.tmp8, sysInfo.tmp9, sysInfo.tmp10);

	int32_t status = mysql_query(conn, query);
	my_ulonglong statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}
	return status;
}

int32_t MySQLService::addTermsCond(TERMS_COND termsCond)
{
	char query[400] = {0};
	sprintf(query, "INSERT INTO %s.TB_TERMS_COND VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s')", DB_NAME, termsCond.customer_link_number, termsCond.terms_code, termsCond.terms_version, termsCond.terms_cont, termsCond.agrtr_cndtn_yn, termsCond.use_yn ? "Y" : "N", termsCond.mod_date, termsCond.reg_date);

	int32_t status = mysql_query(conn, query);
	my_ulonglong statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	printf("%d : %s\n", status, query);
	return status;
}

int32_t MySQLService::addFotaInfo()
{
	char query[200] = {0};
	sprintf(query, "INSERT INTO %s.TB_MEMBER VALUES ()", DB_NAME);

	int32_t status = mysql_query(conn, query);
	my_ulonglong statusDtl = mysql_affected_rows(conn);
	if ((status != 0) || (status == 0 && statusDtl < 1))
	{
		spdlog::error("DB 실패 : {} : {} : {} : {}", mysql_error(conn), status, statusDtl, query);
	}

	return status;
}

void MySQLService::deleteInputData()
{
}

void MySQLService::deleteIotFirm()
{
}

void MySQLService::deleteIotPrdctAuth()
{
}

void MySQLService::deleteKepcoApiAuth()
{
}

void MySQLService::deleteMember()
{
}

void MySQLService::deleteMsgInfo()
{
}

void MySQLService::deleteOutputData()
{
}

void MySQLService::deleteOutputStatData()
{
}

void MySQLService::deleteSysInfo()
{
}

void MySQLService::deleteTermsCond()
{
}

void MySQLService::deleteFotaInfo()
{
}

void MySQLService::updateInputData(uint16_t years)
{
}

void MySQLService::updateIotFirm()
{
}

void MySQLService::updateIotPrdctAuth()
{
}

void MySQLService::updateKepcoApiAuth()
{
}

void MySQLService::updateMember()
{
}

void MySQLService::updateMsgInfo()
{
}

void MySQLService::updateOutputData(uint16_t years)
{
}

void MySQLService::updateOutputStatData(uint16_t years)
{
}

void MySQLService::updateSysInfo()
{
}

void MySQLService::updateTermsCond()
{
}

void MySQLService::updateFotaInfo()
{
}