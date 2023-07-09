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
        std::cout << L_BLACK << std::endl;
        for (auto plugin : plugin_list)
        {
            std::cout << "Plugin: " << plugin.name << std::endl;
            std::cout << "\tPath: " << plugin.path << std::endl;
            std::cout << "\tEnabled: " << (plugin.enabled ? "√" : "×") << std::endl;
            std::cout << "\tExists: " << (plugin.exists ? "√" : "×") << std::endl;
            std::cout << "\tActivated: " << (plugin.activated ? "√" : "×") << std::endl;
        }
        std::cout << NONE << std::endl;
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
            plugin.path = it->second["path"].as<std::string>() + "/lib" + plugin.name + ".so";
            plugin.enabled = it->second["enabled"].as<bool>();
            plugin.exists = pluginExists(plugin.path);
            plugin_list.push_back(plugin);
        }
        loadPlugins();
    }

    void PluginLoader::loadPlugins()
    {
        for (auto& plugin : plugin_list)
        {
            if (plugin.enabled && plugin.exists)
            {
                void *handle = dlopen(plugin.path.c_str(), RTLD_LAZY);
                if (!handle)
                {
                    std::cerr << "Cannot open library: " << dlerror() << '\n';
                    plugin.activated = false;
                    continue;
                }

                // load the symbols
                CreatePluginFunc createPlugin = (CreatePluginFunc)dlsym(handle, "CreatePluginFunc");
                const char *dlsym_error = dlerror();
                if (dlsym_error)
                {
                    std::cerr << "Cannot load symbol 'CreatePluginFunc': " << dlsym_error << '\n';
                    dlclose(handle);
                    plugin.activated = false;
                    continue;
                }

                DestroyPluginFunc destroyPlugin = (DestroyPluginFunc)dlsym(handle, "DestroyPluginFunc");
                dlsym_error = dlerror();
                if (dlsym_error)
                {
                    std::cerr << "Cannot load symbol 'DestroyPluginFunc': " << dlsym_error << '\n';
                    dlclose(handle);
                    plugin.activated = false;
                    continue;
                }

                // use the symbols to do the calculation
                std::unique_ptr<PluginInterface> plugin_interface(createPlugin());
                // plugin_interface->execute();
                plugin.activated = true;
                // std::cout << "Plugin " << plugin.name << ":" << plugin.activated << std::endl;
                addPlugin(plugin.name, std::move(plugin_interface));
                //destroyPlugin(plugin.get());
            }
        }
    }

    std::vector<std::pair<std::string, std::unique_ptr<PluginInterface>>>& PluginLoader::getPlugins()
    {
        return pluginsload;
    }

    bool PluginLoader::pluginExists(std::string path)
    {
        std::ifstream file(path);
        return file.good();
    }

    void PluginLoader::addPlugin(const std::string& name, std::unique_ptr<PluginInterface> plugin)
    {
        pluginsload.emplace_back(name, std::move(plugin));
    }

    void PluginLoader::executePlugin(const std::string& name, SubjectManager* subjectManager)
    {
        // std::cout << "Execute plugin: " << name << std::endl;
        for (auto& plugin : pluginsload)
        {
            // std::cout << "Plugin name: " << plugin.first << std::endl;
            if (plugin.first == name)
            {
                // std::cout << "Find plugin: " << name << std::endl;
                plugin.second->execute(subjectManager);
            }
        }
    }

}
