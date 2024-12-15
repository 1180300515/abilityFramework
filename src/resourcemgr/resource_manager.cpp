#include "resource_manager.h"

#include <sys/socket.h>
#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <json/json.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

void resourceManager::Init() {
  fs::path directoryPath = "resource/ability";
  if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
    DLOG(ERROR) << "Directory does not exist: " << directoryPath << std::endl;
    exit(-1);
  }
  // 遍历目录下的所有文件
  for (const auto &entry : fs::directory_iterator(directoryPath)) {
    if (entry.is_regular_file()) {
      const auto &filePath = entry.path();
      if (filePath.extension() == ".yaml" || filePath.extension() == ".yml") {
        parseYamlFile(filePath);
      }
    }
  }
}
void resourceManager::parseYamlFile(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    DLOG(ERROR) << "Failed to open file: " << filePath;
    return;
  }
  YAML::Node config = YAML::Load(file);
  if (config) {
    Ability ability;
    // 获取根节点
    ability.abilityName = config["abilityName"].as<std::string>();
    ability.type = config["type"].as<std::string>();
    auto location = config["location"];
    auto control = config["control"];
    // 解析依赖
    ability.location.scene = location["scene"].as<std::string>();
    ability.location.x = location["x"].as<float>();
    ability.location.y = location["y"].as<float>();
    ability.location.z = location["z"].as<float>();
    if (config["depend"]) {
      for (const auto& dependItem : config["depend"]) {
        ability.abilityDepend.emplace_back(dependItem.as<std::string>());
      }
    }
    // 不读control
    ability_list.emplace(ability.abilityName , ability);
    DLOG(INFO) << "读取成功: " << filePath;
  } else {
    DLOG(ERROR) << "Failed to parse YAML file: " << filePath;
  }
}

void resourceManager::Run() {
  loop_thread = std::thread([this](){
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket < 0) {
      DLOG(ERROR) << "Cannot open socket" << std::endl;
      exit(-1);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8082); // 监听8082端口
    server_addr.sin_addr.s_addr = INADDR_ANY; // 监听所有网络接口
    if (bind(udp_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
      DLOG(ERROR) << "Bind failed" << std::endl;
      close(udp_socket);
      exit(-1);
    }
    const size_t MAX_BUF_SIZE = 1024;
    char buffer[MAX_BUF_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    while (true) {
      ssize_t bytes_received = recvfrom(udp_socket, buffer, MAX_BUF_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
      if (bytes_received < 0) {
        DLOG(ERROR) << "Recvfrom failed" << std::endl;
        continue;
      }

      buffer[bytes_received] = '\0'; // 确保字符串以空字符结尾
      std::string payload(buffer);
      DLOG(INFO) << "收到位置更新数据" << payload << std::endl;

      // 解析JSON消息
      Json::Reader reader;
      Json::Value root;
      if (!reader.parse(payload, root)) {
        DLOG(ERROR) << "JSON Parsing error" << std::endl;
        continue;
      }
      this->changeLocation(root);
    }
  });
}

void resourceManager::changeLocation(const Json::Value &value) {
  std::unique_lock<std::shared_mutex> lock(lock_list);
  for (const Json::Value& obj : value) {
    std::string name = obj["name"].asString();
    float x = obj["x"].asFloat(); // 使用asFloat获取float值
    float y = obj["y"].asFloat(); // 使用asFloat获取float值
    float z = obj["z"].asFloat(); // 使用asFloat获取float值
    std::string scene = obj["scene"].asString();
    auto it = ability_list.find(name);
    if (it == ability_list.end()) {
      continue;
    }
    it->second.location.x = x;
    it->second.location.y = y;
    it->second.location.z = z;
    it->second.location.scene = scene;
  }
}

std::unordered_map<std::string, Ability> resourceManager::getAbilityList() {
  std::shared_lock<std::shared_mutex> lock(lock_list);
  return ability_list;
}

Ability::locationDef resourceManager::getLocation(const std::string &abilityName) {
    std::shared_lock<std::shared_mutex> lock(lock_list);
    auto it = ability_list.find(abilityName);
    if (it == ability_list.end()) {
        DLOG(ERROR) << "能力不存在: " << abilityName;
        return {};
    }
    return it->second.location;
}
