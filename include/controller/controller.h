#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <map>
#include <mutex>
#include <memory>

#include "dbmanager.h"
#include "global_var.h"
#include "connection_manager.h"

void Preprocessing();

class Controller : public std::enable_shared_from_this<Controller>
{
private:
    int sockid = -1;
    std::shared_ptr<ConnectionManager> connection_;

public:
    // record the key and version
    std::map<std::string, std::string> versionRecord;
    // the instance load from the database
    std::map<std::string, std::shared_ptr<CameraInstance>> camera_instances;
    std::map<std::string, std::shared_ptr<LoudspeakerInstance>> loudspeaker_instances;
    std::map<std::string, std::shared_ptr<SensorInstance>> sensor_instances;
    // store the cloud non-native resource
    std::map<std::string, std::shared_ptr<NonLocalResource>> nonlocal_instances_;

    std::mutex nonlocal_lock_;
    std::mutex camera_lock_;
    std::mutex loudspeaker_lock_;
    std::mutex sensor_lock_;

public:
    std::shared_ptr<Controller> getptr();
    Controller() = default;
    ~Controller() = default;
    /**
     * init the connection manager
     */
    void SetConnection();
    void LoadResource();
    /**
     * set edge address record
    */
    void SetEdgeAddressRecord(std::map<std::string , std::string> record);
    /**
     * judge whether is local resource
     *
     * @param key key value in resource
     *
     * @return is or not
     */
    std::string isLocalResource(std::string key);

    /**
     * run the process
     */
    void Run();

    /**
     * the callback to deal the data from the cloud
     */
    void HandleResourceSync(KeyAndDataPackages data);
    /**
     * the callback to deal the data from the non-native edge
     */
    void HandleResourceSyncEdge(KeyAndDataPackages data);
    /**
     * refresh the key and version record
     */
    void RefreshKVRecord();

    std::string GetNonLocalFormat();
};

#endif