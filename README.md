## 安装sqlite
`sudo apt-get install sqlite3`   
`sudo apt-get install libsqlite3-dev`
## 依赖
1. [yaml-cpp](https://github.com/jbeder/yaml-cpp) 提供yaml处理能力
2. [jsoncpp](https://github.com/open-source-parsers/jsoncpp) 提供json处理能力
3. [rapidjson](https://github.com/Tencent/rapidjson) 用来进行schema校验
4. [mongoose](https://github.com/cesanta/mongoose) 提供http server  
5. [google-glog](https://github.com/google/glog) 提供日志支持
```
sudo apt install libgoogle-glog-dev
sudo apt install libjsoncpp-dev
sudo apt install rapidjson-dev 
sudo apt install libyaml-cpp-dev
其他依赖:
sudo apt install libxrandr-dev
sudo apt install libv4l-dev
sudo apt install libx11-dev
sudo apt install libpulse-dev
```
```
apt install libunwind-dev sqlite3 libsqlite3-dev libyaml-cpp-dev  libgoogle-glog-dev libjsoncpp-dev rapidjson-dev libyaml-cpp-dev libxrandr-dev libv4l-dev libx11-dev libpulse-dev
```

## 目录结构
```text
include 储存头文件
├── abilityrelationmgr
├── databasemgr
├── discoverymgr
├── httpserver
├── lifecyclemgr
├── protos
├── resourcemgr
└── utils
resource
├── db 储存数据库
└── resource_file 储存各种资源文件
    ├── ability 能力资源文件
    ├── crd crd资源文件
    ├── device 设备资源文件
    └── example_for_test 测试用例
src 源文件
├── abilityrelationmgr
├── databasemgr
├── discoverymgr
├── httpserver
├── lifecyclemgr
├── protos
├── resourcemgr
├── utils
└── main.cpp
test 测试
└── resource_mgr_test
log 储存日志文件
bin 可执行文件
```
## 数据结构
1. 数据库结构  
分为crd表，ability表，instance表,cloudaddress表。  
> - CRD&emsp;包括key,value,apigroup,kind,schema.  
> - ABILITY&emsp;包括name,value,namespace,apigroup,kind,version
> - INSTANCE&emsp;包括name,value,namespace,apigroup,kind,version
> - CLOUDADDRESS&emsp;包括address
2. 数据结构  
主要包括了`Ability , CameraInstance , LoudspeakerInstance , SensorInstance` , `MicroPhoneInstance`

## 全局配置
位于`config/start_config.yaml`文件中。

## 默认配置
位于`include/controller/global_var.h`头文件中
- httpserver端口号: 8080

## 硬件资源自动生成Instance
位于类`HardwareScan`中。函数为`AutoGenerateCR()`

