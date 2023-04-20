#include "obs_interface.h"
#include "event_interface.h"

#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <string>

namespace plugs
{

    class PluginInterface
    {
    public:
        Observer *observer;
        virtual ~PluginInterface() = default;
        virtual std::string getName() const = 0;
        virtual std::string getDescription() const = 0;
        virtual void execute(SubjectManager* subjectManager) = 0;
        virtual std::vector<std::string> targetSubjects() const = 0;
        bool registerObserver(SubjectManager* subjectManager){
            std::cout << "Registering Plugin: " << getName() << std::endl;
            for(auto subjectName : targetSubjects()){
                Subject* subject = subjectManager->getSubject(subjectName);
                if(subject){
                    subject->registerObserver(observer);
                    subject->getObserverNums();
                }
            }
            std::cout << "Plugin: " << getName() << " is registered" << std::endl;
            return true;
        }
        void transportMessage2Subject(SubjectManager* subjectManager, std::string subjectName, std::string message){
            Subject* subject = subjectManager->getSubject(subjectName);
            std::cout << "Sending message from " << getName() << " to " << subjectName << std::endl; 
            if(subject){
                std::cout << "Subject not null" << std::endl;
                subject->getObserverNums();
                subject->transportMessage(observer ,message);
            }
        }

    };

    extern "C"
    {
        typedef PluginInterface *(*CreatePluginFunc)();
        typedef void (*DestroyPluginFunc)(PluginInterface *);
    }

    struct PluginInfo{
        std::string name;
        std::string path;
        bool enabled;
        bool exists;
        bool activated;
    };

    class PluginLoader
    {
    public:
        PluginLoader(const std::string &path);
        ~PluginLoader();
        void releasePlugin();
        void listPlugins();
        void addPlugin(const std::string& name, std::unique_ptr<PluginInterface> plugin);
        std::vector<std::pair<std::string, std::unique_ptr<PluginInterface>>>& getPlugins();
        void executePlugin(const std::string& name, SubjectManager* subjectManager);
    private:
        std::vector<std::pair<std::string, std::unique_ptr<PluginInterface>>> pluginsload;
        std::vector<PluginInfo> plugin_list;
        void loadConfigs(std::string path);
        bool pluginExists(std::string path);
        void loadPlugins();
    };

}
#endif // PLUGIN_INTERFACE_H
    