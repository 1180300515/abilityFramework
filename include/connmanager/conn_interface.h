#ifndef CONN_INTERFACE_H
#define CONN_INTERFACE_H

#include "plugincore/plugin_interface.h"

using namespace plugs;

class ConnectionManager : public PluginInterface{
public:
    std::string getName() const override;
    std::string getDescription() const override;
    void execute(SubjectManager* subjectManager) override;
    // bool registerObserver(SubjectManager* subjuectManager) override;
    std::vector<std::string> targetSubjects() const override{
        return {"LANinfo", "AbilityManager"};
    };
};

extern "C" PluginInterface* CreatePluginFunc();
extern "C" void DestroyPluginFunc(PluginInterface* plugin);


#endif // CONN_INTERFACE_H