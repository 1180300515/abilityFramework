#include "resource_manager.h"

#include "glog/logging.h"
#include "color.h"

void block()
{
    std::cout << RED << "输入c,继续,e终止" << NONE << std::endl;
    std::string cmd;
    std::cin >> cmd;
    if (cmd == "e")
    {
        exit(0);
    }
}

int main(int argc, char **argv)
{
    google::InitGoogleLogging("test");
    FLAGS_colorlogtostderr = true;
    FLAGS_logtostderr = true;
    auto test = std::make_shared<ResourceManager>();
    test->Init(true);
    test->RegistCrd("test/resource_mgr_test/ability_crd.yaml", true);
    test->RegistCrd("test/resource_mgr_test/camera_crd.yaml", true);
    test->RegistCrd("test/resource_mgr_test/loudspeaker_crd.yaml", true);
    test->RegistCrd("test/resource_mgr_test/microphone_crd.yaml", true);
    test->RegistCrd("test/resource_mgr_test/sensor_crd.yaml", true);

    std::cout << RED << "camera ability add test" << NONE << std::endl;
    test->AddAbilityInstance("test/resource_mgr_test/camera_ability.yaml", true);
    test->Print();
    std::cout << RED << "camera ability get test" << NONE << std::endl;
    block();
    auto ability = test->GetAbilityInstance("default/cameratest");
    std::cout << RED << "camera ability update test" << NONE << std::endl;
    if (ability)
    {
        std::cout << ability->Marshal() << std::endl;
        ability->description = "change test";
        std::cout << ability->Marshal() << std::endl;
        test->UpdateAbilityInstance(ability->Marshal());
    }
    else
    {
        std::cout << RED << "空指针" << NONE << std::endl;
        exit(1);
    }
    std::cout << RED << "camera ability delete test" << NONE << std::endl;
    block();
    test->DeleteAbilityInstance("default/cameratest");
    test->Print();

    std::cout << RED << "camerainstance1 device add test" << NONE << std::endl;
    block();
    test->AddDeviceInstance("test/resource_mgr_test/camera_instance1.yaml", true);
    test->Print();
    std::cout << RED << "camerainstance1 device get test" << NONE << std::endl;
    block();
    auto device = test->GetDeviceInstance("default/camera1");
    if (!device)
    {
        std::cout << RED << "空指针" << NONE << std::endl;
        exit(1);
    }
    std::cout << device->ToJson().toStyledString() << std::endl;
    std::cout << RED << "camerainstance1 device update test" << NONE << std::endl;
    block();
    device->status.occupancy = true;
    test->UpdateDeviceInstance(device->Marshal());
    std::cout << RED << "camerainstance1 device delete test" << NONE << std::endl;
    block();
    test->DeleteDeviceInstance("default/camera1");
    test->Print();

    std::cout << RED << "resource manager start" << NONE << std::endl;
    block();

    test->Run();
    test->Print();

    std::cout << RED << "loudspeakerinstance1 device add test" << NONE << std::endl;
    block();
    test->AddDeviceInstance("test/resource_mgr_test/loudspeaker_instance1.yaml", true);
    test->Print();
    std::cout << RED << "loudspeakerinstance1 device get test" << NONE << std::endl;
    block();
    auto device1 = test->GetDeviceInstance("default/loudspeaker1");
    if (!device1)
    {
        std::cout << RED << "空指针" << NONE << std::endl;
        exit(1);
    }
    std::cout << device1->ToJson().toStyledString() << std::endl;
    std::cout << RED << "loudspeakerinstance1 device update test" << NONE << std::endl;
    block();
    device1->status.occupancy = true;
    test->UpdateDeviceInstance(device1->Marshal());
    std::cout << RED << "loudspeakerinstance1 device delete test" << NONE << std::endl;
    block();
    test->DeleteDeviceInstance("default/loudspeaker1");
    test->Print();

    std::cout << RED << "loudspeakerinstance2 device add test" << NONE << std::endl;
    block();
    test->AddDeviceInstance("test/resource_mgr_test/loudspeaker_instance2.yaml", true);
    test->Print();

    std::cout << RED << "loudspeakerinstance2 device get test" << NONE << std::endl;
    block();
    auto device2 = test->GetDeviceInstance("default/loudspeaker2");
    if (!device2)
    {
        std::cout << RED << "空指针" << NONE << std::endl;
        exit(1);
    }
    std::cout << device2->ToJson().toStyledString() << std::endl;
    std::cout << RED << "loudspeakerinstance2 device update test" << NONE << std::endl;
    block();
    device2->status.occupancy = true;
    test->UpdateDeviceInstance(device2->Marshal());
    std::cout << RED << "loudspeakerinstance2 device delete test" << NONE << std::endl;
    block();
    test->DeleteDeviceInstance("default/loudspeaker2");
    test->Print();

    std::cout << RED << "sensorinstance1 device add test" << NONE << std::endl;
    block();
    test->AddDeviceInstance("test/resource_mgr_test/sensor_instance1.yaml", true);
    test->Print();
    std::cout << RED << "sensorinstance1 device get test" << NONE << std::endl;
    block();
    auto device3 = test->GetDeviceInstance("default/sensor1");
    if (!device3)
    {
        std::cout << RED << "空指针" << NONE << std::endl;
        exit(1);
    }
    std::cout << device3->ToJson().toStyledString() << std::endl;
    std::cout << RED << "sensorinstance1 device update test" << NONE << std::endl;
    block();
    device3->status.occupancy = true;
    test->UpdateDeviceInstance(device3->Marshal());
    std::cout << RED << "sensorinstance1 device delete test" << NONE << std::endl;
    block();
    test->DeleteDeviceInstance("default/sensor1");
    test->Print();

    std::cout << RED << "camerainstance1 device add test" << NONE << std::endl;
    block();
    test->AddDeviceInstance("test/resource_mgr_test/camera_instance1.yaml", true);
    test->Print();
    std::cout << RED << "camerainstance1 device get test" << NONE << std::endl;
    block();
    auto device4 = test->GetDeviceInstance("default/camera1");
    if (!device4)
    {
        std::cout << RED << "空指针" << NONE << std::endl;
        exit(1);
    }
    std::cout << device4->ToJson().toStyledString() << std::endl;
    std::cout << RED << "camerainstance1 device update test" << NONE << std::endl;
    block();
    device4->status.occupancy = true;
    test->UpdateDeviceInstance(device4->Marshal());
    std::cout << RED << "camerainstance1 device delete test" << NONE << std::endl;
    block();
    test->DeleteDeviceInstance("default/camera1");
    test->Print();

    std::cout << RED << "micinstance1 device add test" << NONE << std::endl;
    block();
    test->AddDeviceInstance("test/resource_mgr_test/acer_micro.yaml", true);
    test->Print();
    std::cout << RED << "micinstance1 device get test" << NONE << std::endl;
    block();
    auto device5 = test->GetDeviceInstance("default/Acermic");
    if (!device5)
    {
        std::cout << RED << "空指针" << NONE << std::endl;
        exit(1);
    }
    std::cout << device5->ToJson().toStyledString() << std::endl;
    std::cout << RED << "micinstance1 device update test" << NONE << std::endl;
    block();
    device5->status.occupancy = true;
    test->UpdateDeviceInstance(device5->Marshal());
    std::cout << RED << "micinstance1 device delete test" << NONE << std::endl;
    block();
    test->DeleteDeviceInstance("default/Acermic");
    test->Print();

    test->Wait();
}