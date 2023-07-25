#include <iostream>

#include <httplib.h>


int main(int argc, char **argv)
{
    httplib::Client ccc("localhost", 8080);
    httplib::Params params;
    params.emplace("port","0");
    params.emplace("abilityName","cameratest");
    params.emplace("cmd","start");
    params.emplace("connectPort","0");
    params.emplace("connectIP","ignore");
    auto res = ccc.Post("/api/AbilityRequest", params);
    if (res)
    {
        std::cout << "success" << std::endl;
    } 
    else
    {
        std::cout << res.error() << std::endl;
    }
}