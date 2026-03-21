# GitHub 上 UE Game Animation Sample C++ 重写项目资源

> 本文整理了 GitHub 上与 Unreal Engine Game Animation Sample（GASP）示例项目相关的 C++ 重写版本及衍生项目。

---

## 背景介绍

**Game Animation Sample**（简称 GASP）是 Epic Games 官方发布的 UE5 动画示例项目，展示了以下核心动画技术：

- **Motion Matching**（动作匹配）：基于 Pose Search 插件实现的高质量运动匹配系统
- **Animation Warping**（动画扭曲）：运动扭曲与方向调整
- **Chooser**（选择器）：动态动画选择框架
- **MetaHuman 角色**：Echo 和 Twinblast 角色

官方项目主要使用 **蓝图（Blueprint）** 编写逻辑，社区中有开发者将其重写为 C++ 版本。

官方下载地址：https://www.unrealengine.com/marketplace/en-US/product/game-animation-sample

---

## C++ 重写项目

### ⭐ CoffeeVampir3/Unreal-3rd-Person-Parkour

**这是目前 GitHub 上最直接的 Game Animation Sample 完整 C++ 重写版本。**

| 属性 | 内容 |
|------|------|
| 仓库链接 | https://github.com/CoffeeVampir3/Unreal-3rd-Person-Parkour |
| 主要语言 | C++ |
| 描述 | 将 UE 官方 Game Animation Sample 示例项目完整重写为 C++ |

**README 原文：**

> This project is a full rewrite of the game animation sample project from unreal into C++.
> 
> The assets contained in this project are under "UE-Only Content - Licensed for Use Only with Unreal Engine-based Products" and sourced from the free pack from epic here: https://www.unrealengine.com/marketplace/en-US/product/game-animation-sample

**特点：**
- 完整的 C++ 重写（Full C++ Rewrite）
- 保留了 Epic 官方资源（动画、角色等）
- 项目中包含 Parkour（跑酷）相关功能扩展

---

## 相关衍生项目（含 C++ 组件）

### PolygonHive/GASPALS（⭐ 1000+）

| 属性 | 内容 |
|------|------|
| 仓库链接 | https://github.com/PolygonHive/GASPALS |
| Star 数 | 1000+ |
| 描述 | Game Animation Sample with ALS Layering，在 GASP 基础上集成了 ALS（Advanced Locomotion System）覆盖层系统 |

**特点：**
- 将 ALS 的覆盖层（Overlay Layering）系统集成进 GASP
- 支持网络同步（Replication）
- 包含武器附加系统、覆盖层切换 Widget
- 有配套 YouTube 教程（[Polygon Hive 频道](https://www.youtube.com/watch?v=RDWNfIqvWBk&list=PLs9e0eJQMI2aaulgKJzC8feN1UEwDkEnq)）

---

### SAM-tak/GASP-ALS-R

| 属性 | 内容 |
|------|------|
| 仓库链接 | https://github.com/SAM-tak/GASP-ALS-R |
| 主要语言 | C++ |
| 描述 | Game Animation Sample with ALS Refactored Layering and GAS（Gameplay Ability System） |

**特点：**
- 使用 C++ 实现
- 集成 ALS-Refactored 覆盖层系统
- 集成 GAS（Gameplay Ability System）
- 模块化动画图（Modular Anim Graphs）和链接层（Linked Layers）

---

### louis-lrs/GameAnimationSample

| 属性 | 内容 |
|------|------|
| 仓库链接 | https://github.com/louis-lrs/GameAnimationSample |
| 主要语言 | C++ |
| UE 版本 | 5.5 |
| 描述 | Unreal Engine 5.5 Game Animation Sample Project（C++ 版本） |

**特点：**
- 基于 UE 5.5 的 C++ 项目结构
- 包含 Motion Matching、Pose Search、Animation Warping 等功能
- 支持 MetaHuman 集成

---

### The-Recon-Project/GASP_DynamicAdditiveOverlay

| 属性 | 内容 |
|------|------|
| 仓库链接 | https://github.com/The-Recon-Project/GASP_DynamicAdditiveOverlay |
| Fork 数 | 35+ |
| 描述 | 在 GASP 中引入动态叠加覆盖（Dynamic Additive Overlay）技术 |

---

## 总结

| 项目 | 是否完整 C++ 重写 | Stars | 推荐度 |
|------|------------------|-------|--------|
| [CoffeeVampir3/Unreal-3rd-Person-Parkour](https://github.com/CoffeeVampir3/Unreal-3rd-Person-Parkour) | ✅ 是（Full C++ Rewrite） | - | ⭐⭐⭐⭐⭐ |
| [SAM-tak/GASP-ALS-R](https://github.com/SAM-tak/GASP-ALS-R) | 部分 C++ | - | ⭐⭐⭐⭐ |
| [louis-lrs/GameAnimationSample](https://github.com/louis-lrs/GameAnimationSample) | C++ 项目结构 | - | ⭐⭐⭐ |
| [PolygonHive/GASPALS](https://github.com/PolygonHive/GASPALS) | 主要蓝图（含扩展） | 1000+ | ⭐⭐⭐⭐（ALS 集成参考） |

**如果你的目标是将 GASP 的蓝图逻辑完整转换为 C++，推荐首先参考：**

👉 **https://github.com/CoffeeVampir3/Unreal-3rd-Person-Parkour**

---

*搜索日期：2026-03-21*
