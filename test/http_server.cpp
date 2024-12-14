#include <httplib.h>

int main() {
  std::cout << "启动一个httpserver" << std::endl;
  httplib::Server server;
  server.Put("/ability/abilityHeartbeat", [](const httplib::Request &req,
                                                 httplib::Response &res) {
    std::cout << req.body << std::endl;
    res.status = 200;
    res.set_content("OK", "text/plain");
  });
  server.listen("0.0.0.0" , 8080);
}
