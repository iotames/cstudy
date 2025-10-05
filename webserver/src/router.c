#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

// 路由处理函数声明
typedef void (*route_handler_t)(int client_fd);

// 发送HTTP响应
static void send_response(int client_fd, const char *status, const char *content_type, const char *body) {
    char response[1024];
    snprintf(response, sizeof(response),
        "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n%s",
        status, content_type, strlen(body), body);
#ifdef _WIN32
    send(client_fd, response, (int)strlen(response), 0);
#else
    write(client_fd, response, strlen(response));
#endif
}

// 处理GET /
static void handle_home(int client_fd) {
    send_response(client_fd, "200 OK", "text/plain", "this is home");
}

// 处理GET /helloapi
static void handle_helloapi(int client_fd) {
    send_response(client_fd, "200 OK", "application/json", "{\"code\":0,\"msg\":\"this is json api\"}");
}

// 路由分发
void route_request(int client_fd, const char *request) {
    // 简单解析请求行
    char method[8], path[256];
    sscanf(request, "%7s %255s", method, path);
    if (strcmp(method, "GET") == 0 && strcmp(path, "/") == 0) {
        handle_home(client_fd);
    } else if (strcmp(method, "GET") == 0 && strcmp(path, "/helloapi") == 0) {
        handle_helloapi(client_fd);
    } else {
        send_response(client_fd, "404 Not Found", "text/plain", "404 Not Found");
    }
}
