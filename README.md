# UE5 C++ Gameplay Demo

基于 **Unreal Engine 5.7 和 C++** 开发的第三人称多人 Gameplay Demo。

项目围绕武器拾取、射击战斗、AI 敌人、钥匙开门、生命与死亡、胜利和重开构建基础玩法流程，重点展示 **C++ Gameplay 开发、组件化设计、数据驱动武器和服务器权威网络同步**。

核心 Gameplay 逻辑使用 C++ 实现，Blueprint 用于资源配置、角色外观及 UMG 界面布局与绑定。

[观看演示视频](https://www.bilibili.com/video/BV1K7eF6hEMy/?share_source=copy_web&vd_source=ac628cef45f9e52f29306cbd29b51d36) · [下载 Windows 版本](https://pan.baidu.com/s/1G41hrr3aLIcJnwJpqF2B9Q?pwd=1234)

## Gameplay

- 使用第三人称角色移动、跳跃、冲刺和探索场景。
- 拾取场景中的武器，射击敌人并消耗弹药。
- 拾取钥匙后，全队共享开门权限。
- 敌人执行巡逻、视觉感知、追击和近距离攻击。
- 玩家死亡后显示重开界面；清空敌人后向各客户端显示胜利界面。
- 通过服务器统一重载关卡，重新开始游戏。

## Features

### 数据驱动武器

通过 `UWeaponDataAsset` 配置武器，区分静态配置与运行时状态。

| 配置项 | 用途 |
| --- | --- |
| `GunName` | 武器名称 |
| `Damage` | 单次命中伤害 |
| `MaxRange` | 射击距离 |
| `MagazineSize` | 弹匣容量 |
| `GunMesh` | 武器模型 |

- 提供手枪与步枪的 DataAsset 配置示例。
- `AWeaponBase` 读取配置，初始化模型和弹药。
- 武器实现 `IInteractable`，接入统一交互流程。
- `UWeaponComponent` 管理当前武器引用与角色挂点。
- 服务器完成武器装备，设置归属并附着到角色。
- 射击时读取当前武器的伤害与射程，并扣除该武器实例的弹药。
- 未装备武器或弹药耗尽时无法射击。
- 弹药 UI 显示“当前弹药 / 弹匣容量”，未持枪时显示“未装备”。

### AI 与战斗

- 使用 `AIController`、导航移动和 Timer 实现定点巡逻与停留。
- 使用 `AI Perception` 视觉感知发现玩家。
- 根据目标距离执行追击或近距离攻击，通过 Timer 控制攻击冷却。
- 当前目标丢失或被判断为死亡时，尝试寻找其他可见且存活的玩家。
- 没有可用目标时恢复巡逻。
- 玩家和敌人共用 `UHealthComponent`，统一处理伤害、生命值和死亡事件。
- 使用动态多播委托通知血条更新与死亡表现。
- 玩家和敌人死亡后进入 Ragdoll 状态。

### 交互与界面

- 使用射线检测选择交互目标并显示提示。
- 武器、钥匙与门通过 `IInteractable` 提供统一交互入口。
- 提供血条、准星、交互提示、弹药、死亡和胜利界面。
- 使用 `IsLocallyControlled()` 与 `IsLocalController()` 区分本地输入、界面和网络副本逻辑。

## Networking

核心玩法采用 **Server Authoritative** 模型：客户端提交操作请求，服务器执行关键判定并同步结果。

### 射击与伤害

客户端通过 `ServerShoot` 提交射击起点与方向，服务器进行基础检查：

- 角色是否存活。
- 射击方向是否为非零向量，并进行标准化处理。
- 射击起点是否位于服务器角色附近的允许范围内。
- 当前武器是否有效且属于该角色。
- 武器配置、射程与剩余弹药是否满足射击条件。

通过检查后，服务器扣除弹药、执行射线检测，并使用武器配置中的伤害调用 `ApplyDamage`。

射击结果与表现分别处理：

- **生命值与死亡状态**：通过属性复制同步。
- **射线与命中位置的调试表现**：通过 `NetMulticast` 同步，在非 Shipping 构建中显示。

### 武器同步

- 武器 Actor 开启网络复制。
- `CurrentWeapon` 复制当前装备的武器引用。
- `CurrentAmmo` 由服务器初始化，并在服务器射击流程中扣除后复制到客户端。
- `bIsEquipped` 使用 `ReplicatedUsing` 同步装备状态。
- 客户端通过 `OnRep_Equipped()` 关闭已装备武器的碰撞与物理模拟，避免武器与角色模型重叠时干扰移动。
- 弹药 UI 从当前武器实例读取剩余弹药，并结合 DataAsset 中的弹匣容量显示文本。

### 交互、钥匙与门

- 客户端通过 `ServerInteract` 请求交互。
- 服务器检查角色存活状态、目标、接口与交互距离。
- 任意玩家拾取钥匙后，服务器更新 `AMyGameStateBase::bTeamHasDoorKey`，全队共享开门权限。
- 钥匙由服务器销毁，并同步 Actor 销毁结果。
- 服务器检查队伍钥匙权限后切换门状态，同步开关状态与门轴旋转。

### 生命、胜利与重开

- `UHealthComponent` 在服务器处理伤害并修改 `CurrentHealth`、`bIsDead`。
- 客户端通过 `OnRep` 广播生命值变化与死亡事件，更新界面和角色表现。
- `GameMode` 在服务器统计剩余敌人，清空后通过各玩家的 `ClientVictory` 显示胜利界面。
- 重开按钮调用 `ServerRestart`，由服务器通过 `ServerTravel` 统一重载关卡。

### 移动与冲刺

- 玩家与敌人使用角色移动及网络移动复制。
- 冲刺输入先在本地修改移动速度，再通过 `ServerSprint` 同步服务器速度，减少输入等待。

## Core Architecture

| 类 | 职责 |
| --- | --- |
| `AMyCharacter` | 玩家移动、射击与交互执行、Server RPC、角色死亡表现 |
| `AMyPlayerController` | Enhanced Input 绑定、本地 UI、弹药文本查询、胜利通知与重开请求 |
| `UWeaponDataAsset` | 武器名称、伤害、射程、弹匣容量与模型配置 |
| `AWeaponBase` | 武器实例、配置读取、弹药消耗、装备与装备状态同步 |
| `UWeaponComponent` | 当前武器引用、拾取入口与武器挂点管理 |
| `UHealthComponent` | 伤害处理、生命值与死亡状态复制、事件广播 |
| `AEnemyAIController` | 巡逻、视觉感知、追击、目标切换与攻击冷却 |
| `AEnemyCharacter` | 敌人攻击、死亡表现与敌人死亡通知 |
| `AMyGameModeBase` | 服务器敌人数量统计与胜利判定 |
| `AMyGameStateBase` | 全队共享钥匙状态复制 |
| `IInteractable` / `ADoorKey` / `ADoor` | 统一交互接口、钥匙拾取与开门 |
| `URestartWidget` | 重开按钮绑定与服务器重开请求入口 |

主要代码入口：

- [角色与射击流程](Source/JobDemo/Private/Character/MyCharacter.cpp)
- [武器配置](Source/JobDemo/Public/Weapons/WeaponDataAsset.h)
- [武器实例](Source/JobDemo/Private/Weapons/WeaponBase.cpp)
- [武器组件](Source/JobDemo/Private/Weapons/WeaponComponent.cpp)
- [AI 控制器](Source/JobDemo/Private/AI/EnemyAIController.cpp)
- [生命组件](Source/JobDemo/Private/Components/HealthComponent.cpp)
- [玩家控制器与 UI](Source/JobDemo/Private/Core/MyPlayerController.cpp)

## Controls

| 输入 | 功能 |
| --- | --- |
| WASD | 移动 |
| 鼠标 | 控制视角 |
| Space | 跳跃 |
| Shift | 冲刺 |
| F | 交互：拾取武器、拾取钥匙、开关门 |
| 鼠标左键 | 射击 |

## Getting Started

开发环境：

- Unreal Engine 5.7
- C++
- Visual Studio
- Windows

运行源码：

1. 克隆或下载仓库。
2. 为 `JobDemo.uproject` 生成 Visual Studio 项目文件。
3. 使用 `Development Editor / Win64` 配置编译。
4. 打开 `JobDemo.uproject`，进入 `/Game/Maps/NewWorld` 地图。
5. 点击 Play 运行。

编辑器多人运行：

1. 在 Play 设置中将玩家数量设为 `2`。
2. 将网络模式设为 `Play As Listen Server`。
3. 使用独立游戏窗口运行，分别操作主机与客户端。

可检查武器拾取与弹药显示、射击伤害、队伍钥匙共享、门状态、AI 目标切换、死亡、胜利和重开流程。

## Performance

此前 Windows Development 打包版本的场景性能记录：

| 项目 | 记录 |
| --- | --- |
| CPU | Intel i5-12600KF |
| GPU | NVIDIA RTX 2080 Ti |
| 内部渲染分辨率 | 约 1400 × 788 |
| Frame | 约 4.71 ms |
| Game Thread | 约 1.44 ms |
| Draw Thread | 约 4.70 ms |
| GPU Time | 约 4.13 ms |
| Draw Calls | 约 291 |

以上为此前测试场景的性能基线，不代表最新源码的重新测量结果。

## Current Scope

当前武器系统支持单武器拾取、装备、射击、弹药消耗及 UI 显示，暂未加入换弹、武器切换和射速控制。

多人重开采用服务器统一重载关卡的方式。

## Screenshot

![Gameplay](Docs/Images/gameplay.png)
