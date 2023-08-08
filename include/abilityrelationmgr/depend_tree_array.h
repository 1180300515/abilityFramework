#ifndef ABILITYRELATIONMGR_DEPEND_TREE_ARRAY_H_
#define ABILITYRELATIONMGR_DEPEND_TREE_ARRAY_H_

#include <json/json.h>

#include <string>
#include <vector>

#include "abilityrelationmgr/ability_instance_info_extract.h"

struct TreeNode {
  AbilityInfoExtract ability;
  std::vector<TreeNode> children;
  int level;

  Json::Value ToJson() const;
};

class DependTreeArray {
 public:
  std::vector<TreeNode> trees;

  bool hasAbility(const std::string &abilityName) const;
  Json::Value ToJson() const;
};

#endif  // ABILITYRELATIONMGR_DEPEND_TREE_ARRAY_H_
