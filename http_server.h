/**
* @Project	[HTTP Module]
* @File 	[http_server.h]
* @Author 	[caydyn@icloud.com]
* @Version 	[1.0]
*/

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <microhttpd.h> //gcc -lmicrohttpt

struct MHD_Daemon *_http_daemon;
#define PORT_NUMBER 80
static char WEB_PATH[128];
#define MAX_API 100

#define true 1
#define false 0

/*gui file type */
#define HTM_FILE 0x1
#define JAS_FILE 0x2
#define CSS_FILE 0x3
#define PNG_FILE 0x4
#define JPG_FILE 0x5
#define XML_FILE 0x6
#define CSV_FILE 0x7
#define PDF_FILE 0x8
#define APP_FILE 0x9

#define UPLOAD_SYMBLE 		"\r\n"
#define UPLOAD_SYMBLE_NB    3
#define FILE_SIZE_FLAG		"name"
#define DOUBLE_QUOTES   	34
#define SIZE_OFFSET     	5
#define NAME_OFFSET     	9

const static char * _404_page = "<html><head><title>404 Error</title></head><body>	\
		<h1>Request Not Found</h1></body></html>";
const static char * _500_page = "<html><head><title>500 Error</title></head><body>	\
	<h1>Request API Not Found or Handle Error</h1></body></html>";

typedef struct {
	char url[512];
	char method[10];
	uint32_t offset;
	uint32_t post_data_len;
	char *post_data;

	/*file upload*/
	char file_name[64];
	uint8_t after;
	uint32_t used;
	uint32_t file_size;
} next_connection_info;

typedef int (*APIFunc) (struct MHD_Connection * connection);
APIFunc APIFA[MAX_API];
#define REGISTER_API(ID, FUNC) APIFA[ID] = FUNC;


uint8_t init_http_server(char *);
void release_http_server(void);


#endif