# Unreal Engine C++ 资源加载详细教学文档（基于 UE 5.5.1 源码）

> 目标：面向**引擎源码阅读者**，系统讲清 UE C++ 中资源加载的概念、调用链、关键类、底层实现与工程实践，重点覆盖 `FStreamableManager`。  
> 代码基线：`D:\UnrealEngine-5.5.1-release`  
> 当前范围：**聚焦通用资源加载**，不展开地图/关卡加载、Level Streaming、World Partition。

---

## 目录

### 这篇文档怎么使用

如果你准备**从头系统读一遍**，最推荐顺序是：

```text
1 -> 4 先建立概念
5 -> 14 理解查询链、加载链、治理链
15 -> 25 进入排障、调试、实战、优化
26 -> 28 做练习、自测与最终收束
```

如果你准备**带着具体问题查阅**，可以按下面方式跳读：

- 想先搞懂“资源到底是什么、路径怎么写”：看 **1 ~ 4**
- 想先搞懂“真正怎么加载”：看 **6 ~ 10**
- 想先搞懂“项目层如何治理”：看 **11 ~ 14**
- 想先解决“为什么加载失败 / 为什么卡 / 为什么打包后出错”：看 **15、17、20、21、24、25**
- 想先拿到能直接改成项目代码的写法：看 **23**
- 想把知识变成自己的能力闭环：看 **26**

### 全文章节分区导读

从结构上看，这篇文档可以分成 5 个阅读区块：

- **基础认知篇（1 ~ 4）**：先把资源、路径、软硬引用这些最容易混的概念讲清
- **主链机制篇（5 ~ 14）**：把 AssetRegistry、LoadObject、LoadPackage、StreamableManager、AssetManager、Cook/Chunk/IoStore 串成主线
- **排障方法篇（15 ~ 22）**：从常见坑、面试追问、排障案例、调试日志、调用栈、类关系图入手，帮你建立“看问题的方法”
- **工程实践篇（23 ~ 25）**：给你模板、性能优化清单、排查决策树，把知识落到项目里
- **学习闭环篇（26 ~ 28）**：用练习题、自测题、源码追问题和最后总结，把整套知识真正收住

### 目录正文

