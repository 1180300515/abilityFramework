/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: ability_relation_manager.cpp
 * Description: ability_relation_manager.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "abilityrelationmgr/ability_relation_manager.h"

#include <glog/logging.h>
#include <sys/stat.h>

#include <iostream>

#include "utils/color.h"
#include "utils/yaml_json_converter.h"

void AbilityRelationManager::Init(std::function<std::vector<AbilityInfoExtract>()> callback_1,
                                  std::function<std::vector<std::string>(std::string)> callback_2)
{
    DLOG(INFO) << L_GREEN << "init abilityrelation manager" << NONE;
    this->abilityinfoextract_callback_ = callback_1;
    this->hardwaredevicelist_callback_ = callback_2;
}

std::string AbilityRelationManager::GetAbilitySupport()
{
    // auto list = this->abilityinfoextract_callback_();
    // for (const auto &iter : list) {
    //     DLOG(INFO) << iter.name;
    // }
    // auto treearray = this->generateDependTreeArray(list);
    // auto treearraywithdevice = this->generateDependTreeArrayWithDevices(treearray);

    // return treearraywithdevice.ToJson().toStyledString();
    auto list = this->abilityinfoextract_callback_();
    std::unordered_map<std::string, AbilityNode> support_result_;
    std::unordered_map<std::string, int> ability_level_;
    std::unordered_map<std::string, std::vector<std::string>> hardware_list_;
    this->determineLevel(list, ability_level_, hardware_list_);
    for (const auto &iter : list)
    {
        if (ability_level_[iter.name] == -1)
        {
            continue;
        }
        if (support_result_.count(iter.name) == 0)
        {
            support_result_[iter.name] =
                AbilityNode(iter.name, ability_level_[iter.name]);
        }
        // 遍历依赖能力
        for (const auto &ability : iter.depends.abilities)
        {
            if (support_result_.count(ability) == 0)
            {
                support_result_[ability] =
                    AbilityNode(ability, ability_level_[ability]);
            }
            support_result_[iter.name].ability_children_.emplace_back(
                &support_result_[ability]);
        }
        // 遍历依赖设备
        for (const auto &device : iter.depends.devices)
        {
            support_result_[iter.name].depend_devices_.emplace_back(hardware_list_[device]);
        }
    }
    Json::Value root;
    for (auto &iter : support_result_)
    {
        // DLOG(INFO) << iter.first;
        std::vector<std::string> tmp = iter.second.toVector();
        for (const auto &as : tmp)
        {
            // DLOG(INFO) << as;
            Json::Reader reader;
            Json::Value support;
            reader.parse(as, support);
            root.append(support);
        }
    }
    return root.toStyledString();
}

void AbilityRelationManager::determineLevel(
    std::vector<AbilityInfoExtract> list,
    std::unordered_map<std::string, int> &result,
    std::unordered_map<std::string, std::vector<std::string>> &devices) {
    DLOG(INFO) << "begin determine ability level";
    while (!list.empty())
    {
        // DLOG(INFO) << "当前大小" << list.size();
        for (auto info = list.begin(); info != list.end();)
        {
            // 二进制存在判定
            if (!fileExists(info->name))
            {
                DLOG(INFO) << "File " << info->name << " does not exist.\n";
                result[info->name] = -1;
                info = list.erase(info); // 擦除该info
                continue;
            }
            // level0能力支持性判定
            if (info->depends.abilities.empty())
            {
                bool support = true;
                for (const auto &device : info->depends.devices)
                {
                    if (devices.count(device) == 0)
                    {
                        devices[device] =
                            this->hardwaredevicelist_callback_(device); // 记录查询结果
                                                                        // DLOG(INFO) << "device list:" << devices[device][0];
                    }
                    if (devices[device].empty())
                    {
                        support = false; // 不支持
                        break;
                    }
                }
                if (support)
                {
                    DLOG(INFO) << "ability " << info->name << " level is 0\n";
                    result[info->name] = 0; // level0能力
                }
                else
                {
                    DLOG(INFO) << "ability " << info->name << " unsupport\n";
                    result[info->name] = -1; // 不支持
                }
                info = list.erase(info); // 擦除该info
            }
            else
            {
                DLOG(INFO) << info->name;
                // 级别确定
                int level = 1;
                bool support = true;
                for (const auto &ability : info->depends.abilities)
                {
                    if (fileExists(ability) && result.count(ability) == 0)
                    {
                        DLOG(INFO) << "ability" << info->name << "uncertain";
                        goto END; // 暂时无法确定
                    }
                    // 依赖能力不支持
                    if (!fileExists(ability) || result[ability] == -1)
                    {
                        support = false;
                        DLOG(INFO) << "ability " << info->name
                                   << " unsupport: (depend ability unsupport)\n";
                        break;
                    }
                    // 等级确定
                    level = std::max(result[ability] + 1, level);
                }
                // 依赖硬件确定
                for (const auto &device : info->depends.devices)
                {
                    if (devices.count(device) == 0)
                    {
                        devices[device] =
                            this->hardwaredevicelist_callback_(device); // 记录查询结果
                    }
                    if (devices[device].empty())
                    {
                        support = false; // 不支持
                        DLOG(INFO) << "ability " << info->name << " unsupport: (hardware unsupport)\n";
                        break;
                    }
                }
                if (support)
                {
                    DLOG(INFO) << "ability " << info->name << " level is " << level;
                    result[info->name] = level; // level0能力
                }
                else
                {

                    result[info->name] = -1; // 不支持
                }
                info = list.erase(info); // 擦除该info
                continue;
            END:
                info++;
            }
        }
    }
    DLOG(INFO) << "determine ability level success";
}


