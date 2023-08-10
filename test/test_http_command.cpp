#include <httplib.h>
#include <json/json.h>

#include <iostream>

void print_usage(const char* name) {
  std::cout << "\nUsage: " << name << " [options]" << std::endl
            << "Options:" << std::endl
            << "  -m, --method       http request method \n"
            << "  -u, --url          http request URL \n"
            << "  -h, --help         print this help info \n"
            << "  -p, --param        Post method "
               "param order:abilityName,IPCPort,cmd,connectPort,connectIP \n"
            << "\n"
            << std::endl;
}

int main(int argc, char** argv) {
  std::string method = "";
  std::string url = "";
  httplib::Params params;
  if (argc < 2) {
    print_usage(argv[0]);
    return 1;
  }
  for (int i = 1; i < argc; i++) {
    std::string t_arg = std::string(argv[i]);
    if (t_arg == "-h" || t_arg == "--help") {
      print_usage(argv[0]);
      return 0;
    } else if (t_arg == "-m" || t_arg == "--method") {
      ++i;
      std::string t_method = argv[i];
      if (t_method != "POST" && t_method != "GET") {
        std::cout << "method must be POST or GET" << std::endl;
        return 1;
      }
      method = t_method;
    } else if (t_arg == "-u" || t_arg == "--url") {
      ++i;
      url = argv[i];
      if (url == "") {
        std::cout << "url error" << std::endl;
        return 1;
      }
    } else if (t_arg == "-p" || t_arg == "--param") {
      if (method == "POST") {
        if (argc <= i + 5) {
          std::cout << "param is not enough" << std::endl;
          return 1;
        }
        params.emplace("abilityName", argv[i + 1]);
        params.emplace("IPCPort", argv[i + 2]);
        params.emplace("cmd", argv[i + 3]);
        params.emplace("connectPort", argv[i + 4]);
        params.emplace("connectIP", argv[i + 5]);
        i = i + 5;
      } else {
        std::cout << "please make sure the method is POST and put the --param "
                     "at the end";
        return 1;
      }
    } else {
      printf("Option %s is not available \n", argv[i]);
      return 1;
    }
  }
  httplib::Client ccc("localhost", 8080);
  if (method == "POST") {
    auto res = ccc.Post(url, params);
    if (res) {
      std::cout << res->body << std::endl;
    }
  } else if (method == "GET") {
    auto res2 = ccc.Get(url);
    if (res2 && res2->status == 200) {
      // 响应成功，解析 JSON
      std::cout << res2->body.c_str() << std::endl;
    }
  }
}
