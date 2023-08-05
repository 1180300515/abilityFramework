#include "resource_manager.h"

#include "glog/logging.h"
#include "color.h"

void block()
{
    std::cout << RED << "正确?输入c继续,e终止" << NONE << std::endl;
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
    test->RegistCrd("resource/resource_file/crd/ability_crd.yaml", true);
    test->RegistCrd("resource/resource_file/crd/camera_crd.yaml", true);
    test->RegistCrd("resource/resource_file/crd/loudspeaker_crd.yaml", true);
    test->RegistCrd("resource/resource_file/crd/microphone_crd.yaml", true);
    test->RegistCrd("resource/resource_file/crd/sensor_crd.yaml", true);

    test->AddAbilityInstance("resource/resource_file/ability/camera_ability.yaml", true);
    test->AddDeviceInstance("resource/resource_file/device/camera_instance1.yaml", true);
    test->AddDeviceInstance("resource/resource_file/device/camera_instance2.yaml", true);
    test->AddDeviceInstance("resource/resource_file/device/sensor_instance1.yaml", true);
    test->AddDeviceInstance("resource/resource_file/device/loudspeaker_instance1.yaml", true);

    test->Run();
    std::cout << RED << "打印当前instance" << NONE << std::endl;
    test->Print();

    std::cout << RED << "测试1 : 请插入camera , 并等待硬件刷新 , 刷新后可继续" << NONE << std::endl;
    block();
    std::cout << RED << "打印所有instance信息" << NONE << std::endl;
    test->Print();
    block();
    std::cout << RED << "拉取相机信息 , 观察是否已经插入" << NONE << std::endl;
    auto device = test->GetDeviceInstance("default/camera2");
    if (!device)
    {
        std::cout << RED << "空指针" << NONE << std::endl;
        exit(1);
    }
    std::cout << device->ToJson().toStyledString() << std::endl;
    block();
    std::cout << RED << "打印资源匹配map , 观察是否正确" << NONE << std::endl;
    test->PrintMatchMap();
    block();

    std::cout << RED << "测试2 : 请拔出camera , 并等待硬件刷新 , 刷新后可继续" << NONE << std::endl;
    block();
    std::cout << RED << "打印所有instance信息" << NONE << std::endl;
    test->Print();
    block();
    std::cout << RED << "拉取相机信息 , 观察是否已经清除硬件信息" << NONE << std::endl;
    auto device1 = test->GetDeviceInstance("default/camera2");
    if (!device1)
    {
        std::cout << RED << "空指针" << NONE << std::endl;
        exit(1);
    }
    std::cout << device1->ToJson().toStyledString() << std::endl;
    block();
    std::cout << RED << "打印资源匹配map , 观察是否正确" << NONE << std::endl;
    test->PrintMatchMap();
    block();
    test->Wait();
}