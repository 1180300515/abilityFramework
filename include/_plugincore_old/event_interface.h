#ifndef EVENT_INTERFACE_H
#define EVENT_INTERFACE_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "subject_mgr.h"

class EventHandler
{
public:

private:
    std::unordered_map<std::string, Subject *> subjects;
};

#endif // EVENT_INTERFACE_H