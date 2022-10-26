# UnrealPuertsImGui

这是一个基础模板工程，可以在Unreal下使用[Puerts](https://github.com/Tencent/puerts)和[ImGui](https://github.com/ocornut/imgui)（由[UnrealImGui](https://github.com/segross/UnrealImGui)）开发Editor工具

本项目同时也维护`PuertsXImGui`这个核心联结插件

## 特色功能与修改点

### UnrealImGui支持编辑器

原版[UnrealImGui](https://github.com/segross/UnrealImGui)仅支持UE运行时。这里封装了一个`SImGuiWidgetEd`把ImGui的显示嵌进了编辑器界面

### 编辑器工具热更新

魔改Puerts使其在ts模块修改时可以抛出通知，`PuertsXImGui`模块接收到事件后重启JS虚拟机达到热更新的目的

源码修改点：
- `Plugins/Puerts/Source/Puerts/Public/PuertsModule.h`：新增一个`FOnModuleChange OnModuleChange`委托
- `Plugins/Puerts/Source/PuertsEditor/Private/FileSystemOperation.cpp`：`UFileSystemOperation::PuertsNotifyChange`新增`IPuertsModule::Get().OnModuleChange.Broadcast(*Path)`事件发送

### ImGui在UE中支持输入法IME

重新封装了`InputText`，`InputTextMultiline`，`InputTextWithHint`，利用其回调函数通过`FUnrealImGuiModule::NotifyActiveImGuiInputText`将用户输入数据发送到UE侧做处理。UE通过`GetTextInputMethodSystem` API来处理IME相关的功能