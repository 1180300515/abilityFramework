/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: ability_relation_manager.h
 * Description: ability_relation_manager.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef ABILITYRELATIONMGR_ABILITY_RELATION_MANAGER_H_
#define ABILITYRELATIONMGR_ABILITY_RELATION_MANAGER_H_

#include <functional>
#include <string>
#include <vector>

#include "abilityrelationmgr/depend_tree_array.h"

class AbilityRelationManager {
   public:
    /**
     * @brief init the abilityraletion manager
     * @param callback
     */
    void Init(std::function<std::vector<AbilityInfoExtract>()> callback_1, std::function<std::vector<std::string>(std::string)> callback_2);
    /**
     * @brief get the ability support
     * @return result
     */
    std::string GetAbilitySupport();

   private:
    /**
     * @brief get the abilityinfo extract list
     */
    std::function<std::vector<AbilityInfoExtract>()> abilityinfoextract_callback_;

    /**
     * @brief get hardware device list
     */
    std::function<std::vector<std::string>(std::string)> hardwaredevicelist_callback_;

    /**
     * @brief base the ability info extract to generate tree array
     * @param list
     * @return
     */
    DependTreeArray generateDependTreeArray(const std::vector<AbilityInfoExtract> &list);

    /**
     * @brief add device into the tree array
     * @param treeArray
     * @return
     */
    DependTreeArray generateDependTreeArrayWithDevices(const DependTreeArray &treeArray);

    /**
     * @brief add device into a tree node
     * @param node
     * @param treeArray
     */
    void generateNodes(const TreeNode &node, DependTreeArray *treeArray);

    /**
     * @brief dikar
     * @param lists
     * @return
     */
    std::vector<std::vector<std::string>> cartesianProduct(const std::vector<std::vector<std::string>> &lists);

    /**
     * @brief judge the ability program is exist or not
     * @param filename
     * @return
     */
    bool fileExists(const std::string &filename);
};

#endif  // ABILITYRELATIONMGR_ABILITY_RELATION_MANAGER_H_
