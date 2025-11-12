#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include <alloca.h>
#include "mongoose.h"
#include "cJSON.h"

#define _POSIX_C_SOURCE 200809L
#define PORT "8000"
#define DB_FILE "products.db"

// strdup不是C标准库的一部分，直到C23才被纳入。在一些严格遵循C99的环境中，strdup可能未定义。
// 这段代码检查C标准版本，如果低于C11，则使用Mongoose的mg_strdup函数来模拟strdup。
// mg_strdup返回一个mg_str结构，我们取其buf字段作为字符串指针。
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#define strdup(s) mg_strdup(mg_str(s)).buf
#endif

// 定义mongoose.h缺失的 mg_http_match_uri 函数
static bool mg_http_match_uri(const struct mg_http_message *hm, const char *pattern) {
    return mg_strcmp(hm->uri, mg_str(pattern)) == 0;
}

// 定义mongoose.h缺失的 mg_strndup 函数
static char *mg_strndup(const char *buf, size_t len) {
    char *result = malloc(len + 1);
    if (result) {
        memcpy(result, buf, len);
        result[len] = '\0';
    }
    return result;
}

// 商品数据结构
typedef struct {
    int id;
    char name[100];
    double price;
    int stock;
    char description[200];
} Product;

// 全局数据库连接
sqlite3 *db = NULL;

// 初始化数据库
int init_database() {
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    // 创建商品表
    const char *sql = "CREATE TABLE IF NOT EXISTS products ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "name TEXT NOT NULL,"
                     "price REAL NOT NULL,"
                     "stock INTEGER DEFAULT 0,"
                     "description TEXT);";
    
    char *err_msg = NULL;
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL错误: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    printf("数据库初始化成功\n");
    return 0;
}

// 发送HTTP响应
void send_response(struct mg_connection *c, int status_code, const char *json_data) {
    mg_printf(c, "HTTP/1.1 %d OK\r\n", status_code);
    mg_printf(c, "Content-Type: application/json\r\n");
    mg_printf(c, "Access-Control-Allow-Origin: *\r\n");
    mg_printf(c, "Content-Length: %lu\r\n", strlen(json_data));
    mg_printf(c, "\r\n");
    mg_printf(c, "%s", json_data);
}

// 发送错误响应
void send_error(struct mg_connection *c, int status_code, const char *message) {
    cJSON *error_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(error_json, "code", status_code);
    cJSON_AddStringToObject(error_json, "message", message);
    
    char *json_str = cJSON_Print(error_json);
    send_response(c, status_code, json_str);
    
    free(json_str);
    cJSON_Delete(error_json);
}

// 解析查询字符串参数
char* get_query_param(struct mg_str query_str, const char *param_name) {
    if (query_str.len == 0) return NULL;
    // 将 mg_str（可能不包含终止符的结构）转换为以 '\0'​ （空字符，NUL）结尾的标准C字符串，以便使用 strtok等依赖于终止符的函数。
    // '0' 是一个字符，其ASCII码值为0。它是C语言中用来标记字符串结束的终止符（null terminator）。
    char *query_string = (char *)malloc(query_str.len + 1);
    if (query_string == NULL) return NULL;
    memcpy(query_string, query_str.buf, query_str.len);
    query_string[query_str.len] = '\0';

    char *token = strtok(query_string, "&");
    char *value = NULL;

    while (token) {
        char *eq_pos = strchr(token, '=');
        if (eq_pos) {
            *eq_pos = '\0';
            if (strcmp(token, param_name) == 0) {
                value = strdup(eq_pos + 1);
                break;
            }
        }
        token = strtok(NULL, "&");
    }
    
    free(query_string);
    return value;
}

// 获取商品列表
void handle_get_products(struct mg_connection *c) {
    const char *sql = "SELECT * FROM products";
    sqlite3_stmt *stmt;
    
    cJSON *products_array = cJSON_CreateArray();
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cJSON *product_json = cJSON_CreateObject();
            cJSON_AddNumberToObject(product_json, "id", sqlite3_column_int(stmt, 0));
            cJSON_AddStringToObject(product_json, "name", (const char*)sqlite3_column_text(stmt, 1));
            cJSON_AddNumberToObject(product_json, "price", sqlite3_column_double(stmt, 2));
            cJSON_AddNumberToObject(product_json, "stock", sqlite3_column_int(stmt, 3));
            cJSON_AddStringToObject(product_json, "description", 
                                   sqlite3_column_text(stmt, 4) ? (const char*)sqlite3_column_text(stmt, 4) : "");
            
            cJSON_AddItemToArray(products_array, product_json);
        }
        sqlite3_finalize(stmt);
    }
    
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "code", 200);
    cJSON_AddItemToObject(response, "data", products_array);
    
    char *json_str = cJSON_Print(response);
    send_response(c, 200, json_str);
    
    free(json_str);
    cJSON_Delete(response);
}

