#include <iostream>

#include <httplib.h>


int main(int argc, char **argv)
{
    if (argc != 6)
    {
        std::cout << "缺少参数" << std::endl;
        return 0;
    }

    httplib::Client ccc("localhost", 8080);
    httplib::Params params;

    
    params.emplace("abilityName",argv[1]);
    params.emplace("IPCPort",argv[2]);
    params.emplace("cmd",argv[3]);
    params.emplace("connectPort",argv[4]);
    params.emplace("connectIP",argv[5]);
    auto res = ccc.Post("/api/AbilityRequest", params);
    if (res)
    {
        std::cout << res->body << std::endl;
    }
}