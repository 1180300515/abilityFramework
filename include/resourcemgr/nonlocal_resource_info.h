#ifndef NONLOCAL_RESOURCE_INFO_H
#define NONLOCAL_RESOURCE_INFO_H

#include <string>
#include <vector>
#include <memory>

#include "common_struct_defination.h"

/**
 * @brief to describe a resource data that will keep in nonlocal host
 */
struct NonLocalResource
{
    std::string key; // namespace/name
    std::string hostname;
    std::string kind;
    std::vector<DeviceDescribe> devicelist;
};

/**
 * deserialization the json string into the NonLocalResource struct
 *
 * @param source the json string
 * @param des the target struct
 *
 * @return success or not
 */
bool UnMarshal(const std::string source, std::shared_ptr<NonLocalResource> des);
/**
 * deserialization the NonLocalResource into the json string
 *
 * @param source the source struct
 *
 * @return the json string
 */
std::string Marshal(std::shared_ptr<NonLocalResource> source);

#endif // NONLOCAL_RESOURCE_INFO_H