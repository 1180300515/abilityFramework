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

## 目录结构
```text
include 储存头文件
├── common
└── mongoose
resource
├── db 储存数据库
└── resource_file 储存各种资源文件
    ├── ability 能力资源文件
    ├── crd crd资源文件
    ├── device 设备资源文件
    └── example_for_test 测试用例
src 源文件
└── controller
    ├── common
    └── mongoose
log 储存日志文件
```
## 数据结构
1. 数据库结构  
分为crd表，ability表，instance表,cloudaddress表。  
> - CRD&emsp;包括key,value,apigroup,kind,schema.  
> - ABILITY&emsp;包括name,value,namespace,apigroup,kind,version
> - INSTANCE&emsp;包括name,value,namespace,apigroup,kind,version
> - CLOUDADDRESS&emsp;包括address
2. 数据结构  
主要包括了`Ability , CameraInstance , LoudspeakerInstance , SensorInstance`

## 全局配置
位于`config/controller_config.yaml`文件中。

## 默认配置
位于`include/controller/global_var.h`头文件中
- 默认udp接收端口号: 8001
- 默认udp广播端口号: 8888

## 数据包类型
- **edge和cloud同步的相关数据包**  
包括`sync-kv , sync-c2s , sync-s2c`
- **egde和edge同步的数据包**   
包括`edgesync`

## 事件类型
- 区分资源操作变动类型：  
包括了`edgeadd` , `edgedelete` , `edgeupdate` , `cloudadd` , `clouddelete` , `cloudupdate` , `edgenonlocal`

