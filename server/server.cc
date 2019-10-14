#include "include/json/json.h"
#include "include/httplib.h"
int main()
{
    using namespace httplib;
    Server server;
    server.Get("/", [] (const Request& req, Response& resp) {
               //std::cout << req.method << "  " << req.version << "  " << req.target << req.body << std::endl;
               std::cout << req.body << std::endl;
               std::string html = "I am Server\r\n";
               resp.set_content(html, "text/html");
               });

    server.Post("/", [] (const Request& req, Response& resp) {
               //std::cout << req.version << req.method << req.target << req.body << std::endl;
               std::cout << req.body << std::endl;
               std::string html = "I am Server\r\n";
               resp.set_content(html, "text/html");
               });
    server.set_base_dir("./template");
    server.listen("0.0.0.0", 9527);
    return 0;
}
