#ifndef ABILITY_RELATION_MANAGER_H
#define ABILITY_RELATION_MANAGER_H

#include <functional>

#include "depend_tree_array.h"

class AbilityRelationManager
{
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
    DependTreeArray generateDependTreeArray(std::vector<AbilityInfoExtract> &list);

    /**
     * @brief add device into the tree array
     * @param treeArray
     * @return
     */
    DependTreeArray generateDependTreeArrayWithDevices(DependTreeArray &treeArray);

    /**
     * @brief add device into a tree node
     * @param node
     * @param treeArray
     */
    void generateNodes(TreeNode &node, DependTreeArray &treeArray);

    /**
     * @brief 笛卡尔乘积
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

#endif // ABILITY_RELATION_MANAGER_H