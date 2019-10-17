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
               //发送三个数据 mac temperature heartrate   
               std::cout << "Post" << std::endl;
               std::cout << req.body << std::endl;
               //把数据给存储到本地
               std::unordered_map<std::string, std::string> body_kv;
               UrlUtil::ParseBody(req.body, body_kv);
               std::cout << "body_kv" << std::endl;
               for (auto e : body_kv)
               {
                    std::cout << e.first << e.second << std::endl;
               }

               const std::string mac_s = body_kv["mac"];
               const std::string temperature_s = body_kv["temperature"];
               const std::string heartrate_s = body_kv["heartrate"];
                
               bool writebool = FileUtil::Write("./tmp_data/" + mac_s + ".info", temperature_s + " " + heartrate_s + "\r\n"); 
               std::cout << "Write : " << writebool << std::endl;

               std::string html = "Server Recived Successfully\r\n";
               resp.set_content(html, "text/html");
               });
    server.set_base_dir("./template");
    server.listen("0.0.0.0", 9527);
    return 0;
}
