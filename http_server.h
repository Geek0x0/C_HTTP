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

#define DEBUG 1

#define PORT_NUMBER 	80
#define MAX_API 		100

struct MHD_Daemon *_http_daemon;
struct MHD_Connection * _cur_connect;
static char WEB_PATH[128];

#define true 1
#define false 0

/*gui file type */
#define HTM_FILE 1
#define JAS_FILE 2
#define CSS_FILE 3
#define PNG_FILE 4
#define JPG_FILE 5
#define XML_FILE 6
#define CSV_FILE 7
#define PDF_FILE 8
#define APP_FILE 9
#define JSO_FILE 10
#define TXT_FILE 11
#define WOF_FILE 12
#define TTF_FILE 13
#define EOT_FILE 14
#define OTF_FILE 15
#define SVG_FILE 16
#define MP3_FILE 17
#define WAV_FILE 18
#define GIF_FILE 19
#define MP4_FILE 20
#define AVI_FILE 21
#define FLV_FILE 22

const static char *mime_type_string[] = {
	"placeholder_not_used",
	"text/html",
	"text/css",
	"application/x-javascript",
	"image/png",
	"image/jpeg",
	"text/xml",
	"application/csv",
	"application/pdf",
	"application/x-msdownload",
	"application/json",
	"application/text",
	"application/font-woff",
	"application/font-ttf",
	"application/vnd.ms-fontobject",
	"application/ont-otf",
	"image/svg+xml",
	"audio/mpeg3",
	"audio/wav",
	"image/gif",
	"audio/mp4",
	"video/x-msvideo",
	"video/x-flv",
};

#define UPLOAD_SYMBLE 		"\r\n"
#define UPLOAD_SYMBLE_NB    3
#define FILE_SIZE_FLAG		"name"
#define DOUBLE_QUOTES   	34
#define SIZE_OFFSET     	5
#define NAME_OFFSET     	9

const static char * _200_page = "<html><head><title>200 Success</title></head><body>	\
		<h1>200, Request successfully processed</h1></body></html>";
const static char * _302_page = "<html><head><title>302 Redirect</title></head><body>	\
		<h1>302, Redirect request</h1></body></html>";
const static char * _404_page = "<html><head><title>404 Error</title></head><body>		\
		<h1>404, Request not found</h1></body></html>";
const static char * _500_page = "<html><head><title>500 Error</title></head><body>		\
		<h1>500, Request API Not Found or Handle Error: [%d]</h1></body></html>";

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

/*
	HOW TO DEFINE THIS CALLBACK FUNCTION !!
	data: geted post request data (sometime is NULL!)
	size: post data size
	response_data: custom response data buffer
	enable_redirect: enable http 302 action
	redirect_url: new url string
 */
typedef int (*APIFunc) (const char *data, const uint32_t size,
                        char **response_data, uint8_t *enable_redirect, char *redirect_url);

APIFunc APIFA[MAX_API];
#define REGISTER_API(ID, FUNC) APIFA[ID] = FUNC;


uint8_t init_http_server(char *);
void release_http_server(void);
void register_api_handle(uint32_t, APIFunc);
inline const char *get_url_param(char *);


#endif