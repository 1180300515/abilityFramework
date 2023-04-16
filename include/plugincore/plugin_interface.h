#include "obs_interface.h"

#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <string>

namespace plugs
{

    class PluginInterface
    {
    public:
        virtual ~PluginInterface() = default;
        virtual std::string getName() const = 0;
        virtual std::string getDescription() const = 0;
        virtual void execute() = 0;
        virtual void registerObserver(Subject *subject) = 0;
    };

    extern "C"
    {
        typedef PluginInterface *(*CreatePluginFunc)();
        typedef void (*DestroyPluginFunc)(PluginInterface *);
    }

    struct PluginInfo{
        std::string name;
        bool enabled;
        bool activated;
    };

    class PluginLoader
    {
    public:
        PluginLoader(const std::string &path);
        ~PluginLoader();
        PluginInterface *getPlugin();
        void releasePlugin();
    private:
        std::vector<PluginInfo> plugin_list;
        void loadConfigs(std::string path);
        void loadPlugins(std::string path);
    };

}
#endif // PLUGIN_INTERFACE_H
