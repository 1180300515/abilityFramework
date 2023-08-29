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
