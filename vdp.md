## VDP部分实现功能

均为HTTP接口

### 1. 设备查询
| 接口名称 | 方法 | 路径 |
| :--- | :--- | :--- |
| 请求设备列表| GET | /api/Devices |

|返回值|类型|说明|
|:---|:---|:---|
|Device列表|JSON||

每一类设备都是一个数组，数组中的每一个元素都是一个JSON对象，可以显示为多个表格，对象的属性和方法如下，传感器种类后续补充：

样例：
```JSON
{
    "cameraDevices": [
        {
            "bus_info": "usb-0000:00:14.0-9",
            "card": "USB2.0 HD UVC WebCam: USB2.0 HD",
            "device_path": "/dev/video0",
            "driver": "uvcvideo",
            "formats": [
                "MJPG",
                "YUYV"
            ]
        }
    ],
    "displayDevices": [
        {
            "height": 1080,
            "mmHeight": 194,
            "mmWidth": 344,
            "screen": 0,
            "width": 1920,
            "x": 0,
            "y": 0
        }
    ],
    "micDevices": [
        {
            "channels": 2,
            "description": "内置音频 模拟立体声",
            "mute": false,
            "name": "alsa_input.pci-0000_00_1f.3.analog-stereo",
            "sampleRate": 44100,
            "volume": 65540
        }
    ],
    "speakerDevices": [
        {
            "channels": 2,
            "description": "内置音频 模拟立体声",
            "mute": false,
            "name": "alsa_output.pci-0000_00_1f.3.analog-stereo",
            "sampleRate": 44100,
            "volume": 26241
        }
    ]
}
```

来源：
```C++
class DeviceProfile
{
public:
    std::vector<CameraHardware> cameraDevices;
    std::vector<DisplayInfo> displayDevices;
    std::vector<AudioDevice> micDevices;
    std::vector<AudioDevice> speakerDevices;

    Json::Value toJson() const
    {
        Json::Value root;
        for (const auto &cam : cameraDevices)
            root["cameraDevices"].append(cam.toJson());
        for (const auto &disp : displayDevices)
            root["displayDevices"].append(disp.toJson());
        for (const auto &mic : micDevices)
            root["micDevices"].append(mic.toJson());
        for (const auto &speaker : speakerDevices)
            root["speakerDevices"].append(speaker.toJson());
        return root;
    }
}

class CameraHardware
{
public:
    std::string device_path;
    std::string driver;
    std::string card;
    std::string bus_info;
    std::vector<std::string> formats;
    //...更多属性和方法...

    Json::Value toJson() const {
        Json::Value root;
        root["device_path"] = device_path;
        root["driver"] = driver;
        root["card"] = card;
        root["bus_info"] = bus_info;
        for (const auto& format : formats)
            root["formats"].append(format);
        return root;
    }
}

class DisplayInfo
{
public:
    int screen;
    int x;
    int y;
    int width;
    int height;
    int mmWidth;
    int mmHeight;

    Json::Value toJson() const {
        Json::Value root;
        root["screen"] = screen;
        root["x"] = x;
        root["y"] = y;
        root["width"] = width;
        root["height"] = height;
        root["mmWidth"] = mmWidth;
        root["mmHeight"] = mmHeight;
        return root;
    }
}

class AudioDevice
{
public:
    std::string name;
    std::string description;
    uint32_t volume;
    bool mute;
    uint32_t sampleRate;
    uint8_t channels;

    Json::Value toJson() const {
        Json::Value root;
        root["name"] = name;
        root["description"] = description;
        root["volume"] = volume;
        root["mute"] = mute;
        root["sampleRate"] = sampleRate;
        root["channels"] = channels;
        return root;
    }
}
```

## 2. 能力运行状态查询
| 接口名称 | 方法 | 路径 |
| :--- | :--- | :--- |
| 请求能力运行列表| GET | /api/AbilityRunning |

