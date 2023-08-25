/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: test_ability_depend_tree.cpp
 * Description: test_ability_depend_tree.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include <dirent.h>
#include <json/json.h>
#include <string.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#define ABILITY_FILE_PATH "resource/resource_file/ability"

struct Depends {
  std::vector<std::string> abilities;
  std::vector<std::string> devices;
};

struct AbilityInfoExtract {
  std::string name;  // only include name , not include namespace
  Depends depends;
};

// class CameraHardwareForTest;
// class AudioHardwareForTest;
// class DisplayHardwareForTest;
// std::vector<AudioHardwareForTest> getAudioList(const std::string &type);
// std::vector<CameraHardwareForTest> getCameraInfo();
// std::vector<DisplayHardwareForTest> getDisplayHardwareForTest();

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

int main(int argc, char **argv) {
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
      std::string file_path =
          std::string(ABILITY_FILE_PATH) + "/" + iter;
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
  std::cout << result.ToJson().toStyledString() << std::endl;
}
