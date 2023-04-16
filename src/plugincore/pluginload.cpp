#include "plugincore/plugin_interface.h"
#include <iostream>
#include <memory>
#include <dlfcn.h>
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace plugs
{
    PluginLoader::PluginLoader(const std::string &path) : plugin_list()
    {
        loadConfigs(path);
    }

    void PluginLoader::listPlugins()
    {
        for (auto plugin : plugin_list)
        {
            std::cout << "Plugin: " << plugin.name << std::endl;
            std::cout << "\tEnabled: " << (plugin.enabled ? "√" : "×") << std::endl;
            std::cout << "\tExists: " << (plugin.exists ? "√" : "×") << std::endl;
            std::cout << "\tActivated: " << (plugin.activated ? "√" : "×") << std::endl;
        }
    }

    void PluginLoader::loadConfigs(std::string path)
    {
        YAML::Node config = YAML::LoadFile(path);

        // 读取plugins节点
        YAML::Node plugins = config["plugins"];

        // 用于存储插件信息的列表

        // 遍历所有插件节点
        for (YAML::const_iterator it = plugins.begin(); it != plugins.end(); ++it)
        {
            PluginInfo plugin;
            plugin.name = it->first.as<std::string>();
            plugin.enabled = it->second["enabled"].as<bool>();
            plugin.exists = pluginExists(it->second["path"].as<std::string>(), plugin.name);
            plugin_list.push_back(plugin);
        }
    }

    void PluginLoader::loadPlugins(std::string path)
    {

    }

    bool pluginExists(std::string path, std::string name)
    {
        std::string fullPath = path + "/" + name + ".a";
        std::ifstream file(fullPath);
        return file.good();
    }

}
