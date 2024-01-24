# 以ModuleGenerator插件为例分析虚幻的模块与插件

## 1. 演示视频
https://github.com/UEProjectShare/ClassProject/assets/25101328/23d1af41-44df-44d9-8423-c34851ada413

---

## 2. 模块
### 2.1 模块简介
首先什么是模块，为什么需要划分模块呢？
虚幻引擎作为一个有百万行以上代码的引擎，代码如何组织会更清晰呢？
虚幻引擎中，借助了[Unreal Build Tool](https://imzlp.com/posts/16643/)引入了模块机制。
### 2.2 模块分类
我们大概看看UE5的引擎源码目录，会发现模块大概按这几类来划分：`Runtime`，`Developer`，`Editor`， `Plugins`。如下图：

![](Images/1.png)

`Runtime`主要负责一些游戏的Gameplay等基础模块，在打出的游戏包体与编辑器都会用到这些模块。

`Developer`主要是一些跨平台相关代码以及一些与游戏无关的底层代码，同样，游戏包和编辑器都会用到。

`Editor`主要是一些编辑器相关代码，只有编辑器用到，打出给玩家的游戏包体是不会包含这些代码的。

`Plugins`主要是一些插件相关，给引擎提供了一些额外的扩展功能，并不是编辑器所必须包含的。
### 2.3 模块的目录结构
![](Images/2.png)

如上图ClassProject包含了两个模块：`ClassProject`模块，此模块与项目同名，`SecondModule`模块。
我们可以看到一个模块文件夹一般包含这些内容：`.h`，`.cpp`，`.build.cs`几类文件。其中`Public`，`Private`文件夹不是必须的。
`ModuleName.build.cs`文件包含模块的编译信息。

一个模块最少要包含以下几个文件：

![](Images/3.png)

通常都包含一个继承自`IModuleInterface`的类，用于注册到引擎中，当启动该模块时，作为模块的执行入口，并且在引擎关闭时，执行模块卸载，可以清理资源。
```cpp
#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(SecondModule, All, All);

class FSecondModule : public IModuleInterface
{
	public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;
};
```
```cpp
#include "SecondModule.h"

DEFINE_LOG_CATEGORY(SecondModule);

#define LOCTEXT_NAMESPACE "FSecondModule"

void FSecondModule::StartupModule()
{
	UE_LOG(SecondModule, Warning, TEXT("SecondModule module has been loaded"));
}

void FSecondModule::ShutdownModule()
{
	UE_LOG(SecondModule, Warning, TEXT("SecondModule module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSecondModule, SecondModule)
```
```cpp
using UnrealBuildTool;
 
public class SecondModule : ModuleRules
{
	public SecondModule(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine"});
 
		PublicIncludePaths.AddRange(new string[] {"SecondModule/Public"});
		PrivateIncludePaths.AddRange(new string[] {"SecondModule/Private"});
	}
}
```
最为关键的就是最后的`IMPLEMENT_MODULE`宏，它是把模块暴露给外部的。

## 3. 插件
### 3.1 什么是 UE 中的插件？
UE 中的插件是通过`uplugin`进行组织的一些模块（Module）集合。可以在引擎中进行方便地启用，能够基于引擎中的已有功能进行开发，也可以包含其他插件中的模块进行扩展。
一般情况下，从功能上进行区分，最常用的模块类型可以分为以下几类：
`Runtime`，引擎的运行时，在`Editor`和打包后都会运行；
`Developer`，在非`Shipping`阶段都会参与编译和运行，用于一些开发阶段的功能，在`Shipping`时会自动剔除，避免测试功能进入发行包；
`Editor`，在`Target`为`Editor`的目标程序加载的类型，如启动引擎的编辑器、Commandlet 等，都会启动`Editor`类型的模块。通常情况下，`Editor`类型的模块用于扩展编辑器功能，如创建独立的视口、给某些资源添加功能按钮、以及配置面板等等。
### 3.2 插件分类
单个插件可以包含多个不同类型的模块，如同时包含`Runtime/Developer/Editor`等类型，只需要在`uplugin`中描述即可。
在引擎的组织结构中，引擎内的插件位于`Engine/Plugins`目录下：

![](Images/4.png)

项目的工程则位于`PROJECT_DIR/Plugins`下：

![](Images/5.png)

引擎和项目中的插件有一定的依赖层级关系，需要控制好依赖层级，避免出现依赖混乱。

1. 引擎中的Module应该只依赖引擎内置的Module，不应该包含其他插件中的Module。
2. 引擎插件中的Module可以依赖引擎内置的 Module 和其他引擎内置插件中的Module，不应包含工程的Module
3. 工程插件的Module可以包含`引擎内置Module`/`引擎插件Module`/`其他工程插件的Module`，不应包含工程里定义的Module
4. 工程中的Modul 则可以包含以上所有 Module 的集合。

依赖关系如下图所示：

![](Images/6.png)

### 3.3 如何描述一个插件？
#### 3.3.1 uplugin
UE 中通过`uplugin`文件描述插件，它是一种基于`json`语法表示一个插件的组织结构和关键信息的配置。

1. 插件的命名、作者等基础信息
2. 插件模块的类型
3. 模块的启动时机
4. 对其他插件的依赖
5. 平台黑白名单
```json
{
  "FileVersion": 3,
  "Version": 1,
  "VersionName": "1.0",
  "FriendlyName": "C++ Module Generator",
  "Description": "An editor tool which allows game or plugin modules to be generated from within the editor.",
  "Category": "Other",
  "CreatedBy": "Sam Carey",
  "CreatedByURL": "https://samcarey.itch.io/",
  "DocsURL": "https://docs.google.com/document/d/1S0iQ4zKNqsqT5XwoW2836c_pnhFELW1eNchFOLbqP-8/edit?usp=sharing",
  "MarketplaceURL": "com.epicgames.launcher://ue/marketplace/product/ec36ec6e94f74075819e2a04f7cb97f3",
  "SupportURL": "www.samcarey.tech",
  "EngineVersion": "5.2.0",
  "CanContainContent": false,
  "Installed": true,
  "Modules": [
    {
      "Name": "ModuleGenerator",
      "Type": "Editor",
      "LoadingPhase": "Default",
      "PlatformAllowList": [
        "Win64",
        "Mac",
        "Linux"
      ]
    }
  ]
}
```
通过该信息，能清晰地知道插件中的模块数量、类型和启动时机。

- 在编译时，UBT也会读取插件的Type，决定模块是否参与编译。
- 在引擎启动时，会根据模块定义的信息进行启动。如`LoadingPhase`配置，在不同的启动阶段加载插件。
```cpp
namespace ELoadingPhase
{
    enum Type
    {
        /** As soon as possible - in other words, uplugin files are loadable from a pak file (as well as right after PlatformFile is set up in case pak files aren't used) Used for plugins needed to read files (compression formats, etc) */
        EarliestPossible,
    
        /** Loaded before the engine is fully initialized, immediately after the config system has been initialized.  Necessary only for very low-level hooks */
        PostConfigInit,
    
        /** The first screen to be rendered after system splash screen */
        PostSplashScreen,
    
        /** Loaded before coreUObject for setting up manual loading screens, used for our chunk patching system */
        PreEarlyLoadingScreen,
    
        /** Loaded before the engine is fully initialized for modules that need to hook into the loading screen before it triggers */
        PreLoadingScreen,
    
        /** Right before the default phase */
        PreDefault,
    
        /** Loaded at the default loading point during startup (during engine init, after game modules are loaded.) */
        Default,
    
        /** Right after the default phase */
        PostDefault,
    
        /** After the engine has been initialized */
        PostEngineInit,
    
        /** Do not automatically load this module */
        None,
    
        // NOTE: If you add a new value, make sure to update the ToString() method below!
        Max
	};
}
```
### 3.4 插件的目录结构
通常一个插件会包含以下几个目录和文件：

1. Content：可选，插件的`uasset`资源，类似于游戏工程的`Content`目录。需要`uplugin`中的CanContainContent=true
2. Resources：插件依赖的其他资源，如插件内的图标`Icon`等等
3. Config：可选，插件的配置文件，类似于工程的`Config`目录，用于存储一些ini的配置项
4. Source：插件中的代码目录
5. *.uplugin：当前插件的`uplugin`描述

需要重点关注的是`Source`目录，它是实现代码插件的关键。通常`Source`下会创建对应`Module`的目录，用于存储不同`Module`的代码，让不同的`Module`的文件组织相互隔离。
## 4. Unreal Beginplay
[BeginPlay | Learning path](https://dev.epicgames.com/community/learning/paths/0w/beginplay)

![](Images/7.png)
