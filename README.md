## 安装sqlite
`sudo apt-get install sqlite3`   
`sudo apt-get install libsqlite3-dev`
## 依赖
1. [yaml-cpp](https://github.com/jbeder/yaml-cpp) 提供yaml处理能力
2. [jsoncpp](https://github.com/open-source-parsers/jsoncpp) 提供json处理能力
3. [rapidjson](https://github.com/Tencent/rapidjson) 用来进行schema校验
4. [httplib](https://github.com/yhirose/cpp-httplib) 提供http server  
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
sudo apt install libalsa-ocaml-dev 
```
```
apt install libunwind-dev sqlite3 libsqlite3-dev libyaml-cpp-dev  libgoogle-glog-dev libjsoncpp-dev rapidjson-dev libyaml-cpp-dev libxrandr-dev libv4l-dev libx11-dev libpulse-dev libalsa-ocaml-dev 
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

## 硬件资源自动生成DeviceInstance
硬件匹配机制为:    
`camera`采用`CameraHardware`中的`businfo`字段。  
`mic`和`speaker`则采用`AudioHardware`中的`name`字段。  
> businfo获取:&ensp;`v4l2-ctl --list-devices --verbose`  
```
例子:
USB 2.0 Camera: USB Camera (usb-0000:00:14.0-1):
	/dev/video0
	/dev/video1
	/dev/media0

VIDIOC_QUERYCAP: ok
```
`usb-0000:00:14.0-1`&ensp;即为businfo。
> name字段获取:  
mic:&ensp; `arecord -l`   
speaker:&ensp; `aplay -l`  
mic组成规则为: &ensp;&ensp; <font color=ff0000>mic.card-[cardID].device-[deviceID].[info]</font>   
speaker组成规则为: &ensp;&ensp; <font color=ff0000>speaker.card-[cardID].device-[deviceID].[info]</font>
```
mic例子:
card 1: sofhdadsp [sof-hda-dsp], device 0: HDA Analog (*) []
  子设备: 1/1
  子设备 #0: subdevice #0

cardID为1，deviceID为0，info为sof-hda-dsp

speaker例子:
card 0: NVidia [HDA NVidia], device 9: HDMI 3 [HDMI 3]
  子设备: 1/1
  子设备 #0: subdevice #0
cardID为0，deviceID为9，info为HDA NVidia
```
mic的`name`为:&ensp; <font color=ff0000>mic.card-1.device-0.sof-hda-dsp</font>