// 获取单个商品
// void handle_get_product(struct mg_connection *c, const char *query_string) {
void handle_get_product(struct mg_connection *c, struct mg_str query_str) {
    char *id_str = get_query_param(query_str, "id");
    // char *id_str = get_query_param(query_string, "id");
    if (!id_str) {
        send_error(c, 400, "缺少id参数");
        return;
    }
    
    int id = atoi(id_str);
    free(id_str);
    
    const char *sql = "SELECT * FROM products WHERE id = ?";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cJSON *product_json = cJSON_CreateObject();
            cJSON_AddNumberToObject(product_json, "id", sqlite3_column_int(stmt, 0));
            cJSON_AddStringToObject(product_json, "name", (const char*)sqlite3_column_text(stmt, 1));
            cJSON_AddNumberToObject(product_json, "price", sqlite3_column_double(stmt, 2));
            cJSON_AddNumberToObject(product_json, "stock", sqlite3_column_int(stmt, 3));
            cJSON_AddStringToObject(product_json, "description", 
                                   sqlite3_column_text(stmt, 4) ? (const char*)sqlite3_column_text(stmt, 4) : "");
            
            cJSON *response = cJSON_CreateObject();
            cJSON_AddNumberToObject(response, "code", 200);
            cJSON_AddItemToObject(response, "data", product_json);
            
            char *json_str = cJSON_Print(response);
            send_response(c, 200, json_str);
            free(json_str);
            cJSON_Delete(response);
        } else {
            send_error(c, 404, "商品未找到");
        }
        sqlite3_finalize(stmt);
    } else {
        send_error(c, 500, "数据库错误");
    }
}

// 创建商品
void handle_create_product(struct mg_connection *c, const char *body) {
    cJSON *product_json = cJSON_Parse(body);
    if (!product_json) {
        send_error(c, 400, "无效的JSON数据");
        return;
    }
    
    cJSON *name = cJSON_GetObjectItem(product_json, "name");
    cJSON *price = cJSON_GetObjectItem(product_json, "price");
    cJSON *stock = cJSON_GetObjectItem(product_json, "stock");
    cJSON *description = cJSON_GetObjectItem(product_json, "description");
    
    if (!name || !price) {
        send_error(c, 400, "缺少必要字段");
        cJSON_Delete(product_json);
        return;
    }
    
    const char *sql = "INSERT INTO products (name, price, stock, description) VALUES (?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name->valuestring, -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, price->valuedouble);
        sqlite3_bind_int(stmt, 3, stock ? stock->valueint : 0);
        sqlite3_bind_text(stmt, 4, description ? description->valuestring : "", -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cJSON *response = cJSON_CreateObject();
            cJSON_AddNumberToObject(response, "code", 200);
            cJSON_AddNumberToObject(response, "id", (int)sqlite3_last_insert_rowid(db));
            cJSON_AddStringToObject(response, "message", "商品创建成功");
            
            char *json_str = cJSON_Print(response);
            send_response(c, 200, json_str);
            free(json_str);
            cJSON_Delete(response);
        } else {
            send_error(c, 500, "创建商品失败");
        }
        sqlite3_finalize(stmt);
    } else {
        send_error(c, 500, "数据库错误");
    }
    
    cJSON_Delete(product_json);
}

