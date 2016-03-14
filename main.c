#include "WPT_server.h"

int main()
{
    init_http_server("");
    getc(stdin);
    release_http_server();
}
