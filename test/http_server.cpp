#include <httplib.h>

//int main() {
//  std::cout << "启动一个httpserver" << std::endl;
//  httplib::Server server;
//  server.Put("/ability/abilityHeartbeat", [](const httplib::Request &req,
//                                                 httplib::Response &res) {
//    std::cout << req.body << std::endl;
//    res.status = 200;
//    res.set_content("OK", "text/plain");
//  });
//  server.listen("0.0.0.0" , 8080);
//}
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>

std::vector<std::map<std::string, std::string>> CartesianProduct(
    const std::unordered_map<std::string, std::vector<std::string>>& child_record) {
  std::vector<std::map<std::string, std::string>> result;

  for (const auto& [key, values] : child_record) {
    if (result.empty()) {
      for (const auto& value : values) {
        std::map<std::string, std::string> new_map;
        new_map[key] = value;
        result.push_back(std::move(new_map));
      }
    } else {
      std::vector<std::map<std::string, std::string>> new_result;
      for (auto& map : result) {
        for (const auto& value : values) {
          map[key] = value;
          new_result.push_back(map);
        }
      }
      result = std::move(new_result);
    }
  }

  return result;
}

int main() {
  std::unordered_map<std::string, std::vector<std::string>> child_record = {
      {"color", {"red", "green", "blue"}},
      {"size", {"small", "large"}}
  };

  auto result = CartesianProduct(child_record);

  for (const auto& map : result) {
    for (const auto& [key, value] : map) {
      std::cout << key << ": " << value << " ";
    }
    std::cout << std::endl;
  }

  return 0;
}