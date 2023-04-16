#include <iostream>
#include "plugincore/plugin_interface.h"
#include "plugincore/obs_interface.h"

using namespace plugs;

// generate helloworld code
int main(){
    std::cout << "Hello World!" << std::endl;
    auto AMplugin = new PluginLoader("config/config.yaml"); 
    AMplugin->listPlugins();
    
    return 0;
}