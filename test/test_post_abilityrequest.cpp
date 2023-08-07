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
    httplib::Params params;
    Json::Value data;

    data["abilityName"] = argv[1];
    data["IPCPort"] = std::stoi(argv[2]);
    data["cmd"] = argv[3];
    data["connectPort"] = std::stoi(argv[4]);
    data["connectIP"] = argv[5];
    Json::FastWriter writer;
    auto res = ccc.Post("/api/AbilityRequest", writer.write(data), "application/json");
    if (res)
    {
        std::cout << res->body << std::endl;
    }
}