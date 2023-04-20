#ifndef ABI_INTERFACE_H
#define ABI_INTERFACE_H

#include "plugincore/plugin_interface.h"

using namespace plugs;

class AbilityManager : public PluginInterface{
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


#endif // ABI_INTERFACE_H