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

官方下载地址：https://www.fab.com/listings/880e319a-a59e-4ed2-b268-b32dac7fa016

---

## C++ 重写 / 重构项目

### ⭐ Anaylan/GASP-Refactored（推荐指数最高）

**这是目前 GitHub 上维护最积极、功能最完善的 GASP C++ 重构版本，以 UE 插件形式提供。**

| 属性 | 内容 |
|------|------|
| 仓库链接 | https://github.com/Anaylan/GASP-Refactored |
| 形式 | **UE 插件（Plugin）** |
| 主要语言 | **C++**（4 个 C++ 模块：GASP、GASPCamera、GASPExtras、GASPEditor） |
| 支持 UE 版本 | 5.5 / 5.6 / 5.7（持续更新） |
| 许可证 | MIT License（可自由使用和修改） |
| 当前版本 | 1.12 |

**README / uplugin 原文：**

> "Completely reworked and improved С++ version of Game Animation Sample."
>
> Description: "Reworked and improved С++ version of Game Animation Sample"

**特点：**
- **完整 C++ 重写**，全部逻辑用 C++ 实现
- 插件化结构，内容分为三个类别：`GASP`（主体）、`GASPCamera`（相机）、`GASPExtras`（可选扩展）
- 集成 ALS 风格的覆盖层（Overlay Layering）系统
- 含 Editor 专用模块（`GASPEditor`）
- 积极维护，版本号已更新至 1.12，跨越多个 UE 版本
- MIT 协议，可直接集成到商业项目
- 提供 Console Commands 调试支持（见 CONSOLE_COMMANDS.md）

---

### CoffeeVampir3/Unreal-3rd-Person-Parkour

**完整 C++ 重写的独立项目（Standalone Project），附加了跑酷功能扩展。**

| 属性 | 内容 |
|------|------|
| 仓库链接 | https://github.com/CoffeeVampir3/Unreal-3rd-Person-Parkour |
| 形式 | **独立 UE 项目（Standalone Project）** |
| 主要语言 | **C++** |
| 描述 | 将 UE 官方 Game Animation Sample 完整重写为 C++，并添加跑酷扩展 |

**README 原文：**

> "This project is a full rewrite of the game animation sample project from unreal into C++."

**特点：**
- 完整的 C++ 重写（Full C++ Rewrite）
- 保留了 Epic 官方资源（动画、角色等）
- 额外添加了 Parkour（跑酷）功能扩展

---

### SAM-tak/GASP-ALS-R

| 属性 | 内容 |
|------|------|
| 仓库链接 | https://github.com/SAM-tak/GASP-ALS-R |
| 主要语言 | C++ |
| 描述 | Game Animation Sample with ALS Refactored Layering and GAS（Gameplay Ability System） |

**特点：**
- C++ 实现
- 集成 ALS-Refactored 覆盖层系统
- 集成 GAS（Gameplay Ability System）
- 模块化动画图（Modular Anim Graphs）和链接层（Linked Layers）
- 包含 Traversal Action 作为 Gameplay Ability

---

## 相关衍生项目（蓝图为主，含部分 C++）

### PolygonHive/GASPALS（⭐ 1000+）

| 属性 | 内容 |
|------|------|
| 仓库链接 | https://github.com/PolygonHive/GASPALS |
| Star 数 | 1000+ |
| 主要语言 | 蓝图（Blueprint） |
| 描述 | Game Animation Sample with ALS Layering，在 GASP 基础上集成了 ALS（Advanced Locomotion System）覆盖层系统 |

**特点：**
- 将 ALS 的覆盖层（Overlay Layering）系统集成进 GASP
- 支持网络同步（Replication）
- 包含武器附加系统、覆盖层切换 Widget
- 有配套 YouTube 教程（[Polygon Hive 频道](https://www.youtube.com/watch?v=RDWNfIqvWBk&list=PLs9e0eJQMI2aaulgKJzC8feN1UEwDkEnq)）
- **注意**：主要使用蓝图，非 C++ 重写

---

### Bizzy1234/GASP-ALS（⭐ 14）

| 属性 | 内容 |
|------|------|
| 仓库链接 | https://github.com/Bizzy1234/GASP-ALS |
| 描述 | Game Animation Sample (UE 5.5) with ALS Layering，包含网络同步 |

---

### The-Recon-Project/GASP_DynamicAdditiveOverlay

| 属性 | 内容 |
|------|------|
| 仓库链接 | https://github.com/The-Recon-Project/GASP_DynamicAdditiveOverlay |
| Fork 数 | 35+ |
| 描述 | 在 GASP 中引入动态叠加覆盖（Dynamic Additive Overlay）技术 |

---

## 总结对比

| 项目 | 类型 | 是否 C++ 重写 | 维护状态 | 推荐度 |
|------|------|--------------|----------|--------|
| [Anaylan/GASP-Refactored](https://github.com/Anaylan/GASP-Refactored) | UE 插件 | ✅ 完整 C++ 重写（MIT 协议） | 🟢 积极维护（v1.12，支持 UE5.7） | ⭐⭐⭐⭐⭐ |
| [CoffeeVampir3/Unreal-3rd-Person-Parkour](https://github.com/CoffeeVampir3/Unreal-3rd-Person-Parkour) | 独立项目 | ✅ 完整 C++ 重写 | 🟡 未知 | ⭐⭐⭐⭐ |
| [SAM-tak/GASP-ALS-R](https://github.com/SAM-tak/GASP-ALS-R) | 独立项目 | ✅ C++（含 GAS） | 🟡 较少更新 | ⭐⭐⭐⭐ |
| [PolygonHive/GASPALS](https://github.com/PolygonHive/GASPALS) | 独立项目 | ❌ 主要蓝图 | 🟢 积极维护 | ⭐⭐⭐（ALS 集成参考） |
| [Bizzy1234/GASP-ALS](https://github.com/Bizzy1234/GASP-ALS) | 独立项目 | ❌ 主要蓝图 | 🟡 少量更新 | ⭐⭐⭐ |

### 推荐选择

**如果你想要一个可以直接集成到项目的 C++ 插件：**

👉 **https://github.com/Anaylan/GASP-Refactored**（MIT 协议，插件形式，持续维护至 UE 5.7）

**如果你想要一个完整重写的独立 UE 项目（含跑酷扩展）：**

👉 **https://github.com/CoffeeVampir3/Unreal-3rd-Person-Parkour**

---

*搜索日期：2026-03-21（第二轮补充搜索）*
