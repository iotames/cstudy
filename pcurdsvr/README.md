## Prompt

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
4. 一定要注意版本问题。代码中哪些符号引用了哪个版本的第三方库，必须说明。

具体功能要求：
1. 每个函数的实现必须完整，项目要完整可用。
2. 要能解析POST提交的json商品数据。
3. 商品详情查询接口示例：/api/product/get?id=3。商品列表查询：/api/product/list。新增：/api/product/create。更新：/api/product/update。删除：/api/product/delete
4. 做完上述步骤完成基础架构后，再做代码修改：使用sqlite和SQL语句实现数据的持久化存储。


## 依赖下载


```bash
git clone https://github.com/DaveGamble/cJSON.git
git clone https://github.com/cesanta/mongoose.git
mkdir pcurdsvr
cp cJSON/cJSON.h cJSON/cJSON.c pcurdsvr/
cp mongoose/mongoose.h mongoose/mongoose.c pcurdsvr/
sudo apt install libsqlite3-dev
```

## 项目结构

初版：

```bash
product_http_server/
├── mongoose.c          # HTTP服务器库
├── mongoose.h
├── cJSON.c             # JSON解析库
├── cJSON.h
├── product_server.c    # 主程序
├── Makefile
└── products.db         # SQLite数据库（自动创建）
```
