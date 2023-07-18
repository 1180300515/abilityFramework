#ifndef DEPEND_TREE_ARRAY_H
#define DEPEND_TREE_ARRAY_H

#include "json/json.h"

#include "ability_instance_info_extract.h"

struct TreeNode {
    AbilityInfoExtract ability;
    std::vector<TreeNode> children;
    int level;

    Json::Value ToJson() const;
};

class DependTreeArray {
public:
    std::vector<TreeNode> trees;

    bool hasAbility(const std::string& abilityName) const;
    Json::Value ToJson() const;
};

#endif //DEPEND_TREE_ARRAY_H