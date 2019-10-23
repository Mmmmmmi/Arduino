#include "include/ctemplate/template.h"
#include "include/json/json.h"
#include "include/httplib.h"
#include "util.hpp"
int main()
{
    using namespace httplib;
    Server server;
    server.Get("/all_info", [] (const Request& req, Response& resp) {

               //浏览器打开
               //std::cout << req.method << "  " << req.version << "  " << req.target << req.body << std::endl;
               //std::cout << req.body << std::endl;

               std::cout << "Get" << std::endl;

               //加载信息
               std::string all_infopath = "./tmp_data/mac_addr_info.txt"; //mac 地址信息
               std::ifstream all_infofile(all_infopath);
               std::string macaddr;
               std::getline(all_infofile, macaddr);
               while(std::getline(all_infofile, macaddr))
               {

               std::string infopath = "./tmp_data/" + macaddr + ".info";
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
                   //std::cout << tmp.size() << std::endl;
                   //std::cout << tmp[0] << std::endl << tmp[1] << std::endl;
                   table_dict->SetValue("temperature", tmp[0]);
                   table_dict->SetValue("heartrate", tmp[1]);
               }
               std::string html;
               ctemplate::Template* tpl;
               tpl = ctemplate::Template::GetTemplate("./template/all_info.html", ctemplate::DO_NOT_STRIP);
               tpl->Expand(&html, &dict);
               resp.set_content(html, "text/html");
               }
    });

    server.Post("/", [] (const Request& req, Response& resp) {
                //下位机发送
                //发送三个数据 mac temperature heartrate   
                std::cout << "Post" << std::endl;
                std::vector<std::string> data; 
                StringUtil::Split(req.body, "\r\n", data);
                const std::string mac_s = data[0].substr(data[0].find(":") + 1);
                const std::string temperature_s = data[1].substr(data[1].find(":")  + 1);
                const std::string heartrate_s = data[2].substr(data[2].find(":") + 1);
                const std::string weight_s = data[3].substr(data[3].find(":") + 1);
                const std::string alcohol_s = data[4].substr(data[4].find(":") + 1);
                const std::string time_s = data[5].substr(data[5].find(":") + 1);
                std::cout << "mac:" << mac_s << std::endl << "temperature:" << temperature_s << std::endl << "heartrate:" << heartrate_s << std::endl << "weight:" << weight_s << std::endl << "alcohol:" << alcohol_s << std::endl << "time:" << time_s << std::endl;;

                std::string all_infopath = "./tmp_data/mac_addr_info.txt"; //将 mac 地址存入
                std::string infopath = "./tmp_data/" + mac_s + ".info"; //写具体数据
                if (!FileUtil::CheckFile(infopath))
                {
                    FileUtil::ADDWrite(all_infopath, mac_s + "\n");
                    std::cout << "New Mac Add" << std::endl; 
                }
                FileUtil::ADDWrite(infopath, temperature_s + "\t" + heartrate_s + "\n");


                std::string html = "Server Recived Successfully\r\n";
                resp.set_content(html, "text/html");
                });
    server.set_base_dir("./template");
    server.listen("0.0.0.0", 9527);
    return 0;
}