1. [先建立整体认知：UE 资源加载到底在做什么](#1-先建立整体认知ue-资源加载到底在做什么)
2. [资源加载里的“资源”到底是什么](#2-资源加载里的资源到底是什么)
3. [路径体系：PackageName、ObjectPath、TopLevelAssetPath、SoftObjectPath](#3-路径体系packagenameobjectpathtoplevelassetpathsoftobjectpath)
4. [硬引用、软引用、软类引用：差异、代价与适用场景](#4-硬引用软引用软类引用差异代价与适用场景)
5. [AssetRegistry：资源发现层，不是加载器](#5-assetregistry资源发现层不是加载器)
6. [最直接的加载 API：`StaticLoadObject`、`LoadObject`、`LoadPackage`](#6-最直接的加载-api-staticloadobjectloadobjectloadpackage)
7. [`UPackage` 与 `FLinkerLoad`：传统包加载路线](#7-upackage-与-flinkerload传统包加载路线)
8. [异步加载总览：旧 AsyncLoading 与新 AsyncLoading2 并存](#8-异步加载总览旧-asyncloading-与新-asyncloading2-并存)
9. [新底层：`AsyncPackageLoader2`、`PackageStore`、`IoDispatcher`](#9-新底层-asyncpackageloader2packagestoreiodispatcher)
10. [重点章节：`FStreamableManager` 设计与实现原理](#10-重点章节-fstreamablemanager-设计与实现原理)
11. [`UAssetManager`：项目级资源调度中枢](#11-uassetmanager项目级资源调度中枢)
12. [Primary Asset 体系：Type、Rules、Bundle、Label、CookRule](#12-primary-asset-体系typerulesbundlelabelcookrule)
13. [Cook、Chunk、IoStore 如何改变运行时加载](#13-cookchunkiostore-如何改变运行时加载)
14. [Lyra 案例：项目层如何站在引擎加载机制之上](#14-lyra-案例项目层如何站在引擎加载机制之上)
15. [常见坑与调试方法](#15-常见坑与调试方法)
16. [高频源码追问 / 面试题视角](#16-高频源码追问--面试题视角)
17. [排障案例集：从现象反推加载链路](#17-排障案例集从现象反推加载链路)
18. [推荐阅读顺序 + 源码阅读路线图](#18-推荐阅读顺序--源码阅读路线图)
19. [术语对照表 / 容易混淆概念表](#19-术语对照表--容易混淆概念表)
20. [资源加载调试日志与常用断点清单](#20-资源加载调试日志与常用断点清单)
21. [从调用栈角度看一次完整资源加载的时序拆解](#21-从调用栈角度看一次完整资源加载的时序拆解)
22. [资源加载相关核心类关系图（类职责总表 + 调用关系图）](#22-资源加载相关核心类关系图类职责总表--调用关系图)
23. [资源加载实战代码模板库（同步/异步/软类/Primary Asset/批量加载）](#23-资源加载实战代码模板库同步异步软类primary-asset批量加载)
24. [资源加载性能优化清单（卡顿来源、预加载策略、避免重复加载、生命周期与缓存设计）](#24-资源加载性能优化清单卡顿来源预加载策略避免重复加载生命周期与缓存设计)
25. [资源加载问题排查决策树（出现某种现象时应该先查哪一层）](#25-资源加载问题排查决策树出现某种现象时应该先查哪一层)
26. [资源加载学习闭环：建议练习题 + 自测题 + 源码追问题](#26-资源加载学习闭环建议练习题--自测题--源码追问题)
27. [关键源码索引（按主题）](#27-关键源码索引按主题)
28. [最后总结：一条真正的 UE 资源加载主线](#28-最后总结一条真正的-ue-资源加载主线)

---

## 1. 先建立整体认知：UE 资源加载到底在做什么

很多人第一次接触 UE 的资源加载，会把它理解成一句话：

> “给一个路径，然后把资源加载出来。”

这句话**不算错，但非常不够**。

从源码角度看，UE 资源加载至少包含下面几层问题：

1. **资源如何被标识**  
   例如 `/Game/UI/WBP_MainMenu.WBP_MainMenu_C`、`/Game/Data/DA_Item.DA_Item`、`/Game/Foo.Bar:SubObject`。

2. **资源如何被发现**  
   也就是“磁盘上/注册表里有哪些资产、它们的类是什么、有哪些依赖、属于哪个 Chunk、有哪些 Bundle”。

3. **资源如何被调度加载**  
   例如同步加载、异步加载、批量加载、优先级控制、取消、等待、Handle 生命周期。

4. **包如何被打开与解析**  
   也就是 `UPackage`、导入表、导出表、依赖、序列化、`PostLoad` 这些底层机制。

5. **Cook 后的运行时与编辑器中的未 Cook 环境有什么区别**  
   在编辑器里常常是“扫描磁盘 + 随时找包文件”；到了运行时则更依赖裁剪后的 AssetRegistry、PackageStore、IoStore。

6. **项目层如何做资源治理**  
   比如按主资源类型扫描、按规则分 Chunk、按 Bundle 预加载、按场景分批装载。

如果把 UE 资源加载画成一个大图，可以先记住下面这条链：

```text
磁盘/IoStore 中的包数据
        │
        ▼
AssetRegistry（发现、查询、依赖、管理关系）
        │
        ├── 项目层：UAssetManager（Primary Asset、规则、Bundle、Cook/Chunk）
        │
        ├── 通用加载层：FStreamableManager（围绕软路径的异步/同步请求管理）
        │
        └── 直接入口：StaticLoadObject / LoadObject / LoadPackage / LoadPackageAsync
                                │
                                ▼
                    包加载器 / 异步加载器
                ├── 传统：FLinkerLoad / AsyncLoading
                └── 新路线：AsyncPackageLoader2 + PackageStore + IoDispatcher
                                │
                                ▼
                       UPackage / UObject 实例化
                                │
                                ▼
                   序列化 -> Preload -> PostLoad -> 可用对象
```

这篇文档的核心目的，就是把这条链从上到下讲清。

在继续往下之前，建议先把 UE 资源加载里最常被混淆的三条链分清：

```text
查询链：AssetRegistry -> FAssetData -> 依赖/标签/路径
加载链：SoftPath / LoadObject / LoadPackage / Streamable -> Package Loader -> UObject
治理链：AssetManager -> PrimaryAsset -> Rules / Bundle / Chunk / Cook
```

后面阅读源码时，凡是看不清某个类在干什么，就先问自己一句：

> 它属于查询链、加载链，还是治理链？

---

## 2. 资源加载里的“资源”到底是什么

在 UE 中，“资源”不是一个单一概念，而是几个层次叠在一起：

### 2.1 文件层

最外层，你可以把资源理解为一个存储在包里的资产。历史上常对应 `.uasset` / `.umap` / `.uexp` / `.ubulk` 等文件组合；在 IoStore 体系下，则不再是简单的一文件一资产的直观模型，而是更偏向容器化的包数据组织。

### 2.2 包层：`UPackage`

UE 运行时真正以“包”为基本装载单位。  
在 `Engine\Source\Runtime\CoreUObject\Public\UObject\Package.h` 里，`UPackage` 就是运行时对包的对象表示。

理解这点非常重要：

- 你想加载一个对象；
- UE 往往不是只把“那个对象的字节”单独抓出来；
- 它会先确保**该对象所在包**被加载；
- 然后再从包中查找顶层对象，必要时触发进一步序列化与后处理。

这也是为什么 `StaticLoadObjectInternal` 里会出现：

```cpp
LoadPackage(NULL, *InOuter->GetOutermost()->GetName(), ...);
```

意思非常直接：  
**既然对象属于这个包，那就先把整个包加载好。**

### 2.3 顶层对象层：Asset / Top-level Object

一个包中通常有一个与包同名的主资产对象，比如：

```text
/Game/Data/DA_Item.DA_Item
```

这里：

- 包名：`/Game/Data/DA_Item`
- 资产对象名：`DA_Item`

UE5 中大量 API 开始强调 `FTopLevelAssetPath`，就是为了把：

- PackageName
- AssetName

这两个概念分开，而不是继续把它们挤成一个旧式字符串。

### 2.4 子对象层：Subobject

有些路径后面还会带 `:SubPath`，例如：

```text
/Game/Foo.Bar:SomeSubObject
```

这表示它不是单纯指向包中的顶层对象，而是指向该对象下的子对象路径。

### 2.5 运行时对象层：`UObject`

最终你真正拿到的是内存中的 `UObject*`，比如：

- `UTexture2D*`
- `USkeletalMesh*`
- `UBlueprint*`
- `UClass*`
- `UPrimaryDataAsset*`

“资源加载成功”本质上是：

> 相关包被定位并打开，导出对象被构造和反序列化，完成 `PostLoad` 等生命周期后，得到可用的 UObject 实例。

---

## 3. 路径体系：PackageName、ObjectPath、TopLevelAssetPath、SoftObjectPath

这一章非常关键，因为后面所有加载 API 本质都在围绕“路径”工作。

### 3.1 `FSoftObjectPath` 的真实含义

在 `Engine\Source\Runtime\CoreUObject\Public\UObject\SoftObjectPath.h` 中，类注释写得非常明确：

> 它内部由一个 `FTopLevelAssetPath`（顶层资产路径）和一个可选的 subobject path 组成。

也就是说，`FSoftObjectPath` 不是“随便一段字符串”，而是：

```text
TopLevelAssetPath + OptionalSubPath
```

源码中可直接看到：

- `GetAssetPath()`
- `GetWithoutSubPath()`
- `GetLongPackageName()`
- `GetAssetName()`
- `GetSubPathString()`

所以你应该把 `FSoftObjectPath` 理解成：

> “一个可序列化、可延迟解析、可用于软引用和异步加载的对象路径描述器”。

### 3.2 常见路径形式

#### 形式 1：只写包路径

```text
/Game/Data/DA_Item
```

这通常不够精确，因为它没有明确对象名。

#### 形式 2：顶层对象路径

```text
/Game/Data/DA_Item.DA_Item
```

这是最常见的资产路径形式。

#### 形式 3：蓝图生成类

```text
/Game/UI/WBP_MainMenu.WBP_MainMenu_C
```

这里的 `_C` 非常关键，表示你要的是**生成出来的类对象**，不是蓝图资产本体。

#### 形式 4：带子对象路径

```text
/Game/Foo.Bar:SubObject
```

这表示对象内部的子对象。

### 3.3 `FTopLevelAssetPath`

`FTopLevelAssetPath` 更偏向**顶层资产的强结构化标识**：

- 包名
- 资产名

分开存储，避免旧式字符串拼拆的脆弱性。

很多新接口已经偏好使用它，例如：

- `StaticLoadAsset(UClass*, FTopLevelAssetPath, ...)`
- `LoadAssetAsync(FTopLevelAssetPath, ...)`

这代表 UE 的方向是：

> 顶层对象路径应尽量结构化，而不是到处传原始字符串。

### 3.4 `FSoftObjectPath::TryLoad()`

在 `SoftObjectPath.h` 里已经写得很清楚：

- `TryLoad()` 会调用 `LoadObject`
- `LoadAsync()` 会调用 `LoadAssetAsync`

这说明：

- `FSoftObjectPath` 不只是一个被动数据结构；
- 它也是连接“路径描述”和“实际加载动作”的入口。

但是要注意，源码注释同时提醒：

> 如果你需要更强的控制能力，比如回调时机、GC 持有、批量请求控制，那么应该用 `FStreamableManager`。

这也就是为什么本文后面要把 `FStreamableManager` 单独拉出来重点讲。

### 3.5 蓝图资产、GeneratedClass 与 `_C`（高频坑专章）

这是 UE 资源加载里最容易把人绕晕的一部分。

很多人会混淆下面四个对象：

1. **蓝图资产本体**：`UBlueprint`
2. **骨架类**：`SkeletonGeneratedClass`
3. **真正运行时类**：`GeneratedClass` / `UBlueprintGeneratedClass`
4. **类默认对象**：`Default__XXX_C`

先看源码：

在 `Engine\Source\Runtime\Engine\Classes\Engine\BlueprintCore.h` 里，`UBlueprintCore` 明确持有：

```cpp
TSubclassOf<class UObject> SkeletonGeneratedClass;
TSubclassOf<class UObject> GeneratedClass;
```

这说明蓝图资产本身并不是“最终要实例化的类”，它更像是：

> 编辑器资产 + 编译产物入口对象

而在 `Engine\Source\Runtime\Engine\Classes\Engine\Blueprint.h` 里，`GetBlueprintClassNames()` 又清楚写出了命名规则：

```cpp
const FString GeneratedClassNameString = FString::Printf(TEXT("%s_C"), *NameToUse.ToString());
const FString SkeletonClassNameString = FString::Printf(TEXT("SKEL_%s_C"), *NameToUse.ToString());
```

同时，`GetBlueprintCDONames()` 还说明了 CDO 的命名方式：

```cpp
Default__%s_C
Default__SKEL_%s_C
```

这几段源码几乎把 `_C` 的本质一次性讲透了：

- `MyBP`：蓝图资产名
- `MyBP_C`：编译生成后的类名
- `Default__MyBP_C`：这个生成类的 CDO
- `SKEL_MyBP_C`：骨架类，用于编辑器编译/反射相关流程

#### 一个最重要的结论

如果你要：

- **读取蓝图资产本体**
  - 你要找的是 `UBlueprint`
  - 路径更像：`/Game/UI/WBP_MainMenu.WBP_MainMenu`

- **拿到可实例化的类**
  - 你要找的是生成类
  - 路径更像：`/Game/UI/WBP_MainMenu.WBP_MainMenu_C`

如果你把这两者混了，就会出现典型问题：

- 你“明明加载成功了”
- 但拿到的是 `UBlueprint*`
- 却想把它当 `UClass*`、`TSubclassOf<>`、`CreateWidget`、`SpawnActor` 的输入

这时往往就会崩在类型不匹配上。

#### `UBlueprintGeneratedClass` 到底是什么

在 `Engine\Source\Runtime\Engine\Classes\Engine\BlueprintGeneratedClass.h` 中：

```cpp
class UBlueprintGeneratedClass : public UClass
```

这句话非常关键，因为它说明：

> 蓝图生成类不是“某种特殊资源对象”，它本质上就是一个 `UClass`。

只是这个 `UClass` 的来源不是原生 C++ 编译，而是蓝图编译流程产物。

因此很多项目代码真正想要的并不是蓝图资产，而是：

> 一个能像普通 `UClass` 一样被使用的 `UBlueprintGeneratedClass`

#### 为什么配置里大量出现 `_C`

看 Lyra 的 `DefaultGame.ini`：

```ini
DefaultUIPolicyClass=/Game/UI/B_LyraUIPolicy.B_LyraUIPolicy_C
ConfirmationDialogClass=/Game/UI/Foundation/Dialogs/W_ConfirmationDefault.W_ConfirmationDefault_C
```

这些配置项之所以写 `_C`，就是因为它们要的是：

- Widget 类
- Policy 类
- 可实例化类对象

而不是蓝图资产本体。

#### 一个实战判断口诀

可以强行记一句：

```text
想看蓝图资源本体：不要 _C
想拿来实例化/当类用：通常要 _C
```

当然，“通常”不是“绝对”，但对于大多数项目代码排障场景，这句口诀已经很实用了。

#### 一条非常有用的排障路径

当你遇到“软类引用加载出来不能用”时，优先检查：

1. 路径是不是指向蓝图资产而不是生成类  
2. 配置项类型是不是 `TSoftClassPtr<>` / `TSubclassOf<>`  
3. 代码最终想要的是 `UBlueprint*` 还是 `UClass*`  
4. 是否误把 `Default__XXX_C` 当成普通类路径使用

这类问题看起来像“资源没加载出来”，实际上经常是：

> 资源加载对了，但对象类型层级理解错了。

---

## 4. 硬引用、软引用、软类引用：差异、代价与适用场景

这一章如果只从项目使用角度讲，很容易流于“会用就行”；但从引擎源码角度，必须搞清楚它们对加载系统意味着什么。

### 4.1 硬引用

典型形式：

```cpp
UPROPERTY(EditAnywhere)
TObjectPtr<UTexture2D> Icon;
```

硬引用意味着：

- 资源依赖是显式可追踪的；
- Cook 和引用分析更容易把它纳入依赖图；
- 当拥有者被加载时，相关依赖通常也更容易被一并拉起；
- 但会增加耦合和启动/切换时的加载成本。

### 4.2 软引用：`TSoftObjectPtr<T>`

`TSoftObjectPtr` 本质上是：

> 路径 + 弱引用语义

它不是已加载对象的强保活句柄。

源码层面的关键点：

- 它内部依赖 `FSoftObjectPath`
- `LoadSynchronous()` 最终会回到 `TryLoad()` / `StaticLoadObject`
- 它能指向未加载资产
- 它本身不等价于“对象已在内存中”

适合场景：

- UI 图标、配置资产、按需加载内容
- 启动后分阶段装载内容
- 想延迟依赖、降低初始包体耦合

### 4.3 软类引用：`TSoftClassPtr<T>`

这是软引用的“类版本”，尤其适合：

- 蓝图类
- 可配置玩法类
- 延迟实例化的 Actor / Widget / Ability 类

最容易踩的坑是：

- 蓝图资产路径和蓝图生成类路径不是一回事；
- 你要实例化类时，常常真正需要的是 `_C` 对应的生成类。

### 4.4 一个非常实用的判断标准

如果你问自己：

> 我现在手上的这个成员，是不是希望“只保存地址描述，真正用到时再加载”？

如果答案是“是”，那你大概率需要的是软引用。

如果你问：

> 我是不是要保证这个对象随着拥有者一同存在，并让依赖关系进入包依赖图？

那大概率更接近硬引用。

---

## 5. AssetRegistry：资源发现层，不是加载器

这是整个资源加载体系里最容易被误解的模块之一。

### 5.1 `IAssetRegistry` 的官方定位

在 `Engine\Source\Runtime\AssetRegistry\Public\AssetRegistry\IAssetRegistry.h` 中，注释非常关键：

> 它是一个全局单例接口，用来访问所有已挂载目录中存储的包与内容文件目录信息。  
> 在编辑器中，这些信息在启动时通过扫描磁盘获得。  
> 在 Cooked 运行时，这些信息是在 Cook 阶段预计算并序列化出来的。

这句话可以拆成两个重要结论：

1. **AssetRegistry 是目录/索引/元数据系统**
2. **它不是对象加载器**

### 5.2 编辑器与运行时的差异

`IAssetRegistry` 注释还明确说明：

- 编辑器：启动时 gather
- Cooked runtime：读取裁剪后的单一序列化文件

这意味着：

- 编辑器里 AssetRegistry 更像“动态扫描 + 缓存视图”
- 运行时里 AssetRegistry 更像“Cook 产出的查询数据库”

### 5.3 `FAssetData` 是什么

`FAssetData` 可以理解为“某个资产在注册表中的记录条目”。  
它通常包含：

- PackageName
- AssetName
- AssetClassPath
- Tags
- ChunkIDs
- Bundle 信息

非常关键的一点：

> `FAssetData` 不是 `UObject*`。

你可以把它当作：

> “资源身份证”，不是“资源本体”。

### 5.4 `bIncludeOnlyOnDiskAssets`

`IAssetRegistry.h` 头部注释对这个参数解释得很细：

- `true`：只用磁盘/注册表中的数据
- `false`：如果对象已经在内存里，可能返回基于内存对象计算出的 AssetData

这背后意味着：

- AssetRegistry 查询不总是“纯磁盘视角”
- UE 在一些接口里会尝试拿“更接近当前运行时状态”的信息
- 但只要你希望稳定、快速、偏注册表视角，就应偏向 `OnDiskAssets`

### 5.5 它到底用来干什么

最常见的职责有四类：

1. **发现资产**
2. **读元信息**
3. **分析依赖关系**
4. **给 AssetManager 提供扫描与管理基础**

特别是 `Manage` 依赖，对 `UAssetManager` 来说非常重要，因为 Primary Asset 的管理图是构建在这层信息之上的。

---

## 6. 最直接的加载 API：`StaticLoadObject`、`LoadObject`、`LoadPackage`

这一章开始正式进入“真正加载”。

### 6.1 `StaticLoadObjectInternal` 的核心思路

关键文件：

- `Engine\Source\Runtime\CoreUObject\Private\UObject\UObjectGlobals.cpp`

`StaticLoadObjectInternal` 的逻辑主线可以概括为：

```text
输入路径
  -> ResolveName2 解析
  -> 如果 Outer 已知，先尝试在内存里找对象
  -> 找不到则加载其最外层包
  -> 再次在包里查找对象
  -> 如有需要，处理 redirector
  -> 返回 UObject
```

源码中的几个关键点：

#### 1）先做名字解析

```cpp
ResolveName2(...)
```

#### 2）能从内存拿就先从内存拿

```cpp
Result = StaticFindObjectFast(...)
```

#### 3）找不到才加载包

```cpp
LoadPackage(NULL, *InOuter->GetOutermost()->GetName(), ...)
```

这是最重要的一行之一。它说明：

> `StaticLoadObject` 其实是“对象查询 + 必要时触发包加载”。

#### 4）如果传的是包路径而不是完整对象路径

源码里有一段逻辑：

```cpp
Assume that the object we're trying to load is the main asset inside of the package
```

也就是说，如果你只传 `/Game/Data/DA_Item`，它会尝试补成：

```text
/Game/Data/DA_Item.DA_Item
```

### 6.2 `StaticLoadObject` 与 `LoadObject<T>`

`LoadObject<T>` 本质上只是模板包装，最后还是走 `StaticLoadObject`。

### 6.3 `LoadPackageInternal`

`LoadPackageInternal` 更接近“包级别加载总入口”。

重要的是，UE5.5 里它已经不是简单的“老式同步读包函数”。

在 `UObjectGlobals.cpp` 中可以看到：

```cpp
if (ShouldAlwaysLoadPackageAsync(PackagePath))
{
    int32 RequestID = LoadPackageAsync(...);
    FlushAsyncLoading(RequestID);
}
```

这段逻辑揭示了一个非常重要的事实：

> **同步 API 不代表底层一定走纯同步实现。**

更准确的说法是：

> 在 UE5.5 中，某些同步包加载会通过异步加载器发起请求，再主动 `FlushAsyncLoading` 等它完成。

### 6.4 `LoadAssetAsync`

`LoadAssetAsync(FTopLevelAssetPath, ...)` 的逻辑也值得看一眼：

它先把资产路径转成包路径，然后调用 `LoadPackageAsync`，等包回来后再：

```cpp
StaticFindObjectFast(UObject::StaticClass(), LoadedPackage, InAssetPath.GetAssetName(), false);
```

也就是说：

> 资产异步加载实际上仍然是“先加载包，再在包中查找目标顶层对象”。

### 6.5 几个常见加载入口的职责对照

很多源码阅读初学者会把这些 API 混成一团。更好的办法是先建立一个职责对照：

| API | 更接近哪一层 | 典型输入 | 主要职责 |
|---|---|---|---|
| `StaticLoadObject` / `LoadObject<T>` | 对象入口层 | 对象路径 | 直接按对象路径查找/触发包加载 |
| `LoadPackage` | 包入口层 | 包路径 | 确保整个包被加载 |
| `LoadAssetAsync` | 资产异步入口层 | `FTopLevelAssetPath` | 异步加载包，再取顶层对象 |
| `FSoftObjectPath::TryLoad()` | 软路径便捷入口 | `FSoftObjectPath` | 从软路径直接触发同步加载 |
| `FStreamableManager` | 工程化调度层 | 一组软路径 | 管理批量请求、Handle、GC、同步/异步桥接 |
| `UAssetManager` | 项目治理层 | `PrimaryAssetId` / Bundle / Type | 按项目规则组织加载与 Cook/Chunk 策略 |

记住这个表，会极大降低你后面读源码时的迷路概率。

### 6.6 一条更细的同步对象加载时序图

如果你想把 `StaticLoadObject` 的行为真正“脑内动画化”，可以用下面这张时序图来理解：

```text
调用者
  -> StaticLoadObject
      -> StaticLoadObjectInternal
          -> ResolveName2
          -> StaticFindObjectFast（先查内存）
              -> 找到且已可用：直接返回
              -> 找不到 / 仍需加载：
                   -> LoadPackage / LoadPackageInternal
                        -> 可能转交 LoadPackageAsync
                        -> 可能 FlushAsyncLoading(RequestId)
                   -> StaticFindObjectFast（回包后再查一次）
                   -> 如有需要处理 Redirector
          -> 返回 UObject*
```

这张图最想强调三件事：

1. **总是先尝试内存命中**
2. **对象加载经常会退化为包加载**
3. **同步入口内部可能借异步底层完成**

---

## 7. `UPackage` 与 `FLinkerLoad`：传统包加载路线

如果你要真正理解 UE 的资源加载，`FLinkerLoad` 无论如何都绕不过去。

### 7.1 为什么传统路线仍然必须讲

虽然 UE5.5 明显在向新的包加载体系演进，但 `FLinkerLoad` 依然重要，因为：

1. 它是理解“包内部结构”的经典入口
2. 旧异步加载系统与它强耦合
3. 新路线里仍保留对它的兼容/支持痕迹
4. 编辑器和一些 fallback 场景里它仍然有现实存在感

### 7.2 它负责什么

在传统路线中，`FLinkerLoad` 的职责大致是：

- 打开包资源
- 读取包摘要
- 建立 NameMap
- 建立 ImportMap
- 建立 ExportMap
- 处理 Depends 信息
- 为对象创建/预加载/后加载提供支撑

你可以把它理解成：

> “包解析器 + 对象导入导出装配器”。

### 7.3 `ProcessPackageSummary`

`LinkerLoad.cpp` 中的 `ProcessPackageSummary` 是非常关键的入口。  
这里会处理：

- 包头
- 名称表
- 导入表
- 导出表
- 依赖关系
- 软对象路径列表等

### 7.4 导入 / 导出思维模型

可以把传统包加载理解为：

```text
包文件
  ├── 名称表：名字池
  ├── 导入表：我依赖外部哪些对象
  ├── 导出表：我能提供哪些对象
  └── 依赖信息：加载顺序与关系
```

然后加载器围绕这些表去：

- 构造对象壳
- 反序列化属性
- 解决引用
- 调用 `PostLoad`

---

## 8. 异步加载总览：旧 AsyncLoading 与新 AsyncLoading2 并存

这部分是 UE5 阅读源码时最容易混乱的地方。

### 8.1 不要用“旧的全没了、新的全替代了”来理解

UE5.5 的现实情况是：

- 上层 API 仍然是那套大家熟悉的接口
- 底层加载器在演进
- 新旧路线并存
- 某些场景 fallback 到旧路线

### 8.2 `IAsyncPackageLoader`

`AsyncPackageLoader.h` 定义的是统一异步包加载接口。  
它的意义在于：

- 屏蔽不同底层实现差异
- 提供统一的 `LoadPackageAsync` 能力
- 让上层调用者不必关心你当前到底是老 loader 还是新 loader

### 8.3 `InitAsyncThread()`：真正的选择总闸门

`Engine\Source\Runtime\CoreUObject\Private\Serialization\AsyncPackageLoader.cpp` 中的 `InitAsyncThread()` 非常关键。

可以看到它会根据：

- `FIoDispatcher` 是否初始化
- 是否禁用 `NoZenLoader`
- 编辑器设置
- 是否存在脚本对象 chunk
- fallback 配置

来选择：

- `MakeAsyncPackageLoader2(IoDispatcher)`
- `MakeEditorPackageLoader(IoDispatcher)`
- `FAsyncLoadingThread`

### 8.4 为什么这件事对阅读源码很重要

因为你会遇到这种现象：

- 某些调用链读着读着进入 `AsyncLoading2.cpp`
- 另一些地方又看到 `FLinkerLoad`
- 再往上看，又都是 `LoadPackageAsync`

这并不矛盾，它们是**演进中的分层兼容结构**。

---

## 9. 新底层：`AsyncPackageLoader2`、`PackageStore`、`IoDispatcher`

这一章讲的是 UE5.5 更现代的运行时加载底座。

### 9.1 为什么需要新路线

传统按文件、按 Linker 思维组织的加载方式，在大型项目、容器化存储、并行调度、Cook 优化等方面会逐渐显露限制。

新路线的核心目标可以概括成：

- 更适合 IoStore / 容器化数据组织
- 更适合异步并行加载
- 更适合在 Cook 后运行时使用预计算元数据
- 更清晰地拆分“包索引/存储定位/IO调度/对象构造”职责

### 9.2 `PackageStore`

`PackageStore` 可以理解为：

> 运行时包元信息数据库

它保存的不是“对象实例”，而是更接近：

- 包 ID
- 导入包信息
- 软包引用
- 预加载依赖

### 9.3 `IoDispatcher`

`IoDispatcher` 是新 IO 路线的核心分发层。  
你可以把它理解成：

> “面向包/块数据请求的统一 IO 调度器”。

### 9.4 新路线不是完全排斥旧概念

在 `AsyncLoading2.cpp` 中仍可以看到诸如 `ALT2_ENABLE_LINKERLOAD_SUPPORT` 之类的兼容痕迹。

这说明：

> 新路线不是凭空从零开始的另一个世界，而是在旧对象模型之上逐步替换加载后端与调度机制。

---

## 10. 重点章节：`FStreamableManager` 设计与实现原理

这一章是全篇重点。

很多项目层开发其实并不直接调用底层包加载器，而是围绕：

- `TSoftObjectPtr`
- `TSoftClassPtr`
- `FStreamableManager`
- `UAssetManager`

来组织资源加载流程。

其中 `FStreamableManager` 是最常见、也最值得深读的核心类。

### 10.1 它到底解决什么问题

如果只有 `LoadObject` / `LoadPackageAsync`，项目层仍然会遇到很多麻烦：

- 软路径批量加载怎么合并？
- 同一个资源被多处请求时怎么复用？
- 已加载资源如何在 Handle 活跃期间避免被 GC 回收？
- 异步回调、取消、等待、优先级如何统一？
- 一个加载请求如何表达“多资源联合完成”？

`FStreamableManager` 就是为这些问题而生的。

你可以把它理解成：

> “围绕 `FSoftObjectPath` 的资源加载编排器与生命周期管理器”。

### 10.2 `FStreamableHandle`：它不是简单回调票据

在 `StreamableManager.h` 中，`FStreamableHandle` 是一个功能很完整的请求句柄：

- `HasLoadCompleted()`
- `WasCanceled()`
- `IsLoadingInProgress()`
- `IsActive()`
- `IsStalled()`
- `IsCombinedHandle()`
- `ReleaseHandle()`
- `CancelHandle()`
- `StartStalledHandle()`
- `WaitUntilComplete()`
- `GetRequestedAssets()`
- `GetLoadedAssets()`
- `GetLoadedCount()`
- `GetProgress()`

这已经说明它不是“一个回调壳”，而是：

> **加载请求的运行时对象模型。**

从设计思想上说，UE 把“资源加载结果”和“资源加载过程”拆开了：

- 结果是 `UObject*`
- 过程由 `FStreamableHandle` 建模

这让引擎能够在“对象还没回来时”就先把请求管理起来。

### 10.3 为什么 Handle 很重要

如果你只用一次性回调，很难表达下面这些语义：

- 这个请求是否仍活跃
- 是否允许取消
- 是否允许晚一点释放
- 我是否要阻塞等待它完成
- 它当前完成进度是多少
- 是否是多个子请求组合而成

而 `FStreamableHandle` 把这些都统一了。

你可以把它的状态变化粗略理解成：

```text
创建请求
  -> Loading
  -> Completed
       ├── 保持 Active（继续保活对象）
       └── ReleaseHandle（允许释放）
  -> 或 Canceled
```

其中要特别区分两个动作：

- `ReleaseHandle()`：更像“我用完了，可以放手”
- `CancelHandle()`：更像“我不想要这个请求继续产生完成语义了”

### 10.4 `RequestAsyncLoadInternal()` 的核心流程

在 `StreamableManager.cpp` 中，`RequestAsyncLoadInternal()` 做了很多关键工作：

1. 创建 `FStreamableHandle`
2. 保存请求资产列表、调试名、优先级
3. 校验路径是否合法
4. 去重目标路径
5. 为每个目标调用 `StreamInternal()`
6. 处理完成回调、子 Handle、组合 Handle 等逻辑

其真实含义是：

> `FStreamableManager` 不是“直接加载某资源”，而是“先创建请求对象，再把请求分发到具体目标上”。

另外，从函数签名也能看出几个项目层很重要的开关：

- `Priority`
- `bManageActiveHandle`
- `bStartStalled`
- `DebugName`

这些参数说明 StreamableManager 的目标从来都不是“最短路径加载一个对象”，而是“让大项目能把加载当成一个可调度系统来管理”。

### 10.5 `StreamInternal()`：它如何连接到真正的包加载

`StreamInternal()` 是 `FStreamableManager` 的关键桥梁。

它的逻辑非常值得逐步理解：

#### 第一步：先找缓存

```cpp
FStreamable* Existing = StreamableItems.FindRef(TargetName);
```

说明 `FStreamableManager` 内部会维护一个 `StreamableItems` 映射，按路径管理流对象状态。

这意味着：

> 它不是“每次都盲发请求”，而是有自己的状态缓存层。

#### 第二步：如果已存在且已加载，直接复用

```cpp
if (Existing->Target)
{
    return Existing;
}
```

#### 第三步：必要时先尝试从内存中找

```cpp
FindInMemory(TargetName, Existing);
```

#### 第四步：决定同步还是异步

源码中有一段非常重要：

```cpp
if (GIsInitialLoad || ThreadContext.IsInConstructor > 0 || bForceSynchronousLoads)
{
    Existing->Target = StaticLoadObject(...);
}
else
{
    Existing->RequestId = LoadPackageAsync(...);
}
```

这说明：

`FStreamableManager` 并不是纯异步工具，它本质是：

> “统一包装同步与异步两种路径的流式资源请求管理器”。

这也解释了为什么项目里常见下面这种现象：

- 某些场景直接 `LoadSynchronous`
- 某些场景 `RequestAsyncLoad`
- 但底层都能复用同一套 manager 与缓存状态

#### 一条更细的 `StreamInternal()` 时序图

```text
业务代码
  -> RequestAsyncLoad / LoadSynchronous
      -> RequestAsyncLoadInternal
          -> 创建 FStreamableHandle
          -> 遍历 RequestedAssets
              -> StreamInternal(Target)
                   -> ResolveRedirects
                   -> 查 StreamableItems 缓存
                   -> FindInMemory
                   -> 若必须同步：
                        -> StaticLoadObject
                   -> 否则异步：
                        -> 提取 PackageName
                        -> FPackagePath::TryFromPackageName
                        -> LoadPackageAsync
                        -> 保存 RequestId
          -> Handle 追踪子请求
          -> 包加载完成回调
              -> AsyncLoadCallbackWrapper
              -> 更新 Handle 状态 / 触发完成委托
```

把这张图吃透之后，再读 `StreamableManager.cpp` 时，很多“为什么这里又在改 Handle、为什么这里又查缓存、为什么这里又转包路径”的疑问都会自然消失。

### 10.6 它为什么围绕“包”而不是“对象字节”

在异步分支中，`StreamInternal()` 会先把 `FSoftObjectPath` 转成包名，再构造 `FPackagePath`，然后调用：

```cpp
LoadPackageAsync(PackagePath, ...)
```

完成回调里再由 Handle 的逻辑把目标对象取出来。

这说明 `FStreamableManager` 与 `LoadAssetAsync`、`StaticLoadObject` 一样，仍然遵循：

> **先包、后对象** 的 UE 资源加载基本规律。

这一点也顺带解释了一个常见误解：

> “既然我请求的是一个贴图，为什么底层在处理 PackagePath？”

答案是：  
因为在 UE 的对象模型里，“贴图对象”并不是脱离包独立存在的，它要先回到包，再从包里找到对应导出对象。

### 10.7 `FStreamableHandle::WaitUntilComplete()`

这个接口很值得重视，因为它说明：

- `FStreamableManager` 可以发起异步请求；
- 但你也可以把 Handle 当成“可控等待点”；
- 比直接暴力 `LoadObject` 更灵活，因为你仍然保留了请求对象语义。

这也是启动流程里很实用的能力：

- 先统一发起多个请求
- 再在某个明确阶段等待它们完成
- 而不是在任意业务代码里散落同步阻塞点

这也顺带解释了为什么很多成熟项目会避免把 `LoadObject` 散落在业务逻辑里，而更愿意：

1. 先把资源请求建模成 Handle  
2. 再在“启动阶段 / UI切换阶段 / 状态进入点”统一等待

### 10.8 GC 持有：为什么它如此常用

`FStreamableManager::AddReferencedObjects()` 中有一段极重要的代码：

```cpp
Collector.AddStableReference(&Pair.Value->Target);
```

它说明：

> 当 `FStreamableManager` 仍持有活动 streamable/handle 时，它会把目标对象作为稳定引用提交给 GC。

这件事太关键了，原因是：

- 软引用本身不保活
- 但项目层往往希望“请求完成后，直到 Handle 释放前，对象仍然安全可用”

`FStreamableManager` 恰好填补了这层语义空白。

换句话说：

- `TSoftObjectPtr` 解决“怎么描述”
- `FStreamableManager` 解决“怎么安全持有到我用完”

### 10.9 Redirect 处理

`StreamableManager.cpp` 中还维护了 `StreamableRedirects`。  
这表示它不仅管理加载状态，还处理资源重定向。

这也是一个很典型的“项目层工程化收益点”：

- 如果你只用最底层 API，自行处理 redirect 会非常分散
- 而通过统一 manager，可以把路径修正与状态复用放在同一个中心

### 10.10 Combined Handle / Stalled Handle 为什么值得关注

从 `FStreamableHandle` 暴露的接口就能看出，它不仅支持普通请求，还支持：

- `IsCombinedHandle()`
- `IsStalled()`
- `StartStalledHandle()`

这意味着它可以表达更复杂的请求关系：

1. **Combined Handle**  
   多个子请求组合成一个“父请求完成语义”。

2. **Stalled Handle**  
   请求对象先创建，但暂不真正下发加载，等外部条件满足后再启动。

对于项目层来说，这两种能力很重要，因为真实业务往往不是：

> “现在马上加载单个资源”

而是：

> “先组装一个资源集，等用户进入某状态再统一启动；全部完成后再推进流程。”

### 10.11 一个最小可用心智模型

如果你在阅读 StreamableManager 源码时开始混乱，可以强行套用下面这套心智模型：

```text
Path（我要谁）
  -> Streamable（这个路径当前状态如何）
  -> Handle（这次请求本身如何管理）
  -> PackageAsync / StaticLoad（真正怎么触发）
  -> UObject（最后拿到谁）
```

只要不把这五层混在一起，读 `StreamableManager.cpp` 会顺很多。

### 10.12 一个典型使用模式

```cpp
FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
TSharedPtr<FStreamableHandle> Handle = Streamable.RequestAsyncLoad(
    MySoftObject.ToSoftObjectPath(),
    FStreamableDelegate::CreateLambda([]
    {
        // 资源可用了
    }),
    FStreamableManager::DefaultAsyncLoadPriority,
    true
);

if (Handle.IsValid())
{
    Handle->WaitUntilComplete();
}
```

这段示例真正想表达的不是 API 用法，而是一个模式：

1. 路径由软引用提供  
2. 请求由 StreamableManager 统一管理  
3. 生命周期由 Handle 表达  
4. 必要时显式等待  
5. 最终由业务决定何时释放

### 10.13 为什么它是项目层最常见的加载抽象

总结一下，`FStreamableManager` 之所以重要，不是因为它“能异步加载”，而是因为它同时提供了：

1. 软路径到真实加载入口的桥梁
2. 请求去重与状态缓存
3. `FStreamableHandle` 生命周期模型
4. 活跃期对象保活
5. 同步/异步桥接
6. 批量/组合请求能力
7. 取消、等待、优先级、回调统一

所以在项目层真正工程化地做资源加载时，它往往是最好的第一层抽象。

### 10.14 一个推荐的认知模型

你可以这样记住：

```text
TSoftObjectPtr / FSoftObjectPath
        │  负责“描述我要谁”
        ▼
FStreamableManager
        │  负责“如何把请求组织起来并安全地拿到它”
        ▼
LoadPackageAsync / StaticLoadObject
        │  负责“真正触发包/对象加载”
        ▼
底层包加载器
```

---

## 11. `UAssetManager`：项目级资源调度中枢

`UAssetManager` 并不是为了替代 `FStreamableManager`，而是为了站在更高层做“资源治理”。

### 11.1 它解决的问题和 `FStreamableManager` 不同

`FStreamableManager` 更关注：

- 给一批路径发起请求
- 管住 Handle
- 管住加载生命周期

而 `UAssetManager` 更关注：

- 哪些是 Primary Asset
- 启动时扫描哪些目录
- 哪些资源应 AlwaysCook / NeverCook
- 哪些资源属于哪个 Chunk
- 哪些 Bundle 要在什么时机一起装入

### 11.2 `UAssetManagerSettings`

在 `AssetManagerSettings.h` 中，可以看到一整套项目级配置：

- `PrimaryAssetTypesToScan`
- `DirectoriesToExclude`
- `PrimaryAssetRules`
- `CustomPrimaryAssetRules`
- `bOnlyCookProductionAssets`
- `bShouldManagerDetermineTypeAndName`
- `bShouldGuessTypeAndNameInEditor`
- `bShouldAcquireMissingChunksOnLoad`

### 11.3 它如何与 AssetRegistry 配合

`AssetManager.cpp` 的主线之一是：

1. 扫描配置中的资源类型
2. 调用 AssetRegistry 查询匹配资产
3. 建立 `PrimaryAssetId -> 资产信息` 缓存
4. 进一步构建管理数据库

### 11.4 它如何与 `FStreamableManager` 配合

虽然 `UAssetManager` 功能很多，但真正发起具体加载时，它仍然高度依赖 `FStreamableManager`。

这意味着：

- `UAssetManager` 不绕开 streamable 层
- 它是建立在 streamable 层之上的策略中枢

所以如果你读 `UAssetManager` 的源码读不懂，常见原因不是 AssetManager 本身太玄学，而是：

> 你还没有把 `FStreamableManager` 这一层读透。

---

## 12. Primary Asset 体系：Type、Rules、Bundle、Label、CookRule

这一套概念如果只从编辑器面板看，很容易显得抽象；从源码看会清晰很多。

### 12.1 `FPrimaryAssetRules`

在 `AssetManagerTypes.h` 中，`FPrimaryAssetRules` 包含：

- `Priority`
- `ChunkId`
- `bApplyRecursively`
- `CookRule`

它表达的是：

> 某类主资源以及它管理的次级资源，在 Cook、Chunk、管理继承上的规则。

### 12.2 `EPrimaryAssetCookRule`

源码里定义了：

- `Unknown`
- `NeverCook`
- `ProductionNeverCook`
- `DevelopmentAlwaysProductionNeverCook`
- `DevelopmentAlwaysProductionUnknownCook`
- `AlwaysCook`

### 12.3 `FPrimaryAssetTypeInfo`

它定义一个主资源类型时，会描述：

- `PrimaryAssetType`
- `AssetBaseClass`
- `bHasBlueprintClasses`
- `bIsEditorOnly`
- 扫描目录
- 指定资产
- 默认 Rules

### 12.4 `UPrimaryDataAsset`

在 `DataAsset.h` 中，`UPrimaryDataAsset` 的注释非常关键：

> 它实现了 `GetPrimaryAssetId`，并支持 AssetBundle，因此可以被 `AssetManager` 手动加载/卸载。

### 12.5 Asset Bundle 的意义

`UPrimaryDataAsset` 会在保存时整理 `AssetBundleData`。  
Bundle 的真实意义不是“多一个名字”，而是：

> 把主资源所关联的一组软引用按用途分组。

---

## 13. Cook、Chunk、IoStore 如何改变运行时加载

这部分如果不讲，很多源码现象都解释不通。

### 13.1 编辑器里的加载世界

编辑器中常见的是：

- 扫描磁盘
- 查注册表
- 直接访问项目内容目录
- 动态变化更频繁

### 13.2 Cook 后的运行时世界

Cook 后则不同：

- AssetRegistry 已预生成
- 很多无关信息已裁剪
- 资源可能被分到不同 Chunk
- 包数据可能进入 IoStore
- 运行时更依赖 PackageStore / IoDispatcher

### 13.3 Chunk 的意义

Chunk 的本质是：

> 决定资源如何被分组打包、部署、安装与按需获取。

### 13.4 Lyra 中的打包设置

`Samples\Games\Lyra\Config\DefaultGame.ini` 里可以看到：

- `UsePakFile=True`
- `bGenerateChunks=true`
- `bChunkHardReferencesOnly=False`

---

## 14. Lyra 案例：项目层如何站在引擎加载机制之上

Lyra 是这份仓库里最适合拿来讲资源加载实践的样板之一。

### 14.1 `LyraAssetManager` 的定位

`ULyraAssetManager::Get()` 会强制从 `GEngine->AssetManager` 中拿到项目自定义的 AssetManager。

### 14.2 同步加载与 StreamableManager

`LyraAssetManager.cpp` 中：

```cpp
return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
```

这很有代表性，因为它说明：

- 项目层并不是到处直接 `LoadObject`
- 即便是同步加载，也倾向于复用 `StreamableManager`

### 14.3 启动阶段的内容加载

Lyra 会在 `StartInitialLoading()` 中：

- 先 `Super::StartInitialLoading()`
- 再安排若干 startup jobs
- 加载 GameplayCue
- 加载 GameData

### 14.4 `WaitUntilComplete()` 的实际使用

Lyra 中有这样一段思路：

- 先 `LoadPrimaryAssetsWithType(...)`
- 得到 `TSharedPtr<FStreamableHandle>`
- 必要时 `Handle->WaitUntilComplete(...)`
- 再取 `Handle->GetLoadedAsset()`

这就是“异步发起、必要时等待”的工程化模式。

---

## 15. 常见坑与调试方法

### 15.1 把 AssetRegistry 当成加载器

最常见误区之一：

> “我拿到 `FAssetData` 了，所以资源已经加载了。”

这是错的。  
`FAssetData` 只是元数据，不是对象实例。

### 15.2 蓝图资产和蓝图生成类混淆

尤其是 UI / Gameplay Ability / PawnClass 等场景，经常混淆：

- `UBlueprint` 资产本体
- `_C` 对应的生成类

### 15.3 软引用不保活

`TSoftObjectPtr` 不是强引用。  
如果你只保留软路径，而没有：

- 活跃 Handle
- 显式强引用
- 其他保活机制

对象在某些时机是可能被回收的。

### 15.4 同步 API 的性能误判

你看到 `LoadSynchronous`、`LoadObject`、`LoadPackage`，不要条件反射认为：

> “这就是纯同步磁盘读取。”

在 UE5.5 中，很多同步入口底层会借助异步加载器，再通过 flush/wait 完成。

### 15.5 在构造期或特殊阶段触发加载

`StreamInternal()` 中可以看到：

```cpp
if (GIsInitialLoad || ThreadContext.IsInConstructor > 0 || bForceSynchronousLoads)
```

这说明在构造期/初始加载期，行为会偏向强制同步或受限路径。

### 15.6 调试建议

如果你要排查加载问题，建议从四层分别看：

1. 路径是否正确
2. 注册表里是否存在
3. 请求是否正确发出
4. 底层加载器是否完成包加载

---

## 16. 高频源码追问 / 面试题视角

这一节不是为了“背题”，而是为了逼你确认自己是否真的理解了加载链路。

---

### 16.1 `LoadObject`、`LoadPackage`、`FStreamableManager` 的区别是什么？

推荐回答框架：

- `LoadObject` / `StaticLoadObject`
  - 更接近对象级入口
  - 输入通常是对象路径
  - 会在必要时触发包加载

- `LoadPackage`
  - 更接近包级入口
  - 目标是确保整个包可用
  - 常被对象加载链路间接触发

- `FStreamableManager`
  - 不是单纯加载函数，而是工程化调度层
  - 管理软路径、异步请求、Handle 生命周期、GC 保活、批量与组合请求

一句话收束：

> `LoadObject` 偏“我要一个对象”；`LoadPackage` 偏“我要一个包”；`FStreamableManager` 偏“我要把一批资源请求组织起来并安全管理其生命周期”。

---

### 16.2 为什么同步加载接口在 UE5.5 里也可能走异步底层？

这是非常常见的追问。

推荐回答：

- 在 `LoadPackageInternal` 里，同步包加载在某些条件下会调用 `LoadPackageAsync`
- 然后通过 `FlushAsyncLoading(RequestId)` 等待完成
- 所以“同步”说的是**调用方语义**，不一定代表底层完全不用异步加载框架

这道题真正考察的是：

> 你是否理解“接口语义”和“底层实现”不是一回事。

---

### 16.3 AssetRegistry 为什么不是加载器？

推荐回答：

- AssetRegistry 负责资产发现、查询、标签、依赖与管理关系
- `FAssetData` 是元数据，不是 `UObject*`
- 它告诉你“有什么”，但不等于“已经把对象实例化进内存”

这题很基础，但极其高频。

---

### 16.4 `_C` 到底是什么？为什么配置里经常写 `_C`？

推荐回答：

- `_C` 通常对应蓝图编译生成类名
- 蓝图资产本体通常是 `UBlueprint`
- 真正能拿来 `SpawnActor`、`CreateWidget`、作为 `TSubclassOf<>` 使用的，往往是生成类
- 所以项目配置里大量需要 `_C`

如果能继续补一句：

> `UBlueprintGeneratedClass` 本质上继承自 `UClass`

基本就算回答到位了。

---

### 16.5 为什么 `FStreamableManager` 会成为项目层主力，而不是大家都直接 `LoadObject`？

推荐回答：

- `LoadObject` 只解决“加载这个对象”
- 大项目真正难的是：
  - 批量请求
  - 生命周期
  - 取消
  - 等待
  - 优先级
  - 保活
  - 重定向
- `FStreamableManager` 恰好把这些工程化问题统一了

---

### 16.6 `TSoftObjectPtr` 和 `FStreamableHandle` 的职责差异是什么？

这是判断你是否把“描述对象”和“管理请求”区分开的关键问题。

推荐回答：

- `TSoftObjectPtr`
  - 负责描述目标
  - 持有软路径
  - 不负责复杂请求管理
  - 不天然保活对象

- `FStreamableHandle`
  - 负责描述一次加载请求的生命周期
  - 能等待、取消、查看进度、收集结果
  - 与 `FStreamableManager` 配合实现请求期间保活

一句话总结：

> `TSoftObjectPtr` 解决“我要谁”，`FStreamableHandle` 解决“我这次怎么拿它、拿到什么时候算结束”。

---

### 16.7 什么时候应该优先读 `UObjectGlobals.cpp`，什么时候应该优先读 `StreamableManager.cpp`？

推荐答案：

- 你想搞清楚“一个对象路径如何变成真实对象”时，优先读 `UObjectGlobals.cpp`
- 你想搞清楚“项目层如何围绕软路径组织请求、保活、等待和批量调度”时，优先读 `StreamableManager.cpp`

如果要进一步看底层：

- 传统路线读 `LinkerLoad.cpp`
- 新路线读 `AsyncPackageLoader.cpp` / `AsyncLoading2.cpp`

---

## 17. 排障案例集：从现象反推加载链路

下面这些案例非常适合用来训练“看到问题就知道该往哪层查”。

---

### 案例 1：`TSoftClassPtr` 加载成功，但 `SpawnActor` 时报类型不对

#### 典型现象

- 你打印路径没问题
- `LoadSynchronous()` 似乎也没报错
- 但最后不是想要的 `UClass`

#### 高概率原因

- 你加载的是蓝图资产本体，而不是 `_C` 生成类

#### 排查顺序

1. 看路径是否带 `_C`
2. 看变量类型是不是 `TSoftClassPtr<>`
3. 看最终业务要的是 `UBlueprint*` 还是 `UClass*`
4. 看是否误把资产路径填进“类配置项”

#### 对应知识点

- 第 3 章路径体系
- 第 3.5 节 `_C` 专章

---

### 案例 2：AssetRegistry 能查到资产，但加载总失败

#### 典型现象

- `GetAssetsByPath` / `GetAssetsByClass` 能查到 `FAssetData`
- 但 `LoadObject` / `RequestAsyncLoad` 拿不到对象

#### 高概率原因

1. 路径填错，查到的是资产记录，不是正确对象路径  
2. 蓝图类路径与资产路径混淆  
3. Cook 后缺包 / 缺 Chunk / 包未部署  
4. Redirect / 重命名未处理正确

#### 排查顺序

1. 打印 `FAssetData.PackageName`、`AssetName`、`AssetClassPath`
2. 确认实际加载时传入的路径
3. 确认是否处于 Cooked runtime
4. 检查是否因为 Chunk/IoStore 内容缺失导致运行时不可取到

#### 对应知识点

- 第 5 章 AssetRegistry
- 第 13 章 Cook / Chunk / IoStore

---

### 案例 3：资源已经加载成功，但过一会又失效/变空

#### 典型现象

- 一开始拿到了对象
- 但后续使用时对象失效，或者某些软引用重新解析为 null

#### 高概率原因

- 你只有软引用，没有强引用或活跃 Handle
- `FStreamableHandle` 被提前释放
- 资源没有被任何系统继续保活

#### 排查顺序

1. 当前是否还有有效 Handle
2. 是否只保留了 `TSoftObjectPtr`
3. 是否需要显式保存 `TObjectPtr` / `UObject*`
4. 是否错误理解了 `LoadSynchronous()` 与后续生命周期

#### 对应知识点

- 第 4 章软引用
- 第 10.8 节 GC 持有

---

### 案例 4：启动阶段卡顿严重，但代码里看起来没写多少同步加载

#### 典型现象

- 启动时卡顿很明显
- 代码里表面上大量使用了 Streamable / Async 接口

#### 高概率原因

1. 某些请求最终调用了 `WaitUntilComplete()`  
2. 某些场景因构造期 / 初始加载期被强制转为同步路径  
3. 同步入口内部借异步底层再 flush，表现上仍然会阻塞  
4. 批量请求分布不合理，导致阶段性集中等待

#### 排查顺序

1. 搜 `WaitUntilComplete`
2. 搜 `LoadSynchronous`
3. 看是否处于 `GIsInitialLoad` / 构造期
4. 看启动任务是否过于集中串行

#### 对应知识点

- 第 6 章同步入口
- 第 10.7 节 Handle 等待
- 第 14 章 Lyra 启动案例

---

### 案例 5：编辑器里没问题，打包后加载失败

#### 典型现象

- Editor PIE 正常
- Shipping / Cooked 运行时失败

#### 高概率原因

1. 资源没被 Cook 进去  
2. 规则配置导致 `NeverCook` / 未被引用  
3. Primary Asset 扫描范围不完整  
4. Chunk 未安装 / 未打进当前包  
5. 依赖在编辑器可扫描到，但运行时裁剪后不存在

#### 排查顺序

1. 看 `AssetManagerSettings`
2. 看 `PrimaryAssetTypesToScan`
3. 看 CookRule / Chunk 配置
4. 看该资源是否真在打包产物里
5. 检查是否过度依赖编辑器扫描行为

#### 对应知识点

- 第 11 章 AssetManager
- 第 12 章 Primary Asset
- 第 13 章 Cook / Chunk / IoStore

---

## 18. 推荐阅读顺序 + 源码阅读路线图

这一节专门解决一个很现实的问题：

> 这套东西我知道很大，但我到底应该从哪一层开始读，才不会一上来就被 UE 源码淹没？

下面我给出三种阅读路线：**最推荐主线**、**偏项目实践路线**、**偏底层实现路线**。

---

### 18.1 最推荐主线：按“从外到内”的顺序读

如果你是第一次系统阅读 UE 资源加载源码，最推荐的顺序是：

#### 第 1 步：先读路径与软引用

先读：

- `SoftObjectPath.h`
- `SoftObjectPtr.h`

目标不是背 API，而是先回答三个问题：

1. `FSoftObjectPath` 里到底存了什么  
2. `TSoftObjectPtr` 到底是不是对象本身  
3. 软引用与 `_C` / 蓝图类路径是什么关系

如果这一步没吃透，后面读任何加载代码都会混乱。

---

#### 第 2 步：再读最直接的对象/包加载入口

然后读：

- `UObjectGlobals.cpp`

重点盯：

- `StaticLoadObjectInternal`
- `StaticLoadObject`
- `LoadAssetAsync`
- `LoadPackageInternal`

这一阶段你要搞清楚的是：

> “一个路径是如何一步步变成包加载、再变成对象查找结果的？”

---

#### 第 3 步：读 `FStreamableManager`

再读：

- `StreamableManager.h`
- `StreamableManager.cpp`

先不要一口气全读，建议按这个顺序：

1. `FStreamableHandle` 对外接口  
2. `RequestAsyncLoadInternal()`  
3. `StreamInternal()`  
4. `AddReferencedObjects()`  
5. Redirect / 缓存相关逻辑

这一阶段你要回答的是：

> “项目层为什么不直接到处 `LoadObject`，而是喜欢围绕 StreamableManager 建模？”

---

#### 第 4 步：读 `UAssetManager`

然后读：

- `AssetManagerSettings.h`
- `AssetManagerTypes.h`
- `AssetManager.cpp`

重点不是一开始就把 AssetManager 所有函数啃完，而是先抓：

1. 扫描范围怎么定义  
2. PrimaryAssetType 怎么建立  
3. Rules / Bundle / Chunk 是怎么表达的  
4. 它最终如何回到 StreamableManager 做加载

这一阶段要形成的认知是：

> `UAssetManager` 是策略层，不是替代加载器。

---

#### 第 5 步：最后再进底层加载器

最后再看：

- `LinkerLoad.cpp`
- `AsyncPackageLoader.cpp`
- `AsyncLoading.cpp`
- `AsyncLoading2.cpp`
- `PackageStore.cpp`
- `IoDispatcher.cpp`

因为到了这一步，你脑子里已经有一张清晰地图：

- 路径层
- 入口层
- 调度层
- 策略层
- 底层实现层

这时再啃底层实现，信息才会挂得住。

---

### 18.2 偏项目实践路线：如果你主要想解决“项目里到底怎么用”

如果你当前目标不是深挖最底层，而是先能把项目里的资源加载写对，建议按下面顺序读：

1. 本文第 3 章路径体系  
2. 本文第 4 章软引用 / 软类引用  
3. 本文第 10 章 `FStreamableManager`  
4. 本文第 11~12 章 `UAssetManager` / Primary Asset  
5. Lyra 的 `LyraAssetManager.cpp`

源码文件建议顺序：

```text
SoftObjectPath.h
-> SoftObjectPtr.h
-> StreamableManager.h
-> StreamableManager.cpp
-> AssetManagerSettings.h
-> AssetManagerTypes.h
-> LyraAssetManager.cpp
-> AssetManager.cpp
```

这条路线的目标是：

> 先学会“正确使用与组织加载”，再回头补底层实现细节。

---

### 18.3 偏底层实现路线：如果你要排查引擎级加载问题

如果你的目标更偏：

- 排查 Cooked runtime 加载失败
- 分析卡顿
- 看懂 UE5 新旧加载体系切换

可以走这条路线：

```text
UObjectGlobals.cpp
-> LinkerLoad.cpp
-> AsyncPackageLoader.cpp
-> AsyncLoading.cpp
-> AsyncLoading2.cpp
-> PackageStore.cpp
-> IoDispatcher.cpp
```

但要强调一句：

> 这条路线只建议在你已经理解路径层和 StreamableManager 层之后再走。

否则非常容易出现这种情况：

- 看到了很多复杂状态机
- 看到了很多线程/调度代码
- 却不知道“上层到底为什么会走到这里”

---

### 18.4 一张“从外到内”的源码阅读路线图

可以把推荐阅读顺序压缩成这张图：

```text
第一圈：先理解“路径”
  SoftObjectPath.h
  SoftObjectPtr.h
        │
        ▼
第二圈：理解“入口”
  UObjectGlobals.cpp
        │
        ▼
第三圈：理解“项目层调度”
  StreamableManager.h / .cpp
        │
        ▼
第四圈：理解“项目级治理”
  AssetManagerSettings.h
  AssetManagerTypes.h
  AssetManager.cpp
  LyraAssetManager.cpp
        │
        ▼
第五圈：理解“底层实现”
  LinkerLoad.cpp
  AsyncPackageLoader.cpp
  AsyncLoading.cpp
  AsyncLoading2.cpp
  PackageStore.cpp
  IoDispatcher.cpp
```

这张图的本质思想是：

> 永远先理解“上层为什么需要这个东西”，再去理解“底层怎么把它做出来”。

---

### 18.5 每个阶段最应该带着什么问题去读

这是阅读源码时非常有用的一种方法：  
**不要只是读代码，要带着问题读。**

#### 读 `SoftObjectPath.h` 时

问自己：

- 路径里到底包含哪些字段？
- 顶层对象和子对象是怎么区分的？
- `_C` 应该出现在什么位置？

#### 读 `UObjectGlobals.cpp` 时

问自己：

- 为什么对象加载会退化成包加载？
- 为什么同步入口里会出现异步加载器？
- redirect 是在什么阶段处理的？

#### 读 `StreamableManager.cpp` 时

问自己：

- 请求状态是谁在维护？
- 已加载对象为什么不会立刻被 GC 干掉？
- 同一路径的重复请求是怎么合并的？
- `WaitUntilComplete` 到底阻塞了什么？

#### 读 `AssetManager.cpp` 时

问自己：

- 它到底在管理什么，而不是加载什么？
- Primary Asset 是怎么从配置与注册表里建立出来的？
- Bundle / CookRule / Chunk 是在哪一层生效的？

#### 读 `AsyncLoading2.cpp` / `PackageStore.cpp` 时

问自己：

- 这些底层结构到底在替代旧路线的哪一部分？
- 为什么 Cook 后运行时更依赖这些东西？
- 新路线与旧路线到底是并存、替代，还是兼容桥接关系？

---

### 18.6 如果你时间有限，最小必读清单是什么？

如果你现在时间很紧，但又想真正建立对 UE 资源加载的靠谱认知，我建议至少读这 6 个文件：

1. `SoftObjectPath.h`
2. `SoftObjectPtr.h`
3. `UObjectGlobals.cpp`
4. `StreamableManager.h`
5. `StreamableManager.cpp`
6. `AssetManagerTypes.h`

为什么是这 6 个？

因为它们刚好覆盖了：

- 路径描述
- 软引用
- 对象/包加载入口
- 工程化调度
- Primary Asset 基本规则模型

只要把这 6 个文件真正读懂，你对 UE 资源加载的理解就已经超过“只会调接口”的层面了。

---

### 18.7 一个实际可执行的阅读节奏建议

如果你真要把这套源码啃下来，我建议按下面的节奏：

#### 第一天

- 读本文第 3、4 章
- 对照 `SoftObjectPath.h` / `SoftObjectPtr.h`
- 把 `_C`、软路径、软类引用搞清

#### 第二天

- 读本文第 6 章
- 对照 `UObjectGlobals.cpp`
- 搞清 `StaticLoadObject` / `LoadPackageInternal`

#### 第三天

- 读本文第 10 章
- 对照 `StreamableManager.h` / `.cpp`
- 搞清 Handle、请求管理、GC 保活

#### 第四天

- 读本文第 11~12 章
- 对照 `AssetManagerSettings.h` / `AssetManagerTypes.h` / `AssetManager.cpp`
- 搞清项目层治理

#### 第五天

- 读本文第 8、9、13 章
- 对照 `AsyncPackageLoader.cpp` / `AsyncLoading2.cpp` / `PackageStore.cpp`
- 建立新旧路线并存的底层认知

这个节奏不是唯一答案，但很适合第一次系统读这一块源码。

---

## 19. 术语对照表 / 容易混淆概念表

这一节的目的不是堆定义，而是把最容易混淆的一批概念放到一起对照。

---

### 19.1 术语对照表

| 术语 | 更准确的含义 | 在资源加载中的位置 |
|---|---|---|
| Asset（资产） | 通常指包中的顶层资源对象 | 你平时说“加载资源”时，大多指它 |
| Package（包） | UE 的基本装载单位，运行时表现为 `UPackage` | 对象加载经常先退化为包加载 |
| Object（对象） | 内存中的 `UObject` 实例 | 最终真正被使用的运行时实体 |
| Object Path | 指向对象的路径，如 `/Game/Foo.Bar` | `LoadObject` 常直接围绕它工作 |
| Package Name | 包路径，如 `/Game/Foo` | `LoadPackage`、`FPackagePath` 更关心它 |
| `FTopLevelAssetPath` | 包名 + 顶层资产名的结构化路径 | 新 API 越来越偏好它 |
| `FSoftObjectPath` | 顶层资产路径 + 可选子对象路径 | 软引用与流式加载的基础 |
| Hard Reference | 强引用/硬依赖 | 更容易进入依赖图与驻留链 |
| Soft Reference | 软引用，只描述目标，不保证已加载 | 常用于按需加载 |
| `FAssetData` | AssetRegistry 中的资产元数据条目 | 用来“查”和“看”，不是对象本体 |
| AssetRegistry | 资产发现与索引系统 | 负责查，不负责真正实例化对象 |
| `StaticLoadObject` | 对象级同步入口 | 需要时会触发包加载 |
| `LoadPackage` | 包级入口 | 保证包进入可用状态 |
| `LoadPackageAsync` | 包异步加载入口 | 底层异步加载统一入口之一 |
| `FStreamableManager` | 软路径请求调度层 | 项目层最常见的工程化加载抽象 |
| `FStreamableHandle` | 一次加载请求的生命周期对象 | 管理等待、取消、进度、结果 |
| `UAssetManager` | 项目级资源治理中枢 | 管理 Primary Asset、规则、Bundle、Chunk |
| Primary Asset | 被 AssetManager 显式管理的主资源 | 资源治理与打包规则核心概念 |
| Secondary Asset | 被主资源间接管理/引用的资源 | 常随主资源规则一起受影响 |
| Bundle | 主资源下的一组命名软引用集合 | 用于分组加载 |
| Chunk | 打包/部署分组单位 | 直接影响 Cook 后运行时可用性 |
| Cook | 为运行时准备资源产物与元数据的过程 | 改变 AssetRegistry 和加载底座行为 |
| IoStore | 新运行时包数据容器体系 | 新加载路线的重要基础 |
| PackageStore | 运行时包元信息数据库 | 新加载路线的关键元数据层 |
| IoDispatcher | 统一 IO 调度层 | 新路线中的包/块数据请求分发中心 |

---

### 19.2 最容易混淆的概念对照

#### 1）`FAssetData` vs `UObject*`

| 项目 | `FAssetData` | `UObject*` |
|---|---|---|
| 本质 | 元数据记录 | 真正运行时对象 |
| 主要来源 | AssetRegistry | 加载系统 / 内存 |
| 能否直接使用资源内容 | 不能 | 能 |
| 主要用途 | 查询、筛选、读标签、分析依赖 | 运行时逻辑、实例化、访问属性 |

一句话记忆：

> `FAssetData` 是“身份证”，`UObject*` 是“人已经来了”。

---

#### 2）包路径 vs 对象路径

| 项目 | 包路径 | 对象路径 |
|---|---|---|
| 示例 | `/Game/Data/DA_Item` | `/Game/Data/DA_Item.DA_Item` |
| 关注点 | 包本身 | 包中的某个对象 |
| 常见入口 | `LoadPackage` | `LoadObject` / `StaticLoadObject` |

一句话记忆：

> 包路径回答“在哪个包里”，对象路径回答“包里的谁”。

---

#### 3）蓝图资产 vs 蓝图生成类

| 项目 | 蓝图资产本体 | 蓝图生成类 |
|---|---|---|
| 常见类型 | `UBlueprint` | `UBlueprintGeneratedClass` / `UClass` |
| 路径例子 | `/Game/UI/WBP_MainMenu.WBP_MainMenu` | `/Game/UI/WBP_MainMenu.WBP_MainMenu_C` |
| 常见用途 | 编辑器资产、查看蓝图元信息 | `SpawnActor`、`CreateWidget`、`TSubclassOf<>` |

一句话记忆：

> 不带 `_C` 更像“蓝图资源本体”，带 `_C` 更像“拿来当类用的编译结果”。

---

#### 4）`TSoftObjectPtr` vs `TSoftClassPtr`

| 项目 | `TSoftObjectPtr<T>` | `TSoftClassPtr<T>` |
|---|---|---|
| 指向什么 | 具体对象资源 | 类对象 |
| 常见用途 | 贴图、数据资产、音效 | Widget 类、Actor 类、Ability 类 |
| 常见坑 | 以为它自动保活对象 | 路径没写 `_C` 导致拿不到类 |

---

#### 5）`TSoftObjectPtr` vs `FStreamableHandle`

| 项目 | `TSoftObjectPtr` | `FStreamableHandle` |
|---|---|---|
| 关注点 | 目标是谁 | 这次请求如何管理 |
| 是否描述生命周期 | 否 | 是 |
| 是否可等待/取消/看进度 | 否 | 是 |
| 是否天然保活对象 | 否 | 借助 manager 可以 |

一句话记忆：

> `TSoftObjectPtr` 是“目标描述”，`FStreamableHandle` 是“请求状态机”。 

---

#### 6）`LoadObject` vs `RequestAsyncLoad`

| 项目 | `LoadObject` | `RequestAsyncLoad` |
|---|---|---|
| 风格 | 直接对象加载 | 工程化请求调度 |
| 典型语义 | 现在就把对象给我 | 帮我组织并跟踪这次加载 |
| 是否返回请求句柄 | 否 | 是 |
| 是否适合复杂项目流程 | 一般 | 很适合 |

---

#### 7）AssetRegistry vs AssetManager

| 项目 | AssetRegistry | AssetManager |
|---|---|---|
| 核心定位 | 发现层 / 索引层 | 治理层 / 策略层 |
| 核心问题 | 有什么、在哪、依赖谁 | 哪些应该被管理、如何被 Cook/Chunk/Bundle 化 |
| 是否直接代表对象已加载 | 否 | 否 |

一句话记忆：

> AssetRegistry 管“资源信息”，AssetManager 管“项目规则”。

---

#### 8）旧加载路线 vs 新加载路线

| 项目 | 旧路线 | 新路线 |
|---|---|---|
| 代表性概念 | `FLinkerLoad`、旧 `AsyncLoading` | `AsyncPackageLoader2`、`PackageStore`、`IoDispatcher` |
| 更适合理解什么 | 包内部结构、经典对象装配模型 | Cook 后运行时、现代 IO / 元数据调度 |
| 在 UE5.5 中状态 | 仍存在、仍重要 | 明显增强、越来越核心 |

一句话记忆：

> UE5.5 不是“旧没了、新全替代”，而是“公共 API 稳定，新旧底层并存演进”。 

---

### 19.3 一个特别容易犯错的“同名不同层”清单

下面这些词经常看起来像一回事，实际上不在同一层：

#### “资源”

它可能指：

- 磁盘里的包文件
- 包中的顶层资产
- 已加载的 `UObject`
- AssetRegistry 条目

所以当别人说“这个资源没加载出来”时，你要追问：

> 你说的是“查不到”、还是“包没开出来”、还是“对象没实例化出来”？

#### “加载成功”

它可能指：

- AssetRegistry 查到了
- 包成功打开了
- 对象被找到了
- 对象完成了 `PostLoad`
- 类可被实例化了

所以“成功”这个词如果不带上下文，几乎没有排障价值。

#### “同步加载”

它可能指：

- 调用方是同步语义
- 业务线程在等待
- 底层用的是异步加载器 + flush

因此“同步”不要只看 API 名字。

---

### 19.4 读源码时，遇到术语混乱怎么办

一个很实用的方法是，每次看到不确定的术语，都先给它贴层级标签：

```text
[路径层]
[发现层]
[入口层]
[调度层]
[治理层]
[底层实现层]
```

例如：

- `FSoftObjectPath` -> 路径层
- `FAssetData` -> 发现层
- `StaticLoadObject` -> 入口层
- `FStreamableHandle` -> 调度层
- `UAssetManager` -> 治理层
- `FLinkerLoad` / `AsyncLoading2` -> 底层实现层

只要层级不混，很多概念自然就不会打架。

---

## 20. 资源加载调试日志与常用断点清单

这一节是给“要真正落地排查问题的人”准备的。  
前面我们讲了很多概念、链路和类；这一节要回答的是：

> 当项目里真的出现“查得到、却加载失败”“软引用没生效”“Cook 后才出问题”“同步加载卡顿”时，我应该先看什么日志、打哪些断点？

---

### 20.1 先建立一个调试原则：永远先判断你卡在哪一层

调试资源加载时，最怕的不是问题复杂，而是**一上来就在错误层级里打转**。

建议你先把问题粗分成下面 6 层：

```text
1. 路径层：路径是不是就写错了？有没有 _C？是不是包路径和对象路径混了？
2. 发现层：AssetRegistry 里到底有没有这条资产记录？
3. 入口层：调用方到底走的是 LoadObject、LoadPackage、还是 StreamableManager？
4. 调度层：请求有没有发起？有没有合并？有没有被等待、取消、或提前完成？
5. 底层加载层：包有没有真正进入 LoadPackage / LoadPackageAsync / Linker / AsyncLoading？
6. 治理层：是不是 AssetManager、Bundle、CookRule、Chunk 规则导致运行时不可用？
```

这个分层思路非常重要。  
因为很多“资源加载失败”其实不是加载器本身坏了，而是：

- 路径写错了；
- 查到的是蓝图资产，不是 `_C` 类；
- AssetRegistry 有记录，但 Cook 后没有对应包；
- 资源被加载过，但很快失去保活；
- 其实不是“没加载”，而是加载完成后业务层拿错对象。

---

### 20.2 最常用的日志类别：先看什么

下面这几个日志类别，是调试资源加载时最有用的一组。

| 日志类别 | 典型源码位置 | 最适合看什么问题 | 你通常会看到什么信号 |
|---|---|---|---|
| `LogStreamableManager` | `Engine\Source\Runtime\Engine\Private\StreamableManager.cpp` | 软路径请求、重复请求合并、同步/异步桥接、redirect、句柄状态 | `RequestAsyncLoad(...)`、`Already Loaded`、`Already in progress`、`Failed attempt to load` |
| `LogAssetManager` | `Engine\Source\Runtime\Engine\Private\AssetManager.cpp` | Primary Asset 扫描、Bundle、Chunk、CookRule、规则冲突 | `Duplicate PrimaryAssetType`、`Failure loading`、`Dump...` 输出 |
| `LogAssetRegistry` | `Engine\Source\Runtime\AssetRegistry\Private\AssetRegistry.cpp` | 资产发现、Registry 状态、依赖/条目问题 | “查得到还是查不到”这一层的问题 |
| `LogLinker` | `Engine\Source\Runtime\CoreUObject\Private\UObject\Linker.cpp` | 包打开、导入导出、序列化阶段问题 | 更偏传统包加载与对象装配问题 |
| `LogStreaming` | `Engine\Source\Runtime\Core\Private\Misc\CoreMisc.cpp` | 加载/Flush 的大体节奏、全局流式加载层面观察 | 更适合看“是不是一直在等/刷” |
| `LogIoDispatcher` | `Engine\Source\Runtime\Core\Private\IO\IoDispatcher.cpp` | Cooked runtime 下的 IO / 容器读取问题 | 更适合排查新底层、IoStore 环境问题 |

一个经验是：

> 不要一上来把所有日志都开到 Verbose。  
> 最好先按怀疑层级逐步打开，否则输出量会大到淹没有效信息。

---

### 20.3 最实用的开日志方式

#### 方式 1：命令行直接开

如果你要在编辑器或游戏启动时就观察加载链路，最稳的方式是直接带 `-LogCmds`：

```text
UnrealEditor.exe YourProject.uproject -log -LogCmds="LogStreamableManager Verbose, LogAssetManager Verbose"
```

如果你怀疑已经深入到底层加载或 Cooked runtime IO，可以再逐步加：

```text
UnrealEditor.exe YourProject.uproject -log -LogCmds="LogStreamableManager Verbose, LogAssetManager Verbose, LogAssetRegistry Verbose, LogLinker Verbose, LogStreaming Verbose, LogIoDispatcher Verbose"
```

建议按下面节奏开，而不是一步拉满：

1. 先 `LogStreamableManager`  
2. 再 `LogAssetManager`  
3. 还不够再看 `LogAssetRegistry` / `LogLinker`  
4. Cook 后运行时再加 `LogIoDispatcher`

#### 方式 2：先缩小问题，再局部加深

例如：

- 软引用加载问题：先只开 `LogStreamableManager`
- Primary Asset / Bundle 问题：再加 `LogAssetManager`
- “编辑器正常，打包后失败”：重点看 `LogAssetManager + LogIoDispatcher + LogLinker`

这比“全开然后盲翻日志”效率高得多。

---

### 20.4 `AssetManager` 自带的几个调试命令，非常值得用

`AssetManager.cpp` 里其实已经内置了一批非常有用的控制台命令：

| 命令 | 作用 | 适合什么场景 |
|---|---|---|
| `AssetManager.DumpTypeSummary` | 打印 Primary Asset 类型概览 | 想看扫描结果是否对 |
| `AssetManager.DumpLoadedAssets` | 打印当前已加载资产状态 | 怀疑某资源是否真的被管起来了 |
| `AssetManager.DumpBundlesForAsset <Type:Name>` | 打印某主资源的 Bundle 内容 | 怀疑 Bundle 配错、漏配 |
| `AssetManager.DumpAssetRegistryInfo` | 打印注册表相关信息 | 想看当前 Registry 规模与状态 |
| `AssetManager.DumpReferencersForPackage <Package>` | 看包的直接引用管理关系 | 追谁在管理/引用某包 |
| `AssetManager.LoadPrimaryAssetsWithType <Type>` | 手动加载某类型主资源 | 做实验、验证加载链 |
| `AssetManager.UnloadPrimaryAssetsWithType <Type>` | 手动卸载某类型主资源 | 验证卸载与重载行为 |

这些命令的意义非常大，因为它们能把“我怀疑 AssetManager 有问题”这种模糊判断，变成可验证的信息。

例如你怀疑一个 Primary Asset 没进 Bundle，可以直接：

```text
AssetManager.DumpBundlesForAsset MyType:MyAsset
```

如果这里就看不到你预期的软引用项，那问题大概率不在底层加载器，而在**治理层配置或扫描结果**。

---

### 20.5 最小可用的断点清单：如果你只想先快速定位

如果你现在没有时间系统读完整个引擎加载体系，我建议先记住下面这组“最小断点套餐”：

1. `ResolveName2`  
2. `StaticLoadObjectInternal`  
3. `LoadPackageInternal`  
4. `LoadPackageAsync`  
5. `FStreamableManager::RequestAsyncLoadInternal`  
6. `FStreamableManager::StreamInternal`  
7. `FStreamableHandle::WaitUntilComplete`  
8. `UAssetManager::LoadPrimaryAssetsWithType`

为什么是这 8 个？

因为它们分别覆盖了：

- 路径解释
- 对象加载入口
- 包加载入口
- 底层异步入口
- Streamable 请求建立
- Streamable 实际分发
- 同步等待桥接
- AssetManager 治理层入口

只要把这 8 个点串起来，绝大多数项目里的资源加载问题，你都能很快定位到是“哪一层出了偏差”。

---

### 20.6 按问题类型打断点：具体怎么下手

#### 场景 1：怀疑路径写错、`_C` 写错、包/对象路径混了

先打：

- `ResolveName2`
- `StaticLoadObjectInternal`

重点看：

- 传入的 `InName` 最终被规范化成什么
- `InOuter` / Package 是怎么被推导出来的
- 你写的是对象路径，还是只是包名
- 你真正需要的是蓝图资产，还是蓝图生成类 `_C`

这一层最典型的问题，不是“底层加载失败”，而是**入口还没走到底层就已经偏了**。

---

#### 场景 2：怀疑 `LoadObject` 明明是同步接口，却还是卡住或走了异步链

先打：

- `StaticLoadObjectInternal`
- `LoadPackageInternal`
- `LoadPackageAsync`
- `FlushAsyncLoading`

重点看：

- `LoadPackageInternal` 是否因为 `ShouldAlwaysLoadPackageAsync(...)` 转去异步入口
- 后面是否立刻 `FlushAsyncLoading(RequestID)`
- 这个“同步加载”到底是纯同步实现，还是“异步加载器 + 同步等待”

这类问题非常适合对照 `UObjectGlobals.cpp` 来看。

---

#### 场景 3：怀疑 `FStreamableManager` 请求没发出去，或被悄悄合并了

先打：

- `FStreamableManager::RequestAsyncLoadInternal`
- `FStreamableManager::StreamInternal`

重点看：

- 传入路径集合在进入内部后是否去重
- 是否命中了 `Already Loaded`
- 是否命中了 `Already in progress`
- 是否被识别为 redirect
- 最终有没有真正进入 `LoadPackageAsync`

如果你看到请求根本没落到底层，大概率问题就在 Streamable 层，而不是更深的加载器。

---

#### 场景 4：怀疑“资源加载过了，但又失效了”

先打：

- `FStreamableManager::StreamInternal`
- `FStreamableManager::AsyncLoadCallback`
- `FStreamableManager::AddReferencedObjects`
- `FStreamableHandle::ReleaseHandle`

重点看：

- Handle 是否还活着
- `ManagedActiveHandles` / 引用关系是否还在
- 对象是不是只被短暂拿到、随后失去保活
- 你手里拿的是软引用、弱语义，还是稳定持有关系

这种问题很多人一开始会误判成“没加载成功”，其实常常是**加载成功了，但生命周期没管住**。

---

#### 场景 5：怀疑 Primary Asset / Bundle / Chunk 配置有问题

先打：

- `UAssetManager::LoadPrimaryAssetsWithType`
- `UAssetManager::OnAssetStateChangeCompleted`
- `UAssetManager::UpdateManagementDatabase`

再配合：

- `AssetManager.DumpTypeSummary`
- `AssetManager.DumpLoadedAssets`
- `AssetManager.DumpBundlesForAsset <Type:Name>`

重点看：

- 该资源有没有被识别成正确的 `PrimaryAssetType`
- Bundle 状态切换时到底收集到了哪些路径
- 管理数据库里是不是根本没有你以为存在的规则
- 报错是不是已经明确指出 Chunk / CookRule 冲突

---

#### 场景 6：编辑器正常，Cook 后运行时失败

先打：

- `UAssetManager` 相关治理入口
- `LoadPackageAsync`
- `LogIoDispatcher` 相关路径
- `LogLinker` 相关路径

重点看：

- 这是发现层缺数据，还是包数据本身不可取
- 是规则层没有把资源带进最终产物
- 还是运行时 IO / PackageStore 路线没找到对应内容

这类问题最忌讳只在编辑器环境验证，因为编辑器常常“能扫描、能补救、能在磁盘上直接找到东西”，而 Cooked runtime 不行。

---

### 20.7 如何读懂一段典型日志：不要只看“报错那一行”

很多人看日志只盯最后一行 `Error`，这是不够的。  
资源加载日志更应该按“前因后果”读：

```text
请求发起
-> 路径是否被规范化
-> 是否已经在内存
-> 是否已在加载中
-> 是否真正走到包加载
-> 回调里找到对象没有
-> Handle 最终是 complete / cancel / fail 哪一种
```

以 `LogStreamableManager` 为例，你真正有价值的往往不是最后一句：

```text
Failed attempt to load ...
```

而是它前面的上下文：

- 之前有没有 `Already Loaded`
- 之前有没有 `Already in progress`
- 路径是否先经过 redirect
- 是哪个 DebugName 发起的请求

这些上下文，往往比最后那个“失败”本身更能说明根因。

---

### 20.8 一个非常实用的排查顺序

如果你只想记一个顺序，我建议你这样排：

#### 第一步：先验证路径

- 是否写成对象路径
- 类路径是否需要 `_C`
- 包名、对象名、子对象路径是否混了

#### 第二步：再验证发现层

- AssetRegistry 里到底有没有
- `FAssetData` 查到的是不是你以为的那个资产

#### 第三步：再看请求有没有真正发起

- 有没有走 `RequestAsyncLoadInternal`
- 有没有进入 `LoadPackageInternal` / `LoadPackageAsync`

#### 第四步：最后再深挖底层

- `Linker`
- `AsyncLoading`
- `AsyncLoading2`
- `IoDispatcher`

这个顺序的好处是：  
你会先排掉最常见、最便宜的问题，再去啃最重的底层实现。

---

## 21. 从调用栈角度看一次完整资源加载的时序拆解

这一节我们不再按“模块介绍”的方式讲，而是换一个更贴近实战调试的视角：

> 当你在调试器里按下 F11，一层一层往里走时，一次完整的资源加载，调用栈到底会长什么样？

这件事非常重要，因为很多人“概念都知道”，但一进调试器还是会迷路。  
真正能把源码读透，往往靠的不是再背几个类名，而是能把**调用栈、日志、时序、对象状态**四件事对上。

---

### 21.1 先记住一个总模型：一次资源加载通常会穿过哪几层调用栈

先把最通用的调用栈骨架记住：

```text
业务层调用
-> 路径/引用层（FSoftObjectPath / TSoftObjectPtr / TSoftClassPtr）
-> 加载入口层（LoadObject / StaticLoadObject / LoadPackage / RequestAsyncLoad）
-> 调度层（FStreamableManager / FStreamableHandle）
-> 底层包加载层（LoadPackageInternal / LoadPackageAsync / Linker / AsyncLoading）
-> 对象可见层（FindObject / ResolveObject / 回调 / PostLoad 完成）
-> 业务层继续使用对象
```

真正调试时，你看到的调用栈深度会因为入口不同而差很多：

- 直接 `LoadObject`：栈会更短，更像“对象入口直插包加载”
- `FStreamableManager::LoadSynchronous`：表面同步，内部可能明显带着异步桥接痕迹
- `RequestAsyncLoad`：请求发起和结果落地通常会分成**两段栈**
- `UAssetManager`：会在最外层再包一层“治理层调用栈”

所以调试第一步，不是看细节，而是先判断：

> 我当前看到的是“入口栈”“等待栈”“回调栈”，还是“治理层外壳栈”？

---

### 21.2 情况一：最直接的同步对象加载调用栈

这是最适合入门的一条主线，因为它最接近“给路径，拿对象”这件事的直觉。

典型业务代码可能长这样：

```cpp
UTexture2D* Tex = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/T_Icon.T_Icon"));
```

当你从这里往里单步，最值得期待看到的调用栈主线可以抽象成：

```text
业务函数
-> LoadObject<T>()
-> StaticLoadObject()
-> StaticLoadObjectInternal()
-> ResolveName2(...)
-> FindObject / 内存查找
-> LoadPackageInternal(...)   // 如果内存里没有，通常会来到这里
-> 包加载底层
-> 回到 StaticLoadObjectInternal()
-> 再次查找对象
-> 返回 UObject*
```

这条栈最关键的认知有两个：

#### 第一，`StaticLoadObjectInternal` 是真正的“对象入口闸门”

很多人以为 `LoadObject<T>` 就已经是关键点了，但真正值得长期盯住的是：

- `StaticLoadObject`
- `StaticLoadObjectInternal`

因为真正的路径规范化、对象查找、必要时触发包加载，都是在这里汇总的。

#### 第二，“对象加载”在很多情况下会退化成“先把包加载好”

你在调试器里经常会看到一种很典型的感觉：

> 我明明是在加载对象，为什么调用栈突然开始进入包、Linker、异步加载器这些东西？

答案就是：

> UE 通常不会把“某个对象的字节”孤零零抓出来，而是先保证它所属包进入可用状态。

所以从调用栈视角看，`LoadObject` 的本质常常是：

```text
先试图直接找到对象
-> 找不到
-> 退化成确保对象所在包已加载
-> 包好了以后再回头找对象
```

这就是为什么 `StaticLoadObjectInternal` 是理解资源加载的第一个关键断点。

---

### 21.3 情况二：同步 API，但底层带异步桥接的调用栈

这一条是很多人第一次真正调试 UE5.5 时最容易“脑子拧巴”的地方：

> 我明明调的是同步接口，为什么调用栈里会出现 `LoadPackageAsync`，甚至还有 `FlushAsyncLoading`？

这正是 `UObjectGlobals.cpp` 很值得细读的原因。

如果你从同步对象加载继续往里走，某些场景下你会看到更像下面这样的栈：

```text
业务函数
-> LoadObject / StaticLoadObject
-> StaticLoadObjectInternal
-> LoadPackageInternal
-> ShouldAlwaysLoadPackageAsync(...)
-> LoadPackageAsync(...)
-> FlushAsyncLoading(RequestID)
-> 回到 LoadPackageInternal
-> 回到 StaticLoadObjectInternal
-> 返回对象
```

这条栈要表达的核心事实是：

> “同步语义”不等于“底层完全不用异步加载器”。  
> 在 UE5.5 里，某些同步入口会借助异步加载基础设施，然后通过 flush 把结果同步化。

从调用栈角度看，这个现象的调试价值非常大：

- 如果你只看最外层 API，会误以为自己完全没碰异步系统
- 但如果你跟进到 `LoadPackageInternal`，会发现同步/异步边界其实没有你想象中那么绝对

因此以后当你遇到：

- 同步加载卡顿
- 同步接口却出现异步相关日志
- 栈里同时出现 `LoadPackageAsync` 和 `FlushAsyncLoading`

都不要惊讶，这反而是**正常且值得理解的实现细节**。

---

### 21.4 情况三：`FStreamableManager::LoadSynchronous` 的调用栈

这是项目层最常见、也最容易让人误判的一条栈。

典型代码可能是：

```cpp
UObject* Obj = StreamableManager.LoadSynchronous(Path);
```

它的调用栈主线通常更像：

```text
业务函数
-> FStreamableManager::LoadSynchronous(...)
-> FStreamableManager::RequestAsyncLoadInternal(...)
-> FStreamableManager::StreamInternal(...)
-> （必要时）LoadPackageAsync(...)
-> FStreamableHandle::WaitUntilComplete(...)
-> AsyncLoadCallbackWrapper(...)
-> FStreamableManager::AsyncLoadCallback(...)
-> FindInMemory(...)
-> 返回 UObject*
```

请注意，这里和上一种同步对象加载的差别非常关键：

#### 差别 1：`LoadSynchronous` 并不是“直接去 LoadObject 的另一层皮”

它更像是：

> 先用 Streamable 体系把请求建模出来，再用等待把这次请求同步化。

也就是说，它的核心不是“直接对象加载”，而是：

1. 把目标路径纳入 `FStreamableHandle`
2. 走 `RequestAsyncLoadInternal`
3. 由 `StreamInternal` 去决定每个目标怎么处理
4. 最后再通过 `WaitUntilComplete` 把异步请求变成同步结果

#### 差别 2：你在调试器里会明显感受到“请求对象”的存在

和 `LoadObject` 路线相比，`StreamableManager` 路线的调用栈里，多了一层非常有存在感的东西：

- `FStreamableHandle`
- `RequestedAssets`
- `ManagedActiveHandles`
- `AsyncLoadCallbackWrapper`

这意味着你调试时不能只问：

> “对象有没有加载出来？”

还要问：

> “这次请求有没有被正确创建、等待、完成、保活、释放？”

这也是为什么项目层一旦进入复杂资源管理，`FStreamableManager` 会比到处散落的 `LoadObject` 更可控。

---

### 21.5 情况四：真正异步 `RequestAsyncLoad` 的调用栈，要拆成两段看

这是调试异步加载时最重要的心智模型：

> 一次异步请求，通常不是“一条连续到底的调用栈”，而是至少两段。

典型代码：

```cpp
StreamableManager.RequestAsyncLoad(Path, FStreamableDelegate::CreateLambda([...]
{
    // 资源就绪后的逻辑
}));
```

从调试器角度看，建议你把它强行拆成两段：

#### 第一段：请求发起栈

```text
业务函数
-> RequestAsyncLoad(...)
-> RequestAsyncLoadInternal(...)
-> StreamInternal(...)
-> （必要时）LoadPackageAsync(...)
-> 返回给业务层一个 FStreamableHandle
```

这段栈回答的问题是：

- 请求有没有真的发出去？
- 路径有没有去重？
- 是已经在内存里，还是已在加载中？
- 有没有真正落到底层包加载入口？

#### 第二段：完成回调栈

```text
底层加载完成
-> AsyncLoadCallbackWrapper(...)
-> FStreamableManager::AsyncLoadCallback(...)
-> FindInMemory(...)
-> Handle 完成状态更新
-> DelegateToCall.ExecuteIfBound()
-> 业务层回调逻辑
```

这段栈回答的问题是：

- 包加载完成后，目标对象到底有没有被找到？
- Handle 是怎么从 pending 变成 complete 的？
- 业务层 delegate 到底有没有被执行？
- 回调执行时对象是真可用，还是路径解析就已经偏了？

调试异步问题时，很多人只盯第一段栈，这是不够的。  
因为“请求发出去了”不代表“对象最终被正确落地并交回业务层”。

---

### 21.6 情况五：`UAssetManager` 外面再包一层治理栈

如果项目层不是直接调 `StreamableManager`，而是从 `UAssetManager` 入口开始，调用栈会再多一层“策略外壳”。

你在调试器里常见的体验会像这样：

```text
业务函数
-> UAssetManager::LoadPrimaryAsset(s) / LoadPrimaryAssetsWithType(...)
-> 解析 PrimaryAssetId / Bundle / Rules
-> 收集待加载的软路径集合
-> 内部转交给 StreamableManager
-> RequestAsyncLoadInternal / StreamInternal
-> 底层包加载
-> Handle 完成
-> OnAssetStateChangeCompleted / 委托回调
-> 业务层继续执行
```

这条栈和直接 Streamable 路线最大的区别在于：

> 你看到的最外层，不再是“路径请求”，而是“主资源治理请求”。

因此调试时要多问几个治理层问题：

- PrimaryAssetId 有没有识别对？
- Bundle 名字是不是就配错了？
- 收集出来的软路径集合是不是比你想象中少？
- CookRule / Chunk 规则有没有导致某些运行时资源根本不该被找到？

所以在 `UAssetManager` 路线里，调用栈不是只告诉你“怎么加载”，还会告诉你“为什么会加载这些、而不是那些”。

---

### 21.7 把一次完整加载拆成“调用栈片段 + 状态变化”

真正高手调试资源加载，不会只看“函数顺序”，还会同时看**状态在每一层怎么变**。

下面给你一个非常实用的观察模板：

| 阶段 | 典型调用点 | 你要观察的状态 |
|---|---|---|
| 路径进入 | `ResolveName2` / `RequestAsyncLoadInternal` | 路径字符串有没有被修正、redirect、去重 |
| 内存快路径 | `FindInMemory` / `FindObject` | 对象是否其实早就在内存 |
| 包加载发起 | `LoadPackageInternal` / `LoadPackageAsync` | 是否真的进入包加载 |
| 调度中 | `StreamInternal` / `FStreamableHandle` | 是否 pending、是否被合并、是否被管理 |
| 加载完成 | `AsyncLoadCallback` / `StaticLoadObjectInternal` 返回前 | 对象是否已可解析 |
| 业务取用 | delegate / 返回值使用处 | 取到的是对象、类、还是错误层级的东西 |

你以后进调试器时，可以强迫自己别只看函数名，而是每到一层都问：

> 此刻“状态”发生了什么变化？

这是把调用栈真正看懂的关键。

---

### 21.8 一个完整例子：从软类路径到最终可实例化类

下面举一个项目里极其常见、也极其容易踩坑的例子：

```cpp
TSoftClassPtr<AActor> SoftClass(TEXT("/Game/Blueprints/BP_Enemy.BP_Enemy_C"));
UClass* EnemyClass = SoftClass.LoadSynchronous();
```

从调用栈角度，这件事可以这样理解：

```text
业务代码
-> TSoftClassPtr::LoadSynchronous()
-> 底层通常转到 StreamableManager::LoadSynchronous()
-> RequestAsyncLoadInternal()
-> StreamInternal()
-> LoadPackageAsync() 或命中内存
-> WaitUntilComplete()
-> AsyncLoadCallback / FindInMemory()
-> 拿到 UBlueprintGeneratedClass / UClass
-> 业务层把它传给 SpawnActor
```

这个例子里最容易错的地方有三个：

1. 路径写成了蓝图资产本体，没有 `_C`
2. 你以为拿到的是对象资源，实际上你需要的是 `UClass`
3. 加载完成后类确实拿到了，但业务层又把它当成别的类型去用

所以这类问题调试时，调用栈的价值不仅是看“有没有加载”，更是看：

> 你最后拿回业务层的那个东西，类型层级到底是不是你想要的。

---

### 21.9 调试器里最常见的三种“误读调用栈”

#### 误读 1：看到 `LoadPackageAsync`，就以为自己走的是纯异步业务流

不一定。  
它也可能只是同步接口在借异步底层，然后马上 flush。

#### 误读 2：看到回调执行了，就以为资源一定可用

不一定。  
你还要看回调里是找到正确对象了，还是只是包完成了、目标对象没对上。

#### 误读 3：看到对象不在内存，就以为一定还没加载过

不一定。  
它也可能是：

- 加载过但已释放
- Handle 生命周期结束后失去保活
- 你查的是对象本体，但真正需要查的是 `_C` 类

所以调用栈一定要和：

- 路径类型
- Handle 状态
- 日志上下文
- 业务层最终取用类型

一起看。

---

### 21.10 你真正应该练会的，不是背栈，而是“见栈识层”

这节最后送你一句非常重要的话：

> 真正高效的资源加载调试，不是把某一条调用栈死背下来，而是看到一段栈，就能立刻判断它属于哪一层、正在解决哪一步问题。

比如你在调试器里看到：

- `ResolveName2`：说明你还在路径/入口层
- `RequestAsyncLoadInternal`：说明你已经进入调度层
- `WaitUntilComplete`：说明你在看“异步请求被同步化”的桥
- `AsyncLoadCallback`：说明你已经来到完成回调阶段
- `UAssetManager::LoadPrimaryAssetsWithType`：说明你最外层是治理逻辑，不是裸加载

当你建立起这种“见栈识层”的能力后，UE 资源加载这套系统就不会再像一团线，而会变成一张分层清晰的地图。

---

## 22. 资源加载相关核心类关系图（类职责总表 + 调用关系图）

前面我们已经分章节把很多类都讲过了，但读到这里，很多人脑子里还是会有一个问题：

> 我知道这些类都重要，但它们彼此到底是什么关系？谁是“路径类”，谁是“请求类”，谁是“治理类”，谁又是真正往底层装载推进的类？

这一节就是专门解决这个问题。  
目标不是再补零散知识点，而是把整套资源加载系统里最关键的类，压缩成一张**可记忆、可复述、可调试**的关系地图。

---

### 22.1 先看一张总关系图：把核心类按层摆平

先看最重要的一张“全景图”：

```text
┌──────────────────────────────────────────────────────────────┐
│ 业务层 / 项目层                                              │
│  你的 Gameplay 代码、UI、角色生成、配置驱动加载逻辑         │
└──────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌──────────────────────────────────────────────────────────────┐
│ 路径与引用层                                                 │
│  FTopLevelAssetPath                                          │
│  FSoftObjectPath                                             │
│  TSoftObjectPtr / TSoftClassPtr                              │
└──────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌──────────────────────────────────────────────────────────────┐
│ 发现与查询层                                                 │
│  IAssetRegistry                                              │
│  FAssetData                                                  │
│  AssetRegistryState                                          │
└──────────────────────────────────────────────────────────────┘
                  │                               │
                  │                               ▼
                  │              ┌────────────────────────────────────┐
                  │              │ 治理层                             │
                  │              │ UAssetManager                      │
                  │              │ FPrimaryAssetId / Type / Rules     │
                  │              │ Bundle / Label / Cook / Chunk      │
                  │              └────────────────────────────────────┘
                  │                               │
                  └───────────────┬───────────────┘
                                  ▼
┌──────────────────────────────────────────────────────────────┐
│ 请求调度层                                                   │
│  FStreamableManager                                          │
│  FStreamableHandle                                           │
│  FStreamable                                                 │
└──────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌──────────────────────────────────────────────────────────────┐
│ 直接加载入口层                                               │
│  StaticLoadObject / LoadObject                               │
│  LoadPackage / LoadPackageAsync                              │
│  UObjectGlobals.cpp 中的统一入口逻辑                         │
└──────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌──────────────────────────────────────────────────────────────┐
│ 底层包加载与运行时装载层                                     │
│  UPackage                                                    │
│  FLinkerLoad                                                 │
│  AsyncPackageLoader2 / AsyncLoading                          │
│  PackageStore / IoDispatcher                                 │
└──────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌──────────────────────────────────────────────────────────────┐
│ 结果层                                                       │
│  UObject / UClass / UBlueprintGeneratedClass / CDO           │
└──────────────────────────────────────────────────────────────┘
```

这张图最值得记住的不是“类名很多”，而是下面这句话：

> UE 资源加载不是一个类在做所有事，而是一组**按层分工**的类在接力。

---

### 22.2 类职责总表：每个核心类到底是干什么的

下面这张表建议你收藏起来。  
如果以后你看源码看到某个类，不知道它该归到哪一类，就回到这张表。

| 类 / 结构 / 系统 | 所属层 | 核心职责 | 不该把它误认为 |
|---|---|---|---|
| `FTopLevelAssetPath` | 路径层 | 表达“包名 + 顶层资产名” | 已加载对象 |
| `FSoftObjectPath` | 路径层 | 表达可延迟解析的软对象路径 | 自动保活句柄 |
| `TSoftObjectPtr<T>` | 引用层 | 用软路径描述“目标对象” | 强引用 |
| `TSoftClassPtr<T>` | 引用层 | 用软路径描述“目标类” | 蓝图资产本体 |
| `IAssetRegistry` | 发现层 | 查询资产信息、依赖、标签、扫描结果 | 真正加载器 |
| `FAssetData` | 发现层 | 表示单条资产元数据记录 | `UObject*` |
| `UAssetManager` | 治理层 | 管理 Primary Asset、Rules、Bundle、Chunk、Cook 关系 | 底层包加载器 |
| `FPrimaryAssetId` | 治理层 | 标识主资源身份 | 对象路径 |
| `FStreamableManager` | 调度层 | 围绕软路径组织同步/异步请求 | 纯底层加载器 |
| `FStreamableHandle` | 调度层 | 表示一次加载请求的生命周期 | 资源对象本体 |
| `FStreamable` | 调度层内部 | 记录某目标路径当前加载状态与关联句柄 | 外部业务接口 |
| `StaticLoadObject` | 入口层 | 对象级同步加载入口 | 完整治理系统 |
| `LoadPackage` | 入口层 | 包级同步加载入口 | 对象级 API |
| `LoadPackageAsync` | 入口层 | 包级异步加载入口 | 项目层请求管理器 |
| `UPackage` | 包层 | 运行时包对象，装着导入导出和对象空间 | 单个资源对象 |
| `FLinkerLoad` | 传统底层 | 负责传统包解析、导入导出、序列化装配 | 项目层 API |
| `AsyncPackageLoader2` | 新底层 | UE5 新路线包加载驱动 | 业务可直接随手调用的高级管理器 |
| `PackageStore` | 新底层元数据层 | 提供 Cooked runtime 包元信息 | 资产注册表替代品 |
| `IoDispatcher` | 新底层 IO 层 | 负责运行时 IO 请求调度 | 资源治理器 |
| `UBlueprintGeneratedClass` | 结果层 | 蓝图编译后的类对象 | 蓝图资产本体 |

从这张表里，你应该能得到一个非常清晰的结论：

> `FSoftObjectPath` 负责“怎么描述目标”，  
> `FAssetData` 负责“怎么知道它存在”，  
> `FStreamableManager` 负责“怎么组织请求”，  
> `LoadPackage*` / `FLinkerLoad` / `AsyncPackageLoader2` 负责“怎么把它真正装进内存”，  
> `UAssetManager` 负责“项目为什么加载它、以及加载规则是什么”。

---

### 22.3 最核心的三条关系链

如果你不想一口气记住所有类，至少先把下面三条链记清。

#### 第一条：路径链

```text
FTopLevelAssetPath
-> FSoftObjectPath
-> TSoftObjectPtr / TSoftClassPtr
```

这条链回答的是：

> “我要加载谁？”

它只负责描述目标，不负责真正装载。

#### 第二条：调度链

```text
TSoftObjectPtr / FSoftObjectPath
-> FStreamableManager
-> FStreamableHandle
-> StreamInternal / AsyncLoadCallback
```

这条链回答的是：

> “我怎么把一次请求组织起来，并且知道它现在处于什么状态？”

#### 第三条：底层装载链

```text
StaticLoadObject / LoadPackage / LoadPackageAsync
-> UObjectGlobals.cpp
-> UPackage
-> FLinkerLoad 或 AsyncPackageLoader2
-> UObject / UClass
```

这条链回答的是：

> “目标资源最终是怎么被真正构造出来的？”

只要这三条链不混，你看资源加载源码时就不会老是把“描述目标”“查询目标”“发起请求”“真正装载”混成一件事。

---

### 22.4 `FStreamableManager` 和 `UAssetManager` 的关系图

这是项目实践里最值得记住的一组关系。

```text
UAssetManager
   │
   ├─ 管理 PrimaryAssetType / PrimaryAssetId
   ├─ 管理 Rules / Bundle / Chunk / CookRule
   ├─ 依赖 AssetRegistry 收集资产信息
   └─ 最终把需要加载的软路径交给
          ▼
     FStreamableManager
          │
          ├─ 创建 FStreamableHandle
          ├─ 合并重复请求
          ├─ 处理同步/异步桥接
          ├─ 处理 redirect / 保活 / 回调
          └─ 推进到底层包加载入口
```

一句话解释这组关系：

> `UAssetManager` 决定“按什么规则加载哪些资源”，  
> `FStreamableManager` 决定“这次具体怎么把这些软路径加载起来”。

所以：

- 你怀疑 **Bundle / Chunk / PrimaryAssetId** 配置有问题，优先看 `UAssetManager`
- 你怀疑 **请求没发出去 / 被合并 / 句柄状态不对 / 加载后不保活**，优先看 `FStreamableManager`

---

### 22.5 `AssetRegistry` 和真正加载器的关系图

这个关系也经常被误解，所以值得单独画。

```text
磁盘 / Cook 产物 / 注册表数据
        │
        ▼
IAssetRegistry
        │
        ├─ 产出 FAssetData
        ├─ 提供查询、过滤、依赖、标签信息
        └─ 告诉你“它看起来存在”
                 │
                 ▼
      FStreamableManager / LoadObject / LoadPackage
                 │
                 ▼
       真正的包加载与对象构造
```

这里最关键的认知是：

> AssetRegistry 是“情报系统”，不是“搬运系统”。

它负责告诉你：

- 有哪些资产
- 路径是什么
- 类是什么
- 依赖是什么

但它并不会因为你查到了 `FAssetData`，就自动把对象放进内存。

---

### 22.6 蓝图相关类的关系图：为什么会有 `_C`

蓝图这块最容易让初学者迷糊，所以也值得单独拎出来。

```text
UBlueprint（蓝图资产本体）
   │
   ├─ SkeletonGeneratedClass
   └─ GeneratedClass
            │
            ▼
   UBlueprintGeneratedClass（本质上是 UClass）
            │
            ├─ 可用于 TSoftClassPtr
            ├─ 可用于 SpawnActor / CreateWidget
            └─ 会有对应 CDO：Default__XXX_C
```

所以这里要牢牢记住：

- 想看蓝图资产本体：通常是不带 `_C`
- 想拿可实例化的类：通常是带 `_C`
- 想看默认对象：通常是 `Default__XXX_C`

这也是为什么很多“加载成功却不能实例化”的问题，根因根本不在加载器，而在**你拿错了层级**。

---

### 22.7 一个最实用的“看到类名就知道该看哪层”的速查图

你以后在源码里看到一个类名，可以先这样归类：

```text
看到 Path / Ptr：
  多半在路径与引用层

看到 AssetRegistry / AssetData：
  多半在发现层

看到 AssetManager / PrimaryAsset / Bundle / Rule：
  多半在治理层

看到 Streamable / Handle：
  多半在请求调度层

看到 LoadPackage / Linker / AsyncLoading / PackageStore / IoDispatcher：
  多半在底层装载层

看到 UBlueprintGeneratedClass / UObject / UClass：
  多半在结果层
```

这张速查图的价值非常大，因为它能帮你在第一次看到一个陌生类时，迅速决定：

- 我现在该往上看调用者？
- 还是往下看实现？
- 还是横向去找同层协作类？

---

### 22.8 调用关系图：一次典型项目加载请求是怎么穿过这些类的

下面这张图，把“项目里一次典型加载请求”压缩成一条主线：

```text
业务代码
  │
  ├─ 直接写死路径 / 软引用字段 / PrimaryAssetId
  │
  ▼
FSoftObjectPath / TSoftObjectPtr / TSoftClassPtr
  │
  ├─ 如果先查资料：IAssetRegistry / FAssetData
  │
  ├─ 如果走治理：UAssetManager
  │          │
  │          └─ 收集 Bundle / Rules / 路径集合
  │
  ▼
FStreamableManager 或 StaticLoadObject / LoadPackage
  │
  ├─ 创建 Handle / 组织请求
  ├─ 内存命中则直接返回
  └─ 未命中则推进到底层包加载
          │
          ▼
   LoadPackageInternal / LoadPackageAsync
          │
          ▼
   FLinkerLoad 或 AsyncPackageLoader2
          │
          ▼
   UPackage / UObject / UClass 构造完成
          │
          ▼
   回调 / 返回值 / 业务使用
```

这张图特别适合做两件事：

1. 你在讲解 UE 资源加载时，用它快速复述全链路  
2. 你在排查问题时，用它判断“我是卡在链路上的哪一段”

---

### 22.9 这套类关系图真正应该怎么用

不要把这一节当成“背诵表”。  
它最有用的用法，其实是下面三种：

#### 用法 1：读源码前先定位层级

看到类名，先判断它在哪一层，再决定要不要继续深挖。

#### 用法 2：排查问题时先缩小范围

例如：

- 路径错 / `_C` 错 -> 看路径与结果层
- 某资源“查得到但加载失败” -> 看发现层和底层装载层之间的断裂
- Bundle 不生效 -> 看治理层和调度层之间的交接
- 加载成功后又失效 -> 看调度层与结果层之间的保活关系

#### 用法 3：把分散章节重新串成一张地图

前面的章节是“逐个讲解”，这一节是“统一归图”。  
你应该反复回到这张图，把：

- 路径体系
- AssetRegistry
- StreamableManager
- AssetManager
- Linker / AsyncLoading / PackageStore

重新挂回同一张结构图里。

当你能把这张图在脑子里复原出来时，说明你对 UE 资源加载已经不是“记 API”，而是“有系统地图”了。

---

## 23. 资源加载实战代码模板库（同步/异步/软类/Primary Asset/批量加载）

这一节的目标非常直接：

> 不只是“看懂原理”，还要给你一组拿来就能改成项目代码的模板。

这里的模板都尽量遵守前面讲过的原则：

- 先分清对象、类、主资源这几个层级
- 该同步的同步，该异步的异步
- 需要生命周期管理时，不要只拿 `TSoftObjectPtr`
- 需要工程化调度时，优先用 `FStreamableManager`
- 需要项目级治理时，优先围绕 `UAssetManager`

你可以把这一节当成“资源加载小抄”。

---

### 23.1 使用这些模板前，先记 4 条原则

#### 原则 1：只拿一下就用，且可接受阻塞，才考虑同步加载

比如：

- 启动阶段少量关键数据
- 编辑器工具脚本
- 明确就是阻塞式初始化

#### 原则 2：涉及 UI、角色外观、音效、特效、批量资源，优先考虑异步

因为这些场景最容易造成：

- 帧卡顿
- 加载抖动
- 生命周期混乱

#### 原则 3：`TSoftObjectPtr` / `TSoftClassPtr` 只是“目标描述”

它们不是：

- 句柄
- 保活器
- 请求管理器

#### 原则 4：只要出现“要取消、要等待、要批量、要回调、要保活”

就应该优先想：

```text
FStreamableManager
+ FStreamableHandle
```

---

### 23.2 模板 1：最简单的同步对象加载

适用场景：

- 资源很少
- 明确允许阻塞
- 就是想快速拿一个对象

```cpp
UPROPERTY(EditDefaultsOnly, Category = "Assets")
TSoftObjectPtr<UTexture2D> IconTexture;

UTexture2D* UMyWidgetData::LoadIconSync() const
{
    if (IconTexture.IsNull())
    {
        return nullptr;
    }

    UTexture2D* Loaded = IconTexture.Get();
    if (!Loaded)
    {
        Loaded = IconTexture.LoadSynchronous();
    }

    return Loaded;
}
```

这个模板的关键点：

- `Get()` 先看内存里是否已经有对象
- `LoadSynchronous()` 才是真正可能触发加载的地方
- 适合“低频、小资源、可以接受阻塞”的场景

不适合：

- Tick 里频繁调用
- 战斗中临时加载大资源
- UI 连续打开时批量拉 widget/贴图

---

### 23.3 模板 2：同步加载一个软类并立即实例化

适用场景：

- 你需要的是 `UClass`
- 常见于 `SpawnActor`、`CreateWidget`
- 路径本质上应当指向 `_C`

```cpp
UPROPERTY(EditDefaultsOnly, Category = "UI")
TSoftClassPtr<UUserWidget> WidgetClass;

UUserWidget* UMyUIManager::CreateWidgetSync(APlayerController* PC)
{
    if (WidgetClass.IsNull() || !PC)
    {
        return nullptr;
    }

    TSubclassOf<UUserWidget> LoadedClass = WidgetClass.Get();
    if (!LoadedClass)
    {
        LoadedClass = WidgetClass.LoadSynchronous();
    }

    if (!LoadedClass)
    {
        return nullptr;
    }

    return CreateWidget<UUserWidget>(PC, LoadedClass);
}
```

这个模板最常见的坑：

1. 路径没写 `_C`
2. 你以为自己要的是蓝图资产，实际上你要的是类
3. 加载成功了，但拿回来的不是 `TSubclassOf<>` 语义

---

### 23.4 模板 3：围绕 `FStreamableManager` 的标准异步对象加载

适用场景：

- 不希望阻塞主线程
- 资源加载完成后执行回调
- 需要保留 handle 来控制生命周期

```cpp
// .h
UPROPERTY(EditDefaultsOnly, Category = "Assets")
TSoftObjectPtr<USkeletalMesh> CharacterMesh;

TSharedPtr<FStreamableHandle> CharacterMeshHandle;

void LoadCharacterMeshAsync();
void OnCharacterMeshLoaded();

// .cpp
void UMyCharacterCustomizer::LoadCharacterMeshAsync()
{
    if (CharacterMesh.IsNull())
    {
        return;
    }

    CharacterMeshHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
        CharacterMesh.ToSoftObjectPath(),
        FStreamableDelegate::CreateUObject(this, &ThisClass::OnCharacterMeshLoaded),
        FStreamableManager::DefaultAsyncLoadPriority
    );
}

void UMyCharacterCustomizer::OnCharacterMeshLoaded()
{
    USkeletalMesh* LoadedMesh = CharacterMesh.Get();
    if (!LoadedMesh)
    {
        CharacterMeshHandle.Reset();
        return;
    }

    // 把 LoadedMesh 应用到角色上
    // MeshComponent->SetSkeletalMesh(LoadedMesh);

    CharacterMeshHandle.Reset();
}
```

这个模板的关键点：

- `RequestAsyncLoad(...)` 返回的 handle 要保存下来
- 回调里优先通过软引用再次 `Get()`
- 用完后是否 `Reset()` 取决于你是否还想继续保留这次请求句柄

如果你完全不保存 handle，会失去：

- 取消
- 等待
- 绑定额外回调
- 生命周期可见性

---

### 23.5 模板 4：可取消的异步软类加载

这个模板很适合 UI、对话框、可重复打开的界面场景。  
思路和 Lyra 的 `AsyncAction_CreateWidgetAsync` 很接近。

```cpp
// .h
UPROPERTY(EditDefaultsOnly, Category = "UI")
TSoftClassPtr<UUserWidget> PopupWidgetClass;

TSharedPtr<FStreamableHandle> PopupWidgetHandle;

void BeginLoadPopup(APlayerController* PC);
void CancelLoadPopup();
void OnPopupClassLoaded(APlayerController* PC);

// .cpp
void UMyPopupSubsystem::BeginLoadPopup(APlayerController* PC)
{
    if (PopupWidgetClass.IsNull() || !PC)
    {
        return;
    }

    CancelLoadPopup();

    PopupWidgetHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
        PopupWidgetClass.ToSoftObjectPath(),
        FStreamableDelegate::CreateUObject(this, &ThisClass::OnPopupClassLoaded, PC),
        FStreamableManager::AsyncLoadHighPriority
    );
}

void UMyPopupSubsystem::CancelLoadPopup()
{
    if (PopupWidgetHandle.IsValid())
    {
        PopupWidgetHandle->CancelHandle();
        PopupWidgetHandle.Reset();
    }
}

void UMyPopupSubsystem::OnPopupClassLoaded(APlayerController* PC)
{
    TSubclassOf<UUserWidget> LoadedClass = PopupWidgetClass.Get();
    if (!LoadedClass || !PC)
    {
        PopupWidgetHandle.Reset();
        return;
    }

    UUserWidget* Widget = CreateWidget<UUserWidget>(PC, LoadedClass);
    if (Widget)
    {
        Widget->AddToViewport();
    }

    PopupWidgetHandle.Reset();
}
```

这个模板的价值在于：

- 重复请求前会先取消旧请求
- 回调只在当前这次请求还有效时落地
- 适合“用户可能快速开关界面”的场景

---

### 23.6 模板 5：批量异步加载多个资源

适用场景：

- 一次进入场景要加载一批资源
- 某个装备方案要同时拉模型、动画、音效、特效
- 希望只在“全部准备完”后再进入下一步

```cpp
// .h
UPROPERTY(EditDefaultsOnly, Category = "Assets")
TSoftObjectPtr<USkeletalMesh> Mesh;

UPROPERTY(EditDefaultsOnly, Category = "Assets")
TSoftObjectPtr<UAnimMontage> Montage;

UPROPERTY(EditDefaultsOnly, Category = "Assets")
TSoftObjectPtr<UNiagaraSystem> NiagaraFX;

TSharedPtr<FStreamableHandle> BatchHandle;

void PreloadLoadoutAsync();
void OnLoadoutAssetsReady();

// .cpp
void UMyLoadoutComponent::PreloadLoadoutAsync()
{
    TArray<FSoftObjectPath> AssetsToLoad;

    if (!Mesh.IsNull())
    {
        AssetsToLoad.Add(Mesh.ToSoftObjectPath());
    }
    if (!Montage.IsNull())
    {
        AssetsToLoad.Add(Montage.ToSoftObjectPath());
    }
    if (!NiagaraFX.IsNull())
    {
        AssetsToLoad.Add(NiagaraFX.ToSoftObjectPath());
    }

    if (AssetsToLoad.IsEmpty())
    {
        return;
    }

    BatchHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
        AssetsToLoad,
        FStreamableDelegate::CreateUObject(this, &ThisClass::OnLoadoutAssetsReady),
        FStreamableManager::DefaultAsyncLoadPriority
    );
}

void UMyLoadoutComponent::OnLoadoutAssetsReady()
{
    USkeletalMesh* LoadedMesh = Mesh.Get();
    UAnimMontage* LoadedMontage = Montage.Get();
    UNiagaraSystem* LoadedFX = NiagaraFX.Get();

    // 所有资源都准备好后，再统一进入下一阶段逻辑

    BatchHandle.Reset();
}
```

这个模板要点非常重要：

- 批量加载时，尽量统一等“全部就绪后”再推进业务
- 不要每个资源各写一套分散回调，除非它们真的独立
- `RequestAsyncLoad` 内部会对重复项做一定去重与管理

---

### 23.7 模板 6：同步等待一个批量请求完成

有些场景你虽然想走 Streamable 体系，但业务阶段又必须等它真的完成。  
这时可以用“异步请求 + 同步等待”的桥接模式。

```cpp
bool UMyPreloader::PreloadBlocking(const TArray<FSoftObjectPath>& Assets)
{
    if (Assets.IsEmpty())
    {
        return true;
    }

    TSharedPtr<FStreamableHandle> Handle =
        UAssetManager::GetStreamableManager().RequestAsyncLoad(
            Assets,
            FStreamableDelegate(),
            FStreamableManager::AsyncLoadHighPriority,
            false,
            false,
            TEXT("MyPreloader")
        );

    if (!Handle.IsValid())
    {
        return false;
    }

    const EAsyncPackageState::Type Result = Handle->WaitUntilComplete();
    return Result == EAsyncPackageState::Complete;
}
```

这个模板非常适合解释前面讲过的那件事：

> 有些“同步加载”其实不是完全绕开异步体系，而是借助请求句柄再等待完成。

但要记住：

- 这依然可能阻塞
- 只是组织方式比到处散落 `LoadObject` 更工程化

---

### 23.8 模板 7：通过 `UAssetManager` 加载 Primary Asset

适用场景：

- 你项目已经建立了 Primary Asset 体系
- 资源分组、Bundle、CookRule 都围绕 AssetManager 管理
- 想按“主资源身份”加载，而不是直接写死对象路径

```cpp
// .h
UPROPERTY(EditDefaultsOnly, Category = "Assets")
FPrimaryAssetId ExperienceId;

TSharedPtr<FStreamableHandle> ExperienceHandle;

void LoadExperienceAsync();
void OnExperienceLoaded();

// .cpp
void UMyExperienceSubsystem::LoadExperienceAsync()
{
    if (!ExperienceId.IsValid())
    {
        return;
    }

    TArray<FName> Bundles;
    Bundles.Add(TEXT("Game"));

    ExperienceHandle = UAssetManager::Get().LoadPrimaryAsset(
        ExperienceId,
        Bundles,
        FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded)
    );
}

void UMyExperienceSubsystem::OnExperienceLoaded()
{
    UObject* LoadedAsset = UAssetManager::Get().GetPrimaryAssetObject(ExperienceId);
    if (!LoadedAsset)
    {
        ExperienceHandle.Reset();
        return;
    }

    // Cast 到你的主资源类型后继续业务
    ExperienceHandle.Reset();
}
```

这个模板体现的是：

- 外部使用 `FPrimaryAssetId`
- 内部再由 AssetManager 去决定实际路径集合
- 更适合中大型项目的资源治理

---

### 23.9 模板 8：按类型批量加载 Primary Asset

这个模式和 Lyra 启动阶段很像，适合：

- 启动预热
- 某一类主资源统一加载
- 需要等整类资源准备完成后再继续

```cpp
void UMyGameBootstrapper::PreloadAbilitySets()
{
    const FPrimaryAssetType AssetType(TEXT("AbilitySet"));

    TSharedPtr<FStreamableHandle> Handle =
        UAssetManager::Get().LoadPrimaryAssetsWithType(AssetType);

    if (!Handle.IsValid())
    {
        return;
    }

    Handle->WaitUntilComplete(0.0f, false);

    // 到这里，这一类主资源的加载已经完成
}
```

如果你在编辑器工具或早期启动逻辑里需要“先拉齐一整类资源”，这是非常实用的模板。

但也要注意：

- 这类写法本质上很可能是阻塞式的
- 要控制使用时机，别在高频交互期滥用

---

### 23.10 模板 9：封装一个项目级同步加载辅助函数

很多项目都会像 Lyra 一样，做一个轻量封装，把日志、保活、判空统一起来。

```cpp
UCLASS()
class UMyAssetLibrary : public UObject
{
    GENERATED_BODY()

public:
    template <typename T>
    static T* LoadAssetChecked(const TSoftObjectPtr<T>& SoftPtr)
    {
        if (SoftPtr.IsNull())
        {
            return nullptr;
        }

        if (T* Existing = SoftPtr.Get())
        {
            return Existing;
        }

        T* Loaded = Cast<T>(UAssetManager::GetStreamableManager().LoadSynchronous(
            SoftPtr.ToSoftObjectPath(),
            false
        ));

        ensureAlwaysMsgf(Loaded, TEXT("Failed to load asset: %s"), *SoftPtr.ToString());
        return Loaded;
    }
};
```

这种封装的好处：

- 项目里同步加载风格统一
- 错误日志更集中
- 后续更容易统一替换策略

但不要把它误用成“所有地方都随手同步加载”的借口。

---

### 23.11 模板 10：封装一个组件级异步加载成员模式

如果你的组件/子系统经常要异步加载不同资源，推荐固定成“成员 handle + 开始/取消/完成”三段式。

```cpp
// .h
TSharedPtr<FStreamableHandle> ActiveHandle;

void StartLoad(const FSoftObjectPath& AssetPath);
void CancelLoad();
void HandleLoadComplete(FSoftObjectPath AssetPath);

// .cpp
void UMyLoaderComponent::StartLoad(const FSoftObjectPath& AssetPath)
{
    CancelLoad();

    if (!AssetPath.IsValid())
    {
        return;
    }

    ActiveHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
        AssetPath,
        FStreamableDelegate::CreateUObject(this, &ThisClass::HandleLoadComplete, AssetPath)
    );
}

void UMyLoaderComponent::CancelLoad()
{
    if (ActiveHandle.IsValid())
    {
        ActiveHandle->CancelHandle();
        ActiveHandle.Reset();
    }
}

void UMyLoaderComponent::HandleLoadComplete(FSoftObjectPath AssetPath)
{
    UObject* LoadedObject = AssetPath.ResolveObject();

    // 使用 LoadedObject

    ActiveHandle.Reset();
}
```

这是很推荐的项目级组织方式，因为它天然回答了 3 个问题：

1. 现在有没有正在进行的请求  
2. 新请求到来时旧请求怎么处理  
3. 完成后句柄在哪里释放

---

### 23.12 模板 11：常用“不要这样写”的反例

下面这些写法非常常见，但不推荐。

#### 反例 1：在高频路径里直接 `LoadSynchronous`

```cpp
void UMyHUDWidget::Tick(...)
{
    UTexture2D* Icon = IconSoftPtr.LoadSynchronous(); // 不推荐
}
```

问题：

- Tick 中阻塞
- 重复触发加载语义
- 维护者难以判断实际成本

#### 反例 2：异步请求发出去后完全不保存 handle

```cpp
UAssetManager::GetStreamableManager().RequestAsyncLoad(
    Path,
    FStreamableDelegate::CreateUObject(this, &ThisClass::OnLoaded)
);
```

问题：

- 不能取消
- 不能等待
- 很难观察请求生命周期

#### 反例 3：把 `TSoftClassPtr` 当成蓝图资产本体来用

```cpp
TSoftClassPtr<UUserWidget> WidgetClass = ...;
UObject* Obj = WidgetClass.LoadSynchronous(); // 语义上容易混乱
```

更推荐：

```cpp
TSubclassOf<UUserWidget> LoadedClass = WidgetClass.LoadSynchronous();
```

---

### 23.13 这一节模板库该怎么练

如果你想真正把这些模板变成自己的能力，我建议按下面顺序练：

1. 先练模板 1、2  
   先把“对象”和“类”分清

2. 再练模板 3、5  
   熟悉 handle、异步回调、批量请求

3. 再练模板 7、8  
   进入 Primary Asset / AssetManager 思维

4. 最后把模板 10 变成你项目里的统一模式  
   这是最像真实工程代码的一步

当你练到能自己从这套模板里挑出合适模式，而不是“所有情况都写 `LoadSynchronous`”，你就真的进入工程化资源加载了。

---

## 24. 资源加载性能优化清单（卡顿来源、预加载策略、避免重复加载、生命周期与缓存设计）

这一节不再只是讲“能不能加载出来”，而是专门回答一个工程上更现实的问题：

> 怎么把资源加载做得**更稳、更少卡顿、更少重复、更可控**？

很多项目后期的资源问题，其实都不是“不会加载”，而是：

- 能加载，但会卡；
- 能加载，但重复加载太多；
- 能加载，但生命周期设计差，导致反复进出内存；
- 能加载，但预加载时机不对，结果把卡顿从 A 点搬到了 B 点。

所以这一节的目标，是给你一张真正能落地执行的**性能优化清单**。

---

### 24.1 先建立性能认知：资源加载卡顿通常卡在哪

资源加载的卡顿，通常不是单一来源，而是几种成本叠加：

```text
路径解析
-> 请求组织
-> 包读取 / IO
-> 导入导出与序列化
-> 对象构造
-> PostLoad / 资源初始化
-> 业务层拿到资源后的二次初始化
```

也就是说，用户感知到的“这一下卡了”，背后可能是：

1. 你在错误时机做了同步加载  
2. 底层真的在读磁盘 / IoStore  
3. 包很大、依赖很多  
4. 对象创建后还有额外初始化成本  
5. 资源刚加载完又被业务层做了更重的操作

所以第一原则是：

> 不要把所有卡顿都简单归因成“磁盘慢”或“引擎加载慢”。

很多时候，真正的性能优化第一步，是**先把卡顿拆层**。

---

### 24.2 最常见的 8 类卡顿来源

#### 1）在高频路径里做同步加载

这是最经典的一类问题，比如：

- Tick
- 输入响应路径
- 技能释放瞬间
- UI 打开瞬间
- 角色生成瞬间才拉一堆资源

只要这里出现 `LoadSynchronous()`、`LoadObject()`、`WaitUntilComplete()`，就应该高度警惕。

#### 2）单次请求背后带出巨大依赖链

你以为只是加载一个蓝图类，实际上它可能连带：

- 材质
- 贴图
- SkeletalMesh
- 动画
- Niagara
- 数据资产

这时“一个资源”根本不止一个资源。

#### 3）重复加载同一批软路径

常见于：

- 多个系统各自请求同一个资源
- 没有统一预热层
- 每次打开页面都重新发起一轮加载

如果没有好的请求合并和缓存设计，性能会被这种“重复但不显眼”的加载慢慢拖垮。

#### 4）加载成功后马上释放，又很快再次加载

这是生命周期设计问题，不是加载 API 问题。

比如：

- 进背包界面加载图标，关掉就全释放
- 3 秒后再打开背包，又全量重拉
- 战斗中同类特效一会儿在，一会儿全掉

这种抖动会让 IO、CPU、内存都反复付费。

#### 5）预加载时机错误

不是所有预加载都是好事。

如果你把：

- 启动阶段不该预热的大资源
- 某一整类暂时根本用不到的主资源
- 很远未来才可能用到的 UI / 音效 / 地图相关资产

都提前拉起来，表面上“避免了后面卡顿”，实际上只是把卡顿堆到启动期或切场景期。

#### 6）把异步写成了“伪异步”

典型形式：

```text
RequestAsyncLoad(...)
-> 立刻 WaitUntilComplete()
```

这在组织上更工程化，但性能上未必比同步好。  
如果调用时机不对，它依然会阻塞。

#### 7）每个资源各自一个回调，导致加载碎片化

比如同一功能需要 8 个资源，你写成：

- 8 次独立请求
- 8 套独立完成逻辑
- 8 次零碎状态切换

这会让时序很碎、管理很乱，也更容易在业务上制造抖动。

#### 8）业务层缺少资源状态机

如果业务层没有明确区分：

- 未请求
- 加载中
- 已加载
- 可复用
- 需要释放

那么重复请求、重复构造、错误释放几乎一定会出现。

---

### 24.3 性能优化第一清单：先消灭最危险的同步点

下面这张清单建议你直接用来扫项目代码：

| 位置 | 是否允许同步加载 | 建议 |
|---|---|---|
| Tick / 高频更新 | 基本不允许 | 一律改异步或提前预热 |
| 输入响应路径 | 尽量不允许 | 提前预加载、缓存结果 |
| UI 打开瞬间 | 尽量不允许 | 页面进入前预热、界面分层加载 |
| 角色生成瞬间 | 谨慎 | 常用角色外观提前批量预热 |
| 启动阶段 | 允许但要克制 | 只加载真正的关键资源 |
| 编辑器工具流程 | 可接受 | 但仍建议控制规模 |
| 明确阻塞式初始化 | 可接受 | 要把阻塞点集中而不是分散 |

如果你只能做一轮最便宜的优化，我建议先做这个：

> 全局搜索 `LoadSynchronous`、`LoadObject`、`WaitUntilComplete`，然后检查它们是不是出现在高频路径里。

这通常是回报最高的一轮优化。

---

### 24.4 预加载策略清单：什么时候该提前加载，什么时候不该

预加载不是“越多越好”，而是“越准越好”。

#### 适合预加载的内容

- 启动后必用的核心 GameData
- 当前局内高概率会反复使用的 UI 类
- 本局角色/武器/能力的公共资源
- 很快就会展示的首屏内容
- 某个功能模块一旦进入就几乎必用的一组资源

#### 不适合预加载的内容

- 很低概率使用的稀有资源
- 需要很久以后才会出现的内容
- 体积巨大、依赖很多、却不一定会用到的资源
- 只是“怕出问题所以全拉一遍”的资源

#### 一个实用的分层预热模型

```text
启动预热：
  只拉最核心数据和必备类

功能预热：
  进入某模式/某页面前，拉该功能最可能使用的一组资源

临近预热：
  真正要展示前几秒，拉马上要出现的资源
```

这个模型的重点不是术语，而是：

> 把“什么时候加载”设计成多层，而不是只有“现在加载”和“启动全拉”两种极端。

---

### 24.5 避免重复加载的清单：先解决“隐形浪费”

很多项目性能不佳，不是因为某次加载特别重，而是因为**重复加载太多**。

下面是最实用的排查清单：

#### 清单 1：是否存在多个系统各自持有同一路径

比如：

- HUD 自己拉一次图标
- 商店界面再拉一次同图标
- 背包界面又单独拉一次

这时你应该考虑：

- 是否做统一图标缓存层
- 是否做统一 UI 资源服务层

#### 清单 2：是否每次进入功能都重新请求同一资源

例如：

- 菜单每打开一次都重新请求按钮蓝图类
- 每次切武器都重新请求已加载过的音效集

如果这些资源高频复用，就应该有更稳定的缓存或保活策略。

#### 清单 3：是否把可批量的请求拆成了很多零碎请求

如果一组资源总是一起出现，那它们就应该：

- 一起预热
- 一起加载
- 一起维护生命周期

而不是各自分散在不同逻辑角落里。

#### 清单 4：是否重复做“查 + 加载 + Cast + 初始化”

很多项目性能浪费不只在加载本身，还在“加载后又重复做相同处理”。  
所以缓存时别只缓存对象本身，也要考虑是否缓存：

- 已验证过的类
- 已构造好的 UI
- 已整理好的数据映射

---

### 24.6 生命周期设计清单：什么时候该保活，什么时候该释放

资源加载性能优化，本质上有一大半是生命周期设计。

你至少要先把资源分成这几类：

#### A 类：全局常驻

适合：

- 全局 UI 基础类
- 核心配置数据
- 整局都会反复用到的公共资源

策略：

- 启动或进入大场景时拉起
- 保持稳定驻留
- 不要频繁进出内存

#### B 类：模块常驻

适合：

- 某玩法模式内高频复用资源
- 某 UI 模块专属资源
- 某系统打开期间反复使用的内容

策略：

- 模块激活时加载
- 模块退出时统一释放

#### C 类：临时资源

适合：

- 一次性展示
- 短期使用
- 明显低频

策略：

- 按需加载
- 用后释放
- 但要避免短时间内反复加载/释放抖动

如果你没有这类分层，通常就会出现两个极端：

1. 什么都不敢留，导致反复重载  
2. 什么都一直留，导致内存膨胀

真正好的方案，一定是按资源角色做生命周期分层。

---

### 24.7 缓存设计清单：缓存什么，比“是否缓存”更重要

缓存不是单选题。  
在资源加载里，至少有四类东西可以缓存：

#### 1）缓存路径到对象结果

适合：

- 高频访问的小对象
- 常用贴图 / 数据资产

#### 2）缓存路径到类结果

适合：

- `TSoftClassPtr`
- 常用 Widget 类
- 常用 Actor 类

#### 3）缓存请求句柄或活跃状态

适合：

- 正在加载中的资源
- 需要取消/等待/观察状态的请求

#### 4）缓存“已准备好的业务结果”

比如：

- 已经创建好的 Widget
- 已整理好的配置表结构
- 已完成二次初始化的对象包装

很多项目优化做不到位，就是因为只缓存了“裸对象”，却没有缓存真正业务里反复要用的结果。

---

### 24.8 `FStreamableManager` 视角下的性能优化重点

如果你的项目已经大量使用 `FStreamableManager`，那下面这些点最重要：

#### 1）尽量复用统一的 manager，而不是到处各建一份

统一 manager 的好处：

- 更容易观察请求
- 更容易合并重复路径
- 更容易集中管理生命周期

#### 2）批量请求优于零碎请求

如果一组资源总是一起出现，优先：

```text
一次 RequestAsyncLoad(数组)
```

而不是：

```text
N 次单独 RequestAsyncLoad(单路径)
```

#### 3）明确 handle 的所有权

每个 handle 都应该回答清楚：

- 谁持有它
- 持有到什么时候
- 完成后是否释放
- 取消时谁负责收尾

如果 handle 没有明确所有权，项目后期很容易出现：

- 请求泄漏
- 提前释放
- 状态不可见

#### 4）不要把 `WaitUntilComplete` 当常规流程

它适合桥接、适合少数关键阶段，但不应成为常规业务路径的默认写法。

---

### 24.9 `UAssetManager` 视角下的性能优化重点

如果你已经进入 Primary Asset 体系，优化重点会从“单个资源怎么拉”变成“整组资源如何治理”。

#### 重点 1：按玩法/模块划分 Primary Asset

不要把完全不同生命周期的资源混进同一类管理里。

#### 重点 2：Bundle 要按“共同出现”来设计

Bundle 的设计原则不是“按美术目录分”，而是：

> 哪些资源会在同一业务时机一起出现？

#### 重点 3：CookRule / Chunk 要服务运行时加载策略

不要把 Cook / Chunk 只当打包配置。  
它们最终会影响：

- 运行时是否能找到资源
- 是否能按预期分块到达
- 某次加载会不会带出过多无关内容

#### 重点 4：启动预热要有明确预算

如果 `LoadPrimaryAssetsWithType()` 一上来拉太多，你只是把卡顿集中到启动阶段。  
所以每类主资源都应该问一句：

> 它真的是启动必需的吗？

---

### 24.10 一套很实用的性能优化执行顺序

如果你接手一个已经卡顿明显的项目，我建议按这个顺序做：

#### 第一步：找同步热点

全局搜：

- `LoadSynchronous`
- `LoadObject`
- `WaitUntilComplete`

先把高频路径里的同步点列出来。

#### 第二步：找重复请求热点

重点看：

- 同一路径是否在多个系统被重复请求
- 同一功能是否每次进入都重复发请求

#### 第三步：找错误预热

检查：

- 启动是否拉了过多资源
- 某些低频资源是否被无脑提前加载

#### 第四步：找生命周期抖动

看是否存在：

- 刚加载完就释放
- 短时间内重复进出内存
- 本应常驻的资源却频繁重建

#### 第五步：最后才深挖底层包加载细节

只有当前四步排完以后，才值得进一步深挖：

- 包是否过大
- 依赖是否过深
- 某些 PostLoad 是否异常重

这套顺序的意义在于：

> 先抓最常见、最容易改、收益最大的工程问题，再去攻最重的底层问题。

---

### 24.11 一个可直接落地的性能优化检查表

你可以把下面这张表直接当项目检查单来用：

| 检查项 | 是 / 否 | 优化方向 |
|---|---|---|
| Tick / 输入 / 高频 UI 路径里有同步加载吗？ |  | 改异步或提前预热 |
| 同一路径是否在多个系统重复请求？ |  | 做统一服务层或缓存层 |
| 一组总是同时出现的资源是否被拆成零碎加载？ |  | 合并成批量请求 |
| 启动阶段是否拉了很多低频资源？ |  | 拆成分层预热 |
| 资源加载后是否很快又被释放？ |  | 调整生命周期分层 |
| 是否缺少模块级缓存？ |  | 建立模块常驻资源池 |
| 是否过度依赖 `WaitUntilComplete`？ |  | 改成真正异步完成流 |
| 是否缺少统一加载入口？ |  | 建立项目级加载封装 |
| Primary Asset / Bundle 设计是否贴近业务时机？ |  | 重新按玩法组织 |
| 日志和调试信息是否足够定位重复加载？ |  | 增加 debug name / dump 手段 |

如果一个项目这 10 项里有 5 项以上都是“是问题”，那它的资源加载性能通常一定还有明显优化空间。

---

### 24.12 最后给你一个性能优化口诀

这一节最后，把最重要的东西压成一句口诀：

> **少在高频点同步，少做低价值预热，少让资源反复进出内存，尽量把“重复请求”变成“统一请求”，把“临时拿一下”变成“按生命周期管理”。**

如果你真的把这句话落实到项目里，资源加载性能通常就会比“想到哪拉到哪”高出一个层级。

---

## 25. 资源加载问题排查决策树（出现某种现象时应该先查哪一层）

前面我们已经写了：

- 常见坑
- 排障案例
- 调试日志与断点
- 调用栈时序

但真正到项目里排问题时，你最常遇到的还是这个局面：

> 现象我看到了，但我第一步到底该先查路径层、发现层、调度层、治理层，还是底层装载层？

这一节的目标，就是把“排查顺序”固化成一棵能快速落地的决策树。  
你不一定要逐字背下来，但最好养成这种排障顺序。

---

### 25.1 先记住总决策原则：永远先排便宜问题，再排昂贵问题

资源加载问题最容易犯的错，是一上来就：

- 跳进 `AsyncLoading2.cpp`
- 怀疑 IoStore / PackageStore
- 觉得是引擎底层坏了

但现实里，更多问题其实出在：

1. 路径写错  
2. `_C` 写错  
3. 查到的是资产元数据，不是对象  
4. 请求根本没发对  
5. 生命周期没管住  
6. Cook / Bundle / Chunk 规则没配对

所以这棵决策树的总原则只有一句：

> **先查路径层，再查发现层，再查请求调度层，再查治理层，最后才查底层装载层。**

除非你已经有非常强的证据表明问题就在底层。

---

### 25.2 总体排查决策树（最值得先记的一张）

先给你一张总图：

```text
出现资源加载问题
        │
        ├─ A. 是“根本找不到 / 路径明显不对”吗？
        │      └─ 先查 路径层
        │
        ├─ B. 是“AssetRegistry 查得到，但对象拿不到”吗？
        │      └─ 先查 发现层 -> 再查 入口层 / 底层装载层
        │
        ├─ C. 是“异步请求像没发出去 / 回调没来”吗？
        │      └─ 先查 调度层
        │
        ├─ D. 是“Primary Asset / Bundle / Chunk / Cook 后才出问题”吗？
        │      └─ 先查 治理层
        │
        ├─ E. 是“加载成功过，但后面又失效 / 又重新加载”吗？
        │      └─ 先查 生命周期 / 缓存 / 调度层
        │
        └─ F. 是“明显卡顿 / 阻塞 / 大抖动”吗？
               └─ 先查 调用时机 + 同步点 + 预热策略
```

这张图最重要的意义是：

> 先按“现象类型”决定排查起点，而不是看到任何问题都统一冲到底层。

---

### 25.3 决策树 1：如果现象是“根本找不到资源”

典型现象：

- `LoadSynchronous()` 返回空
- `LoadObject()` 返回空
- `TSoftClassPtr` 拿不到类
- 报错里直接出现 invalid package name / failed attempt to load

先按下面顺序排：

```text
第一步：查路径层
  ├─ 路径是不是对象路径？
  ├─ 该不该带 _C？
  ├─ 包名和对象名是不是混了？
  └─ 子对象路径是不是写错了？

第二步：查引用层
  ├─ 是 TSoftObjectPtr 还是 TSoftClassPtr？
  └─ 你要对象，还是要类？

第三步：再查发现层
  ├─ AssetRegistry 里到底有没有这条记录？
  └─ 查到的是不是你以为的那个类型？
```

这里最常见的根因其实就三类：

1. 路径写错  
2. `_C` 层级搞错  
3. 你拿的是类路径，却按对象去读

所以：

> 只要现象是“根本找不到”，第一优先几乎永远不是底层加载器，而是路径层。

---

### 25.4 决策树 2：如果现象是“AssetRegistry 查得到，但加载失败”

这个现象非常有迷惑性，因为它让人误以为：

> “既然都查到了，说明资源应该没问题，那一定是加载器坏了。”

其实不一定。

正确顺序应该是：

```text
第一步：确认查到的是 FAssetData，不是 UObject
        │
第二步：确认对象路径 / 类路径是否与 FAssetData 对应
        │
第三步：检查是否是 Cook 后产物缺失 / 路径重定向 / 运行时不可达
        │
第四步：再查 LoadPackage / LoadPackageAsync / Linker / IoDispatcher
```

优先排查问题：

- 你查到的是元数据，不是对象
- 编辑器能查到，不代表 Cooked runtime 一定能取到
- 路径在注册表里存在，不代表最终包数据一定存在

这种场景最适合先看：

- `FAssetData`
- `GetObjectPathString()`
- `LogAssetRegistry`
- `LogAssetManager`
- `LogLinker`

---

### 25.5 决策树 3：如果现象是“异步请求像没发出去”

典型现象：

- 调了 `RequestAsyncLoad()`，但没反应
- 回调没来
- 句柄状态不符合预期
- 看起来像请求被吞了

第一优先查的是**调度层**，不是底层。

```text
第一步：查 RequestAsyncLoadInternal 是否进去了
        │
第二步：查路径集合是否为空 / 有 null / 被去重后变空
        │
第三步：查是不是 Already Loaded / Already in progress
        │
第四步：查 handle 是否被保存、取消、提前释放
        │
第五步：再查是否真的落到了 LoadPackageAsync
```

这个现象最常见的根因包括：

- 路径本来就是空
- 资源已经在内存里，所以你误以为“没请求”
- 已经有同路径请求在进行中，被合并了
- handle 没持有住
- 回调对象失效了

所以：

> “像没发出去”这件事，70% 以上先查 `FStreamableManager` / `FStreamableHandle`。

---

### 25.6 决策树 4：如果现象是“回调到了，但对象还是不对”

典型现象：

- 回调执行了，但 `Get()` 还是空
- 回调里拿到的不是预期类型
- `SpawnActor` / `CreateWidget` 还是失败

正确排查顺序：

```text
第一步：查你回调里取的是对象还是类
        │
第二步：查软引用类型是不是选错了
        │
第三步：查是否缺 _C
        │
第四步：查回调执行时对象是否真已 ResolveObject 成功
        │
第五步：再查业务层后续 Cast / 创建逻辑
```

这个现象最典型的根因不是“没加载”，而是：

- 加载层级拿错了
- 资源类型理解错了
- 回调只是说明包完成了，但你真正要的类/对象层级不对

一句话说：

> 回调到了，只说明“这次流程走到了某个完成点”，不自动等于“你拿到的就是对的对象”。

---

### 25.7 决策树 5：如果现象是“加载成功了，但很快又失效”

典型现象：

- 刚加载完可用，过一会儿又空了
- 页面再次打开又重新加载
- 资源一会儿在、一会儿不在

这时第一优先不要查路径，应该查：

```text
生命周期层 / 调度层 / 缓存设计
```

排查顺序：

```text
第一步：查 handle 有没有被保存
        │
第二步：查管理对象是否还活着
        │
第三步：查资源是否只有软引用，没有稳定持有
        │
第四步：查是不是模块退出时统一释放了
        │
第五步：查缓存策略是否导致反复进出内存
```

这种问题最常见的根因：

- 只有路径，没有保活关系
- handle 完成后立刻释放，但业务还以为资源会长期在
- 资源本该模块常驻，却被当成一次性资源处理

所以如果现象是“加载成功过”，说明你至少已经过了路径层和入口层；这时更应该怀疑**生命周期设计**。

---

### 25.8 决策树 6：如果现象是“编辑器正常，打包后失败”

这是一个非常重要的专门分支。

因为这类问题大概率不是普通路径错，而是**治理层 / Cooked runtime 环境差异**。

排查顺序建议固定成：

```text
第一步：查 AssetManager / Primary Asset / Bundle / CookRule
        │
第二步：查 Chunk 与运行时可达性
        │
第三步：查 Cook 后是否真的产出了对应资源
        │
第四步：查 PackageStore / IoDispatcher / Linker 日志
```

为什么这里不先查普通路径层？

因为如果编辑器一直正常，而 Cook 后才出问题，通常说明：

- 编辑器能扫描磁盘补救
- Cooked runtime 不具备同样条件

这时更高概率的问题在：

- 治理规则没覆盖到
- 资源没被带进产物
- 分块/打包可达性不对

---

### 25.9 决策树 7：如果现象是“启动或切换场景时明显卡顿”

这个现象第一优先不是“是不是某个路径错了”，而是：

```text
先查性能层 / 调用时机 / 同步点 / 预加载策略
```

建议排查顺序：

```text
第一步：搜有没有 LoadSynchronous / WaitUntilComplete
        │
第二步：看是否在错误时机批量拉了大资源
        │
第三步：看是否把很多低频资源塞进启动预热
        │
第四步：看是否存在重复加载
        │
第五步：最后才看底层包是否特别大、依赖是否特别深
```

如果你一看到卡顿就先去查 `AsyncLoading2`，往往效率不高。  
因为更常见的现实是：

- 调用时机不对
- 同步点放错地方
- 一次性拉了太多

---

### 25.10 决策树 8：如果现象是“同一个资源总是被重复加载”

这是非常典型的工程问题。

优先排查：

```text
缓存层 / 调度层 / 模块边界
```

顺序建议：

```text
第一步：查是否多个系统各自请求同一路径
        │
第二步：查是否缺统一服务层或加载封装
        │
第三步：查是否每次进入功能都重新发请求
        │
第四步：查 handle / 已加载结果是否被统一复用
```

这个问题经常不是 API 用错，而是项目架构没有统一入口。

所以：

> 现象是“总在重复加载”时，优先看项目组织方式，而不是先看底层加载器。

---

### 25.11 一张可直接照着走的“现象 -> 优先层级”速查表

| 现象 | 第一优先排查层 | 第二优先排查层 | 第三优先排查层 |
|---|---|---|---|
| 根本找不到资源 | 路径层 | 引用层 | 发现层 |
| 查得到但加载失败 | 发现层 | 入口层 | 底层装载层 |
| 异步像没发出去 | 调度层 | 生命周期层 | 底层入口层 |
| 回调到了但对象不对 | 引用层 | 结果层 | 业务使用层 |
| 加载成功后又失效 | 生命周期层 | 缓存层 | 调度层 |
| 编辑器正常打包后失败 | 治理层 | Cook 产物层 | 新运行时底座 |
| 启动/切换明显卡顿 | 性能层 | 调用时机层 | 底层装载层 |
| 同一路径反复加载 | 缓存层 | 调度层 | 项目组织层 |

如果你时间非常紧，这张表就够用了。

---

### 25.12 真正实战时，这棵决策树怎么用

最推荐的用法不是“全文通读一遍后再想”，而是：

#### 第一步：先给现象贴标签

比如：

- “找不到”
- “查得到但拿不到”
- “异步没反应”
- “打包后才坏”
- “会卡”
- “会重复”

#### 第二步：按决策树决定第一站

不要一上来乱搜全引擎源码。  
先决定你的第一站到底是：

- 路径层
- 调度层
- 治理层
- 底层装载层

#### 第三步：只要第一站没排干净，不要急着跳下一层

这是最重要的纪律。

例如：

- `_C` 还没确认，就别急着怀疑 IoDispatcher
- handle 是否还活着还没确认，就别急着怀疑 PackageStore
- Bundle 规则还没确认，就别急着怀疑 Linker

很多排障效率低，不是因为不会调试，而是因为**跳层太快**。

---

### 25.13 最后给你一个排障口诀

把这一节压成一句口诀，就是：

> **先看路径对不对，再看查没查到，再看请求发没发，再看资源活没活，最后才看底层装没装。**

如果问题只在打包后出现，再额外补一句：

> **编辑器能找到，不代表 Cook 后一定能拿到；先查治理，再查产物，再查运行时底座。**

这两句话，基本就是整棵资源加载排查决策树的实战版总结。

---

## 26. 资源加载学习闭环：建议练习题 + 自测题 + 源码追问题

这一节的目标，是把这篇文档从“可读”变成“可练、可检验、可复盘”。

因为真正学会 UE 资源加载，不是看完一篇长文就结束，而是要完成一个闭环：

```text
读概念
-> 看源码
-> 自己写一点
-> 自己排一点错
-> 回头再解释一遍
```

只要这个闭环没形成，你就很容易停留在：

- 术语都见过
- 章节都看过
- 真遇到问题还是不会下手

所以这一节，我给你三套东西：

1. 建议练习题：帮你把知识落到手上  
2. 自测题：帮你判断自己到底有没有真的学会  
3. 源码追问题：帮你从“会用”走到“会读源码”

---

### 26.1 最推荐的学习闭环顺序

建议你按下面这个顺序练，而不是随便挑：

```text
第 1 轮：概念校准
  目标：分清路径、对象、类、AssetRegistry、Streamable、AssetManager

第 2 轮：代码实操
  目标：自己写出同步、异步、软类、Primary Asset、批量加载

第 3 轮：调试复盘
  目标：能通过日志、断点、调用栈解释“为什么这样加载”

第 4 轮：源码追问
  目标：能从一个 API 顺着看到核心实现层
```

这四轮走完，你对 UE 资源加载的理解会比“只看过文档”扎实很多。

---

### 26.2 建议练习题（基础组）

这一组的目标是：  
先把最基础、最容易混淆的对象层级练扎实。

#### 练习 1：分别加载对象资源和类资源

要求：

- 准备一个 `TSoftObjectPtr<UTexture2D>`
- 再准备一个 `TSoftClassPtr<UUserWidget>`
- 分别用同步方式把它们加载出来

练习目标：

- 分清“对象”和“类”
- 分清 `LoadSynchronous()` 后拿到的到底是什么
- 理解为什么类路径经常需要 `_C`

#### 练习 2：故意把 `_C` 去掉，再观察现象

要求：

- 先让软类引用正常工作
- 再故意把路径改成不带 `_C`
- 记录日志、返回值和后续创建逻辑的变化

练习目标：

- 把“蓝图资产本体”和“蓝图生成类”差异真正练成直觉

#### 练习 3：用 `AssetRegistry` 查一个资源，再真正加载它

要求：

- 先查出 `FAssetData`
- 打印对象路径、类路径、标签
- 再尝试实际加载

练习目标：

- 体会 `FAssetData != UObject*`
- 练会“查询层”和“加载层”的分离思维

---

### 26.3 建议练习题（进阶组）

这一组的目标是：  
进入项目工程化资源加载思维。

#### 练习 4：自己写一个标准异步加载函数

要求：

- 使用 `FStreamableManager::RequestAsyncLoad`
- 保存 `FStreamableHandle`
- 在回调里读取结果
- 加载完成后正确释放或保留句柄

练习目标：

- 理解 handle 生命周期
- 理解“请求发起”和“结果回调”是两段流程

#### 练习 5：自己写一个“可取消”的异步加载

要求：

- 连续触发两次加载
- 第二次开始前取消第一次
- 确认旧回调不会错误污染新状态

练习目标：

- 练会请求所有权
- 练会避免 UI / 子系统重复请求导致状态错乱

#### 练习 6：把 3 个资源改成批量加载

要求：

- 准备 3 个总是一起出现的资源
- 从“3 次独立请求”改成“1 次批量请求”
- 对比代码组织方式

练习目标：

- 感受批量请求在时序和管理上的优势
- 理解为什么“共同出现的资源”更适合一起加载

---

### 26.4 建议练习题（项目治理组）

这一组的目标是：  
从“会调 API”升级到“理解项目级资源治理”。

#### 练习 7：定义一个自己的 Primary Asset 类型

要求：

- 选一类你项目中的数据资产
- 在 `AssetManager` 配置中把它纳入 Primary Asset 体系
- 让它能被按类型加载

练习目标：

- 理解 `PrimaryAssetType` 不是普通标签
- 理解为什么项目级治理需要稳定的身份体系

#### 练习 8：为一个 Primary Asset 增加 Bundle 概念

要求：

- 给主资源拆出一组 Bundle
- 分别观察加载不同 Bundle 时带出的资源集合

练习目标：

- 理解 Bundle 不只是“分类”，而是运行时加载分组

#### 练习 9：模拟“编辑器正常，打包后失败”的排查

要求：

- 故意让一个资源不进入正确治理规则
- 在编辑器里验证它仍可能能查到
- 再思考 Cooked runtime 为什么会失败

练习目标：

- 真正建立“治理层 -> 产物层 -> 运行时底座”的排障思维

---

### 26.5 自测题（概念判断题）

这部分不要急着看答案，先自己答。

#### 题 1

`FAssetData` 能代表对象已经被加载到内存。  

你的答案应该是：**错**  

#### 题 2

`TSoftObjectPtr` 天然能保证对象在后续逻辑里一直不被 GC 回收。  

你的答案应该是：**错**  

#### 题 3

`TSoftClassPtr` 常常对应的是可实例化类路径，所以经常需要 `_C`。  

你的答案应该是：**对**  

#### 题 4

同步加载接口在 UE5.5 中完全不会走异步加载基础设施。  

你的答案应该是：**错**  

#### 题 5

`UAssetManager` 的核心定位更偏治理层，而不是底层包加载器。  

你的答案应该是：**对**

---

### 26.6 自测题（简答题）

下面这些题，最好能不看文档，直接口述或写下来。

#### 题 1：`LoadObject` 和 `FStreamableManager::LoadSynchronous` 的核心区别是什么？

你至少应该答到：

- 前者更偏对象入口
- 后者更偏“先建请求、再同步等待”
- 后者天然更适合工程化请求管理

#### 题 2：为什么说 AssetRegistry 不是加载器？

你至少应该答到：

- 它提供的是元数据和发现能力
- 查到 `FAssetData` 不等于对象已在内存

#### 题 3：为什么“加载成功了但后面又失效”常常不是路径问题？

你至少应该答到：

- 说明至少某个阶段曾经成功过
- 更应优先怀疑生命周期、缓存、handle、保活关系

#### 题 4：为什么“编辑器正常，打包后失败”要优先查治理层？

你至少应该答到：

- 编辑器能扫描磁盘补救
- Cook 后运行时更依赖治理结果和最终产物可达性

---

### 26.7 自测题（时序题）

这一组题，专门检验你是否真正理解“调用链”。

#### 题 1：请按顺序说出一次典型同步对象加载大致会经过哪些步骤

你至少应该能说出类似：

```text
路径进入
-> StaticLoadObjectInternal
-> 内存查找
-> 必要时 LoadPackageInternal
-> 包加载
-> 再次找对象
-> 返回结果
```

#### 题 2：请说出一次异步 `RequestAsyncLoad` 至少分成哪两段

你至少应该答到：

1. 请求发起栈  
2. 完成回调栈

#### 题 3：请说出从 `UAssetManager` 入口发起加载时，最外层比直接 `StreamableManager` 多了什么

你至少应该答到：

- Primary Asset / Bundle / Rules 的治理外壳

---

### 26.8 源码追问题（入门级）

这一组问题，适合你边看源码边回答。

#### 追问 1

`FSoftObjectPath` 里到底存了哪些关键字段？  
你可以去 `SoftObjectPath.h` 找答案。

#### 追问 2

`StaticLoadObjectInternal` 在“先查内存”和“触发包加载”之间是怎么决策的？  
你可以去 `UObjectGlobals.cpp` 找答案。

#### 追问 3

`FStreamableManager::RequestAsyncLoadInternal` 里为什么要先清理 null、去重、整理请求集？  
你可以去 `StreamableManager.cpp` 找答案。

#### 追问 4

`FStreamableHandle::WaitUntilComplete` 到底阻塞了什么？  
你可以去 `StreamableManager.cpp` 结合调用关系理解。

---

### 26.9 源码追问题（进阶级）

这一组问题，适合你已经能看懂主链后再做。

#### 追问 5

为什么 `LoadPackageInternal` 某些场景会转去 `LoadPackageAsync`？  
这说明了同步/异步关系的什么本质？

#### 追问 6

`FStreamableManager` 是如何把“路径请求”转化成“包加载请求”的？

#### 追问 7

`AsyncLoadCallback` 到底是在什么时候把“包加载完成”转化成“对象可取用”？

#### 追问 8

`UAssetManager` 是怎样从配置和注册表里建立 Primary Asset 视图的？

---

### 26.10 源码追问题（高手级）

这组问题更适合做“深读任务”，不要求一次答完。

#### 追问 9

旧 `FLinkerLoad` 路线和 `AsyncPackageLoader2` 路线，在 UE5.5 中到底是如何并存的？

#### 追问 10

`PackageStore` 在 Cooked runtime 中替代或增强了旧体系里的哪些职责？

#### 追问 11

如果一个同步接口最后借助异步加载器 + flush 完成，它在工程语义上到底算“同步”还是“异步”？

#### 追问 12

如果你要为项目做一套统一资源服务层，它应该落在：

- 软引用层之上？
- StreamableManager 之上？
- AssetManager 之上？

为什么？

---

### 26.11 一个最实用的复盘方法：把问题讲给自己听

当你做完上面的练习和自测后，最推荐的最后一步不是继续看新资料，而是做一次复盘：

你尝试不用看文档，只回答下面 5 个问题：

1. 资源路径、资产元数据、运行时对象，这三者到底是什么关系？  
2. `LoadObject`、`FStreamableManager`、`UAssetManager` 三者分工是什么？  
3. 为什么 `_C` 经常是排障关键？  
4. 为什么同步接口有时也会看到异步底层？  
5. 如果打包后失败，你第一站该查哪一层？

如果你能把这 5 个问题用自己的话讲顺，说明你已经不是“背知识点”，而是真的形成了理解结构。

---

### 26.12 最后给你一个学习闭环口诀

把这一节压缩成一句话，就是：

> **先学会分层，再学会写模板，再学会看调用栈，再学会按现象排障，最后再逼自己去追核心源码。**

如果你能把这条线真正走通，那你对 UE 资源加载的掌握程度，就会从“知道几个 API”提升到“能独立解释、实现、排障和优化”。

---

## 27. 关键源码索引（按主题）

### 27.1 路径与软引用

- `Engine\Source\Runtime\CoreUObject\Public\UObject\SoftObjectPath.h`
- `Engine\Source\Runtime\CoreUObject\Private\UObject\SoftObjectPath.cpp`
- `Engine\Source\Runtime\CoreUObject\Public\UObject\SoftObjectPtr.h`

### 27.2 对象/包直接加载入口

- `Engine\Source\Runtime\CoreUObject\Private\UObject\UObjectGlobals.cpp`

### 27.3 传统包加载

- `Engine\Source\Runtime\CoreUObject\Public\UObject\Package.h`
- `Engine\Source\Runtime\CoreUObject\Private\UObject\LinkerLoad.cpp`

### 27.4 异步加载总入口与加载器选择

- `Engine\Source\Runtime\CoreUObject\Public\Serialization\AsyncPackageLoader.h`
- `Engine\Source\Runtime\CoreUObject\Private\Serialization\AsyncPackageLoader.cpp`
- `Engine\Source\Runtime\CoreUObject\Private\Serialization\AsyncLoading.cpp`
- `Engine\Source\Runtime\CoreUObject\Public\Serialization\AsyncLoading2.h`
- `Engine\Source\Runtime\CoreUObject\Private\Serialization\AsyncLoading2.cpp`

### 27.5 StreamableManager（重点）

- `Engine\Source\Runtime\Engine\Classes\Engine\StreamableManager.h`
- `Engine\Source\Runtime\Engine\Private\StreamableManager.cpp`

### 27.6 AssetRegistry

- `Engine\Source\Runtime\AssetRegistry\Public\AssetRegistry\IAssetRegistry.h`
- `Engine\Source\Runtime\CoreUObject\Public\AssetRegistry\AssetData.h`
- `Engine\Source\Runtime\AssetRegistry\Public\AssetRegistry\AssetRegistryState.h`

### 27.7 AssetManager / Primary Asset

- `Engine\Source\Runtime\Engine\Classes\Engine\AssetManager.h`
- `Engine\Source\Runtime\Engine\Private\AssetManager.cpp`
- `Engine\Source\Runtime\Engine\Classes\Engine\AssetManagerSettings.h`
- `Engine\Source\Runtime\Engine\Classes\Engine\AssetManagerTypes.h`
- `Engine\Source\Runtime\Engine\Classes\Engine\DataAsset.h`
- `Engine\Source\Runtime\Engine\Classes\Engine\PrimaryAssetLabel.h`

### 27.8 新运行时底座

- `Engine\Source\Runtime\Core\Public\IO\PackageStore.h`
- `Engine\Source\Runtime\Core\Private\IO\PackageStore.cpp`
- `Engine\Source\Runtime\Core\Private\IO\IoDispatcher.cpp`

### 27.9 Cook 与示例

- `Engine\Source\Editor\UnrealEd\Private\Commandlets\AssetRegistryGenerator.cpp`
- `Samples\Games\Lyra\Source\LyraGame\System\LyraAssetManager.h`
- `Samples\Games\Lyra\Source\LyraGame\System\LyraAssetManager.cpp`
- `Samples\Games\Lyra\Config\DefaultGame.ini`

---

## 28. 最后总结：一条真正的 UE 资源加载主线

如果把整篇文档压缩成一条主线，你可以这样记：

### 第一步：先知道“我要找谁”

- `FSoftObjectPath`
- `TSoftObjectPtr`
- `FTopLevelAssetPath`

### 第二步：再知道“系统里有没有这个东西”

- `AssetRegistry`
- `FAssetData`

### 第三步：发起真正的加载请求

- `StaticLoadObject`
- `LoadPackage`
- `LoadPackageAsync`
- `FStreamableManager`

### 第四步：包与对象被构造出来

- `UPackage`
- `FLinkerLoad`
- `AsyncPackageLoader2`
- `PackageStore`
- `IoDispatcher`

### 第五步：项目层做治理

- `UAssetManager`
- Primary Asset
- Asset Bundle
- Chunk / CookRule

---

## 一句话收束

UE 的资源加载不是一个 API，而是一整套分层系统：

> **路径描述层 -> 资源发现层 -> 请求调度层 -> 包加载底层 -> 项目治理层**

而在这套系统里，**`FStreamableManager` 是项目代码最值得深读的中枢类之一**：  
它把软路径、同步/异步桥接、Handle 生命周期、GC 保活与真正的包加载连接了起来，是“从知道资源路径，到安全地把资源用起来”之间最关键的一层。

