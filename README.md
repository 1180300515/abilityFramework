# coderproject-insider

这是一个动态加载的软件框架，采用观察者Observer作为插件模型，Subject话题作为通信枢纽，实现各个插件模块间的解耦。

已完成：
- [x] Observer和Subject抽象
- [*] 插件PluginInterface抽象
- [*] 实现Plugin动态库编译和加载
- [*] 加载后的Plugin可向Subject发送消息

TODO：
- [ ] 修复Subject中其他Observer收不到消息的问题
- [ ] 定义消息格式，满足模块间通信需要
- [ ] 测试在不用复制的情况下，传输对象地址来使用
- [ ] 融入DBManager和Connectionmanager模块
- [ ] 加入AbilityPool模块和LANDiscovery模块