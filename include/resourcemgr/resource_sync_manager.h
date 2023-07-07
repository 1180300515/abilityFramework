#ifndef RESOURCE_SYNC_MANAGER_H
#define RESOURCE_SYNC_MANAGER_H

#include <memory>

#include "connection_manager.h"

class ResourceSyncManager
{
private:
    std::shared_ptr<ConnectionManager> connection_;

public:
    ResourceSyncManager();
    ~ResourceSyncManager() = default;

    void CloudSyncThread();
    void EdgeSyncServerThread();
    void EdgeSyncThread();
    void Run();
};

#endif // RESOURCE_SYNC_MANAGER_H