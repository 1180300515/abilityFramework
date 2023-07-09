#include "controller/cloud_connection.h"
#include "controller/global_var.h"

void CloudConnection::RegisterCallback(std::function<void(KeyAndDataPackages)> func)
{
    callback_ = func;
}
bool CloudConnection::CheckConnection()
{
     if (this->cloudAddress_ != cloud_address && cloud_address != "")
    {
        return false;
    }
    return true;
}