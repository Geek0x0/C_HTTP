/**
* @Project	[HTTP Module]
* @File 	[http_server.c]
* @Author 	[caydyn@icloud.com]
* @Version 	[1.0]
*/


#include "WPT_server.h"

/**
 * register request api handle
 * @param CallbackID [request handle id]
 * @param Func       [request handle func]
 */
static inline void register_api_handle(uint32_t CallbackID,
                                       APIFunc Func)
{
	REGISTER_API(CallbackID, Func);
}

/**
 * set response mime type
 * @param response [response handle]
 * @param type     [file type]
 */
inline void set_mime(struct MHD_Response *response,
                     uint8_t type)
{
	char *type_string = NULL;

	switch (type)
	{
		case HTM_FILE:
			type_string = "text/html";
			break;
		case CSS_FILE:
			type_string = "text/css";
			break;
		case JAS_FILE:
			type_string = "application/x-javascript";
			break;
		case PNG_FILE:
			type_string = "image/png";
			break;
		case JPG_FILE:
			type_string = "image/jpeg";
			break;
		case XML_FILE:
			type_string = "text/xml";
			break;
		case CSV_FILE:
			type_string = "application/csv";
			break;
		case PDF_FILE:
			type_string = "application/pdf";
			break;
		case APP_FILE:
			type_string = "application/x-msdownload";
			break;
	}
	MHD_add_response_header(response,
	                        MHD_HTTP_HEADER_CONTENT_TYPE, type_string);
}

/**
 * response 500 error
 * @param connection [http connect handle]
 */
