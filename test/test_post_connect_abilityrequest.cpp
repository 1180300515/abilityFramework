#include <iostream>

#include <httplib.h>


int main(int argc, char **argv)
{
    httplib::Client ccc("localhost", 8080);
    httplib::Params params;
    params.emplace("port","0");
    params.emplace("abilityName","cameratest");
    params.emplace("cmd","connect");
    params.emplace("connectPort","1");
    params.emplace("connectIP","ignore");
    auto res = ccc.Post("/api/AbilityRequest", params);
    if (res)
    {
        std::cout << res->body << std::endl;
    }
}