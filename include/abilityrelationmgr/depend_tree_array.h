/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: depend_tree_array.h
 * Description: depend_tree_array.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef ABILITYRELATIONMGR_DEPEND_TREE_ARRAY_H_
#define ABILITYRELATIONMGR_DEPEND_TREE_ARRAY_H_

#include <json/json.h>

#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <glog/logging.h>

#include "abilityrelationmgr/ability_instance_info_extract.h"

class AbilityNode
{
public:
    AbilityNode(std::string name, int level)
        : ability_name_(name), level_(level){};
    AbilityNode() = default;

    std::vector<std::string> toVector();

    std::vector<AbilityNode *> ability_children_;
    std::vector<std::vector<std::string>> depend_devices_;

private:
    std::vector<std::vector<std::string>>
    cartesianProduct(const std::vector<std::vector<std::string>> &lists);
    std::string ability_name_;
    int level_;
};

struct TreeNode {
    AbilityInfoExtract ability;
    std::vector<TreeNode> children;
    int level;
    unsigned long abilityId;

    Json::Value ToJson() const;
    void generateHashId();
};

class DependTreeArray {
   public:
    std::vector<TreeNode> trees;

    bool hasAbility(const std::string &abilityName) const;
    bool hasAbilityInstance(unsigned long abilityId);
    Json::Value ToJson() const;
};

#endif  // ABILITYRELATIONMGR_DEPEND_TREE_ARRAY_H_