// 更新商品
void handle_update_product(struct mg_connection *c, const char *body) {
    cJSON *product_json = cJSON_Parse(body);
    if (!product_json) {
        send_error(c, 400, "无效的JSON数据");
        return;
    }
    
    cJSON *id = cJSON_GetObjectItem(product_json, "id");
    if (!id) {
        send_error(c, 400, "缺少商品ID");
        cJSON_Delete(product_json);
        return;
    }
    
    const char *sql = "UPDATE products SET name=?, price=?, stock=?, description=? WHERE id=?";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, cJSON_GetObjectItem(product_json, "name")->valuestring, -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, cJSON_GetObjectItem(product_json, "price")->valuedouble);
        sqlite3_bind_int(stmt, 3, cJSON_GetObjectItem(product_json, "stock")->valueint);
        sqlite3_bind_text(stmt, 4, 
                         cJSON_GetObjectItem(product_json, "description") ? 
                         cJSON_GetObjectItem(product_json, "description")->valuestring : "", -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 5, id->valueint);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            if (sqlite3_changes(db) > 0) {
                cJSON *response = cJSON_CreateObject();
                cJSON_AddNumberToObject(response, "code", 200);
                cJSON_AddStringToObject(response, "message", "商品更新成功");
                
                char *json_str = cJSON_Print(response);
                send_response(c, 200, json_str);
                free(json_str);
                cJSON_Delete(response);
            } else {
                send_error(c, 404, "商品未找到");
            }
        } else {
            send_error(c, 500, "更新商品失败");
        }
        sqlite3_finalize(stmt);
    } else {
        send_error(c, 500, "数据库错误");
    }
    
    cJSON_Delete(product_json);
}

// 删除商品
// void handle_delete_product(struct mg_connection *c, const char *query_string) {
//     char *id_str = get_query_param(query_string, "id");
void handle_delete_product(struct mg_connection *c, struct mg_str query_str) {
    char *id_str = get_query_param(query_str, "id");
    if (!id_str) {
        send_error(c, 400, "缺少id参数");
        return;
    }
    
    int id = atoi(id_str);
    free(id_str);
    
    const char *sql = "DELETE FROM products WHERE id = ?";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            if (sqlite3_changes(db) > 0) {
                cJSON *response = cJSON_CreateObject();
                cJSON_AddNumberToObject(response, "code", 200);
                cJSON_AddStringToObject(response, "message", "商品删除成功");
                
                char *json_str = cJSON_Print(response);
                send_response(c, 200, json_str);
                free(json_str);
                cJSON_Delete(response);
            } else {
                send_error(c, 404, "商品未找到");
            }
        } else {
            send_error(c, 500, "删除商品失败");
        }
        sqlite3_finalize(stmt);
    } else {
        send_error(c, 500, "数据库错误");
    }
}

// HTTP请求处理函数
// 使用mg_str的buf和len字段，或使用辅助函数
// void event_handler(struct mg_connection *c, int ev, void *ev_data) {
void event_handler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        
        // URI匹配：mg_http_match_uri仍然可用
        if (mg_http_match_uri(hm, "/api/product/list")) {
        // if (mg_strcmp(hm->uri, mg_str("/api/product/list")) == 0) {
            handle_get_products(c);
        } else if (mg_http_match_uri(hm, "/api/product/get")) {
            // 使用hm->query（mg_str类型）而不是hm->query.ptr
            handle_get_product(c, hm->query);  // 修改handle_get_product函数以接受mg_str
        } else if (mg_http_match_uri(hm, "/api/product/create")) {
            // 比较方法：使用mg_strcasecmp比较mg_str
            if (mg_strcasecmp(hm->method, mg_str("POST")) == 0) {
                // 复制body内容到C字符串
                char *body = mg_strndup(hm->body.buf, hm->body.len);
                handle_create_product(c, body);
                free(body);
            } else {
                send_error(c, 405, "方法不允许");
            }
        } else if (mg_http_match_uri(hm, "/api/product/update")) {
            if (mg_strcasecmp(hm->method, mg_str("POST")) == 0) {
                char *body = mg_strndup(hm->body.buf, hm->body.len);
                handle_update_product(c, body);
                free(body);
            } else {
                send_error(c, 405, "方法不允许");
            }
        } else if (mg_http_match_uri(hm, "/api/product/delete")) {
            handle_delete_product(c, hm->query);  // 修改handle_delete_product函数以接受mg_str
        } else {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", 
                         "{\"code\":200,\"message\":\"商品管理API服务器运行正常\"}");
        }
    }
}

int main() {
    // 初始化数据库
    if (init_database() != 0) {
        return 1;
    }
    
    // 创建Mongoose管理器和HTTP监听器
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    
    printf("启动HTTP服务器，端口 %s\n", PORT);
    mg_http_listen(&mgr, "http://0.0.0.0:" PORT, event_handler, &mgr);
    
    // 事件循环
    while (1) {
        mg_mgr_poll(&mgr, 1000);
    }
    
    // 清理资源
    mg_mgr_free(&mgr);
    sqlite3_close(db);
    
    return 0;
}