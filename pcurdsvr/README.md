## Prompt

完整性要求：这是最重要的要求。完整实现每一个函数，如果由于篇幅限制无法实现，就直接说做不了，不用写了。不要长篇大论，然后缺胳膊断腿的。

基本需求：合理利用C语言生态的第三方组件库，做一个简单的HTTP服务器程序，以API接口的形式，提供商品的增删改查服务。

数据格式：业务数据的展示和交互使用JSON格式。

输出要求：必须包含完整的构建教程，代码的注释要详尽。代码和构建都要符合工程化标准。

项目结构和代码质量要求：
1. 按项目工程化的实践标准，划分多个目录，拆分多个文件。各功能代码，按函数和文件进行拆分，明确职责边界，充分解耦。
2. 不论是C语言标准库还是第三方库，必须说明版本，还有是否使用某些扩展，否则经常符号找不到。

第三方库要求：
1. 涉及的所有第三方库的选择，要有候选库对比，第三方库的资源链接或项目主页。
2. 最终选择的第三方库，要有获选理由和下载教程，使用教程。
3. 第三方库的获取方式：首选从官方URL或项目主页的源码文件获取，其次为官方编译好的链接库，最后是从apt install之类的命令行工具获取。
4. 第三方库必须明确注明版本，避免下载的版本和项目引用的版本不一致。

代码要求：
1. 完整实现每一个函数，项目要完整可用。绝对不能由于篇幅限制而只展示什么关键结构。
2. 要能解析POST提交的json商品数据。
3. 商品详情查询接口示例：/api/product/get?id=3。商品列表查询：/api/product/list。新增：/api/product/create。更新：/api/product/update。删除：/api/product/delete
4. 使用sqlite和SQL语句实现数据的持久化存储。
5. 代码中哪些符号是第三方库的引用，必须使用注释加以说明，必须带版本号。


## 依赖下载


```bash
# git clone https://github.com/DaveGamble/cJSON.git
# git clone https://github.com/cesanta/mongoose.git
# mkdir pcurdsvr
# cp cJSON/cJSON.h cJSON/cJSON.c pcurdsvr/
# cp mongoose/mongoose.h mongoose/mongoose.c pcurdsvr/
# sudo apt install libsqlite3-dev

wget https://github.com/cesanta/mongoose/archive/refs/tags/7.13.tar.gz
tar -xzf 7.13.tar.gz
cp mongoose-7.13/mongoose.c src/
cp mongoose-7.13/mongoose.h include/

wget https://github.com/DaveGamble/cJSON/archive/refs/tags/v1.7.17.tar.gz
tar -xzf v1.7.17.tar.gz
cp cJSON-1.7.17/cJSON.c src/
cp cJSON-1.7.17/cJSON.h include/

wget https://www.sqlite.org/2024/sqlite-amalgamation-3450000.zip
unzip sqlite-amalgamation-3450000.zip
cp sqlite-amalgamation-3450000/sqlite3.c src/
cp sqlite-amalgamation-3450000/sqlite3.h include/
```

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
