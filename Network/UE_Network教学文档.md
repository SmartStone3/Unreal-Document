# Unreal Engine C++ Dedicated Server 网络同步完全指南

## 目录

1. [网络同步基本概念与原理](#1-网络同步基本概念与原理)
2. [核心架构解析](#2-核心架构解析)
3. [属性复制系统](#3-属性复制系统)
4. [RPC远程过程调用](#4-rpc远程过程调用)
5. [Actor生命周期与相关性](#5-actor生命周期与相关性)
6. [所有权（Ownership）系统详解](#6-所有权ownership系统详解)
7. [相关性（Relevancy）系统详解](#7-相关性relevancy系统详解)
8. [源码核心流程解析](#8-源码核心流程解析)
9. [Dedicated Server配置与部署](#9-dedicated-server配置与部署)
10. [使用案例与最佳实践](#10-使用案例与最佳实践)
11. [性能优化与调试](#11-性能优化与调试)
12. [Iris复制系统（UE5新特性）](#12-iris复制系统ue5新特性)

---

## 1. 网络同步基本概念与原理

### 1.1 客户端-服务器架构

Unreal Engine采用**客户端-服务器（Client-Server）**模型进行网络通信。在这种架构中：

- **Server（服务器）**：拥有权威的游戏状态，负责处理所有游戏逻辑、物理碰撞、伤害计算等核心功能
- **Client（客户端）**：仅负责渲染和输入处理，呈现由服务器授权的状态

```
┌─────────────────────────────────────────────────────────────────┐
│                        NETWORK ARCHITECTURE                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│    ┌──────────┐      ┌──────────┐      ┌──────────┐            │
│    │  Client  │      │  Client  │      │  Client  │            │
│    │    1     │      │    2     │      │    N     │            │
│    └────┬─────┘      └────┬─────┘      └────┬─────┘            │
│         │                 │                 │                   │
│         │    Network      │                 │                   │
│         └────────────────┴─────────────────┘                   │
│                           │                                     │
│                    ┌──────┴──────┐                              │
│                    │   Server    │                              │
│                    │  (Authoritative) │                          │
│                    └─────────────┘                              │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 网络模式（Network Modes）

UE支持四种网络模式，通过`ENetMode`枚举定义：

| 网络模式 | 枚举值 | 描述 |
|---------|--------|------|
| Standalone | `NM_Standalone` | 单人游戏或本地多人游戏，同时充当服务器和客户端 |
| Dedicated Server | `NM_DedicatedServer` | 纯服务器，无本地玩家，仅处理游戏逻辑和网络 |
| Listen Server | `NM_ListenServer` | 主机玩家同时作为服务器运行 |
| Client | `NM_Client` | 连接到远程服务器的客户端 |

### 1.3 网络角色（Network Role）

每个Actor在网络上都有两个关键角色属性：

- **`Role`（本地角色）**：当前机器上该Actor的角色
- **`RemoteRole`（远程角色）**：其他机器上该Actor的角色

```cpp
// NetworkRole.h 中的枚举定义
enum class ENetRole
{
    /** 本地是无效角色 */
    ROLE_None,
    
    /** 本地是模拟代理（仅用于表现） */
    ROLE_SimulatedProxy,
    
    /** 本地是自治代理（本地玩家控制） */
    ROLE_AutonomousProxy,
    
    /** 本地是网络权威（服务器或拥有者） */
    ROLE_Authority
};
```

**角色分配示例：**

```
Server上的PlayerController:     Role = Authority,         RemoteRole = AutonomousProxy
Server上的Character:             Role = Authority,         RemoteRole = SimulatedProxy
Client上的PlayerController:      Role = AutonomousProxy,    RemoteRole = Authority  
Client上的其他Character:         Role = SimulatedProxy,     RemoteRole = Authority
```

### 1.4 复制（Replication）原理

复制是UE网络同步的核心机制，其基本流程如下：

```
Server端状态变化
        │
        ▼
┌───────────────────┐
│  标记脏数据（Dirty）│
└────────┬──────────┘
         │
         ▼
┌───────────────────┐
│  收集需要复制的属性│
└────────┬──────────┘
         │
         ▼
┌───────────────────┐
│  序列化属性数据   │
└────────┬──────────┘
         │
         ▼
┌───────────────────┐
│  通过网络发送     │
└────────┬──────────┘
         │
         ▼
┌───────────────────┐
│  Client反序列化   │
└────────┬──────────┘
         │
         ▼
┌───────────────────┐
│  应用到本地Actor  │
└───────────────────┘
```

---

## 2. 核心架构解析

### 2.1 核心网络类层次结构

```
UObject
├── UActorComponent
│   ├── UMovementComponent
│   │   └── UCharacterMovementComponent
│   ├── UNetConnection
│   └── UReplicationDriver
│
└── AActor (网络复制的基础)
    ├── APawn
    │   ├── ACharacter
    │   │   └── AYourCharacter
    │   └── AVehicle
    │
    ├── APlayerController
    ├── AGameModeBase
    └── AGameStateBase
```

### 2.2 关键网络组件

#### UNetDriver

`UNetDriver`是网络通信的核心管理器，负责：
- 管理所有网络连接
- 处理Actor复制
- 协调网络tick

**关键源码位置：** `Engine/Source/Runtime/Engine/Classes/Engine/NetDriver.h`

```cpp
// 核心复制函数
virtual int32 ServerReplicateActors(float DeltaSeconds);

// 关键成员
class UNetConnection* ServerConnection;    // 客户端连接到服务器
TArray<UNetConnection*> RemoteConnections;  // 服务器上的客户端连接
UReplicationDriver* ReplicationDriver;      // 复制驱动
```

#### UNetConnection

代表一个网络连接，每个客户端连接对应一个`UNetConnection`实例。

```cpp
// 关键属性
class UNetDriver* NetDriver;           // 所属网络驱动
class UWorld* OwningWorld;             // 所属世界
int32 ChildConnections;                // 子连接数量
uint8 PlayerId;                        // 玩家ID
ENetRole Route;                        // 路由角色
```

#### UChannel

通道是数据传输的基本单位，包括：
- **Actor Channel**：专门用于Actor复制
- **Control Channel**：处理连接控制
- **Voice Channel**：处理语音数据

```cpp
// UChannel 基类
class UChannel
{
public:
    enum class EChannelType
    {
        CHTYPE_None,
        CHTYPE_Control,
        CHTYPE_Actor,
        CHTYPE_Voice,
        // ...
    };
    
    EChannelType ChannelType;
    UNetConnection* Connection;
    int32 ChannelIndex;
};
```

### 2.3 复制系统架构

UE5提供三种复制系统：

1. **Generic Replication System**：传统复制系统
2. **Replication Graph**：优化的复制系统（UE4.20+）
3. **Iris Replication System**：UE5全新引入的高性能复制系统

---

## 3. 属性复制系统

### 3.1 属性复制基础

属性复制使用`UPROPERTY`宏中的`Replicated`或`ReplicatedUsing`说明符。

#### 基础属性复制

```cpp
// MyActor.h
#include "Net/UnrealNetwork.h"

class AMyActor : public AActor
{
    GENERATED_BODY()

public:
    // 基础复制属性
    UPROPERTY(Replicated)
    float Health;

    // 带RepNotify的复制（值变化时自动回调）
    UPROPERTY(ReplicatedUsing=OnRep_Score)
    int32 Score;

    // 复制条件变体
    UPROPERTY(ReplicatedUsing=OnRep_Transform, Replicated)
    FVector_NetQuantize100 Position;

protected:
    UFUNCTION()
    void OnRep_Score();

    UFUNCTION()
    void OnRep_Transform();

    // 必须在头文件中声明此函数
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

```cpp
// MyActor.cpp
#include "MyActor.h"

void AMyActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    // 使用DOREPLIFETIME宏复制简单属性
    DOREPLIFETIME(AMyActor, Health);
    
    // 使用DOREPLIFETIME_CONDITION复制带条件的属性
    DOREPLIFETIME_CONDITION(AMyActor, Score, COND_OwnerOnly);
}

void AMyActor::OnRep_Score()
{
    // 当客户端收到复制的Score值时自动调用
    UE_LOG(LogTemp, Warning, TEXT("Score updated to: %d"), Score);
    // 更新UI、播放特效等
}

void AMyActor::OnRep_Transform()
{
    // 位置更新回调
}
```

### 3.2 复制条件（Replication Conditions）

UE提供丰富的复制条件来优化网络流量：

| 条件 | 描述 | 适用场景 |
|------|------|---------|
| `COND_None` | 无条件复制（默认） | 始终需要同步的数据 |
| `COND_InitialOnly` | 仅在Actor首次出现时复制 | 静态属性 |
| `COND_OwnerOnly` | 仅复制给Owner | 玩家私有数据 |
| `COND_SkipOwner` | 跳过Owner复制给其他客户端 | 广播给其他人 |
| `COND_SimulatedOnly` | 仅复制给模拟代理 | 纯表现数据 |
| `COND_AutonomousOnly` | 仅复制给自治代理 | AI不需要的数据 |
| `COND_SimulatedOrPhysics` | 复制给模拟代理或物理对象 | 物理相关 |
| `COND_InitialOrOwner` | 首次出现或Owner时复制 | 静态+私有数据 |
| `COND_Custom` | 自定义条件 | 需要手动处理 |

**使用示例：**

```cpp
// 私有数据只给拥有者
UPROPERTY(ReplicatedUsing=OnRep_Ammo, replicated)
int32 AmmoCount;

// 仅客户端需要的敏感信息
UPROPERTY(ReplicatedUsing=OnRep_SecretInfo, Replicated)
FString SecretData;

// 模拟数据不需要发送给AI
UPROPERTY(ReplicatedUsing=OnRep_Visuals)
FVector TargetLocation;
```

### 3.3 RepNotify机制

RepNotify是UE提供的自动化同步回调机制：

```cpp
// 完整示例：玩家生命值同步
class AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth)
    float CurrentHealth;

    UPROPERTY(ReplicatedUsing=OnRep_MaxHealth)
    float MaxHealth;

    UFUNCTION()
    void OnRep_CurrentHealth();

    UFUNCTION()
    void OnRep_MaxHealth();

    void SetHealth(float NewHealth);
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void OnHealthUpdate() const;  // 实际更新UI的逻辑
};
```

```cpp
// Character实现
void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AMyCharacter, CurrentHealth);
    DOREPLIFETIME(AMyCharacter, MaxHealth);
}

void AMyCharacter::OnRep_CurrentHealth()
{
    OnHealthUpdate();
}

void AMyCharacter::OnRep_MaxHealth()
{
    OnHealthUpdate();
}

void AMyCharacter::SetHealth(float NewHealth)
{
    if (GetLocalRole() == ROLE_Authority)
    {
        CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
        // 属性修改会自动触发复制
    }
}

void AMyCharacter::OnHealthUpdate() const
{
    // 所有客户端和服务器都会执行这里
    // 可以更新血条UI、播放受伤特效等
    
    if (IsLocallyControlled())
    {
        // 仅本地玩家更新血条
    }
}
```

### 3.4 复杂属性复制

#### 结构体复制

```cpp
// 自定义结构体
USTRUCT()
struct FPlayerStats
{
    GENERATED_BODY()

    UPROPERTY()
    float Score;

    UPROPERTY()
    int32 Kills;

    UPROPERTY()
    int32 Deaths;

    UPROPERTY()
    FString PlayerName;
};

// Actor中使用
UCLASS()
class APlayerState : public AInfo
{
    UPROPERTY(Replicated)
    FPlayerStats Stats;
};

// 结构体需要实现复制支持
// UE5会自动处理USTRUCT的复制
```

#### 数组复制

```cpp
// 动态数组复制
UPROPERTY(Replicated)
TArray<FVector> RecentLocations;

UPROPERTY(Replicated)
TArray<AActor*> VisibleActors;

// 数组操作会自动触发复制
void AMyActor::AddLocation(FVector NewLoc)
{
    if (GetLocalRole() == ROLE_Authority)
    {
        RecentLocations.Add(NewLoc);
        // 自动触发复制到所有客户端
    }
}
```

#### 对象引用复制

```cpp
// 复制对象引用
UPROPERTY(Replicated)
AWeapon* CurrentWeapon;

UPROPERTY(Replicated)
TArray<AProjectile*> ActiveProjectiles;

// 复制弱引用
UPROPERTY(Replicated)
TWeakObjectPtr<AActor> TargetActor;
```

### 3.5 复制属性宏详解

```cpp
// 基本复制
DOREPLIFETIME(AActor, PropertyName)

// 带条件的复制
DOREPLIFETIME_CONDITION(AActor, PropertyName, COND_OwnerOnly)

// 复制带增量更新支持的数组
DOREPLIFETIME_WITH_PARAMS(AActor, ArrayProperty, bRefetch, bForce)

// 内联模板版本
DOREPLIFETIME_INLINE(AActor, PropertyName, bRefetch)
```

---

## 4. RPC远程过程调用

### 4.1 RPC基础

RPC（Remote Procedure Call）允许在网络中的不同机器上调用函数。

```cpp
// RPC函数必须在头文件中标记
// 格式：函数返回类型前需要添加网络规范说明符

// Server RPC - 在服务器执行，从客户端调用
UFUNCTION(Server, Reliable)  // Reliable保证送达
void ServerApplyDamage(AActor* Target, float Damage);

// Client RPC - 在客户端执行，从服务器调用
UFUNCTION(Client, Reliable)
void ClientShowDamageEffect(float DamageAmount);

// Multicast RPC - 在所有客户端执行，从服务器调用
UFUNCTION(NetMulticast, Reliable)
void MulticastPlayDeathEffect();
```

### 4.2 RPC规范说明符

| 说明符 | 描述 | 可靠性 |
|--------|------|--------|
| `Server` | 在服务器执行 | 取决于后续关键字 |
| `Client` | 在Actor的Owner客户端执行 | 取决于后续关键字 |
| `NetMulticast` | 在所有已连接客户端执行 | 取决于后续关键字 |
| `Reliable` | 保证送达，必要时重传 | 是 |
| `Unreliable` | 可能丢包，不重传 | 否 |
| `WithValidation` | 需要验证函数 | - |

### 4.3 Server RPC

Server RPC从客户端调用，在服务器执行：

```cpp
// 头文件
class AMyCharacter : public ACharacter
{
public:
    UFUNCTION(Server, Reliable)
    void ServerTryFire();

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerTryFireWithValidation();
};
```

```cpp
// 源文件
void AMyCharacter::ServerTryFire_Implementation()
{
    // 仅在服务器执行
    // 可以安全地进行伤害计算
    
    // 检查权限
    if (!HasAuthority())
    {
        return;
    }
    
    // 执行开火逻辑
    Fire();
    
    // 广播给所有客户端
    MulticastPlayFireEffects();
}

bool AMyCharacter::ServerTryFireWithValidation_Implementation()
{
    // 验证客户端请求
    // 返回false可以拒绝请求
    return CanFire();
}
```

### 4.4 Client RPC

Client RPC从服务器调用，在客户端执行：

```cpp
// 头文件
class AMyCharacter : public ACharacter
{
public:
    UFUNCTION(Client, Reliable)
    void ClientOnHit(float Damage, AActor* DamageCauser);
    
    UFUNCTION(Client, Reliable)
    void ClientShowGameOverUI();
};
```

```cpp
// 源文件
void AMyCharacter::ClientOnHit_Implementation(float Damage, AActor* DamageCauser)
{
    // 仅在拥有此Actor的客户端执行
    
    // 播放受伤音效
    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
    }
    
    // 显示伤害数字
    ShowDamageNumber(Damage);
}

void AMyCharacter::ClientShowGameOverUI_Implementation()
{
    // 显示游戏结束界面
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        // 显示UI
    }
}
```

### 4.5 Multicast RPC

Multicast RPC从服务器调用，在所有相关客户端执行：

```cpp
// 头文件
class AProjectile : public AActor
{
public:
    UFUNCTION(NetMulticast, Reliable)
    void MulticastExplode();
};
```

```cpp
// 源文件
void AProjectile::MulticastExplode_Implementation()
{
    // 在服务器和所有客户端执行
    
    // 播放爆炸特效
    if (ExplosionEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation());
    }
    
    // 播放爆炸音效
    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
    }
    
    // 屏幕震动（仅本地客户端）
    if (IsLocallyControlled())
    {
        // 震动
    }
}
```

### 4.6 RPC执行流程图

```
                    Server RPC调用流程
    ┌─────────────┐     ┌─────────────┐
    │   Client    │────▶│   Server    │
    │ ServerFunc()│     │_Implementation()
    └─────────────┘     └─────────────┘
              │               │
              │ 序列化        │
              ▼               ▼
         Network Data    反序列化
         
                    Client RPC调用流程
    ┌─────────────┐     ┌─────────────┐
    │   Server    │────▶│   Client    │
    │ ClientFunc()│     │_Implementation()
    └─────────────┘     └─────────────┘
              │               │
              │ 序列化        │
              ▼               ▼
         Network Data    反序列化
         
                   Multicast RPC调用流程
    ┌─────────────┐     ┌─────────────┐
    │   Server    │────▶│  All Clients│
    │MulticastFunc│     │_Implementation()
    └─────────────┘     └─────────────┘
              │               │
              │ 广播          │
              ▼               ▼
         Network Data    各自反序列化
```

### 4.7 RPC使用注意事项

#### 参数限制

```cpp
// ✅ 支持的参数类型
UFUNCTION(Server, Reliable)
void ServerTest(
    int32 IntParam,           // 基本类型
    FVector VectorParam,      // 结构体
    AActor* ActorParam,       // 对象引用
    TArray<int32> ArrayParam  // 数组
);

// ❌ 不支持的参数类型
UFUNCTION(Server, Reliable)
void ServerBad(
    UObject* ObjectParam,     // 不支持：非Actor对象
    UPROPERTY()指针          // 不支持：普通UObject*
);
```

#### Actor上下文要求

```cpp
// Server RPC只能从以下情况调用：
// 1. 从拥有该Actor的客户端调用
// 2. 从服务器调用

void AMyCharacter::TryMeleeAttack()
{
    // 正确：检查本地Role
    if (GetLocalRole() < ROLE_AutonomousProxy)
    {
        return;
    }
    
    // 调用Server RPC
    ServerMeleeAttack();
}
```

#### 可靠性选择

```cpp
// 可靠：重要游戏逻辑
UFUNCTION(Server, Reliable)
void ServerPickupItem(AItem* Item);

// 不可靠：高频更新（位置、旋转）
UFUNCTION(Server, Unreliable)
void ServerUpdateRotation(FRotator NewRotation);

// 可靠但可跳过：UI更新
UFUNCTION(Client, Reliable)
void ClientUpdateScore(int32 NewScore);
```

---

## 5. Actor生命周期与相关性

### 5.1 Actor复制启用

```cpp
// C++中启用Actor复制
class AMyActor : public AActor
{
public:
    AMyActor()
    {
        // 启用复制
        bReplicates = true;
        
        // 复制移动组件（对于移动的Actor很重要）
        bReplicateMovement = true;
        
        // 设置NetPriority（优先级）
        NetPriority = 1.0f;
        
        // 设置最小复制间隔
        MinNetUpdateFrequency = 2.0f;
    }
};
```

---

## 6. 所有权（Ownership）系统详解

### 6.1 所有权基础概念

在Unreal Engine的网络系统中，**所有权（Ownership）**是一个核心概念，它决定了：

1. **哪些客户端可以接收特定Actor的属性更新**
2. **RPC（远程过程调用）可以发送到哪些客户端**
3. **属性复制条件如何生效**（如`COND_OwnerOnly`）

#### Owner与Owning Connection的区别

```
┌────────────────────────────────────────────────────────────────────┐
│                        所有权关系层级                                │
├────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   PlayerController (客户端A)  ◄── Owning Connection (连接A)        │
│          │                                                          │
│          ▼                                                          │
│   Pawn/Character (客户端A控制的角色) ◄── Owner                     │
│          │                                                          │
│          ▼                                                          │
│   Weapon (武器) ◄── Owner                                          │
│          │                                                          │
│          ▼                                                          │
│   InventoryItem (物品) ◄── Owner                                    │
│                                                                     │
└────────────────────────────────────────────────────────────────────┘
```

- **Owner（拥有者）**：一个Actor对象指向另一个Actor的指针
- **Owning Connection（所属连接）**：与Actor的Owner相关联的网络连接

### 6.2 所有权的工作原理

#### 设置Owner

```cpp
// 方法1：在构造函数中设置
AWeapon::AWeapon()
{
    bReplicates = true;
    // 在装备时由Character设置Owner
}

// 方法2：手动设置Owner
void ACharacter::EquipWeapon(AWeapon* Weapon)
{
    if (Weapon)
    {
        Weapon->SetOwner(this);  // Character成为Weapon的Owner
        Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "HandSocket");
    }
}

// 方法3：Blueprint设置
// 在Blueprint中：Set Owner节点
```

#### 获取Owner

```cpp
// 获取直接Owner
AActor* Owner = MyActor->GetOwner();

// 获取最外层Owner（递归获取）
AActor* GetOuterOwner(AActor* Actor)
{
    AActor* CurrentOwner = Actor->GetOwner();
    AActor* FinalOwner = Actor;
    
    while (CurrentOwner)
    {
        FinalOwner = CurrentOwner;
        CurrentOwner = CurrentOwner->GetOwner();
    }
    
    return FinalOwner;
}

// ActorComponent获取Owner
AActor* ComponentOwner = MyComponent->GetOwner();
```

#### 获取Owning Connection

```cpp
// 通过Actor获取Owning Connection
UNetConnection* Connection = MyActor->GetNetConnection();

// 通过PlayerController获取Owning Connection
UNetConnection* PCConnection = MyPlayerController->GetNetConnection();

// 通过Pawn获取Owning Connection
UNetConnection* PawnConnection = nullptr;
if (APlayerController* PC = MyPawn->GetController<APlayerController>())
{
    PawnConnection = PC->GetNetConnection();
}
```

### 6.3 所有权与属性复制

#### 复制条件与所有权

```cpp
// PlayerState.h - 玩家状态示例
class AMyPlayerState : public APlayerState
{
public:
    // 仅Owner可见（玩家自己的数据）
    UPROPERTY(ReplicatedUsing=OnRep_Health, Replicated)
    float Health;

    // 仅Owner可见（敏感数据）
    UPROPERTY(ReplicatedUsing=OnRep_Ammo, Replicated)
    int32 AmmoCount;

    // 跳过Owner，广播给其他所有人
    UPROPERTY(ReplicatedUsing=OnRep_Kills)
    int32 KillCount;

    // 仅Owning Client可见
    UPROPERTY(ReplicatedUsing=OnRep_PrivateToken, Replicated)
    FString PrivateToken;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        
        // COND_OwnerOnly：仅复制给Owner
        DOREPLIFETIME_CONDITION(AMyPlayerState, Health, COND_OwnerOnly);
        DOREPLIFETIME_CONDITION(AMyPlayerState, AmmoCount, COND_OwnerOnly);
        
        // COND_SkipOwner：跳过Owner，复制给其他人
        DOREPLIFETIME_CONDITION(AMyPlayerState, KillCount, COND_SkipOwner);
        
        // COND_OwnerOnly用于敏感数据
        DOREPLIFETIME_CONDITION(AMyPlayerState, PrivateToken, COND_OwnerOnly);
    }
};
```

#### 条件复制详解

| 条件 | 说明 | 典型用途 |
|------|------|---------|
| `COND_OwnerOnly` | 只复制给Actor的Owner | 玩家私有数据（背包、任务等） |
| `COND_SkipOwner` | 复制给除Owner外的所有人 | 击杀数、成就等广播数据 |
| `COND_InitialOnly` | 仅首次出现时复制 | 静态配置数据 |
| `COND_AutonomousOnly` | 仅复制给自治代理 | 本地玩家专有数据 |

### 6.4 所有权与RPC

#### Server RPC的所有权验证

```cpp
// 玩家控制器的武器开火RPC
class AMyCharacter : public ACharacter
{
public:
    // Server RPC - 只能由拥有此Character的玩家调用
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerFire();

    UFUNCTION(Server, Reliable)
    void ServerReload();
};

// 实现
void AMyCharacter::ServerFire_Implementation()
{
    // 验证调用者是否拥有此Actor
    // UE会自动验证，但可以添加额外检查
    
    if (GetLocalRole() != ROLE_AutonomousProxy)
    {
        // 客户端尝试调用但不是自治代理
        UE_LOG(LogMyGame, Warning, TEXT("ServerFire called by non-autonomous proxy"));
        return;
    }
    
    // 检查冷却和弹药
    if (!CanFire())
    {
        return;
    }
    
    // 执行开火
    Fire();
}

bool AMyCharacter::ServerFire_Validate()
{
    // 验证客户端请求的合法性
    return true;  // 可以添加更复杂的验证
}
```

#### Client RPC的所有权分发

```cpp
// 服务器调用Client RPC时，根据Owner决定目标客户端
class AMyProjectile : public AActor
{
public:
    // Client RPC - 仅在Owner的客户端执行
    UFUNCTION(Client, Reliable)
    void ClientOnHit(AActor* HitActor);
    
    // Multicast - 广播给所有客户端
    UFUNCTION(NetMulticast, Reliable)
    void MulticastOnExplode();
};

void AMyProjectile::MulticastOnExplode_Implementation()
{
    // 所有客户端播放爆炸效果
    PlayExplosionEffect();
}

// 服务器调用Client RPC - 只发送给Owner
void AMyProjectile::ServerDealDamage()
{
    // 造成伤害...
    
    // 只通知击中者的Owner（被击中的玩家）
    if (AActor* HitActor = GetInstigator())
    {
        // 只有被击中的玩家会收到这个回调
        HitActor->ClientOnHit(this);  // 客户端收到伤害飘字
    }
}
```

### 6.5 所有权在实际游戏中的应用

#### 武器系统

```cpp
// 武器配置 - 仅武器所有者需要完整数据
class AMyWeapon : public AActor
{
public:
    UPROPERTY(Replicated, ReplicatedUsing=OnRep_Ammo)
    int32 AmmoInClip;

    UPROPERTY(Replicated)
    int32 TotalAmmo;

    UPROPERTY(Replicated)
    float Damage;

    UPROPERTY(Replicated)
    EWeaponState WeaponState;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        
        // 弹药数仅Owner需要知道精确值
        DOREPLIFETIME_CONDITION(AMyWeapon, AmmoInClip, COND_OwnerOnly);
        DOREPLIFETIME_CONDITION(AMyWeapon, TotalAmmo, COND_OwnerOnly);
        
        // 武器状态和伤害可以广播
        DOREPLIFETIME(AMyWeapon, Damage);
        DOREPLIFETIME(AMyWeapon, WeaponState);
    }
};
```

#### 技能/魔法系统

```cpp
// 技能组件
class UMyAbilityComponent : public UActorComponent
{
public:
    UPROPERTY(Replicated, ReplicatedUsing=OnRep_Cooldowns)
    TMap<int32, float> Cooldowns;

    UPROPERTY(Replicated, ReplicatedUsing=OnRep_AvailableAbilities)
    TArray<bool> AvailableAbilities;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        
        // 冷却时间只有自己需要知道
        DOREPLIFETIME_CONDITION(UMyAbilityComponent, Cooldowns, COND_OwnerOnly);
        
        // 可用技能可以共享（让队友看到你能用什么）
        DOREPLIFETIME(UMyAbilityComponent, AvailableAbilities);
    }
};
```

#### 背包/库存系统

```cpp
// 库存组件
class UMyInventoryComponent : public UActorComponent
{
public:
    UPROPERTY(Replicated, ReplicatedUsing=OnRep_Items)
    TArray<FInventoryItem> Items;

    UPROPERTY(Replicated, ReplicatedUsing=OnRep_Gold)
    int32 Gold;

    UPROPERTY(Replicated, ReplicatedUsing=OnRep_EquippedItems)
    TArray<int32> EquippedItems;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        
        // 背包内容只有自己可见
        DOREPLIFETIME_CONDITION(UMyInventoryComponent, Items, COND_OwnerOnly);
        DOREPLIFETIME_CONDITION(UMyInventoryComponent, Gold, COND_OwnerOnly);
        
        // 装备可以显示给其他人看
        DOREPLIFETIME(UMyInventoryComponent, EquippedItems);
    }
};
```

### 6.6 所有权相关的重要属性

```cpp
// Actor.h中的所有权相关属性
class AActor
{
public:
    // Owner指针 - 用于复制和RPC
    UPROPERTY()
    TObjectPtr<AActor> Owner;

    // 如果为true，此Actor仅对其Owner可见（不发送给其他客户端）
    // PlayerController默认设置为true
    uint8 bOnlyRelevantToOwner : 1;

    // 如果为true，使用Owner的相关性判断
    // 子Actor会继承Owner的相关性
    uint8 bNetUseOwnerRelevancy : 1;
    
    // 始终对所有客户端可见（忽略距离检查）
    // 谨慎使用，会增加带宽消耗
    uint8 bAlwaysRelevant : 1;
};
```

### 6.7 常见问题与解决方案

| 问题 | 原因 | 解决方案 |
|------|------|---------|
| Client RPC不执行 | Owner为空或Owner没有连接 | 确保正确设置Owner |
| 属性不复制给Owner | 忘记实现GetLifetimeReplicatedProps | 添加DOREPLIFETIME |
| RPC被拒绝 | 客户端没有权限 | 检查HasAuthority和Role |
| 数据泄露 | 敏感数据使用了COND_None | 改用COND_OwnerOnly |

---

## 7. 相关性（Relevancy）系统详解

### 7.1 相关性基础概念

**相关性（Relevancy）**是UE网络复制中的关键优化机制。服务器只向客户端发送与该客户端相关的Actor，节省带宽和CPU资源。

```
┌────────────────────────────────────────────────────────────────────┐
│                      相关性过滤示意图                                │
├────────────────────────────────────────────────────────────────────┤
│                                                                     │
│                        Server                                       │
│                         │                                           │
│         ┌───────────────┼───────────────┐                         │
│         │               │               │                         │
│    All Actors      Relevant to      Irrelevant to                │
│    on Server       Client A          Client A                     │
│         │               │               │                         │
│         ▼               ▼               ▼                         │
│    [Hundreds]      [Dozens]        [Not Sent]                     │
│                                                                     │
│    • Player's Pawn         • Nearby Enemies                       │
│    • Teammates             • Nearby Props                         │
│    • Nearby Enemies        • Active Projectiles                  │
│    • Projectiles           • GameMode Objects                    │
│    • Static Props                                              │
│    • Distant Enemies                                            │
│                                                                     │
└────────────────────────────────────────────────────────────────────┘
```

### 7.2 相关性判断流程

服务器在决定Actor是否对特定客户端相关时，会执行以下检查：

```cpp
// AActor::IsNetRelevantFor 核心逻辑（简化版）
bool AActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, FVector SrcLocation) const
{
    // 1. bAlwaysRelevant 检查 - 始终相关
    if (bAlwaysRelevant)
        return true;
    
    // 2. Owner检查 - 如果有Owner，使用Owner的相关性
    if (bNetUseOwnerRelevancy && Owner)
        return Owner->IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
    
    // 3. bOnlyRelevantToOwner检查 - 仅Owner可见
    if (bOnlyRelevantToOwner)
    {
        // 检查RealViewer是否是此Actor的Owner相关
        return IsRelevancyOwnerFor(RealViewer, ViewTarget, SrcLocation);
    }
    
    // 4. 附加Actor检查 - 跟随父Actor的相关性
    if (GetAttachParentActor())
        return GetAttachParentActor()->IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
    
    // 5. 隐藏和碰撞检查
    if (bHidden && !HasAnyFlags(RF_Transitional))
    {
        if (!GetRootComponent()->GetCollisionEnabled() != ECollisionEnabled::NoCollision)
            return false;
    }
    
    // 6. 距离检查（如果启用了基于距离的相关性）
    if (bDistanceRelevant)
    {
        float Distance = FVector::Dist(GetActorLocation(), SrcLocation);
        if (Distance > NetCullDistanceSquared)
            return false;
    }
    
    return true;
}
```

### 7.3 相关性判断规则详解

#### 规则1：始终相关（bAlwaysRelevant）

```cpp
// 某些Actor应该始终对所有客户端可见
class AGameModeBase : public AActor
{
    AGameModeBase()
    {
        bReplicates = true;
        bAlwaysRelevant = true;  // 所有客户端都需要GameMode
    }
};

class APlayerState : public AInfo
{
    APlayerState()
    {
        bAlwaysRelevant = true;  // 玩家状态需要同步给所有人
    }
};
```

#### 规则2：所有者相关性（bOnlyRelevantToOwner）

```cpp
// 仅Owner可见的Actor
class APlayerController : public AController
{
    APlayerController()
    {
        bReplicates = true;
        bOnlyRelevantToOwner = true;  // 仅玩家自己需要自己的Controller
    }
};

// 武器通常也设置这个
class AWeapon : public AActor
{
    AWeapon()
    {
        bReplicates = true;
        bOnlyRelevantToOwner = true;  // 只复制给装备它的玩家
    }
};
```

#### 规则3：使用Owner的相关性（bNetUseOwnerRelevancy）

```cpp
// 武器使用其Owner（Character）的相关性
class AWeapon : public AActor
{
    AWeapon()
    {
        bReplicates = true;
        bNetUseOwnerRelevancy = true;  // 继承Owner的相关性判断
        // 这样武器会和Character保持相同的可见性
    }
};
```

#### 规则4：距离相关性

```cpp
// 设置可见距离
class AMyActor : public AActor
{
    AMyActor()
    {
        bReplicates = true;
        
        // 设置净视觉距离（厘米）
        NetCullDistanceSquared = 9000000.0f;  // 3000单位
        
        // 启用距离相关检查
        bDistanceRelevant = true;
    }
};
```

### 7.4 自定义相关性判断

#### 重写IsNetRelevantFor

```cpp
// 自定义相关性逻辑
class AMyMonster : public ACharacter
{
public:
    // 感知半径
    float PerceptionRadius = 2000.0f;

protected:
    virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, FVector SrcLocation) const override
    {
        // 基础检查
        if (Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation))
            return true;
        
        // 自定义逻辑：检查是否在感知范围内
        if (ViewTarget)
        {
            float Distance = FVector::Dist(GetActorLocation(), ViewTarget->GetActorLocation());
            if (Distance <= PerceptionRadius)
                return true;
            
            // 正在战斗的怪物始终相关
            if (bIsInCombat)
                return true;
        }
        
        return false;
    }
};
```

#### 重写IsRelevancyOwnerFor

```cpp
// bOnlyRelevantToOwner为true时使用
class AMyWeapon : public AActor
{
protected:
    virtual bool IsRelevancyOwnerFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override
    {
        // 检查RealViewer是否是我们的Owner
        if (AActor* Owner = GetOwner())
        {
            // 更宽松的检查：Owner、Owning PlayerController、或Owning Pawn
            return Owner == RealViewer ||
                   Owner == ViewTarget ||
                   (Owner->IsA(APlayerController::StaticClass()) && 
                    Cast<APlayerController>(Owner)->GetPawn() == ViewTarget        return false;
    }
};
```

### 7.5 相关性与优先级的交互

#### 优先级系统

```cpp
// 复制优先级
class AMyActor : public AActor
{
public:
    AMyActor()
    {
        // 优先级设置
        NetPriority = 1.0f;  // 默认值
        
        // 高优先级Actor
        // PlayerController: 3.0
        // Pawn: 3.0
        // Weapon: 2.0
        // 普通Actor: 1.0
    }
    
    // 动态优先级
    virtual float GetNetPriority(const FVector& ViewPos, const FVector& ViewDir, 
                                  class UNetConnection* Connection, float Time) const override
    {
        // 基于距离调整优先级
        float BasePriority = NetPriority;
        
        if (APawn* ViewerPawn = Connection ? Connection->PlayerController->GetPawn() : nullptr)
        {
            float Distance = FVector::Dist(GetActorLocation(), ViewerPawn->GetActorLocation());
            
            // 越近优先级越高
            if (Distance < 500.0f)
                BasePriority *= 2.0f;
            else if (Distance > 2000.0f)
                BasePriority *= 0.5f;
        }
        
        // 考虑时间因子（避免饥饿）
        float TimeSinceLastRep = Time - LastRepTime;
        
        return BasePriority * TimeSinceLastRep;
    }
};
```

### 7.6 PreReplication机制

```cpp
// 在复制前动态决定是否复制某些属性
class AMyActor : public AActor
{
public:
    UPROPERTY(Replicated)
    FVector SimulatedPosition;

    UPROPERTY(Replicated)
    float DetailedPrivateData;  // 仅Owner需要

protected:
    virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override
    {
        Super::PreReplication(ChangedPropertyTracker);
        
        // 对Owner条件使用DOREPLIFETIME_ACTIVE_OVERRIDE
        
        // 模拟位置始终复制
        DOREPLIFETIME_ACTIVE_OVERRIDE(SimulatedPosition, true);
        
        // 私有数据仅对Owner复制
        bool bShouldReplicatePrivate = (GetOwner() != nullptr);
        DOREPLIFETIME_ACTIVE_OVERRIDE(DetailedPrivateData, bShouldReplicatePrivate);
    }
};
```

### 7.7 完整复制流程图

```
┌─────────────────────────────────────────────────────────────────────────┐
│                     ServerReplicateActors 完整流程                       │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  1. 收集所有复制的Actor                                                 │
│     │                                                                  │
│     ▼                                                                  │
│  2. Update Frequency检查                                               │
│     ├── 检查NetUpdateFrequency                                         │
│     ├── 检查MinNetUpdateFrequency                                      │
│     └── 过滤掉未到更新时间的Actor                                       │
│     │                                                                  │
│     ▼                                                                  │
│  3. Dormancy检查                                                        │
│     ├── DORM_Initial: 首次后跳过                                        │
│     ├── DORM_Dormant: 不复制                                           │
│     └── DORM_Awake: 始终复制                                           │
│     │                                                                  │
│     ▼                                                                  │
│  4. 初始相关性检查                                                      │
│     ├── bAlwaysRelevant: 总是加入列表                                   │
│     ├── bOnlyRelevantToOwner: 检查Owner                                 │
│     └── bNetUseOwnerRelevancy: 使用Owner判断                           │
│     │                                                                  │
│     ▼                                                                  │
│  5. Per-Connection相关性检查                                            │
│     ├── 对于每个连接调用IsNetRelevantFor()                              │
│     ├── 检查距离                                                        │
│     ├── 检查可见性                                                      │
│     └── 检查加载状态                                                    │
│     │                                                                  │
│     ▼                                                                  │
│  6. PreReplication调用                                                 │
│     ├── 对每个通过的Actor调用PreReplication()                          │
│     └── 允许动态修改复制条件                                           │
│     │                                                                  │
│     ▼                                                                  │
│  7. 优先级排序                                                          │
│     ├── 调用GetNetPriority()                                           │
│     ├── 按优先级排序                                                    │
│     └── 带宽受限时优先复制高优先级Actor                                 │
│     │                                                                  │
│     ▼                                                                  │
│  8. 带宽饱和检查                                                        │
│     ├── 检查连接带宽                                                    │
│     ├── 带宽满时停止复制                                               │
│     └── 低优先级Actor可能本帧跳过                                       │
│     │                                                                  │
│     ▼                                                                  │
│  9. 属性复制                                                           │
│     ├── 收集变化的属性                                                  │
│     ├── 序列化到网络包                                                  │
│     └── 发送到客户端                                                    │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 7.8 优化相关性的最佳实践

```cpp
// 优化示例
class AMyGameActor : public AActor
{
public:
    AMyGameActor()
    {
        bReplicates = true;
        
        // 1. 正确设置相关性
        bNetUseOwnerRelevancy = true;  // 使用Owner的相关性
        
        // 2. 合理设置优先级
        NetPriority = 1.5f;  // 比默认值稍高
        
        // 3. 合理设置更新频率
        NetUpdateFrequency = 10.0f;  // 每秒10次
        
        // 4. 距离优化
        NetCullDistanceSquared = 4000000.0f;  // 2000单位
    }
    
protected:
    virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, 
                                  FVector SrcLocation) const override
    {
        // 自定义优化：AI相关的特殊判断
        if (bIsActiveAI)
        {
            // AI始终对所有玩家相关
            return true;
        }
        
        return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
    }
};
```

### 7.9 调试相关性

```cpp
// 相关性调试工具
void AMyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

#if WITH_EDITOR
    if (GEngine && GEngine->bOnScreenDebugMessagesEnabled)
    {
        // 显示复制信息
        FString DebugInfo = FString::Printf(
            TEXT("Role: %d, RemoteRole: %d, Replicates: %d, Owner: %s"),
            (int32)GetLocalRole(),
            (int32)GetRemoteRole(),
            bReplicates,
            *GetNameSafe(GetOwner())
        );
        
        // 显示相关性状态
        if (GetNetDriver())
        {
            DebugInfo += FString::Printf(TEXT("\nNetMode: %d"), (int32)GetNetMode());
        }
        
        DrawDebugString(GetActorLocation(), DebugInfo, nullptr, FColor::White, 0.0f, true);
    }
#endif
}
```

```bash
# 控制台调试命令
net.stats.replication      # 显示复制统计
net.showrelevance         # 显示相关性信息
log LogNetPlayer Debug    # 详细日志
```

### 7.10 相关性相关Console变量

```bash
# 相关性调试
net.ShowRelevance 1                    # 显示相关性调试信息
net.PacketHandlerStats 0              # 数据包统计
net.IrrelevantTimeout 1.5              # 无关联超时时间

# 距离相关
net.DistanceCullSquared 1000000        # 距离剔除（平方）
net.UseDistanceBasedRelevancy 1        # 启用基于距离的相关性

# 带宽相关
net.MaxPacketSize 1280                # 最大数据包
net.ClientMaxTickRate 30              # 客户端最大tick率
```

### 7.11 Actor复制启用

```cpp
// C++中启用Actor复制
class AMyActor : public AActor
{
public:
    AMyActor()
    {
        // 启用复制
        bReplicates = true;
        
        // 复制移动组件（对于移动的Actor很重要）
        bReplicateMovement = true;
        
        // 设置NetPriority（优先级）
        NetPriority = 1.0f;
        
        // 设置最小复制间隔
        MinNetUpdateFrequency = 2.0f;
    }
};
```

### 7.12 Actor相关性（Relevancy）

服务器只向客户端发送相关的Actor：

```cpp
// IsNetRelevantFor 检查
bool AActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, FVector SrcLocation) const
{
    // 默认检查：
    // 1. bAlwaysRelevant - 始终相关
    // 2. Owner相关 - 如果Owner对Viewer可见
    // 3. 距离相关 - 在可见距离内
    // 4. Attach链 - 附加的Actor
}
```

#### 优化相关性的方法

```cpp
class AMyActor : public AActor
{
public:
    AMyActor()
    {
        bReplicates = true;
        
        // 始终相关（谨慎使用）
        bAlwaysRelevant = false;
        
        // 即使不在视野中也复制
        bNetUseOwnerRelevancy = true;
        
        // 跨Actor通道复制
        bReplicatesViaChannel = true;
    }
};
```

### 7.13 复制优先级

```cpp
// NetUpdateFrequency - 更新频率
float NetUpdateFrequency = 10.0f;  // 每秒10次

// NetPriority - 优先级
float NetPriority = 1.0f;  // 相对优先级

// 特殊Actor类型
void AImportantActor::SetReplicationPriority()
{
    NetPriority = 3.0f;  // 更高优先级
    NetUpdateFrequency = 20.0f;  // 更频繁更新
}
```

### 7.14 Actor通道管理

```cpp
// 预制Actor通道
void AMyGameMode::PreInitializeComponents()
{
    Super::PreInitializeComponents();
    
    // 为特定Actor预分配通道
}

// 手动关闭通道
void AMyActor::OnRep_Owner()
{
    Super::OnRep_Owner();
    
    // 当Owner改变时，可能需要重建通道
}
```

---

## 8. 源码核心流程解析

### 8.1 ServerReplicateActors主流程

```cpp
    // 2. 按相关性排序
    SortByRelevancy(ActorList);
    
    // 3. 遍历连接和Actor进行复制
    for (UNetConnection* Connection : Connections)
    {
        for (FActorPriority& ActorPri : ActorList)
        {
            // 检查相关性
            if (IsActorRelevantToConnection(ActorPri.Actor, Connection))
            {
                // 复制Actor
                ReplicateActor(ActorPri.Actor, Connection);
            }
        }
    }
    
    return NumReplicated;
}
```

### 8.2 属性复制流程

```cpp
// ActorChannel.cpp - 复制属性
bool UActorChannel::ReplicateActor()
{
    // 1. 创建复制描述符
    FReplicationChangelistMgr* ChangelistMgr = ReplicationInfo->GetChangelistMgr();
    
    // 2. 收集需要复制的属性
    TArray<uint16> RepIndices;
    TArray<FRepLayoutCmd> CmdBuffer;
    BuildChangedReps(ChangelistMgr, Connection->Driver->RepLayouts, RepIndices, CmdBuffer);
    
    // 3. 序列化属性数据
    for (uint16 RepIndex : RepIndices)
    {
        // 比较属性值并序列化
        SerializeItem(Writer, Property, Data, static_cast<int32>(RepIndex));
    }
    
    // 4. 发送数据
    return WriteBitStream();
}
```

### 8.3 RPC处理流程

```cpp
// NetConnection.cpp - 处理RPC
void UNetConnection::HandleRPC(UObject* Object, UFunction* Function, void* Parameters)
{
    // 1. 确定RPC目标
    ENetRole TargetRole = DetermineRPCRoute(Function);
    
    // 2. 验证权限
    if (!ValidateRPC(Object, Function))
    {
        return;
    }
    
    // 3. 序列化参数
    FArchive Payload = SerializeParameters(Function, Parameters);
    
    // 4. 根据类型分发
    switch (Function->FunctionFlags & FUNC_NetFuncFlags)
    {
        case FUNC_NetServer:
            // 转发到服务器
            ForwardToServer(Object, Function, Payload);
            break;
            
        case FUNC_NetClient:
            // 执行在客户端
            ExecuteOnClient(Object, Function, Payload);
            break;
            
        case FUNC_NetMulticast:
            // 广播给所有相关客户端
            BroadcastToClients(Object, Function, Payload);
            break;
    }
}
```

### 8.4 网络Tick流程

```cpp
// Engine.cpp - 网络Tick
void UEngine::TickNet(float DeltaSeconds)
{
    // 1. 更新所有网络驱动
    for (UNetDriver* Driver : NetDrivers)
    {
        if (Driver->ServerConnection)
        {
            // 客户端：处理服务器连接
            Driver->ProcessServerTravel();
        }
        else
        {
            // 服务器：复制Actor到客户端
            Driver->ServerReplicateActors(DeltaSeconds);
        }
    }
    
    // 2. 更新连接状态
    for (UNetConnection* Connection : ActiveConnections)
    {
        Connection->Flush();
    }
}
```

### 8.5 关键数据结构

#### FRepChangedPropertyTracker

跟踪属性变化：

```cpp
// 跟踪单个属性的变化
struct FRepChangedPropertyTracker
{
    TArray<uint16> ChangedProperties;  // 变化的属性索引
    TArray<uint8>  ShadowData;         // 阴影数据（用于比较）
    
    // 标记属性为脏
    void MarkPropertyDirty(uint16 RepIndex);
    
    // 检查属性是否变化
    bool IsPropertyDirty(uint16 RepIndex) const;
};
```

#### FOutBunch

网络数据打包：

```cpp
// 网络数据包
class FOutBunch
{
public:
    uint32 Bits;           // 数据位数
    uint8* Data;           // 数据指针
    int32 BufferSize;      // 缓冲区大小
    
    // 写入基本类型
    void WriteInt(int32 Value);
    void WriteFloat(float Value);
    void WriteVector(FVector Value);
    void WriteString(FString Value);
    
    // 压缩写入
    void WriteIntCompressed(int32 Value);  // 压缩整数
    void WriteVectorQuantized(FVector Value);  // 量化向量
};
```

---

## 9. Dedicated Server配置与部署

### 9.1 项目配置

#### Target.cs配置

```csharp
// MyProject.Target.cs
using UnrealBuildTool;

public class MyProjectTarget : TargetRules
{
    public MyProjectTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        
        // 包含服务器代码
        bBuildWithServerCode = true;
        
        // 是否作为服务器编译
        // 这个在Build.cs中控制
    }
}

// MyProjectServer.Target.cs - 专用服务器Target
public class MyProjectServerTarget : TargetRules
{
    public MyProjectServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;  // 服务器Target
        bIsBuildingServer = true;
    }
}
```

#### Build.cs配置

```csharp
// MyProject.Build.cs
using UnrealBuildTool;

public class MyProject : ModuleRules
{
    public MyProject(ReadOnlyTargetRules Target) : base(Target)
    {
        // ... 其他配置
        
        // 服务器专用代码
        if (Target.bBuildAllModules || Target.bCompileServerCode)
        {
            PublicIncludePaths.Add("MyProject/Server");
        }
    }
}
```

### 9.2 GameMode配置

```cpp
// 自定义GameMode
class AMyGameMode : public AGameModeBase
{
public:
    AMyGameMode();
    
    // 服务器初始化
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    
    // 玩家登录
    virtual void PostLogin(APlayerController* NewPlayer) override;
    
    // 处理玩家离开
    virtual void Logout(AController* Exiting) override;
    
    // 设置Pawn
    virtual void HandleSeamlessTravelPlayer(AController*& NewController) override;
};
```

```cpp
// GameMode实现
void AMyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    
    UE_LOG(LogMyGame, Log, TEXT("Player logged in: %s"), *NewPlayer->GetName());
    
    // 初始化玩家数据
    if (APlayerState* PS = NewPlayer->GetPlayerState<APlayerState>())
    {
        PS->SetScore(0);
        PS->SetPlayerName("Player");
    }
}

void AMyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    
    UE_LOG(LogMyGame, Log, TEXT("Player logged out: %s"), *Exiting->GetName());
}
```

### 9.3 GameState和PlayerState

```cpp
// 自定义GameState
class AMyGameState : public AGameStateBase
{
public:
    UPROPERTY(Replicated, ReplicatedUsing=OnRep_GameTime)
    float GameTime;

    UPROPERTY(Replicated)
    int32 AlivePlayerCount;

    UFUNCTION()
    void OnRep_GameTime();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMyGameState, GameTime);
    DOREPLIFETIME(AMyGameState, AlivePlayerCount);
}

// 自定义PlayerState
class AMyPlayerState : public APlayerState
{
public:
    UPROPERTY(Replicated, ReplicatedUsing=OnRep_Kills)
    int32 Kills;

    UPROPERTY(Replicated, ReplicatedUsing=OnRep_Deaths)
    int32 Deaths;

    UPROPERTY(Replicated)
    FString PlayerName;

    UFUNCTION()
    void OnRep_Kills();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

### 9.4 服务器启动参数

```bash
# 启动Dedicated Server
./MyProjectServer.exe /Game/Maps/Lobby?Listen -server -log

# 常用参数说明
# /Game/Maps/MapName  - 指定地图
# ?Listen             - 监听连接
# -server             - 指定为服务器
# -log                - 输出日志
# -Port=7777          - 指定端口
# -QueryPort=27015   - Steam查询端口
# -PlayerCount=10    - 最大玩家数
```

### 9.5 服务器编译

```bash
# 编译服务器版本（Windows）
# 方法1：使用UnrealBuildTool
UnrealBuildTool.exe MyProjectServer Win64 Development -Project="MyProject.uproject" -Build.cs

# 方法2：从VS编译
# 在Visual Studio中，选择 "Development Server" 配置

# 编译Linux服务器（交叉编译）
UnrealBuildTool.exe MyProjectServer Linux Development -Project="MyProject.uproject" -CrossCompile
```

---

## 10. 使用案例与最佳实践

### 10.1 玩家状态同步案例

```cpp
// 完整示例：玩家生命值和状态同步
// PlayerCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyPlayerCharacter.generated.h"

UCLASS()
class MYGAME_API AMyPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyPlayerCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    // 生命值属性
    UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadWrite)
    float Health;

    UPROPERTY(ReplicatedUsing=OnRep_MaxHealth)
    float MaxHealth;

    // 护盾属性（仅Owning Client可见）
    UPROPERTY(ReplicatedUsing=OnRep_Shield, Replicated)
    float Shield;

    // 武器列表
    UPROPERTY(Replicated)
    TArray<TSubclassOf<AWeapon>> AvailableWeapons;

    // 当前武器索引
    UPROPERTY(ReplicatedUsing=OnRep_CurrentWeaponIndex)
    int32 CurrentWeaponIndex;

public:
    // Server RPC：请求使用武器
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerEquipWeapon(int32 WeaponIndex);

    // Server RPC：请求开火
    UFUNCTION(Server, Reliable)
    void ServerFire();

    // Multicast：播放开火效果
    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayFireEffects();

    // Client RPC：显示伤害反馈
    UFUNCTION(Client, Reliable)
    void ClientShowDamageIndicator(float Damage);

    // 受伤函数
    float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* InstigatorController, AActor* DamageCauser) override;

private:
    UFUNCTION()
    void OnRep_Health();

    UFUNCTION()
    void OnRep_MaxHealth();

    UFUNCTION()
    void OnRep_Shield();

    UFUNCTION()
    void OnRep_CurrentWeaponIndex();

    void UpdateHealthHUD() const;
};
```

```cpp
// PlayerCharacter.cpp
#include "Player/MyPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Weapon.h"

AMyPlayerCharacter::AMyPlayerCharacter()
{
    // 启用复制
    bReplicates = true;
    bReplicateMovement = true;

    // 设置默认值
    MaxHealth = 100.0f;
    Health = MaxHealth;
    Shield = 0.0f;
    CurrentWeaponIndex = 0;
}

void AMyPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 基础生命值 - 复制到所有客户端
    DOREPLIFETIME(AMyPlayerCharacter, Health);
    DOREPLIFETIME(AMyPlayerCharacter, MaxHealth);
    
    // 护盾 - 仅Owner可见（敏感数据）
    DOREPLIFETIME_CONDITION(AMyPlayerCharacter, Shield, COND_OwnerOnly);
    
    // 武器列表 - 仅初始时复制
    DOREPLIFETIME_CONDITION(AMyPlayerCharacter, AvailableWeapons, COND_InitialOnly);
    
    // 当前武器索引 - 广播给所有人
    DOREPLIFETIME(AMyPlayerCharacter, CurrentWeaponIndex);
}

float AMyPlayerCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* InstigatorController, AActor* DamageCauser)
{
    if (GetLocalRole() != ROLE_Authority)
    {
        // 客户端不处理伤害，由服务器处理
        return 0.0f;
    }

    float ActualDamage = Super::TakeDamage(Damage, DamageEvent, InstigatorController, DamageCauser);

    // 应用伤害到护盾
    if (Shield > 0)
    {
        float ShieldDamage = FMath::Min(Shield, ActualDamage);
        Shield -= ShieldDamage;
        ActualDamage -= ShieldDamage;
    }

    // 应用伤害到生命值
    Health = FMath::Max(0.0f, Health - ActualDamage);

    // 通知所有客户端受伤（通过属性复制自动触发OnRep）
    if (Health <= 0)
    {
        // 玩家死亡处理
        HandleDeath(DamageEvent, InstigatorController, DamageCauser);
    }
    else if (InstigatorController && InstigatorController != GetController())
    {
        // 通知攻击者
        AMyPlayerCharacter* Attacker = Cast<AMyPlayerCharacter>(InstigatorController->GetPawn());
        if (Attacker)
        {
            // 伤害统计等
        }
    }

    return ActualDamage;
}

void AMyPlayerCharacter::ServerEquipWeapon_Implementation(int32 WeaponIndex)
{
    if (GetLocalRole() != ROLE_Authority)
    {
        return;
    }

    // 验证武器索引
    if (!AvailableWeapons.IsValidIndex(WeaponIndex))
    {
        return;
    }

    // 更换武器
    CurrentWeaponIndex = WeaponIndex;
    
    // 可选：销毁旧武器、生成新武器等
}

bool AMyPlayerCharacter::ServerEquipWeapon_Validate(int32 WeaponIndex)
{
    return AvailableWeapons.IsValidIndex(WeaponIndex);
}

void AMyPlayerCharacter::ServerFire_Implementation()
{
    if (GetLocalRole() != ROLE_Authority)
    {
        return;
    }

    // 检查是否可以开火
    if (!CanFire())
    {
        return;
    }

    // 执行开火逻辑（服务器权威）
    if (AWeapon* Weapon = GetCurrentWeapon())
    {
        Weapon->Fire();
    }

    // 广播效果给所有客户端
    MulticastPlayFireEffects();
}

void AMyPlayerCharacter::MulticastPlayFireEffects_Implementation()
{
    // 播放开火特效（所有客户端）
    if (FireEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(this, FireEffect, GetMesh()->GetSocketLocation("MuzzleSocket"));
    }

    // 播放开火声音
    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    }

    // 屏幕震动（仅本地控制者）
    if (IsLocallyControlled())
    {
        // 震动
    }
}

void AMyPlayerCharacter::OnRep_Health()
{
    UpdateHealthHUD();
}

void AMyPlayerCharacter::OnRep_MaxHealth()
{
    UpdateHealthHUD();
}

void AMyPlayerCharacter::OnRep_Shield()
{
    UpdateHealthHUD();
}

void AMyPlayerCharacter::OnRep_CurrentWeaponIndex()
{
    // 更新武器显示
    if (AWeapon* NewWeapon = GetCurrentWeapon())
    {
        // 绑定武器到角色
    }
}

void AMyPlayerCharacter::UpdateHealthHUD() const
{
    if (IsLocallyControlled())
    {
        // 更新本地HUD
        // 显示生命值和护盾
    }
}
```

### 10.2 项目物同步案例

```cpp
// Projectile.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyProjectile.generated.h"

UCLASS()
class MYGAME_API AMyProjectile : public AActor
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UProjectileMovementComponent* MovementComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(Replicated, ReplicatedUsing=OnRep_Exploded)
    bool bExploded;

    UPROPERTY(Replicated)
    float Damage;

    UPROPERTY(Replicated)
    AActor* OwnerActor;

public:
    AMyProjectile();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void Tick(float DeltaTime) override;

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
    UFUNCTION()
    void OnRep_Exploded();

    void Explode();
};

UCLASS()
class MYGAME_API AMyGrenade : public AMyProjectile
{
    GENERATED_BODY()

public:
    UPROPERTY(Replicated, ReplicatedUsing=OnRep_BounceCount)
    int32 BounceCount;

    UPROPERTY(Replicated)
    FVector InitialVelocity;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void NotifyHit(FVector NormalImpulse, const FHitResult& HitResult) override;

private:
    UFUNCTION()
    void OnRep_BounceCount();
};
```

```cpp
// Projectile.cpp
#include "Weapons/MyProjectile.h"
#include "Components/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

AMyProjectile::AMyProjectile()
{
    bReplicates = true;
    bReplicateMovement = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = MeshComponent;

    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
    MovementComponent->InitialSpeed = 3000.0f;
    MovementComponent->MaxSpeed = 4000.0f;

    Damage = 50.0f;
    bExploded = false;
}

void AMyProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMyProjectile, bExploded);
    DOREPLIFETIME(AMyProjectile, Damage);
    DOREPLIFETIME(AMyProjectile, OwnerActor);
}

void AMyProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 服务器端处理
    if (GetLocalRole() == ROLE_Authority)
    {
        // 检查是否超时（防止无限飞行）
        if (GetActorLocation().Z < -10000.0f)
        {
            Destroy();
        }
    }
}

void AMyProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
    // 服务器处理碰撞
    if (GetLocalRole() == ROLE_Authority && !bExploded)
    {
        Explode();
    }
}

void AMyProjectile::Explode()
{
    if (bExploded)
    {
        return;
    }

    bExploded = true;  // 自动复制到所有客户端

    // 造成伤害
    if (OwnerActor)
    {
        UGameplayStatics::ApplyRadialDamage(
            this,
            Damage,
            GetActorLocation(),
            500.0f,
            UDamageType::StaticClass(),
            TArray<AActor*>(),
            OwnerActor,
            GetController()
        );
    }

    // 播放特效
    MulticastPlayExplosionEffects();
}

void AMyProjectile::MulticastPlayExplosionEffects_Implementation()
{
    if (ExplosionEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation());
    }

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
    }
}

void AMyProjectile::OnRep_Exploded()
{
    // 客户端：爆炸效果已经通过Multicast播放
    // 这里可以处理本地特有的效果
    
    if (!HasAuthority())
    {
        // 屏幕震动
    }
}
```

### 10.3 物品和库存系统案例

```cpp
// InventoryComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyInventoryComponent.generated.h"

USTRUCT()
struct FInventoryItem
{
    GENERATED_BODY()

    UPROPERTY()
    int32 ItemID;

    UPROPERTY()
    int32 Quantity;

    UPROPERTY()
    FString ItemName;

    UPROPERTY()
    UTexture2D* Icon;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYGAME_API UMyInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(Replicated, ReplicatedUsing=OnRep_Items)
    TArray<FInventoryItem> Items;

    UPROPERTY(Replicated, ReplicatedUsing=OnRep_Gold)
    int32 Gold;

public:
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool AddItem(int32 ItemID, int32 Quantity);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool RemoveItem(int32 ItemID, int32 Quantity);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool AddGold(int32 Amount);

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerAddItem(int32 ItemID, int32 Quantity);

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRemoveItem(int32 ItemID, int32 Quantity);

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UFUNCTION()
    void OnRep_Items();

    UFUNCTION()
    void OnRep_Gold();
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYGAME_API UMyEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(Replicated)
    TArray<int32> EquippedItems;

    UPROPERTY(Replicated)
    int32 CurrentWeaponIndex;

    UPROPERTY(Replicated)
    int32 CurrentArmorIndex;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
```

```cpp
// InventoryComponent.cpp
#include "Components/MyInventoryComponent.h"
#include "Net/UnrealNetwork.h"

void UMyInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(UMyInventoryComponent, Items);
    DOREPLIFETIME(UMyInventoryComponent, Gold);
}

bool UMyInventoryComponent::AddItem(int32 ItemID, int32 Quantity)
{
    if (GetOwner()->GetLocalRole() != ROLE_Authority)
    {
        // 客户端尝试添加 - 通过RPC处理
        ServerAddItem(ItemID, Quantity);
        return true;
    }

    // 查找现有物品
    for (FInventoryItem& Item : Items)
    {
        if (Item.ItemID == ItemID)
        {
            Item.Quantity += Quantity;
            return true;
        }
    }

    // 新物品
    FInventoryItem NewItem;
    NewItem.ItemID = ItemID;
    NewItem.Quantity = Quantity;
    Items.Add(NewItem);

    return true;
}

void UMyInventoryComponent::ServerAddItem_Implementation(int32 ItemID, int32 Quantity)
{
    // 服务器验证
    if (GetOwner()->GetLocalRole() != ROLE_Authority)
    {
        return;
    }

    // 添加物品（通过属性复制自动同步）
    AddItem(ItemID, Quantity);
}

bool UMyInventoryComponent::ServerAddItem_Validate(int32 ItemID, int32 Quantity)
{
    // 验证
    return Quantity > 0 && Quantity <= 1000;
}

bool UMyInventoryComponent::RemoveItem(int32 ItemID, int32 Quantity)
{
    if (GetOwner()->GetLocalRole() != ROLE_Authority)
    {
        return false;
    }

    for (int32 i = Items.Num() - 1; i >= 0; i--)
    {
        if (Items[i].ItemID == ItemID)
        {
            Items[i].Quantity -= Quantity;
            if (Items[i].Quantity <= 0)
            {
                Items.RemoveAt(i);
            }
            return true;
        }
    }

    return false;
}
```

### 10.4 技能系统同步案例

```cpp
// SkillComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MySkillComponent.generated.h"

UENUM(BlueprintType)
enum class ESkillState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Casting     UMETA(DisplayName = "Casting"),
    Cooldown    UMETA(DisplayName = "Cooldown")
};

USTRUCT()
struct FSkillInfo
{
    GENERATED_BODY()

    UPROPERTY()
    int32 SkillID;

    UPROPERTY()
    ESkillState State;

    UPROPERTY()
    float CooldownRemaining;

    UPROPERTY()
    float CastTimeRemaining;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYGAME_API UMySkillComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(Replicated, ReplicatedUsing=OnRep_Skills)
    TArray<FSkillInfo> Skills;

    UPROPERTY(Replicated, ReplicatedUsing=OnRep_ActiveSkillIndex)
    int32 ActiveSkillIndex;

public:
    UFUNCTION(BlueprintCallable, Category="Skill")
    void UseSkill(int32 SkillIndex);

    UFUNCTION(BlueprintCallable, Category="Skill")
    float GetCooldown(int32 SkillIndex) const;

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerUseSkill(int32 SkillIndex);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlaySkillEffects(int32 SkillIndex);

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UFUNCTION()
    void OnRep_Skills();

    UFUNCTION()
    void OnRep_ActiveSkillIndex();

    void ExecuteSkill(int32 SkillIndex);
    void ApplySkillEffects(int32 SkillIndex, AActor* Target);
};
```

```cpp
// SkillComponent.cpp
#include "Components/MySkillComponent.h"
#include "Net/UnrealNetwork.h"

void UMySkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UMySkillComponent, Skills);
    DOREPLIFETIME(UMySkillComponent, ActiveSkillIndex);
}

void UMySkillComponent::UseSkill(int32 SkillIndex)
{
    // 客户端检查
    if (!Skills.IsValidIndex(SkillIndex))
    {
        return;
    }

    if (Skills[SkillIndex].State != ESkillState::Idle)
    {
        return;
    }

    // 调用Server RPC
    ServerUseSkill(SkillIndex);
}

void UMySkillComponent::ServerUseSkill_Implementation(int32 SkillIndex)
{
    // 服务器验证
    if (!Skills.IsValidIndex(SkillIndex))
    {
        return;
    }

    if (GetOwner()->GetLocalRole() != ROLE_Authority)
    {
        return;
    }

    // 检查冷却
    if (Skills[SkillIndex].State != ESkillState::Idle)
    {
        return;
    }

    // 开始技能
    ExecuteSkill(SkillIndex);
}

bool UMySkillComponent::ServerUseSkill_Validate(int32 SkillIndex)
{
    // 可以添加更多验证
    return Skills.IsValidIndex(SkillIndex);
}

void UMySkillComponent::ExecuteSkill(int32 SkillIndex)
{
    FSkillInfo& Skill = Skills[SkillIndex];
    Skill.State = ESkillState::Casting;
    ActiveSkillIndex = SkillIndex;

    // 广播技能开始效果
    MulticastPlaySkillEffects(SkillIndex);

    // 延迟执行技能效果（在服务器）
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindUObject(this, &UMySkillComponent::ApplySkillEffects, SkillIndex, nullptr);
    
    GetWorld()->GetTimerManager().SetTimer(
        SkillTimerHandle,
        TimerDelegate,
        1.0f,  // 技能施法时间
        false
    );
}

void UMySkillComponent::ApplySkillEffects(int32 SkillIndex, AActor* Target)
{
    // 服务器执行技能效果
    FSkillInfo& Skill = Skills[SkillIndex];
    Skill.State = ESkillState::Cooldown;
    Skill.CooldownRemaining = 10.0f;  // 冷却时间

    // 应用技能逻辑...
    
    // 广播技能完成效果
    MulticastPlaySkillEffects(SkillIndex);
}

void UMySkillComponent::MulticastPlaySkillEffects_Implementation(int32 SkillIndex)
{
    // 播放视觉和音效效果
    // 所有客户端执行
}
```

---

## 11. 性能优化与调试

### 11.1 网络性能优化技巧

#### 属性复制优化

```cpp
// 1. 使用量化类型减少带宽
UPROPERTY(Replicated)
FVector_NetQuantize100 Position;  // 位置 - 两位小数精度

UPROPERTY(Replicated)
FVector_NetQuantizeNormal Normal;  // 法线 - 标准化向量

// 2. 使用复制条件减少不必要的数据
UPROPERTY(ReplicatedUsing=OnRep_PrivateData, Replicated)
FString PrivateData;  // 使用COND_OwnerOnly

// 3. 合并小属性
USTRUCT()
struct FCombinedStats
{
    GENERATED_BODY()
    
    UPROPERTY()
    float Health;
    
    UPROPERTY()
    float Mana;
    
    UPROPERTY()
    float Stamina;
};

// 代替三个单独的属性
UPROPERTY(Replicated)
FCombinedStats CombinedStats;
```

#### 复制频率优化

```cpp
// MyActor.cpp
void AMyActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    // 高频更新属性
    DOREPLIFETIME(AMyActor, Position);
    
    // 低频更新属性（使用条件）
    DOREPLIFETIME_CONDITION(AMyActor, Stats, COND_InitialOnly);
}

// 动态调整更新频率
void AMyActor::SetReplicationFrequency(float Frequency)
{
    // 更改NetUpdateFrequency
    NetUpdateFrequency = Frequency;
}
```

### 11.2 网络调试命令

```bash
# 列出网络统计
netstats

# 显示详细网络信息
netshow all

# 冻结网络模拟
netfreeze

# 网络模拟延迟
nettimeout 100  # 100ms延迟

# 显示带宽使用
stat net

# 详细复制信息
log LogNet player=1

# 启用复制调试
net.DevForceAllChannelPackets 1
```

### 11.3 网络分析工具

```cpp
// 代码中添加网络调试
void AMyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    #if WITH_EDITOR
    if (GEngine->bOnScreenDebugMessagesEnabled)
    {
        FString DebugInfo = FString::Printf(TEXT("Role: %d, RemoteRole: %d"), 
            (int32)GetLocalRole(), (int32)GetRemoteRole());
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, DebugInfo);
    }
    #endif
}
```

### 11.4 常见问题与解决方案

| 问题 | 原因 | 解决方案 |
|------|------|---------|
| RPC未执行 | 网络模式不正确 | 检查GetLocalRole() |
| 属性不复制 | 未调用GetLifetimeReplicatedProps | 实现该函数 |
| 客户端崩溃 | 在Server RPC中访问空指针 | 添加空指针检查 |
| 延迟高 | 复制频率太高 | 降低NetUpdateFrequency |
| 带宽高 | 复制了不必要的数据 | 使用复制条件 |

### 11.5 网络断言和检查

```cpp
// 在关键位置添加网络检查
void AMyCharacter::ServerFire_Implementation()
{
    // 强制检查 - 只在开发版本有效
    checkf(GetLocalRole() == ROLE_AutonomousProxy, TEXT("ServerFire called on non-client!"));
    
    // 运行检查
    if (!HasAuthority())
    {
        UE_LOG(LogMyGame, Error, TEXT("Unauthorized call to ServerFire"));
        return;
    }
    
    // 检查Pawn有效性
    if (!IsValid(this))
    {
        return;
    }
    
    // 执行开火逻辑
}
```

---

## 12. Iris复制系统（UE5新特性）

### 12.1 Iris概述

Iris是UE5引入的全新复制系统，旨在解决传统复制系统的性能瓶颈：

- **更低的CPU开销**
- **更少的内存使用**
- **更好的可扩展性**
- **支持更多并发连接**

### 12.2 Iris配置

```cpp
// 在ProjectSettings中启用Iris
// Project Settings > Physics > Replication > Enable Iris

// 或通过命令行
// -iris

// 代码中检查Iris是否启用
#if WITH_IRIS
    // Iris特定代码
#endif
```

### 12.3 使用Iris进行复制

```cpp
// Iris复制配置
class AMyActor : public AActor
{
public:
    // Iris特定配置
    UPROPERTY(EditAnywhere, Category = "Replication|Iris")
    uint8 bUseIrisReplication : 1;

    // 优先级（Iris使用）
    UPROPERTY(EditAnywhere, Category = "Replication|Iris", meta = (ClampMin = "0.0", ClampMax = "10.0"))
    float IrisReplicationPriority;

protected:
    virtual void SetupIrisReplication() override;
};
```

### 12.4 Replication Graph with Iris

```cpp
// 使用Replication Graph
class UMyReplicationGraph : public UReplicationGraph
{
public:
    virtual void ResetGameWorldState() override;
    virtual void OnWorldBeginPlay() override;
    
protected:
    virtual void OnAddNetworkActor(const FNewReplicatedActorInfo& ActorInfo, FActorReplicationInfo& OutActorInfo) override;
    virtual void OnRemoveNetworkActor(const FNewReplicatedActorInfo& ActorInfo) override;
};
```

---

## 附录

### A. 常用宏和函数参考

```cpp
// 复制相关
DOREPLIFETIME(ClassName, PropertyName)
DOREPLIFETIME_CONDITION(ClassName, PropertyName, Condition)
DOREPLIFETIME_WITH_PARAMS(ClassName, PropertyName, bRefetch, bForce)

// 角色检查
HasAuthority()                      // 是否为服务器
GetLocalRole()                       // 获取本地角色
GetRemoteRole()                     // 获取远程角色
IsLocallyControlled()                // 是否本地控制

// 网络模式
GetNetMode()                         // 获取网络模式
IsNetMode(NM_DedicatedServer)        // 是否为Dedicated Server

// RPC
GetNetConnection()                   // 获取网络连接
IsReplicationPausedForConnection()   // 复制是否暂停
```

### B. 网络相关Console变量

```bash
// 复制相关
net.RepPayloadThreshold 42              // 有效载荷阈值
net.IrrelevantTimeout 1.5               // 无关联超时
net.DelayTimeout 30.0                    // 延迟超时
net.MaxClients 100                       // 最大客户端数
net.MinNetUpdateFrequency 2.0            // 最小更新频率

// 带宽相关
net.MaxPacketSize 1280                  // 最大数据包大小
net.UnattributedQuality 0                // 未分配质量

// 调试
net.PacketHandlerStats 0                // 数据包处理器统计
net.VerifyReplication 1                  // 验证复制

// Iris (UE5+)
iris.AssemblyDir "Iris/Assembly"          // 程序集目录
iris.Concurrency 4                       // 并发数
```

### C. 参考资料

- [Unreal Engine Networking and Multiplayer Documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/networking-and-multiplayer-in-unreal-engine)
- [Multiplayer Programming Quick Start](https://dev.epicgames.com/documentation/en-us/unreal-engine/multiplayer-programming-quick-start-for-unreal-engine)
- [Actor Replication](https://dev.epicgames.com/documentation/en-us/unreal-engine/actor-replication)
- [RPCs](https://dev.epicgames.com/documentation/en-us/unreal-engine/rpcs)
- [Property Replication](https://dev.epicgames.com/documentation/en-us/unreal-engine/property-replication)
- [Iris Replication System](https://dev.epicgames.com/documentation/en-us/unreal-engine/iris-replication-system)
- [Replication Graph](https://dev.epicgames.com/documentation/en-us/unreal-engine/replication-graph)
- [Setting Up Dedicated Servers](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-dedicated-servers)

---

*文档版本：UE 5.5*
*最后更新：2026年3月*
