#include <httplib.h>

int main(int argc, char **argv)
{

    httplib::Client ccc("localhost", 8080);
    auto res = ccc.Get("/api/Devices");

    if (res && res->status == 200)
    {
        // 响应成功，解析 JSON
        std::cout << res->body.c_str() << std::endl;
    }
}