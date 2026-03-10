# LimYoung 的 UE 插件收藏整理

> 本文整理自 GitHub 用户 [Lim-Young (LimYoung)](https://github.com/Lim-Young) Star 的 Unreal Engine 相关插件和工具（共 249+ 个），按功能分类进行介绍。

---

## 目录

- [🎮 Gameplay 系统](#-gameplay-系统)
  - [任务系统](#任务系统)
  - [对话系统](#对话系统)
  - [交互系统](#交互系统)
  - [物品与背包系统](#物品与背包系统)
  - [AI 决策系统](#ai-决策系统)
  - [感知与目标系统](#感知与目标系统)
  - [卡牌与回合制](#卡牌与回合制)
  - [其他 Gameplay 系统](#其他-gameplay-系统)
- [🏃 角色移动与动画](#-角色移动与动画)
  - [角色移动](#角色移动)
  - [动画与骨骼](#动画与骨骼)
  - [受击与肢解](#受击与肢解)
- [🧭 导航与寻路](#-导航与寻路)
- [🌐 网络与多人游戏](#-网络与多人游戏)
  - [网络框架与复制](#网络框架与复制)
  - [在线服务](#在线服务)
  - [REST & 数据通信](#rest--数据通信)
- [🖥️ UI 系统](#️-ui-系统)
- [🔧 编辑器工具](#-编辑器工具)
  - [通用编辑器扩展](#通用编辑器扩展)
  - [属性与元数据编辑](#属性与元数据编辑)
  - [资源管理与烘焙](#资源管理与烘焙)
- [🌍 PCG 与程序化生成](#-pcg-与程序化生成)
- [⚡ 物理与模拟](#-物理与模拟)
- [🎨 渲染与视觉效果](#-渲染与视觉效果)
  - [粒子与特效](#粒子与特效)
  - [体积云与光照](#体积云与光照)
  - [材质与贴图](#材质与贴图)
  - [LOD 与 Impostor](#lod与impostor)
- [🚀 性能优化](#-性能优化)
  - [PSO 与着色器优化](#pso-与着色器优化)
  - [帧率与负载管理](#帧率与负载管理)
  - [Tick 与定时器优化](#tick-与定时器优化)
  - [项目精简与分析](#项目精简与分析)
- [💻 编程语言扩展](#-编程语言扩展)
- [🗺️ 地形与环境](#️-地形与环境)
- [🔫 战斗与射击](#-战斗与射击)
- [🎵 音频](#-音频)
- [🏗️ 关卡设计](#️-关卡设计)
- [📦 数据管理与工具库](#-数据管理与工具库)
- [🌏 本地化](#-本地化)
- [🎬 媒体](#-媒体)
- [📐 几何与 Mesh](#-几何与-mesh)
- [🔨 构建与管线工具](#-构建与管线工具)
- [🤖 AI 辅助开发](#-ai-辅助开发)
- [🔗 DCC 桥接工具](#-dcc-桥接工具)
- [📚 文档与学习资源](#-文档与学习资源)
- [🎯 游戏框架与模板](#-游戏框架与模板)
- [🎮 输入系统](#-输入系统)

---

## 🎮 Gameplay 系统

### 任务系统

| 插件 | Stars | 简介 |
|------|-------|------|
| [sinbad/SUQS](https://github.com/sinbad/SUQS) | ⭐166 | **Steve's Unreal Quest System** — 数据驱动的任务系统，支持通过配置数据来定义任务逻辑，适合需要灵活任务管理的项目。 |
| [Lazy-Jason/LazyNerveQuest](https://github.com/Lazy-Jason/LazyNerveQuest) | ⭐26 | **LazyNerveQuest** — 功能全面的任务管理系统，支持可视化图形编辑器、模块化目标设计、动态UI以及完整的 Blueprint/C++ 支持。 |

### 对话系统

| 插件 | Stars | 简介 |
|------|-------|------|
| [Mountea-Framework/MounteaDialogueSystem](https://github.com/Mountea-Framework/MounteaDialogueSystem) | ⭐358 | **Mountea Dialogue System** — 开源的对话系统，提供专属的对话树编辑器和验证系统，不仅适用于复杂对话场景。 |
| [janikowski-dev/Chronicle](https://github.com/janikowski-dev/Chronicle) | ⭐43 | **Chronicle** — 叙事驱动的游戏工具包，包含对话图表、自定义规则、角色管理、任务系统和过场动画时间线。 |

### 交互系统

| 插件 | Stars | 简介 |
|------|-------|------|
| [Mountea-Framework/MounteaInteractionSystem](https://github.com/Mountea-Framework/MounteaInteractionSystem) | ⭐113 | **Mountea Interaction System** — 基于组件的 Actor 智能交互系统，面向设计师友好，支持高可扩展性。 |
| [Lim-Young/GameplayInputSystem](https://github.com/Lim-Young/GameplayInputSystem) | ⭐15 | **GameplayInputSystem** — 基于 GameplayTag 的全局输入监听系统，集成交互模块，支持 QTE 等自定义交互事件的创建和管理。 |

### 物品与背包系统

| 插件 | Stars | 简介 |
|------|-------|------|
| [mattyman174/GenericItemization](https://github.com/mattyman174/GenericItemization) | ⭐170 | **GenericItemization** — 灵活的物品系统插件，利用 DataTable 和 Instanced Structs 提供通用的物品和词缀生成功能。 |
| [bohdon/GameItemsPlugin](https://github.com/bohdon/GameItemsPlugin) | ⭐82 | **GameItemsPlugin** — 提供创建游戏物品、背包和装备系统的类和工具。 |

### AI 决策系统

| 插件 | Stars | 简介 |
|------|-------|------|
| [sinbad/SUSS](https://github.com/sinbad/SUSS) | ⭐68 | **Steve's UtilityAI SubSystem** — UE5 的效用 AI 子系统，支持通过效用评分来驱动 AI 行为决策。 |
| [hollsteinm/ReasonablePlanningAI](https://github.com/hollsteinm/ReasonablePlanningAI) | ⭐163 | **ReasonablePlanningAI** — 设计师驱动的 AI 模块扩展插件，支持数据驱动的效用 AI 和目标导向行动规划（GOAP）。 |

### 感知与目标系统

| 插件 | Stars | 简介 |
|------|-------|------|
| [Sn-a-ke/SenseSystem](https://github.com/Sn-a-ke/SenseSystem) | ⭐19 | **SenseSystem** — 灵活、可扩展的多功能感知系统，用于检测和响应游戏对象。 |
| [Lim-Young/TargetTraceSystem](https://github.com/Lim-Young/TargetTraceSystem) | ⭐9 | **TargetTraceSystem** — 通用目标追踪系统，可集中管理各种目标追踪需求，特别适合敌人 AI 系统。 |
| [Vaei/Vigil](https://github.com/Vaei/Vigil) | ⭐55 | **Vigil** — 游戏聚焦系统，支持数据驱动、异步处理和可选的网络预测。 |

### 卡牌与回合制

| 插件 | Stars | 简介 |
|------|-------|------|
| [Kodingroo/K_Cards-Game-Feature](https://github.com/Kodingroo/K_Cards-Game-Feature) | ⭐4 | **K_Cards** — UE 5.2 卡牌游戏框架，使用 C++ 和蓝图实现，包含回合制逻辑、AI 行为、玩家状态管理和 GAS 集成。 |

### 其他 Gameplay 系统

| 插件 | Stars | 简介 |
|------|-------|------|
| [gerlogu/CustomInGameConsole](https://github.com/gerlogu/CustomInGameConsole) | ⭐39 | **CustomInGameConsole** — 可在 Release 版本中轻松创建自定义控制台命令的工具。 |
| [gamedevware/charon](https://github.com/gamedevware/charon) | ⭐82 | **Charon** — 强大的游戏数据管理工具，提供结构化的游戏数据设计和自动源码生成，支持 UE 集成。 |
| [GGgRain/Unreal-Joint](https://github.com/GGgRain/Unreal-Joint) | ⭐84 | **Unreal-Joint** — 通用模块化播放框架（Epic MegaGrants 获奖项目），适用于多种回放/过场场景。 |

---

## 🏃 角色移动与动画

### 角色移动

| 插件 | Stars | 简介 |
|------|-------|------|
| [ProjectBorealis/PBCharacterMovement](https://github.com/ProjectBorealis/PBCharacterMovement) | ⭐1443 | **PBCharacterMovement** — 半条命2风格的经典 FPS 移动组件，用 C++ 实现。 |
| [ShadowfallStudios/ALS-Community](https://github.com/ShadowfallStudios/ALS-Community) | ⭐— | **ALS-Community** — 社区维护的高级运动系统 V4，针对 UE 5.4 进行了复制优化，附带额外功能和 Bug 修复。 |
| [Vaei/PredictedMovement](https://github.com/Vaei/PredictedMovement) | ⭐220 | **PredictedMovement** — 扩展角色移动组件（CMC），增加对预测能力的支持。 |
| [Reddy-dev/SMN2](https://github.com/Reddy-dev/SMN2) | ⭐113 | **SMN2** — 在 UE5 蓝图中轻松实现角色移动网络预测。 |
| [lootera89/LedgeDetection](https://github.com/lootera89/LedgeDetection) | ⭐17 | **LedgeDetection** — 简单、高效的边缘检测系统，自动检测前方边缘并阻止玩家坠落，支持 UE 5.3+。 |

### 动画与骨骼

| 插件 | Stars | 简介 |
|------|-------|------|
| [Vaei/TurnInPlace](https://github.com/Vaei/TurnInPlace) | ⭐236 | **TurnInPlace** — 基于 Actor 的原地转身解决方案，替代基于 Mesh 的方案，避免了大量已知问题。 |

### 受击与肢解

| 插件 | Stars | 简介 |
|------|-------|------|
| [Vaei/ProcHitReact](https://github.com/Vaei/ProcHitReact) | ⭐135 | **ProcHitReact** — 程序化物理驱动的骨骼网格体受击反应系统，功能强大，满足各种受击效果需求。 |
| [Thyke/Dismemberment](https://github.com/Thyke/Dismemberment) | ⭐11 | **Dismemberment** — 高级肢解系统，支持动态肢体分离、武器掉落和可自定义的身体部位数据。 |

---

## 🧭 导航与寻路

| 插件 | Stars | 简介 |
|------|-------|------|
| [midgen/uesvon](https://github.com/midgen/uesvon) | ⭐255 | **UE-SVon** — 3D 导航网格生成和寻路插件，支持三维空间的导航。 |
| [darbycostello/Nav3D](https://github.com/darbycostello/Nav3D) | ⭐214 | **Nav3D** — 基于稀疏体素八叉树的 3D 寻路系统，针对 UE5 全面优化，支持战术推理。 |
| [midgen/AeonixNavigation](https://github.com/midgen/AeonixNavigation) | ⭐36 | **AeonixNavigation** — 3D SVO（稀疏体素八叉树）导航、寻路和移动插件。 |
| [cem-akkaya/MGDynamicNavigation](https://github.com/cem-akkaya/MGDynamicNavigation) | ⭐13 | **MGDynamicNavigation** — 轻量级的独立本地动态导航系统。 |

---

## 🌐 网络与多人游戏

### 网络框架与复制

| 插件 | Stars | 简介 |
|------|-------|------|
| [sha3sha3/UE-DSSPlugin](https://github.com/sha3sha3/UE-DSSPlugin) | ⭐95 | **UE-DSSPlugin** — 可扩展的 MMO 服务器实例化、副本实例化和玩家负载均衡子系统。 |
| [SJOEC/LobbyManager](https://github.com/SJOEC/LobbyManager) | ⭐19 | **LobbyManager** — 事件驱动的多人游戏会话子系统，支持 C++ 和蓝图。 |
| [YyepPo/UE5-NetDormancy-Demo](https://github.com/YyepPo/UE5-NetDormancy-Demo) | ⭐6 | **UE5-NetDormancy-Demo** — 展示网络休眠（Net Dormancy）优化网络复制的示例项目。 |
| [cem-akkaya/NetworkedPhysics](https://github.com/cem-akkaya/NetworkedPhysics) | ⭐14 | **NetworkedPhysics** — UE 5.7 网络化物理 Pawn 教程项目。 |
| [Cutter-H/ReplicatedDrawingWidgetPlugin](https://github.com/Cutter-H/ReplicatedDrawingWidgetPlugin) | ⭐9 | **ReplicatedDrawingWidgetPlugin** — 多人游戏中可复制的绘图控件插件，支持拖拽绘制。 |
| [Vaei/PushPawn](https://github.com/Vaei/PushPawn) | ⭐83 | **PushPawn** — 网络预测的有机软碰撞系统，在角色靠近其他 Pawn 时提供良好的游戏手感且不会造成同步问题。 |

### 在线服务

| 插件 | Stars | 简介 |
|------|-------|------|
| [betidestudio/EOSIntegrationKit](https://github.com/betidestudio/EOSIntegrationKit) | ⭐406 | **EIK (Epic Online Services Integration Kit)** — 实现 Epic 在线服务（EOS）与 UE 的无缝集成，提供 EOS SDK 的完整访问。 |

### REST & 数据通信

| 插件 | Stars | 简介 |
|------|-------|------|
| [ufna/VaRest](https://github.com/ufna/VaRest) | ⭐— | **VaRest** — UE 的 REST API 插件，支持 RESTful 后端和 JSON 通信。 |
| [rdelian/DeJson](https://github.com/rdelian/DeJson) | ⭐19 | **DeJson** — 在蓝图中方便地处理 JSON 数据的插件。 |

---

## 🖥️ UI 系统

| 插件 | Stars | 简介 |
|------|-------|------|
| [Adriwin06/Ultimate-CommonUI-Menu-System](https://github.com/Adriwin06/Ultimate-CommonUI-Menu-System) | ⭐311 | **Ultimate CommonUI Menu System** — 使用 CommonUI 构建的终极模块化菜单系统，提供完整的选项菜单（画质、音频、输入等）。 |
| [truong-bui/AsyncLoadingScreen](https://github.com/truong-bui/AsyncLoadingScreen) | ⭐— | **AsyncLoadingScreen** — 免费开源的异步加载画面插件，可在项目设置中轻松配置加载界面。 |
| [Luna-Blueshift/BlueshiftGameSettings](https://github.com/Luna-Blueshift/BlueshiftGameSettings) | ⭐37 | **BlueshiftGameSettings** — 扩展用户设置功能，支持蓝图中的游戏用户设置和 UI 绑定框架。 |
| [DoubleDeez/MDViewModel](https://github.com/DoubleDeez/MDViewModel) | ⭐55 | **MDViewModel** — UE5 的 MVVM 插件，支持在 UMG Widget、Actor 和 Object 蓝图中自动数据绑定。 |
| [hollsteinm/UE4-Map-And-Mini-Map](https://github.com/hollsteinm/UE4-Map-And-Mini-Map) | ⭐23 | **UE4-Map-And-Mini-Map** — 支持基于体积的小地图渲染和可缩放平移的地图实现，无需烘焙资产。 |
| [oivio/LyraUI](https://github.com/oivio/LyraUI) | ⭐14 | **LyraUI** — 将 Lyra 的 UI 主菜单抽取为独立的可复用插件。 |
| [Pavreally/WindowManager](https://github.com/Pavreally/WindowManager) | ⭐12 | **WindowManager** — UE5 函数库，用于快速 UI 原型制作和简化界面管理。 |

---

## 🔧 编辑器工具

### 通用编辑器扩展

| 插件 | Stars | 简介 |
|------|-------|------|
| [aquanox/SubsystemBrowserPlugin](https://github.com/aquanox/SubsystemBrowserPlugin) | ⭐431 | **SubsystemBrowserPlugin** — 在编辑器中添加子系统浏览器面板，浏览运行中的子系统并编辑属性。 |
| [ElgSoft/ElgEditorScripting](https://github.com/ElgSoft/ElgEditorScripting) | ⭐235 | **ElgEditorScripting** — UE5 编辑器脚本扩展插件，极大扩展了编辑器工具控件（Editor Utility Widget）的功能。 |
| [jorgenpt/Hermes](https://github.com/jorgenpt/Hermes) | ⭐205 | **Hermes** — 支持直接通过深链接（Deep Link）跳转到 UE 编辑器中指定内容的插件。 |
| [tsubasamusu/TsubasamusuUnrealAssist](https://github.com/tsubasamusu/TsubasamusuUnrealAssist) | ⭐26 | **TsubasamusuUnrealAssist** — 编辑器扩展插件，添加了各种实用的编辑器功能。 |
| [arnaud-jamin/Cog](https://github.com/arnaud-jamin/Cog) | ⭐551 | **Cog** — 基于 Dear ImGui 构建的 UE 调试工具集，提供丰富的运行时调试能力。 |
| [hzFishy/EditorLevelSelector](https://github.com/hzFishy/EditorLevelSelector) | ⭐3 | **EditorLevelSelector** — 简单的编辑器关卡选择器插件。 |
| [gerlogu/CustomInGameConsole](https://github.com/gerlogu/CustomInGameConsole) | ⭐39 | **CustomInGameConsole** — 允许在 Release 版本中创建和使用自定义控制台命令。 |

### 属性与元数据编辑

| 插件 | Stars | 简介 |
|------|-------|------|
| [DoubleDeez/MDMetaDataEditor](https://github.com/DoubleDeez/MDMetaDataEditor) | ⭐179 | **MDMetaDataEditor** — 在 UE 5.1+ 中编辑蓝图属性、函数和参数的元数据。 |
| [PsinaDev/ue-instanced-object-details](https://github.com/PsinaDev/ue-instanced-object-details) | ⭐4 | **ue-instanced-object-details** — 修复 Instanced (EditInlineNew) UObject 的属性显示问题。 |
| [Lokillouu/PropertyEditorSetter_UE](https://github.com/Lokillouu/PropertyEditorSetter_UE) | ⭐7 | **PropertyEditorSetter_UE** — 拦截属性编辑事件，确保具有 Setter 说明符的 UPROPERTY 正确调用 Setter 函数。 |
| [pramberg/NeatFunctions](https://github.com/pramberg/NeatFunctions) | ⭐11 | **NeatFunctions** — 为 UFUNCTION 添加新的元数据标签，改善蓝图中的使用体验。 |

### 资源管理与烘焙

| 插件 | Stars | 简介 |
|------|-------|------|
| [magnv/EasyCookEditor](https://github.com/magnv/EasyCookEditor) | ⭐11 | **EasyCookEditor** — 简化 UE4 资产烘焙的编辑器插件，主要用于 Modding。 |
| [vicvasper/SmartFoldersPlugin_UE5.6](https://github.com/vicvasper/SmartFoldersPlugin_UE5.6) | ⭐4 | **SmartFoldersPlugin** — 简化 UE 资产组织的工具，消除内容浏览器的杂乱。 |

---

## 🌍 PCG 与程序化生成

| 插件 | Stars | 简介 |
|------|-------|------|
| [PCGEx/PCGExtendedToolkit](https://github.com/PCGEx/PCGExtendedToolkit) | ⭐591 | **PCGExtendedToolkit** — UE PCG 系统的缺失拼图，提供图论、高级寻路、空间操作、过滤和资产管理等功能。 |
| [Esri/cityengine_for_unreal](https://github.com/Esri/cityengine_for_unreal) | ⭐258 | **CityEngine for Unreal** — 将 ArcGIS CityEngine 的强大程序化建模能力引入 UE。 |
| [AdrianPanGithub/HoudiniEngineForUnreal](https://github.com/AdrianPanGithub/HoudiniEngineForUnreal) | ⭐122 | **HoudiniEngineForUnreal** — 从零重建的 Houdini Engine，I/O 速度提升 2-15 倍，专为程序化景观和城市工具集优化。 |
| [Jay2645/Unreal-Polygonal-Map-Gen](https://github.com/Jay2645/Unreal-Polygonal-Map-Gen) | ⭐381 | **Unreal-Polygonal-Map-Gen** — 基于多边形地图生成算法的 UE4 岛屿生成实现。 |
| [cem-akkaya/AutoMapDemo](https://github.com/cem-akkaya/AutoMapDemo) | ⭐9 | **AutoMapDemo** — 程序化地图生成的技术概念验证项目。 |

---

## ⚡ 物理与模拟

| 插件 | Stars | 简介 |
|------|-------|------|
| [fgrenoville/Async-Physics-Suspension](https://github.com/fgrenoville/Async-Physics-Suspension) | ⭐18 | **Async-Physics-Suspension** — 展示 UE5 中通过自定义 FSimCallback 实现异步 Chaos 物理的示例项目，含四弹簧悬挂车辆。 |
| [Dragomirson/PhysXInstancedSubsystem](https://github.com/Dragomirson/PhysXInstancedSubsystem) | ⭐10 | **PhysXInstancedSubsystem** — 高性能 PhysX 实例化插件，专为大规模物理体场景设计，支持数千个实例的高效模拟。 |
| [cem-akkaya/ThermoForgeDemo](https://github.com/cem-akkaya/ThermoForgeDemo) | ⭐7 | **ThermoForgeDemo** — 模拟和查询真实热力、气候和热条件的 UE 插件示例项目。 |

---

## 🎨 渲染与视觉效果

### 粒子与特效

| 插件 | Stars | 简介 |
|------|-------|------|
| [eanticev/niagara-destruction-driver](https://github.com/eanticev/niagara-destruction-driver) | ⭐200 | **niagara-destruction-driver** — 将 CHAOS 可破坏物转换为高性能 GPU 模拟的可破坏静态网格体，由 Niagara 粒子驱动。 |
| [DevDingDangDong/MaaassParticle](https://github.com/DevDingDangDong/MaaassParticle) | ⭐33 | **MaaassParticle** — 通过 Niagara 渲染大规模人群，并可通过状态管理控制群体行为。 |

### 体积云与光照

| 插件 | Stars | 简介 |
|------|-------|------|
| [mxcop/vapor](https://github.com/mxcop/vapor) | ⭐13 | **Vapor** — 基于 Nubis 的 UE5 物理体积云渲染插件。 |
| [Thyke/RevealObject](https://github.com/Thyke/RevealObject) | ⭐5 | **RevealObject** — 利用材质参数集（MPC）的手电筒/揭示系统，高效驱动全局着色器效果。 |

### 材质与贴图

| 插件 | Stars | 简介 |
|------|-------|------|
| [vicvasper/Paint-System-5.4](https://github.com/vicvasper/Paint-System-5.4) | ⭐5 | **Paint-System** — UE 5.4 的 C++ 涂绘系统，支持在静态和骨骼网格体上动态涂绘材质，含 RGB 和 Alpha 通道控制。 |

### LOD与Impostor

| 插件 | Stars | 简介 |
|------|-------|------|
| [Erlandys/ImpostorBaker](https://github.com/Erlandys/ImpostorBaker) | ⭐35 | **ImpostorBaker** — 为静态网格体生成 Impostor（替身贴片）的 UE5 插件，用于远距离 LOD 优化。 |

---

## 🚀 性能优化

### PSO 与着色器优化

| 插件 | Stars | 简介 |
|------|-------|------|
| [Flassari/PSOCacheBuster](https://github.com/Flassari/PSOCacheBuster) | ⭐116 | **PSOCacheBuster** — 为非发行版和非编辑器构建清除 PSO 驱动缓存，便于测试和分析首次运行体验。 |
| [gregorik/PSO-Autopilot](https://github.com/gregorik/PSO-Autopilot) | ⭐8 | **PSO-Autopilot** — 内存安全的异步着色器编译和 PSO 预热系统。 |

### 帧率与负载管理

| 插件 | Stars | 简介 |
|------|-------|------|
| [eanticev/ue-gameplay-work-balancer](https://github.com/eanticev/ue-gameplay-work-balancer) | ⭐121 | **ue-gameplay-work-balancer** — 帮助将工作负载分片（Time Slice）到多帧执行，保持稳定帧率。 |
| [HaukeThiessen/EnergySavingPlugin](https://github.com/HaukeThiessen/EnergySavingPlugin) | ⭐37 | **EnergySavingPlugin** — 在无输入或窗口失焦时降低最大帧率和画面分辨率以节省能耗，参考了《堡垒之夜》的实践。 |

### Tick 与定时器优化

| 插件 | Stars | 简介 |
|------|-------|------|
| [Thyke/EnhancedTick](https://github.com/Thyke/EnhancedTick) | ⭐32 | **EnhancedTick** — 优化的 Tick 管理方案，支持缓存友好的批量 Tick 操作，大幅提升多 Actor 场景性能。 |
| [Thyke/EnhancedTimerManager](https://github.com/Thyke/EnhancedTimerManager) | ⭐10 | **EnhancedTimerManager** — 高性能的定时器管理方案，支持时间膨胀感知、游戏暂停处理等扩展功能。 |

### 项目精简与分析

| 插件 | Stars | 简介 |
|------|-------|------|
| [daftsoftware/StarterProject](https://github.com/daftsoftware/StarterProject) | ⭐248 | **StarterProject** — 轻量级 UE 起始项目，去除臃肿内容，减少打包体积并提升编辑器和游戏性能。 |
| [vicvasper/Optilogger_UE5.x](https://github.com/vicvasper/Optilogger_UE5.x) | ⭐6 | **Optilogger** — 深度分析关卡资源（网格体、贴图、材质、动画、音频、光照、后处理）的优化分析插件。 |
| [ArcstoneGames/DependencyAnalyser](https://github.com/ArcstoneGames/DependencyAnalyser) | ⭐3 | **DependencyAnalyser** — 测量和分析资产依赖大小瓶颈的 UE5 插件。 |
| [Imloopdev/Perfectly-Optimized](https://github.com/Imloopdev/Perfectly-Optimized) | ⭐— | **Perfectly-Optimized** — 专为低端硬件优化的 UE5 精简启动模板。 |

---

## 💻 编程语言扩展

| 插件 | Stars | 简介 |
|------|-------|------|
| [UnrealSharp/UnrealSharp](https://github.com/UnrealSharp/UnrealSharp) | ⭐— | **UnrealSharp** — 支持使用 C#（.NET 9）开发 UE5 游戏的插件，提供热重载功能。 |
| [jmgomez/NimForUE](https://github.com/jmgomez/NimForUE) | ⭐537 | **NimForUE** — UE5 的 Nim 语言插件，提供原生性能和热重载，介于 C++ 和蓝图之间的完整互操作。 |
| [Sebight/seraph](https://github.com/Sebight/seraph) | ⭐11 | **Seraph** — AngelScript 的 C++ 封装库，简化类型注册、函数绑定和脚本与原生代码的互操作。 |
| [ySion/DaxSystem](https://github.com/ySion/DaxSystem) | ⭐5 | **DaxSystem** — UE5 高性能复合数据系统，完整支持 AngelScript。 |

---

## 🗺️ 地形与环境

| 插件 | Stars | 简介 |
|------|-------|------|
| [gregorik/TerraDyne](https://github.com/gregorik/TerraDyne) | ⭐35 | **TerraDyne** — 实验性高性能原生 C++ 地形插件，适用于 UE 5.7。 |
| [Casmo/UnrealZoneMarker](https://github.com/Casmo/UnrealZoneMarker) | ⭐4 | **UnrealZoneMarker** — 支持绘制区域并使用样条线和动态网格组件填充区域的插件。 |
| [BakonGuy/UPolyZones](https://github.com/BakonGuy/UPolyZones) | ⭐12 | **UPolyZones** — 多边形区域绘制插件，可测试点是否位于多边形内部。 |

---

## 🔫 战斗与射击

| 插件 | Stars | 简介 |
|------|-------|------|
| [YyepPo/Data_Driven_Projectile](https://github.com/YyepPo/Data_Driven_Projectile) | ⭐25 | **Data_Driven_Projectile** — 轻量级高性能弹道系统，使用数据驱动方式和中央子弹管理器，支持大量子弹且 CPU 开销极低。 |
| [prathameshkhanzode/Advanced-Spray-System-UE5-Cpp](https://github.com/prathameshkhanzode/Advanced-Spray-System-UE5-Cpp) | ⭐8 | **Advanced-Spray-System** — UE5 喷涂系统组件，支持实时表面检测、法线对齐和多人游戏同步。 |

---

## 🎵 音频

| 插件 | Stars | 简介 |
|------|-------|------|
| [Lokillouu/MetaSounds-Notify](https://github.com/Lokillouu/MetaSounds-Notify) | ⭐12 | **MetaSounds-Notify** — 让 MetaSound 系统与外部世界通信！可从 MetaSound 内部向对象、Actor 和蓝图发送通知。 |
| [LeoO5/O5SpectrumWwisePlugin](https://github.com/LeoO5/O5SpectrumWwisePlugin) | ⭐4 | **O5SpectrumWwisePlugin** — 轻量级 Wwise 插件，嵌入实时频谱分析器，并通过公共 C++ API 将 FFT/幅度数据流式传输到游戏运行时。 |

---

## 🏗️ 关卡设计

| 插件 | Stars | 简介 |
|------|-------|------|
| [HorizonGamesRoland/ActorIO](https://github.com/HorizonGamesRoland/ActorIO) | ⭐290 | **ActorIO** — 受 Source 引擎启发的关卡脚本工具，为 Actor 提供输入/输出系统，使关卡逻辑可视化编排。 |
| [unknownworlds/prefabricator-ue5](https://github.com/unknownworlds/prefabricator-ue5) | ⭐16 | **Prefabricator-UE5** — Prefab（预制体）系统的 UE5 维护版本，支持关卡中的模块化组件复用。 |

---

## 📦 数据管理与工具库

| 插件 | Stars | 简介 |
|------|-------|------|
| [zompi2/UE4EnhancedCodeFlow](https://github.com/zompi2/UE4EnhancedCodeFlow) | ⭐133 | **UE4EnhancedCodeFlow** — 大幅提升 C++ 游戏流程实现质量的代码插件，提供延迟执行、定时器等便捷函数。 |
| [pyoneerC/Array-Utils](https://github.com/pyoneerC/Array-Utils) | ⭐20 | **Array-Utils** — 面向 UE 核心数据结构的高性能 STL 风格工具集，用 C++ 编写。 |
| [Sixze/FabulousUtility](https://github.com/Sixze/FabulousUtility) | ⭐96 | **FabulousUtility** — 各类实用函数、节点和工具的集合，适用于任何类型的 UE 项目。 |
| [jign/SkyLifeguard](https://github.com/jign/SkyLifeguard) | ⭐8 | **SkyLifeguard** — 受契约式设计（DbC）启发的 UE5 断言式编程框架。 |
| [Islamantin/BlueprintRetarget](https://github.com/Islamantin/BlueprintRetarget) | ⭐5 | **BlueprintRetarget** — 当父类丢失时重新定向无效蓝图的小工具。 |
| [cem-akkaya/GameColors](https://github.com/cem-akkaya/GameColors) | ⭐6 | **GameColors** — 高效组织和管理游戏玩法中颜色的插件。 |

---

## 🌏 本地化

| 插件 | Stars | 简介 |
|------|-------|------|
| [zompi2/UE4EasyLocalizationTool](https://github.com/zompi2/UE4EasyLocalizationTool) | ⭐120 | **UE4EasyLocalizationTool** — 通过导入 CSV 文件来简化 UE 本地化流程的插件。 |

---

## 🎬 媒体

| 插件 | Stars | 简介 |
|------|-------|------|
| [surgeshi/FFMPEGMediaUE5](https://github.com/surgeshi/FFMPEGMediaUE5) | ⭐26 | **FFMPEGMediaUE5** — 基于 FFMPEG 的 UE5 媒体插件，支持更多平台（Linux/Android/Mac/Windows）和更多视频格式（含 Alpha 视频）。 |
| [jwatsn/ThumbnailPlugin](https://github.com/jwatsn/ThumbnailPlugin) | ⭐5 | **ThumbnailPlugin** — 在运行时为模型生成缩略图的简单插件。 |
| [earthstorm2015/ueblueprint](https://github.com/earthstorm2015/ueblueprint) | ⭐6 | **ueblueprint** — UE 蓝图可视化 Web 库，可在网页中展示蓝图节点图。 |

---

## 📐 几何与 Mesh

| 插件 | Stars | 简介 |
|------|-------|------|
| [alicepm800/Exploding-Geometry-Script-Plugin](https://github.com/alicepm800/Exploding-Geometry-Script-Plugin) | ⭐6 | **Exploding-Geometry-Script-Plugin** — 通过修改距离变量在编辑器中创建顶点爆炸效果。 |
| [LingFengSBH/SplineSweepMesh](https://github.com/LingFengSBH/SplineSweepMesh) | ⭐8 | **SplineSweepMesh** — 样条线扫描网格体生成插件。 |

---

## 🔨 构建与管线工具

| 插件 | Stars | 简介 |
|------|-------|------|
| [RedpointGames/uet](https://github.com/RedpointGames/uet) | ⭐133 | **UET (Unreal Engine Tool)** — 让构建和测试 UE 项目及插件变得简单，支持 BuildGraph 分发、远程自动化测试等。 |
| [Naotsun19B/PluginBuilder](https://github.com/Naotsun19B/PluginBuilder) | ⭐35 | **PluginBuilder** — 支持使用多引擎版本构建插件并压缩为 ZIP 格式的编辑器插件，方便商城提交。 |
| [jobrogi/-UE-Plugin-Multi-Version-Builder](https://github.com/jobrogi/-UE-Plugin-Multi-Version-Builder) | ⭐2 | **UE-Plugin-Multi-Version-Builder** — 自定义 PowerShell 脚本，用于构建多个版本的 UE 插件，适合 Fab 商城分发。 |
| [pafuhana1213/UnrealEngine-UpdateTracker](https://github.com/pafuhana1213/UnrealEngine-UpdateTracker) | ⭐44 | **UnrealEngine-UpdateTracker** — 自动监控 UE 私有 GitHub 仓库更新，使用 AI 总结重要变更的服务。 |

---

## 🤖 AI 辅助开发

| 插件 | Stars | 简介 |
|------|-------|------|
| [ChiR24/Unreal_mcp](https://github.com/ChiR24/Unreal_mcp) | ⭐359 | **Unreal_mcp** — 综合性 MCP 服务器，通过原生 C++ 自动化桥接插件让 AI 助手控制 UE 编辑器。 |
| [Natfii/UnrealClaude](https://github.com/Natfii/UnrealClaude) | ⭐253 | **UnrealClaude** — Claude Code CLI 与 UE 5.7 的集成，在编辑器中直接获取内置 UE5.7 文档上下文的 AI 编码辅助。 |
| [stefan1000/p4mcp-server](https://github.com/stefan1000/p4mcp-server) | ⭐5 | **p4mcp-server** — 社区支持的 Perforce P4 版本控制 MCP 服务器。 |

---

## 🔗 DCC 桥接工具

| 插件 | Stars | 简介 |
|------|-------|------|
| [nitecon/AssetsBridge](https://github.com/nitecon/AssetsBridge) | ⭐6 | **AssetsBridge** — 提供 UE 和 Blender 之间的资产桥接能力。 |
| [Jim-Kroovy/Mr-Mannequins-Tools](https://github.com/Jim-Kroovy/Mr-Mannequins-Tools) | ⭐541 | **Mr-Mannequins-Tools** — Blender 工具集，支持 UE 角色的骨骼绑定、Blender 导入和 Unreal 导出。 |
| [skarndev/umodel_tools](https://github.com/skarndev/umodel_tools) | ⭐135 | **umodel_tools** — Blender 插件，支持导入 UE 游戏的静态网格体和关卡地图。 |
| [mayloglog/FakeBones](https://github.com/mayloglog/FakeBones) | ⭐25 | **FakeBones** — 增强从 Maya、Unreal 等软件导入骨架可视化的 Blender 插件。 |

---

## 📚 文档与学习资源

| 插件 | Stars | 简介 |
|------|-------|------|
| [tranek/GASDocumentation](https://github.com/tranek/GASDocumentation) | ⭐— | **GASDocumentation** — 对 UE5 GameplayAbilitySystem 插件的深入理解，附带简单的多人游戏示例项目。 |
| [fjz13/UnrealSpecifiers](https://github.com/fjz13/UnrealSpecifiers) | ⭐985 | **UnrealSpecifiers** — UE5 标识符详解，包含 100+ 个标识符以及 300+ 个 Meta 的解释和示例。 |
| [khaled71612000/UnrealThreads](https://github.com/khaled71612000/UnrealThreads) | ⭐12 | **UnrealThreads** — 展示 UE 中各种多线程技术的实现项目。 |
| [lucoiso/UEProject_Elementus](https://github.com/lucoiso/UEProject_Elementus) | ⭐197 | **UEProject_Elementus** — 使用 UE5 新特性（GAS等）的项目模板。 |
| [mikeroyal/Game-Console-Dev-Guide](https://github.com/mikeroyal/Game-Console-Dev-Guide) | ⭐334 | **Game-Console-Dev-Guide** — 游戏主机开发指南，涵盖 Xbox、PS5、Switch、Steam Deck 的 UE5/Unity/Godot 开发。 |

---

## 🎯 游戏框架与模板

| 插件 | Stars | 简介 |
|------|-------|------|
| [hfjooste/UltimateStarterKit](https://github.com/hfjooste/UltimateStarterKit) | ⭐353 | **UltimateStarterKit** — 加速 UE 游戏开发的终极起始工具包插件。 |
| [ydrive/EasySynth](https://github.com/ydrive/EasySynth) | ⭐227 | **EasySynth** — 轻松创建合成图像数据集的 UE 插件，适用于计算机视觉和机器学习。 |

---

## 🎮 输入系统

| 插件 | Stars | 简介 |
|------|-------|------|
| [rafaelvaloto/Gamepad-CoOp](https://github.com/rafaelvaloto/Gamepad-CoOp) | ⭐8 | **Gamepad-CoOp** — 手柄管理插件，简化本地多人游戏体验的创建，提供引擎输入系统的抽象层。 |

---

> 📌 **注：** 本文整理的是用户 [Lim-Young](https://github.com/Lim-Young) 在 GitHub 上 Star 的 UE 相关仓库。Stars 数量截至 2026 年 3 月，部分仓库 Stars 数量标记为 "—" 表示该仓库 Star 数未获取到或为大型仓库（超过 1000 Stars）。该用户总共 Star 了 532 个仓库，其中 249 个被归类到 "Unreal Plugin" 列表中，由于 GitHub 页面渲染限制，本文可能未覆盖全部仓库，但已尽力包含绝大多数 UE 插件。