|返回值|类型|说明|
|:---|:---|:---|
|abilityName|string|能力名|
|abilityPort|int|能力端口|
|last_update|int|最后更新时间(unix time)|
|port|int|生命周期端口|
|status|string|生命周期状态|

能力端口为实际VDP建立业务连接时连接的端口，需要使用能力控制接口完成connect操作后才会有值，否则为0。

返回的为一个数组，返回值和类型如上：

```JSON
[
    {
        "abilityName": "camera",
        "abilityPort": 0,
        "last_update": 1980623230,
        "port": 46179,
        "status": "STANDBY"
    }
]
```

## 3. 能力支持查询
| 接口名称 | 方法 | 路径 |
| :--- | :--- | :--- |
| 请求能力支持列表| GET | /api/AbilitySupport |


|返回值|类型|说明|
|:---|:---|:---|
|能力支持列表|JSON||
|name|string|能力名|
|level|int|能力级别|
|depends|JSON|依赖|
|abilities|array|依赖能力|
|devices|array|依赖设备|


样例：
```JSON
[
    {
        "depends": {
            "abilities": [
                "none"
            ],
            "devices": [
                "hostname:luo980-aorus15gyc,name:alsa_output.pci-0000_00_1f.3.analog-stereo,description:内置音频 模拟立体声,volume:26241,mute:false,sampleRate:44100,channels:2"
            ]
        },
        "level": 0,
        "name": "loudspeaker"
    },
    {
        "depends": {
            "abilities": [
                "none"
            ],
            "devices": [
                "hostname:luo980-aorus15gyc,device_path:/dev/video0,driver:uvcvideo,card:USB2.0 HD UVC WebCam: USB2.0 HD,bus_info:usb-0000:00:14.0-9,formats:[MJPG YUYV ]"
            ]
        },
        "level": 0,
        "name": "camera"
    }
]

```

提供一个devices中按键查询值的函数，可以获得hostname和它拥有的Device相关信息
```C++
std::string findValueByKey(const std::string& keyValueString, const std::string& key) {
    std::unordered_map<std::string, std::string> keyValueMap;

    std::stringstream ss(keyValueString);
    std::string pair;
    while (std::getline(ss, pair, ',')) {
        std::stringstream pair_ss(pair);
        std::string key, value;
        std::getline(pair_ss, key, ':');
        std::getline(pair_ss, value, ':');
        std::cout << "key: " << key << ", value: " << value << std::endl;
        keyValueMap[key] = value;
    }

    if (keyValueMap.find(key) != keyValueMap.end()) {
        return keyValueMap[key];
    } else {
        return "Key not found";
    }
}
```

## 4. 能力运行控制

| 接口名称 | 方法 | 路径 |
| :--- | :--- | :--- |
| 请求能力运行控制| POST | api/AbilityRequest |

|请求参数|类型|说明|
|:---|:---|:---|
|abilityName|string|能力名|
|port|int|生命周期端口|
|cmd|string|命令|
|connectIP|string|连接IP|
|connectPort|int|连接端口|

能力名可以从能力支持查询中获得，对于跨端相机能力，因为树莓派端是生产者，所以connectIP和connectPort不需要指定，但不能为空，否则会出错。

start可启动的能力仅限于AbilitySupport中得到的列表中的能力。

cmd的值可以为：start, connect, disconnect, terminate

## 5. 服务器发现

服务器地址发现由设备发现实现，设备发现由UDP实现，广播端口为8888，HTTP server端口为8080，设备发现的消息格式为：

广播包样例：
```
std::string message = std::string(hostname) + "+" + loadavg + "+" + std::to_string(std::time(nullptr));

luo980-aorus15gyc+Idle,Stable+1685002675
```

因此可以通过广播建立hostname和IP的映射关系：
| hostname | IP |
| :--- | :--- |
|luo980-aorus15gyc| 192.168.2.12|
|raspberrypi| 192.168.2.19|
|...|...|