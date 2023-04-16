#include "plugincore/plugin_interface.h"
#include <iostream>
#include <memory>
#include <dlfcn.h>
#include <yaml-cpp/yaml.h>

namespace plugs{

    PluginLoader::PluginLoader(const std::string &path){
        loadPlugins(path);
    }

    void PluginLoader::loadConfigs(std::string path){

    }



}