## Prompt

基本需求：
合理利用C语言生态的第三方组件库，做一个简单的HTTP服务器程序，以API接口的形式，提供商品的增删改查服务。

业务代码要求：
1. 完整实现每一个函数，项目要完整可用。绝对不能由于篇幅限制而只展示什么关键结构。
2. 要能解析POST提交的json商品数据。
3. 商品详情查询接口示例：GET /api/product/get?id=3。商品列表查询：GET /api/product/list。新增：POST /api/product/create。更新：POST /api/product/update。删除：POST /api/product/delete
4. 使用sqlite和SQL语句实现数据的持久化存储。

完整性要求【最重要】：
完整实现每一个函数，这是最重要的要求。如果由于篇幅限制无法实现，就直接说做不了，不用写了。不要长篇大论，然后缺胳膊断腿的。

数据格式：
业务数据的展示和交互使用JSON格式。

输出要求：
详细列出项目的每个依赖，要包含完整的构建教程，Makefile和源代码的注释要详尽。代码和构建都要符合工程化标准。

工程化要求：
1. 按项目工程化的实践标准，划分多个目录，拆分多个文件。各功能代码，按函数和文件拆分，明确职责边界，充分解耦。
2. 多处代码的共同部分，要抽离出来封装成函数。
3. 不论是C语言标准库还是第三方库，必须说明版本，还有是否使用某些扩展，否则经常符号找不到。
4. 代码中哪些符号是第三方库的引用，必须使用注释加以说明，必须带版本号。

第三方库要求：
1. 涉及的所有第三方库的选择，要有候选库对比，第三方库的资源链接或项目主页。
2. 最终选择的第三方库，要有获选理由和下载教程，使用教程。
3. 第三方库的获取方式：首选从官方URL或项目主页的源码文件获取，其次为官方编译好的链接库，最后是从apt install之类的命令行工具获取。
4. 第三方库必须明确注明版本，避免下载的版本和项目引用的版本不一致。


## 项目结构

初版：

```bash
pcurdsvr/
├── mongoose.c          # HTTP服务器库
├── mongoose.h
├── cJSON.c             # JSON解析库
├── cJSON.h
├── product_server.c    # 主程序
├── Makefile
└── products.db         # SQLite数据库（自动创建）
```

第二版：

```bash
pcurdsvr/
├── src/
│   ├── main.c
│   ├── http_server.c
│   ├── http_server.h
│   ├── database.c
│   ├── database.h
│   ├── product_handler.c
│   ├── product_handler.h
│   └── cJSON.c
├── include/
│   ├── cJSON.h
│   ├── mongoose.h
│   └── sqlite3.h
├── build/
├── Makefile
└── README.md
```


## 依赖下载

### SQLite

- https://www.sqlite.org/download.html

1. https://www.sqlite.org/2025/sqlite-src-3510000.zip 13.54M 完整原始版本。这是发布时受版本控制的所有代码的快照。所有其他源码包都从此包派生出去。当前版本：`3.51.0` 
2. https://www.sqlite.org/2025/sqlite-amalgamation-3510000.zip 2.74M 合并压缩版本。超过 100 个单独的源文件被连接成一个名为 `sqlite3.c` 的 C 代码大文件。
3. https://www.sqlite.org/2025/sqlite-doc-3510000.zip 10.91 MB 文档作为静态 HTML 文件的捆绑包。

```bash
# sudo apt install libsqlite3-dev
wget -c https://www.sqlite.org/2025/sqlite-amalgamation-3510000.zip
unzip sqlite-amalgamation-3510000.zip
cp sqlite-amalgamation-3510000/sqlite3.c src/
cp sqlite-amalgamation-3510000/sqlite3.h include/
```

### Mongoose

```bash
# git clone https://github.com/cesanta/mongoose.git
# cp mongoose/mongoose.h mongoose/mongoose.c pcurdsvr/
wget -c https://github.com/cesanta/mongoose/archive/refs/tags/7.13.tar.gz
tar -xzf 7.13.tar.gz
cp mongoose-7.13/mongoose.c src/
cp mongoose-7.13/mongoose.h include/
```

### cJSON

```bash
# git clone https://github.com/DaveGamble/cJSON.git
# cp cJSON/cJSON.h cJSON/cJSON.c pcurdsvr/
wget -c https://github.com/DaveGamble/cJSON/archive/refs/tags/v1.7.17.tar.gz
tar -xzf v1.7.17.tar.gz
cp cJSON-1.7.17/cJSON.c src/
cp cJSON-1.7.17/cJSON.h include/
```


## 构建

```bash
# 编译程序
make

# 运行编译好的程序。相当于执行: ./build/pcurdsvr
make run

# 清理所有的编译对象文件
make clean
```


## API测试

```bash
# 创建商品
curl -X POST http://localhost:8000/api/product/create \
  -H "Content-Type: application/json" \
  -d '{"name":"iPhone 16","price":5999.99,"description":"The New iPhone"}'

# 查询商品列表
curl http://localhost:8000/api/product/list

# 查询单个商品
curl "http://localhost:8000/api/product/get?id=1"

# 更新商品
curl -X POST http://localhost:8000/api/product/update \
  -H "Content-Type: application/json" \
  -d '{"id":1,"name":"My HarmonyOS Phone","price":7999.99,"description":"The HUAWEI HarmonyOS phone"}'

# 删除商品
curl -X POST http://localhost:8000/api/product/delete \
  -H "Content-Type: application/json" \
  -d '{"id":1}'
```