DependTreeArray AbilityRelationManager::generateDependTreeArray(const std::vector<AbilityInfoExtract> &list)
{
    std::unordered_map<std::string, TreeNode> map;

    for (const auto &ability : list) {
        TreeNode node{ability, {}, 0, 0};
        map[ability.name] = node;
    }

    for (const auto &ability : list) {
        for (const auto &depend : ability.depends.abilities) {
            // DLOG(INFO) << map[depend].children.at(0).ability.name;
            // DLOG(INFO) << ability.name << "depends:" << depend;
            // DLOG(INFO) << map[depend].ToJson();
            if (map.count(depend) > 0) {
                map[ability.name].children.push_back(map[depend]);
                map[ability.name].level = map[depend].level + 1;
            }
        }
    }

    DependTreeArray treeArray;

    for (const auto &pair : map) {
        // if (pair.second.level == 0) {
            DLOG(INFO) << pair.second.ToJson();
            treeArray.trees.push_back(pair.second);
        // }
    }

    return treeArray;
}

DependTreeArray AbilityRelationManager::generateDependTreeArrayWithDevices(const DependTreeArray &treeArray)
{
    DependTreeArray newTreeArray;

    for (auto &tree : treeArray.trees) {
        generateNodes(tree, &newTreeArray);
    }

    for (auto abilityInstance = newTreeArray.trees.begin(); abilityInstance != newTreeArray.trees.end(); abilityInstance++)
    {
        abilityInstance->generateHashId();
    }
    

    return newTreeArray;
}

void AbilityRelationManager::generateNodes(const TreeNode &node, DependTreeArray *treeArray)
{
    if (!node.ability.depends.devices.empty()) {
        std::vector<std::vector<std::string>> deviceLists;

        for (const auto &device : node.ability.depends.devices) {
            auto deviceList = hardwaredevicelist_callback_(device);
            // DLOG(INFO) << device << deviceList.size();
            if (!deviceList.empty()) {
                deviceLists.push_back(deviceList);
            } else {
                return;  // If any required device list is empty, don't create any
                         // instance.
            }
        }

        // Generate all combinations of devices.
        std::vector<std::vector<std::string>> combinations = cartesianProduct(deviceLists);
        for (const auto &combination : combinations) {
            if (!fileExists(node.ability.name)) {
                std::cout << "File " << node.ability.name << " does not exist.\n";
                return;
            }
            TreeNode newNode{node.ability, node.children, node.level};
            newNode.ability.depends.devices = combination;  // Store the devices into the ability instance.
            treeArray->trees.push_back(newNode);
        }
    // }
    } else
    {
        TreeNode newNode{node.ability, node.children, node.level};
        treeArray->trees.push_back(newNode);
    }
    
    // for (auto &child : node.children) {
    //     generateNodes(child, treeArray);
    // }
}

std::vector<std::vector<std::string>> AbilityRelationManager::cartesianProduct(const std::vector<std::vector<std::string>> &lists)
{
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

bool AbilityRelationManager::fileExists(const std::string &filename)
{
    struct stat buffer;
    return (stat(("bin/" + filename).c_str(), &buffer) == 0);
}

bool AbilityRelationManager::abilityInstanceExists(unsigned long abilityId)
{
    // auto list = this->abilityinfoextract_callback_();
    // for (const auto &iter : list) {
    //     DLOG(INFO) << iter.name;
    // }
    // auto treearray = this->generateDependTreeArray(list);
    // auto treearraywithdevice = this->generateDependTreeArrayWithDevices(treearray);
    // std::cout << treearraywithdevice.ToJson().toStyledString() << std::endl;
    std::string support_list = GetAbilitySupport();
    Json::Reader reader;
    Json::Value value;
    reader.parse(support_list, value);
    std::cout << "wants id: " << abilityId << std::endl;
    for (auto support : value)
    {
        if (support["id"].asUInt64() == abilityId)
        {
            DLOG(INFO) << "id matched";
            return true;
        }
        
    }
    
    return false;
}
