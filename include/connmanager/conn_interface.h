#ifndef CONN_INTERFACE_H
#define CONN_INTERFACE_H

#include "plugincore/plugin_interface.h"

using namespace plugs;

class ConnectionManager : public PluginInterface{
    std::string getName() const override;
    std::string getDescription() const override;
    void execute() override;
    void registerObserver(Subject *subject) override;
};


#endif // CONN_INTERFACE_H