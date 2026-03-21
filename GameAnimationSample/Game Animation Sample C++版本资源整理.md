# GitHub 上 Game Animation Sample C++ 重写版本整理

> 搜索时间：2026-03-21  
> 关键词：`GameAnimationSample C++`、`UE5 Game Animation Sample CPP`  
> 原始示例项目：UE 官方提供的 [Game Animation Sample](https://www.unrealengine.com/marketplace/zh-CN/learn/game-animation-sample)（以蓝图为主）

---

## 搜索结论

在 GitHub 上搜索后，找到了以下使用 C++ 重写或扩展 UE Game Animation Sample 的项目。需要注意的是，目前**没有发现**将官方蓝图逻辑完整、逐行翻译为 C++ 的专项仓库，但下列项目在 C++ 语言标记下包含了对 Game Animation Sample 功能的复现或扩展：

---

## 找到的相关仓库

### 1. Dzjox/GameAnimationSample

- **链接**：[https://github.com/Dzjox/GameAnimationSample](https://github.com/Dzjox/GameAnimationSample)
- **主要语言**：C++
- **描述**：在 C++ 工程框架下实现的 Game Animation Sample，Source 目录包含 `CharacterBase.cpp/.h` 以及若干以 `Cat` 为前缀的 Actor/Component/Interface 子模块（`CatActors`、`CatComponents`、`CatInterfaces`）。项目同时保留了原始 Content 资产目录。
- **Fork 数**：1（说明已有人在此基础上继续开发）
- **最近更新**：2026-03-10

### 2. louis-lrs/GameAnimationSample

- **链接**：[https://github.com/louis-lrs/GameAnimationSample](https://github.com/louis-lrs/GameAnimationSample)
- **主要语言**：C++
- **描述**：基于 UE 5.5 的 Game Animation Sample C++ 工程，README 中明确列出了以下功能模块：
  - 角色移动系统（Character Movement）
  - Motion Matching 动画系统
  - Animation Warping
  - Pose Search
  - MetaHuman 集成
- **所用插件**：`AnimationWarping`、`PoseSearch`、`AnimationLocomotionLibrary`、`MotionWarping`、`HairStrands`、`Chooser`、`RigLogic`、`LiveLink`
- **最近更新**：2026-03-17

---

## 其他相关仓库（非纯 C++ 重写，但有参考价值）

| 仓库 | 语言 | 说明 | 链接 |
|------|------|------|------|
| RuslanVafin/LearningAgents_GameAnimationSample | — | 在 Game Animation Sample 中测试 LearningAgents 插件（UE 5.5） | [链接](https://github.com/RuslanVafin/LearningAgents_GameAnimationSample) |
| alex-goldfish/SoG_GameAnimationSample | C# | 包含脚本/工具层扩展 | [链接](https://github.com/alex-goldfish/SoG_GameAnimationSample) |

---

## 使用建议

1. 如果你希望**学习 C++ 与 Animation 系统的结合**，可以重点参考 `louis-lrs/GameAnimationSample`，它的 README 结构清晰，功能覆盖了 Motion Matching 和 Pose Search 核心模块。
2. 如果你希望**在 C++ 架构下扩展角色逻辑**，可参考 `Dzjox/GameAnimationSample`，其 `CatActors`/`CatComponents` 的模块化拆分方式对大型项目有借鉴意义。
3. 这两个项目均依赖 UE 官方的 Content 资产，单独编译 C++ 代码需配合 Epic Games Launcher 下载原始 Game Animation Sample 内容包。

---

## 官方资源

- 官方示例下载：Epic Games Launcher → 学习 → Game Animation Sample
- 官方文档：[Unreal Engine Game Animation Sample](https://dev.epicgames.com/documentation/zh-cn/unreal-engine/game-animation-sample-project-in-unreal-engine)
- 相关技术：[Motion Matching](https://dev.epicgames.com/documentation/zh-cn/unreal-engine/motion-matching-in-unreal-engine)、[Pose Search](https://dev.epicgames.com/documentation/zh-cn/unreal-engine/pose-search-in-unreal-engine)
