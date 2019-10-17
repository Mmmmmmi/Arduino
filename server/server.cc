#include "include/ctemplate/template.h"
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

               //加载信息
               std::ifstream file(infopath);
               assert(file.is_open());
               ctemplate::TemplateDictionary dict("all_info");
               std::string line;
               std::vector<std::pair<std::string, std::string>> all_info;
               while(std::getline(file, line))
               {
                    ctemplate::TemplateDictionary* table_dict = dict.AddSectionDictionary("all_info");
                    std::vector<std::string> tmp;
                    StringUtil::Split(line, "\t" , tmp);
                    assert(tmp.size() == 2);
                    table_dict->SetValue("temperature", tmp[0]);
                    table_dict->SetValue("heartrate", tmp[1]);
               }
               ctemplate::Template* tpl;
               tpl = ctemplate::Template::GetTemplate("./template/all_info.html", ctemplate::DO_NOT_STRIP);
               tpl->Expand(&html, &dict);

               //std::string html = "I am Server\r\n";
               resp.set_content(html, "text/html");
    });

    server.Post("/", [] (const Request& req, Response& resp) {
                //下位机发送
                //发送三个数据 mac temperature heartrate   
                std::cout << "Post" << std::endl;
                std::cout << req.body << std::endl;
                //把数据给存储到本地
                /*
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
                   */
                std::vector<std::string> data; 
                StringUtil::Split(req.body, "\r\n", data);
                /*
                   for (auto e : data)
                   {
                   std::cout << e << std::endl;
                   }
                   */

                const std::string mac_s = data[0].substr(data[0].find(":") + 1);
                const std::string temperature_s = data[1].substr(data[1].find(":")  + 1);
                const std::string heartrate_s = data[2].substr(data[2].find(":") + 1);
                std::cout << mac_s << std::endl << temperature_s << std::endl << heartrate_s << std::endl;

                std::string infopath = "./tmp_data/" + mac_s + ".info";
                FileUtil::ADDWrite(infopath, temperature_s + "\t" + heartrate_s + "\n");


                std::string html = "Server Recived Successfully\r\n";
                resp.set_content(html, "text/html");
    });
    server.set_base_dir("./template");
    server.listen("0.0.0.0", 9527);
    return 0;
}
