#include <dirent.h>
#include <json/json.h>
#include <string.h>
#include <sys/stat.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "include/get_audio_hardware.h"
#include "include/get_camera_hardware.h"
#include "include/get_display_hardware.h"

#define ABILITY_FILE_PATH "resource/resource_file/ability"

struct Depends {
  std::vector<std::string> abilities;
  std::vector<std::string> devices;
};

struct AbilityInfoExtract {
  std::string name;  // only include name , not include namespace
  Depends depends;
};

std::vector<CameraHardwareForTest> camera;
std::vector<AudioHardwareForTest> mic;
std::vector<AudioHardwareForTest> speaker;
std::vector<DisplayHardwareForTest> display;
char hostname[256] = {};

struct TreeNode {
  AbilityInfoExtract ability;
  std::vector<TreeNode> children;
  int level;

  Json::Value ToJson() const {
    Json::Value jsonNode;
    jsonNode["name"] = ability.name;
    jsonNode["level"] = level;
    for (const auto &ability : ability.depends.abilities) {
      jsonNode["depends"]["abilities"].append(ability);
    }
    for (const auto &device : ability.depends.devices) {
      jsonNode["depends"]["devices"].append(device);
    }
    for (const auto &child : children) {
      jsonNode["children"].append(child.ToJson());
    }
    return jsonNode;
  }
};

class DependTreeArray {
 public:
  std::vector<TreeNode> trees;

  bool hasAbility(const std::string &abilityName) const {
    for (auto &tree : trees) {
      if (tree.ability.name == abilityName) {
        return true;
      }
    }
    return false;
  }
  Json::Value ToJson() const {
    Json::Value jsonTreeArray;
    for (const auto &tree : trees) {
      jsonTreeArray.append(tree.ToJson());
    }
    return jsonTreeArray;
  }
};

DependTreeArray generateDependTreeArray(
    const std::vector<AbilityInfoExtract> &list) {
  std::unordered_map<std::string, TreeNode> map;

  for (const auto &ability : list) {
    TreeNode node{ability, {}, 0};
    map[ability.name] = node;
  }

  for (const auto &ability : list) {
    for (const auto &depend : ability.depends.abilities) {
      if (map.count(depend) > 0 &&
          map[depend].children.at(0).ability.name != "none") {
        map[depend].children.push_back(map[ability.name]);
        map[ability.name].level = map[depend].level + 1;
      }
    }
  }

  DependTreeArray treeArray;

  for (const auto &pair : map) {
    if (pair.second.level == 0) {
      treeArray.trees.push_back(pair.second);
    }
  }

  return treeArray;
}

bool fileExists(const std::string &filename) {
  struct stat buffer;
  return (stat(("bin/" + filename).c_str(), &buffer) == 0);
}

std::vector<std::string> get_hardware_string_list(const std::string &type) {
  std::vector<std::string> result;
  if (type == "mic") {
    for (const auto &iter : mic) {
      result.emplace_back(iter.ToKeyValue(std::string(hostname)));
    }
    return result;
  } else if (type == "speaker") {
    for (const auto &iter : speaker) {
      result.emplace_back(iter.ToKeyValue(std::string(hostname)));
    }
    return result;
  } else if (type == "display") {
    for (const auto &iter : display) {
      result.emplace_back(iter.ToKeyValue(std::string(hostname)));
    }
    return result;
  } else if (type == "camera") {
    for (const auto &iter : camera) {
      result.emplace_back(iter.ToKeyValue(std::string(hostname)));
    }
    return result;
  }
  return result;
}

std::vector<std::vector<std::string>> cartesianProduct(
    const std::vector<std::vector<std::string>> &lists) {
  std::vector<std::vector<std::string>> result;
  std::vector<std::string> combination(lists.size());

  size_t counter = 0;
  while (true) {
    for (size_t i = 0; i < lists.size(); ++i) {
      combination[i] = (lists[i])[counter % lists[i].size()];
    }

    result.push_back(combination);

    ++counter;
    for (size_t i = 0; i < lists.size(); ++i) {
      if (counter % lists[i].size() != 0) {
        break;
      } else if (i == lists.size() - 1) {
        return result;
      }
    }
  }
}

void generateNodes(const TreeNode &node, DependTreeArray *treeArray) {
  if (!node.ability.depends.devices.empty()) {
    std::vector<std::vector<std::string>> deviceLists;

    for (const auto &device : node.ability.depends.devices) {
      auto deviceList = get_hardware_string_list(device);

      if (!deviceList.empty()) {
        deviceLists.push_back(deviceList);
      } else {
        return;  // If any required device list is empty, don't create any
                 // instance.
      }
    }

    // Generate all combinations of devices.
    std::vector<std::vector<std::string>> combinations =
        cartesianProduct(deviceLists);
    for (const auto &combination : combinations) {
      if (!fileExists(node.ability.name)) {
        std::cout << "File " << node.ability.name << " does not exist.\n";
        return;
      }
      TreeNode newNode{node.ability, node.children, node.level};
      newNode.ability.depends.devices =
          combination;  // Store the devices into the ability instance.
      treeArray->trees.push_back(newNode);
    }
  }

  for (auto &child : node.children) {
    generateNodes(child, treeArray);
  }
}

DependTreeArray generateDependTreeArrayWithDevices(
    const DependTreeArray &treeArray) {
  DependTreeArray newTreeArray;

  for (auto &tree : treeArray.trees) {
    generateNodes(tree, &newTreeArray);
  }

  return newTreeArray;
}

int main(int argc, char **argv) {
  if (gethostname(hostname, sizeof(hostname)) != 0) {
    return 1;
  } else {
    hostname[sizeof(hostname) - 1] = '\0';
  }
  mic = getAudioList("record");
  speaker = getAudioList("playback");
  camera = getCameraInfo();
  display = getDisplayInfo();
  std::vector<std::string> ability_file_names;
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(ABILITY_FILE_PATH)) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
        continue;
      }
      ability_file_names.emplace_back(ent->d_name);
    }
    closedir(dir);
  } else {
    std::cout << "Could not open directory : " << ABILITY_FILE_PATH
              << std::endl;
  }
  std::vector<AbilityInfoExtract> list;
  for (const auto &iter : ability_file_names) {
    YAML::Node ability;
    try {
      std::string file_path = std::string(ABILITY_FILE_PATH) + "/" + iter;
      ability = YAML::LoadFile(file_path);
    } catch (YAML::ParserException e) {
      std::cout << "config yaml is malformed." << std::endl;
      exit(0);
    } catch (YAML::BadFile e) {
      std::cout << "file can't be load" << std::endl;
      exit(0);
    }
    AbilityInfoExtract new_info;
    new_info.name = ability["metadata"]["name"].as<std::string>();
    for (const auto &a : ability["depends"]["abilities"]) {
      new_info.depends.abilities.emplace_back(a.as<std::string>());
    }
    for (const auto &a : ability["depends"]["devices"]) {
      new_info.depends.devices.emplace_back(a.as<std::string>());
    }
    list.emplace_back(new_info);
  }

  auto result = generateDependTreeArray(list);
  auto treewithdevice = generateDependTreeArrayWithDevices(result);
  std::cout << treewithdevice.ToJson().toStyledString() << std::endl;
  return 0;
}
