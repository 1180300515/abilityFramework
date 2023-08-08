#include "abilityrelationmgr/depend_tree_array.h"

bool DependTreeArray::hasAbility(const std::string& abilityName) const {
  for (auto& tree : trees) {
    if (tree.ability.name == abilityName) {
      return true;
    }
  }
  return false;
}

Json::Value DependTreeArray::ToJson() const {
  Json::Value jsonTreeArray;
  for (const auto& tree : trees) {
    jsonTreeArray.append(tree.ToJson());
  }
  return jsonTreeArray;
}

Json::Value TreeNode::ToJson() const {
  Json::Value jsonNode;
  jsonNode["name"] = ability.name;
  jsonNode["level"] = level;
  for (const auto& ability : ability.depends.abilities) {
    jsonNode["depends"]["abilities"].append(ability);
  }
  for (const auto& device : ability.depends.devices) {
    jsonNode["depends"]["devices"].append(device);
  }
  for (const auto& child : children) {
    jsonNode["children"].append(child.ToJson());
  }
  return jsonNode;
}
