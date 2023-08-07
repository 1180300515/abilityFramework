#include <iostream>

#include <httplib.h>

#include "json/json.h"

int main(int argc, char **argv)
{
    if (argc != 6)
    {
        std::cout << "缺少参数" << std::endl;
        return 0;
    }

    httplib::Client ccc("localhost", 8080);

    // Json::Value data;
    // data["abilityName"] = argv[1];
    // data["IPCPort"] = std::stoi(argv[2]);
    // data["cmd"] = argv[3];
    // data["connectPort"] = std::stoi(argv[4]);
    // data["connectIP"] = argv[5];
    // Json::FastWriter writer;
    // auto res = ccc.Post("/api/AbilityRequest", writer.write(data), "application/json");
    // if (res)
    // {
    //     std::cout << res->body << std::endl;
    // }
    httplib::Params params;
    params.emplace("abilityName", argv[1]);
    params.emplace("IPCPort", argv[2]);
    params.emplace("cmd", argv[3]);
    params.emplace("connectPort", argv[4]);
    params.emplace("connectIP", argv[5]);
    auto res = ccc.Post("/api/AbilityRequest", params);
    if (res)
    {
        std::cout << res->body << std::endl;
    }
}