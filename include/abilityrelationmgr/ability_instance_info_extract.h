/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: ability_instance_info_extract.h
 * Description: ability_instance_info_extract.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef ABILITYRELATIONMGR_ABILITY_INSTANCE_INFO_EXTRACT_H_
#define ABILITYRELATIONMGR_ABILITY_INSTANCE_INFO_EXTRACT_H_

#include <string>
#include <vector>

struct Depends {
  std::vector<std::string> abilities;
  std::vector<std::string> devices;
};

struct AbilityInfoExtract {
  std::string name;  // only include name , not include namespace
  Depends depends;
};

#endif  // ABILITYRELATIONMGR_ABILITY_INSTANCE_INFO_EXTRACT_H_
