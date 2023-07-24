#include <httplib.h>

int main(int argc, char **argv)
{
     httplib::Client ccc("localhost", 8080);
    auto res2 = ccc.Get("/api/AbilitySupport");

    if (res2 && res2->status == 200)
    {
        // 响应成功，解析 JSON
        std::cout << res2->body.c_str() << std::endl;
    }
}