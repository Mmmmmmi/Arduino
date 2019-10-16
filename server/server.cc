#include "include/json/json.h"
#include "include/httplib.h"
#include "util.hpp"
int main()
{
    using namespace httplib;
    Server server;
    server.Get("/", [] (const Request& req, Response& resp) {
               //浏览器打开
               //std::cout << req.method << "  " << req.version << "  " << req.target << req.body << std::endl;
               //std::cout << req.body << std::endl;
               std::cout << "Get" << std::endl;
               std::string html = "I am Server\r\n";
               resp.set_content(html, "text/html");
               });

    server.Post("/", [] (const Request& req, Response& resp) {
               //下位机发送
               //std::cout << req.version << req.method << req.target << req.body << std::endl;
               std::cout << "Post" << std::endl;
               //std::cout << req.body << std::endl;
               std::string html = "Server Recived Successfully\r\n";
               //把数据给存储到本地
               std::vector<std::string> data;
               StringUtil::Split(req.body, "\r\n", data);
               
               for (auto e : data)
               {
                    std::cout << e << std::endl;
               }

               resp.set_content(html, "text/html");
               });
    server.set_base_dir("./template");
    server.listen("0.0.0.0", 9527);
    return 0;
}
