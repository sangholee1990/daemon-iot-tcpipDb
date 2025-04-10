#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <ctime>
#include <sys/time.h>
#include <cstddef>
#include <fstream>
#include <math.h>
#include <float.h>
#include <arpa/inet.h>
#include <iomanip> 

#define MAX_CLIENT_NO 1000
#define RECV_BUFFER_SIZE 4096
#define SOCKET_READ_TIMEOUT 60

#define APP_VER_MAJOR   0
#define APP_VER_MINOR   0  
#define APP_VER_PATCH   1

#define CTRL_REQUEST_FIRMVER            0x0001
#define CTRL_REQUEST_FIRMURL            0x0002
#define GET_SYSTEM_TIME                 0x0003
#define CTRL_REQUEST_INPUT_DATA         0x0008
#define CTRL_REQUEST_IOT_FIRM           0x0009
#define CTRL_REQUEST_MEMBER             0x0010
#define CTRL_REQUEST_IOT_PRDCT_AUTH     0x0011
#define CTRL_REQUEST_KEPCO_API_AUTH     0x0012
#define CTRL_REQUEST_MSG_INFO           0x0013
#define CTRL_REQUEST_OUTPUT_DATA        0x0014
#define CTRL_REQUEST_OUTPUT_STAT_DATA   0x0015
#define CTRL_REQUEST_SYS_INFO           0x0016
#define CTRL_REQUEST_TERMS_COND         0x0017
#define CTRL_CREATE_INPUT               0x0030
#define CTRL_CREATE_MEMBER              0x0031
#define CTRL_CREATE_IOT_PRDCT_AUTH      0x0032
#define CTRL_CREATE_KEPCO_API_AUTH      0x0033
#define CTRL_CREATE_MSG_INFO            0x0034
#define CTRL_CREATE_OUTPUT_DATA         0x0035
#define CTRL_CREATE_OUTPUT_STAT_DATA    0x0036
#define CTRL_CREATE_SYS_INFO            0x0037
#define CTRL_CREATE_TERMS_COND          0x0038
#define CTRL_UPDATE_MEMBER              0x0050
#define CTRL_UPDATE_IOT_PRDCT_AUTH      0x0051
#define CTRL_UPDATE_KEPCO_API_AUTH      0x0052
#define CTRL_UPDATE_MSG_INFO            0x0053
#define CTRL_UPDATE_OUTPUT_DATE         0x0054
#define CTRL_UPDATE_OUTPUT_STAT_DATA    0x0055 
#define CTRL_UPDATE_SYS_INFO            0x0056
#define CTRL_DELETE_MEMBER              0x0070
#define CTRL_DELETE_IOT_PRDCT_AUTH      0x0071
#define CTRL_DELETE_KEPCO_API_AUTH      0x0072
#define CTRL_DELETE_MSG_INFO            0x0073
#define CTRL_DELETE_OUTPUT_DATA         0x0074
#define CTRL_DELETE_OUTPUT_STAT_DATA    0x0075
#define CTRL_DELETE_SYS_INFO            0x0076