inline void return_500 (struct MHD_Connection * connection)
{
	struct MHD_Response *response =
	    MHD_create_response_from_buffer (strlen (_500_page),
	                                     (void *) _500_page, MHD_RESPMEM_PERSISTENT);
	set_mime(response, HTM_FILE);
	MHD_queue_response (connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
	MHD_destroy_response (response);
}

/**
 * response 404 error
 * @param connection [http connect handle]
 */
inline void return_404 (struct MHD_Connection * connection)
{
	struct MHD_Response *response =
	    MHD_create_response_from_buffer (strlen (_404_page),
	                                     (void *) _404_page, MHD_RESPMEM_PERSISTENT);
	set_mime(response, HTM_FILE);
	MHD_queue_response (connection, MHD_HTTP_NOT_FOUND, response);
	MHD_destroy_response (response);
}

/**
 * http request file
 * @param  url  [request url string]
 * @param  fd   [return file fd]
 * @param  size [return file size]
 * @param  type [return file type]
 * @return      [success: 1, failure: 0]
 */
inline static uint8_t url_to_file(const char *url, int *fd,
                                  int *size, int *type)
{
	struct stat f_stat;
	char file_path[512] = { 0 };

	if (!strcmp(url, "/"))
		sprintf(file_path, "%s/%s", WEB_PATH, "index.html");
	else
		sprintf(file_path, "%s%s", WEB_PATH, url);

	if ((*fd = open(file_path, O_RDONLY)) < 0)
		goto __ERR_PROCESS;

	if (fstat(*fd, &f_stat) < 0)
		goto __ERR_PROCESS;

	if ((*size = f_stat.st_size) < 0)
		goto __ERR_PROCESS;

	if (strstr(url, ".html"))
		*type = HTM_FILE;
	else if (strstr(url, ".css"))
		*type = CSS_FILE;
	else if (strstr(url, ".js"))
		*type = JAS_FILE;
	else if (strstr(url, ".png"))
		*type = PNG_FILE;
	else if (strstr(url, ".jpg"))
		*type = JPG_FILE;
	else if (strstr(url, ".xml"))
		*type = XML_FILE;
	else if (strstr(url, ".csv"))
		*type = CSV_FILE;
	else if (strstr(url, ".pdf"))
		*type = PDF_FILE;
	else if (strstr(url, ".exe"))
		*type = APP_FILE;
	else
		*type = HTM_FILE;

	return 1;

__ERR_PROCESS:
	fprintf(stderr, "%s, %d, %s, path:%s\n",
	        __FILE__, __LINE__, __FUNCTION__, file_path);
	return 0;
}

/**
 * http requset api
 * @param  url        [request url string]
 * @param  connection [http connect handle]
 */
inline uint8_t url_to_api(const char *url,
                          struct MHD_Connection * connection)
{
	int id, err;

	id = atoi(url);
	if (APIFA[id])
		err = APIFA[id](connection);
	else
		return false;

	if (err < 0)
	{
		return_500(connection);
		return true;
	}
	else
		return false;
}

inline void process_get_url_requert(const char *url,
                                    struct MHD_Connection * connection)
{
	struct MHD_Response *response;
	int fd, size, type;

	if (url_to_file(url, &fd, &size, &type))
	{
		response =
		    MHD_create_response_from_fd(size, fd);
		set_mime(response, type);
		MHD_queue_response(connection, MHD_HTTP_OK, response);
		MHD_destroy_response(response);
		return;
	}
	else if (url_to_api(url, connection))
		return;
	else
		return_404(connection);
}

/**
 * requset post file
 * @param  content   [post data]
 * @param  len       [current post data len]
 * @param  file_name [save file name]
 * @param  file_size [post file total size]
 * @return           [current success save file size]
 */
static inline int upload_handle(char *content,
                                int len, char *file_name, int *file_size)
{
	int filter = UPLOAD_SYMBLE_NB;
	char *real = NULL, *find_size = NULL, get_size[4] = { 0 };
	int fd, size, write_size = 0;

	if (!content)
		return (-1);

	find_size = strstr(content, FILE_SIZE_FLAG);
	if (find_size)
	{
		memcpy(get_size, find_size + SIZE_OFFSET + 1,
		       strchr(find_size + SIZE_OFFSET + 1, DOUBLE_QUOTES) -
		       strchr(find_size + SIZE_OFFSET, DOUBLE_QUOTES) - 1);
	}

	*file_size = atoi(get_size);

	real = strstr(content, UPLOAD_SYMBLE);
	real++;
	while (filter)
	{
		real = strstr(real, UPLOAD_SYMBLE);
		real++;
		filter--;
	}

	write_size = len - (real - content) - 1;
	fd = open(file_name, O_RDWR | O_CREAT);
	size = write(fd, real + 1, write_size);
	close(fd);

	return size;
}

/**
 * frist process upload file
 * @param info [last connect handle]
 * @param data [upload data]
 * @param size [upload data size]
 */
static inline void upload_Begin_proc(next_connection_info* info,
                                     char *data, int size)
{
	upload_handle(data, size, info->file_name, &(info->file_size));
}

static inline void upload_Other_proc(next_connection_info* info,
                                     char* data, int size)
{
	int err, fd, len;

	if (!info->after)
	{
		err = upload_handle(data, size, info->file_name,
		                    &(info->file_size));
		if (err > 0)
		{
			info->used += err;
			info->after = true;
		}
	}
	else
	{
		fd = open(info->file_name, O_RDWR | O_APPEND);
		if (info->used + size < info->file_size)
			len = size;
		else
			len = info->file_size - info->used;
		err = write(fd, data, len);
		info->used += err;
		close(fd);
	}
}

/**
 * http request handle
 */
inline int http_request_handle (void *cls, struct MHD_Connection *connection, const char *url,
                                const char *method, const char *version, const char *upload_data,
                                size_t * upload_data_size, void **con_cls)
{
	if (!(*con_cls))
	{
		if (!strcmp(method, MHD_HTTP_METHOD_POST))
		{
			next_connection_info *next_connection;
			next_connection = calloc(sizeof(char),
			                         sizeof(next_connection_info));
			strcpy(next_connection->url, url);
			strcpy(next_connection->method, MHD_HTTP_METHOD_POST);
			next_connection->post_data_len
			    = atoi (MHD_lookup_connection_value (connection,
			            MHD_HEADER_KIND,
			            MHD_HTTP_HEADER_CONTENT_LENGTH));

			if (strstr(url, "upload"))
			{
				const char *name = MHD_lookup_connection_value(connection,
				                   MHD_GET_ARGUMENT_KIND, "file_name");
				if (!name)
				{
					free(next_connection);
					return MHD_NO;
				}
				else
					strcpy(next_connection->file_name, name);


				next_connection->post_data = NULL;
			}
			else
			{
				next_connection->post_data = calloc(sizeof(char),
				                                    next_connection->post_data_len);
				if (!next_connection->post_data)
					return MHD_NO;
			}
			*con_cls = (void *)next_connection;
		}
		else if (!strcmp(method, MHD_HTTP_METHOD_GET))
			process_get_url_requert(url, connection);
	}
	else
	{
		next_connection_info *next_connection = ((next_connection_info *)(*con_cls));

		if (!strcmp(next_connection->method,  MHD_HTTP_METHOD_POST))
		{
			if (*upload_data_size)
			{
				if (next_connection->post_data_len == *upload_data_size)
				{
					if (strstr(next_connection->url, "upload"))
						upload_Begin_proc(next_connection,
						                  (char *)upload_data, *upload_data_size);
					else
						memcpy(next_connection->post_data, upload_data,
						       next_connection->post_data_len);

					*upload_data_size = 0;
				}
				else
				{
					if (strstr(next_connection->url, "upload"))
						upload_Other_proc(next_connection,
						                  (char *)upload_data, *upload_data_size);
					else
						memcpy((next_connection->post_data + next_connection->offset),
						       upload_data, *upload_data_size);

					next_connection->offset += *upload_data_size;
					*upload_data_size = 0;
				}
			}
			else
			{
				url_to_api(url, connection);
				if (next_connection->post_data)
				{
					free(next_connection->post_data);
					free(next_connection);
				}
			}
		}
		else
			return_404(connection);
	}
	return MHD_YES;
}

/**
 * init http server
 * @return [http server handle]
 */
uint8_t init_http_server(char *path)
{
	if (!path)
		return 0;

	sprintf(WEB_PATH, "%s", path);

	_http_daemon = MHD_start_daemon (
	                   MHD_USE_SELECT_INTERNALLY,
	                   PORT_NUMBER, NULL, NULL, &http_request_handle, NULL,
	                   MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int) 120, MHD_OPTION_END);
	if (!_http_daemon)
		return false;

	printf ("HTTP Server is running on %d.\n", PORT_NUMBER);
	return true;
}

/**
 * clear http handle
 */
void release_http_server(void)
{
	MHD_stop_daemon(_http_daemon);
}