#include "abilitystatusmgr/ability_status_manager.h"

AbilityStatusManager::AbilityStatusManager()
{
}
AbilityStatusManager::~AbilityStatusManager()
{
}

void AbilityStatusManager::Init(std::function<std::string()> controller_info_callback, std::function<std::string()> ability_info_callback)
{
    this->get_ability_info = ability_info_callback;
    this->get_controller_info = controller_info_callback;
}

Json::Value AbilityStatusManager::GetAbilityStatusById(unsigned long abilityInstanceId)
{
    int abilityPort = GetAbilityPort(abilityInstanceId);
    Json::Value status;
    if (abilityPort != 0)
    {
        httplib::Client abilityCli("127.0.0.1", abilityPort);
        auto res = abilityCli.Get("/api/status");
        if (res)
        {
            Json::Reader reader;
            try
            {
                reader.parse(res->body, status);
                DLOG(INFO) << status.toStyledString();
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }

    int controllerPort = GetControllerPort(abilityInstanceId);
    httplib::Client controllerCli("127.0.0.1", controllerPort);
    std::stringstream ss;
    ss << "/api/desire" << "?" << "abilityInstanceId=" << abilityInstanceId;
    auto res = controllerCli.Get(ss.str());
    Json::Value desire;
    if (res)
    {
        Json::Reader reader;
        try
        {
            reader.parse(res->body, desire);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    Json::Value result;
    result["status"] = status;
    result["desire"] = desire;
    return result;
}

int AbilityStatusManager::UpdateAbilityDesireById(std::string req)
{
    Json::Reader reader;
    Json::Value request;
    try
    {
        reader.parse(req, request);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    unsigned long abilityInstanceId = request["abilityInstanceId"].asUInt64();
    int abilityPort = GetAbilityPort(abilityInstanceId);
    int controllerPort = GetControllerPort(abilityInstanceId);
    httplib::Client controllerCli("127.0.0.1", controllerPort);
    Json::Value desire;
    desire["desire"] = request["desire"];
    desire["abilityPort"] = abilityPort;
    desire["abilityInstanceId"] = abilityInstanceId;
    auto res = controllerCli.Put("/api/desire", desire.toStyledString(), "application/json");
    if (res)
    {
        DLOG(INFO) << "post new desire to controller port: " << controllerPort;
    }
    return 0;
}

int AbilityStatusManager::GetAbilityPort(unsigned long abilityInstanceId)
{
    httplib::Client abilityFramework_cli("127.0.0.1", 8080);
    auto res = abilityFramework_cli.Get("/api/AbilityRunning");
    int port = 0;
    if (res)
    {
        DLOG(INFO) << "in parsing";
        Json::Reader reader;
        Json::Value list;
        try
        {
            reader.parse(res->body, list);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        for (auto ability : list)
        {
            // DLOG(INFO) << ability.toStyledString();
            unsigned long id = ability["abilityInstanceId"].asUInt64();
            if (id == abilityInstanceId)
            {   
                port = ability["abilityPort"].asInt();
                DLOG(INFO) << "find target ability " << ability["abilityName"] << " abilityPort: " << port;
            }
        }
        
    }
    return port;
}

int AbilityStatusManager::GetControllerPort(unsigned long abilityInstanceId)
{
    httplib::Client abilityFramework_cli("127.0.0.1", 8080);
    auto res = abilityFramework_cli.Get("/api/AbilityRunning");
    std::string abilityName;
    int controller_port = 0;
    if (res)
    {
        Json::Reader reader;
        Json::Value list;
        try
        {
            reader.parse(res->body, list);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        for (auto ability : list)
        {
            if (ability["abilityInstanceId"].asUInt64() == abilityInstanceId)
            {
                DLOG(INFO) << "find target ability " << ability["abilityName"].asString();
                abilityName = ability["abilityName"].asString();
            }
        }
    }
    res = abilityFramework_cli.Get("/api/AbilityController");
    if (res)
    {
        Json::Reader reader;
        Json::Value list;
        try
        {
            reader.parse(res->body, list);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        for (auto controller : list)
        {
            if (controller["name"].asString() == abilityName + "Controller")
            {
                DLOG(INFO) << "find target controller " << abilityName << " port: " << controller["port"].asInt();
                controller_port = controller["port"].asInt();
            }
        }
    }
    return controller_port;
}
