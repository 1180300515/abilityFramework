/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: depend_tree_array.cpp
 * Description: depend_tree_array.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "abilityrelationmgr/depend_tree_array.h"

bool DependTreeArray::hasAbility(const std::string& abilityName) const
{
    for (auto& tree : trees) {
        if (tree.ability.name == abilityName) {
            return true;
        }
    }
    return false;
}

bool DependTreeArray::hasAbilityInstance(unsigned long abilityId)
{
    for (auto& tree : trees) {
        std::cout << "has id : " << tree.abilityId << std::endl;
        if (tree.abilityId == abilityId) {
            return true;
        }
    }
    return false;
}

Json::Value DependTreeArray::ToJson() const
{
    Json::Value jsonTreeArray;
    for (const auto& tree : trees) {
        jsonTreeArray.append(tree.ToJson());
    }
    return jsonTreeArray;
}

Json::Value TreeNode::ToJson() const
{
    //std::stringstream ss;
    Json::Value jsonNode;
    jsonNode["name"] = ability.name;
    jsonNode["id"] = abilityId;
    //ss << ability.name;
    jsonNode["level"] = level;
    //ss << level;
    for (const auto& ability : ability.depends.abilities) {
        jsonNode["depends"]["abilities"].append(ability);
        //ss << ability;
    }
    for (const auto& device : ability.depends.devices) {
        Json::Reader reader;
        Json::Value device_json;
        reader.parse(device,device_json);
        jsonNode["depends"]["devices"].append(device_json);
        //ss << device;
    }
    for (const auto& child : children) {
        jsonNode["children"].append(child.ToJson());
        //ss << child.ToJson();
    }
    // std::hash<std::string> hash;
    // jsonNode["id"] = hash(ss.str());
    return jsonNode;
}

void TreeNode::generateHashId()
{
    std::stringstream ss;
    ss << ability.name;
    ss << level;
    for (const auto& ability : ability.depends.abilities) {
        ss << ability;
    }
    for (const auto& device : ability.depends.devices) {
        ss << device;
    }
    for (const auto& child : children) {
        ss << child.ToJson();
    }
    std::hash<std::string> hash;
    abilityId = hash(ss.str());
    //std::cout << "abilityId:" << abilityId << std::endl;
}

std::vector<std::string> AbilityNode::toVector()
{
    // DLOG(INFO) << "in to Vector";
    std::vector<std::vector<std::string>> lists;
    for (const auto iter : ability_children_)
    {
        lists.emplace_back(iter->toVector());
    }
    for (const auto iter : depend_devices_)
    {
        lists.emplace_back(iter);
    }
    std::vector<std::vector<std::string>> tmp = this->cartesianProduct(lists);
    // DLOG(INFO) << "finish cart";
    std::vector<std::string> result;
    if (tmp.empty())
    {
        Json::Value jnode;
        jnode["name"] = this->ability_name_;
        jnode["level"] = this->level_;
        std::hash<std::string> hash;
        jnode["id"] = hash(jnode.toStyledString());
        result.emplace_back(jnode.toStyledString());
    }
    
    for (const auto &iter : tmp)
    {
        Json::Value jnode;
        int count = 0;
        jnode["name"] = this->ability_name_;
        jnode["level"] = this->level_;
        for (const auto &ai : iter)
        {   
            Json::Reader reader;
            Json::Value temp;
            bool result = reader.parse(ai, temp);
            // DLOG(INFO) << result << ai;
            if (count < ability_children_.size())
            {
                jnode["depends"]["abilities"].append(temp);
                count++;
            }
            else
            {
                jnode["depends"]["devices"].append(temp);
                count++;
            }
            std::hash<std::string> hash;
            jnode["id"] = hash(jnode.toStyledString());
        }
        result.emplace_back(jnode.toStyledString());
    }
    return result;
}

std::vector<std::vector<std::string>> AbilityNode::cartesianProduct(
    const std::vector<std::vector<std::string>> &lists)
{
    if (lists.empty())
    {
        return {};
    }
    
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> combination(lists.size());

    size_t counter = 0;
    while (true)
    {
        for (size_t i = 0; i < lists.size(); ++i)
        {
            combination[i] = (lists[i])[counter % lists[i].size()];
        }

        result.push_back(combination);

        ++counter;
        for (size_t i = 0; i < lists.size(); ++i)
        {
            if (counter % lists[i].size() != 0)
            {
                break;
            }
            else if (i == lists.size() - 1)
            {
                return result;
            }
        }
    }
}