#ifndef CONN_INTERFACE_H
#define CONN_INTERFACE_H

#include "plugincore/plugin_interface.h"

using namespace plugs;

class ConnectionManager : public PluginInterface{
public:
    Observer *observer;
    ConnectionManager() : observer(new Observer()){};
    std::string getName() const override;
    std::string getDescription() const override;
    void execute(SubjectManager* subjectManager) override;
    bool registerObserver(SubjectManager* subjuectManager) override;
    std::vector<std::string> targetSubjects() const override{
        return {"LANinfo", "CAM"};
    };
    void transportMessage2Subject(SubjectManager *subjectManager, std::string subjectName, std::string message) override;

};

extern "C" PluginInterface* CreatePluginFunc();
extern "C" void DestroyPluginFunc(PluginInterface* plugin);


#endif // CONN_INTERFACE_H