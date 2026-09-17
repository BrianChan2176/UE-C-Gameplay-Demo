# UE5 C++基础网络肉鸽闯关 Gameplay Demo

基于 **Unreal Engine 5.7 和 C++** 开发的第三人称多人 Gameplay Demo。

项目围绕武器拾取、射击战斗、AI 敌人、钥匙开门、生命与死亡、多人闯关、局内成长、胜利和重开构建基础玩法流程，重点展示 **C++ Gameplay 开发、组件化设计、数据驱动武器、AI 行为以及服务器权威网络同步**。

核心 Gameplay 逻辑使用 C++ 实现，Blueprint 用于资源配置、角色外观及 UMG 界面布局与绑定。

[观看演示视频](https://www.bilibili.com/video/BV1MeeG6MEfL/?share_source=copy_web&vd_source=ac628cef45f9e52f29306cbd29b51d36) · [下载 Windows 版本](https://pan.baidu.com/s/1cF-q1w1QnbsT1ex26_Ha6g?pwd=1234)

## Gameplay

* 使用第三人称角色移动、跳跃、冲刺和探索场景。
* 拾取场景中的武器，射击敌人并消耗弹药。
* 拾取钥匙后，全队共享开门权限。
* 敌人执行固定点巡逻或随机巡逻、视觉感知、追击和近距离攻击。
* 每关可随机生成一定数量的敌人。
* 当前关卡敌人全部死亡后，由服务器推进至下一关。
* 支持 `NewWorld → LevelTwo → LevelThree` 三关连续闯关流程。
* 每通过一关，玩家本局伤害倍率提升。
* 第一、第二关完成后向各客户端显示通关奖励提示。
* 第三关完成后向各客户端显示最终胜利界面。
* 玩家死亡后显示重开界面。
* 通过服务器统一重载关卡，重新开始游戏并重置本局成长。

## Features

### 多人闯关与局内成长

在原有多人战斗流程基础上加入三关连续闯关和简单的 Roguelite 局内成长机制。

关卡流程：

`NewWorld → LevelTwo → LevelThree → Victory`

* `GameMode` 在服务器统计当前关卡剩余敌人。
* 当前关卡敌人全部死亡后，根据当前地图决定下一阶段。
* 第一关结束后通过 `ServerTravel` 进入 `LevelTwo`。
* 第二关结束后进入 `LevelThree`。
* 第三关结束后遍历所有 PlayerController，通过 Client RPC 向各客户端显示最终胜利 UI。
* 多人关卡切换由服务器统一控制，保证所有玩家进入相同关卡。

### 随机敌人生成与巡逻

新增 `AAISpawner` 用于关卡中的随机敌人生成。

* 每局在指定 `BoxComponent` 范围内随机生成敌人。
* 当前生成数量范围为 `3 \~ 6`。
* 随机选择生成区域内的 X / Y 坐标。
* 通过向下射线检测地面位置后生成 AI。
* 新生成敌人继续使用原有 AI 感知、追击和攻击逻辑。

AI 同时支持两种巡逻方式：

* 如果配置了巡逻点，则按照 Patrol Points 定点巡逻。
* 如果没有配置巡逻点，则通过 NavMesh 使用 `GetRandomReachablePointInRadius` 随机寻找可达位置巡逻。
* 当 AI 进入追击状态后停止随机巡逻逻辑。
* 丢失目标或没有存活目标时恢复巡逻状态。

### 局内伤害成长

新增自定义 `UMyGameInstance` 保存跨关卡的本局成长数据。

初始伤害倍率：

`DamageMultiplier = 1.0`

每通过一关：

`DamageMultiplier += 1.2`

实际射击伤害：

`FinalDamage = WeaponDamage × DamageMultiplier`

例如步枪基础伤害为 `30`：

|关卡|伤害倍率|实际伤害|
|-|-:|-:|
|第一关|1.0|30|
|第二关|2.2|66|
|第三关|3.4|102|

由于 `GameInstance` 在关卡切换过程中不会重新创建，因此可以保存当前 Run 的成长状态。

玩家重新开始游戏时调用 `ResetRun()`，将伤害倍率恢复为 `1.0`。

### 通关奖励反馈

第一关和第二关完成后，服务器向所有玩家发送通关奖励提示。

* `GameMode` 遍历当前所有 PlayerController。
* 通过 `ClientReward` Client RPC 通知对应客户端。
* 本地 `PlayerController` 显示 `RewardWidget`。
* Reward UI 显示约 `2.5` 秒后自动隐藏。
* 第三关结束后不再显示过关提示，而是进入最终 Victory UI。

当前 Reward UI 主要用于表现局内成长结果，成长逻辑目前为固定伤害倍率提升。

### 数据驱动武器

通过 `UWeaponDataAsset` 配置武器，区分静态配置与运行时状态。

|配置项|用途|
|-|-|
|`GunName`|武器名称|
|`Damage`|单次命中基础伤害|
|`MaxRange`|射击距离|
|`MagazineSize`|弹匣容量|
|`GunMesh`|武器模型|

* 提供手枪与步枪的 DataAsset 配置示例。
* `AWeaponBase` 读取配置，初始化模型和弹药。
* 武器实现 `IInteractable`，接入统一交互流程。
* `UWeaponComponent` 管理当前武器引用与角色挂点。
* 服务器完成武器装备，设置归属并附着到角色。
* 射击时读取当前武器的伤害与射程，并扣除该武器实例的弹药。
* 基础武器伤害会结合当前 Run 的 `DamageMultiplier` 计算最终伤害。
* 未装备武器或弹药耗尽时无法射击。
* 弹药 UI 显示“当前弹药 / 弹匣容量”，未持枪时显示“未装备”。

### AI 与战斗

* 使用 `AIController`、导航移动和 Timer 实现定点巡逻与随机巡逻。
* 使用 `AI Perception` 视觉感知发现玩家。
* 根据目标距离执行追击或近距离攻击，通过 Timer 控制攻击冷却。
* 当前目标丢失或被判断为死亡时，尝试寻找其他可见且存活的玩家。
* 没有可用目标时恢复巡逻。
* 玩家和敌人共用 `UHealthComponent`，统一处理伤害、生命值和死亡事件。
* 使用动态多播委托通知血条更新与死亡表现。
* 玩家和敌人死亡后进入 Ragdoll 状态。
* AI 可由关卡中的 `AAISpawner` 动态生成。

### 交互与界面

* 使用射线检测选择交互目标并显示提示。
* 武器、钥匙与门通过 `IInteractable` 提供统一交互入口。
* 提供血条、准星、交互提示、弹药、通关奖励、死亡和胜利界面。
* 使用 `IsLocallyControlled()` 与 `IsLocalController()` 区分本地输入、界面和网络副本逻辑。
* 第一、第二关完成后显示 Reward UI。
* 第三关完成后显示最终 Victory UI。

## Networking

核心玩法采用 **Server Authoritative** 模型：客户端提交操作请求，服务器执行关键判定并同步结果。

### 射击与伤害

客户端通过 `ServerShoot` 提交射击起点与方向，服务器进行基础检查：

* 角色是否存活。
* 射击方向是否为非零向量，并进行标准化处理。
* 射击起点是否位于服务器角色附近的允许范围内。
* 当前武器是否有效且属于该角色。
* 武器配置、射程与剩余弹药是否满足射击条件。

通过检查后，服务器扣除弹药、执行射线检测。

基础伤害读取当前武器 DataAsset：

`WeaponDataAsset->Damage`

然后结合 `GameInstance` 保存的局内成长倍率计算最终伤害：

`FinalDamage = WeaponDataAsset->Damage × DamageMultiplier`

最终由服务器使用 `ApplyDamage` 处理伤害。

射击结果与表现分别处理：

* **生命值与死亡状态**：通过属性复制同步。
* **射线与命中位置的调试表现**：通过 `NetMulticast` 同步，在非 Shipping 构建中显示。

### 武器同步

* 武器 Actor 开启网络复制。
* `CurrentWeapon` 复制当前装备的武器引用。
* `CurrentAmmo` 由服务器初始化，并在服务器射击流程中扣除后复制到客户端。
* `bIsEquipped` 使用 `ReplicatedUsing` 同步装备状态。
* 客户端通过 `OnRep\_Equipped()` 关闭已装备武器的碰撞与物理模拟，避免武器与角色模型重叠时干扰移动。
* 弹药 UI 从当前武器实例读取剩余弹药，并结合 DataAsset 中的弹匣容量显示文本。

### 交互、钥匙与门

* 客户端通过 `ServerInteract` 请求交互。
* 服务器检查角色存活状态、目标、接口与交互距离。
* 任意玩家拾取钥匙后，服务器更新 `AMyGameStateBase::bTeamHasDoorKey`，全队共享开门权限。
* 钥匙由服务器销毁，并同步 Actor 销毁结果。
* 服务器检查队伍钥匙权限后切换门状态，同步开关状态与门轴旋转。

### 生命、关卡推进与胜利

* `UHealthComponent` 在服务器处理伤害并修改 `CurrentHealth`、`bIsDead`。
* 客户端通过 `OnRep` 广播生命值变化与死亡事件，更新界面和角色表现。
* `GameMode` 在服务器统计当前关卡剩余敌人。
* 关卡开始后延迟统计敌人数量，等待随机生成流程完成。
* 敌人死亡时通知 `GameMode` 更新 `RemainingEnemies`。
* 当前关卡敌人清空后由服务器判断当前地图。
* 第一关通过 `ServerTravel` 进入第二关。
* 第二关通过 `ServerTravel` 进入第三关。
* 第三关完成后通过各玩家的 `ClientVictory` 显示最终胜利界面。
* 第一、第二关切换前通过 `ClientReward` 显示过关奖励提示。

### 多人关卡切换与重开

多人关卡流程由服务器统一控制：

`NewWorld → LevelTwo → LevelThree`

使用：

`GetWorld()->ServerTravel(...)`

完成服务器和客户端统一关卡切换。

重开按钮调用 `ServerRestart`：

* 服务器将 `GameInstance` 中的局内伤害倍率重置为 `1.0`。
* 服务器通过 `ServerTravel` 返回 `NewWorld`。
* 开始新的 Run。

### 移动与冲刺

* 玩家与敌人使用角色移动及网络移动复制。
* 冲刺输入先在本地修改移动速度，再通过 `ServerSprint` 同步服务器速度，减少输入等待。

## Core Architecture

|类|职责|
|-|-|
|`AMyCharacter`|玩家移动、射击与交互执行、Server RPC、最终伤害计算、角色死亡表现|
|`AMyPlayerController`|Enhanced Input 绑定、本地 UI、弹药文本、Reward / Victory 通知与重开请求|
|`AMyGameModeBase`|服务器敌人数量统计、关卡推进与最终胜利判定|
|`UMyGameInstance`|保存跨关卡的本局伤害成长倍率与 Run 重置|
|`AMyGameStateBase`|全队共享钥匙状态复制|
|`AAISpawner`|在指定区域随机生成敌人|
|`AEnemyAIController`|定点/随机巡逻、视觉感知、追击、目标切换与攻击冷却|
|`AEnemyCharacter`|敌人攻击、死亡表现与敌人死亡通知|
|`UWeaponDataAsset`|武器名称、基础伤害、射程、弹匣容量与模型配置|
|`AWeaponBase`|武器实例、配置读取、弹药消耗、装备与装备状态同步|
|`UWeaponComponent`|当前武器引用、拾取入口与武器挂点管理|
|`UHealthComponent`|伤害处理、生命值与死亡状态复制、事件广播|
|`IInteractable` / `ADoorKey` / `ADoor`|统一交互接口、钥匙拾取与开门|
|`URewardWidget`|关卡完成后的局内成长提示|
|`URestartWidget`|重开按钮绑定与服务器重开请求入口|

主要代码入口：

* [角色与射击流程](Source/JobDemo/Private/Character/MyCharacter.cpp)
* [GameMode 与关卡推进](Source/JobDemo/Private/Core/MyGameModeBase.cpp)
* [GameInstance 与局内成长](Source/JobDemo/Private/Core/MyGameInstance.cpp)
* [随机敌人生成](Source/JobDemo/Private/AI/AISpawner.cpp)
* [AI 控制器](Source/JobDemo/Private/AI/EnemyAIController.cpp)
* [武器配置](Source/JobDemo/Public/Weapons/WeaponDataAsset.h)
* [武器实例](Source/JobDemo/Private/Weapons/WeaponBase.cpp)
* [武器组件](Source/JobDemo/Private/Weapons/WeaponComponent.cpp)
* [生命组件](Source/JobDemo/Private/Components/HealthComponent.cpp)
* [玩家控制器与 UI](Source/JobDemo/Private/Core/MyPlayerController.cpp)

## Controls

|输入|功能|
|-|-|
|WASD|移动|
|鼠标|控制视角|
|Space|跳跃|
|Shift|冲刺|
|F|交互：拾取武器、拾取钥匙、开关门|
|鼠标左键|射击|

## Getting Started

开发环境：

* Unreal Engine 5.7
* C++
* Visual Studio
* Windows

运行源码：

1. 克隆或下载仓库。
2. 为 `JobDemo.uproject` 生成 Visual Studio 项目文件。
3. 使用 `Development Editor / Win64` 配置编译。
4. 打开 `JobDemo.uproject`。
5. 进入 `/Game/Maps/NewWorld` 地图。
6. 点击 Play 运行。

编辑器多人运行：

1. 在 Play 设置中将玩家数量设为 `2`。
2. 将网络模式设为 `Play As Listen Server`。
3. 使用独立游戏窗口运行，分别操作主机与客户端。

可检查：

* 武器拾取与装备。
* 数据驱动武器伤害。
* 弹药消耗与 UI。
* 射击与伤害同步。
* 队伍钥匙共享。
* 门状态同步。
* AI 固定巡逻与随机巡逻。
* AI 感知、追击与目标切换。
* 随机敌人生成。
* 玩家和敌人死亡。
* 三关连续切换。
* 跨关卡伤害成长。
* Reward UI。
* Victory UI。
* 多人重开。

## Packaging

Windows Development 打包版本支持三关连续运行。

项目打包配置中显式加入：

* `/Game/Maps/NewWorld`
* `/Game/Maps/LevelTwo`
* `/Game/Maps/LevelThree`

保证 `ServerTravel` 使用的后续地图能够被正确 Cook 到 Windows 包中。

同时将部分依赖 Lambda 的 Timer 调整为成员函数与成员 `FTimerHandle` 管理，例如：

* `AMyGameModeBase::CountEnemies`
* `AMyPlayerController::HideRewardWidget`

用于提高关卡切换过程中对象生命周期管理的稳定性。

## Performance

此前 Windows Development 打包版本的场景性能记录：

|项目|记录|
|-|-|
|CPU|Intel i5-12600KF|
|GPU|NVIDIA RTX 2080 Ti|
|内部渲染分辨率|约 1400 × 788|
|Frame|约 4.71 ms|
|Game Thread|约 1.44 ms|
|Draw Thread|约 4.70 ms|
|GPU Time|约 4.13 ms|
|Draw Calls|约 291|

以上为此前测试场景的性能基线，不代表最新源码的重新测量结果。

## Current Scope

当前 Demo 已支持：

* 第三人称移动、跳跃与冲刺。
* 多人服务器权威基础网络架构。
* 数据驱动武器。
* 武器拾取、装备、射击与弹药消耗。
* 武器和弹药 UI。
* AI 感知、巡逻、追击、攻击与目标切换。
* AI 随机生成与随机巡逻。
* 生命值、伤害、死亡和 Ragdoll。
* 钥匙共享与开门交互。
* 三关连续多人闯关。
* 跨关卡局内伤害成长。
* 通关 Reward UI。
* 最终 Victory UI。
* 多人统一重开。
* Windows 打包运行。

当前武器系统支持单武器拾取、装备、射击、弹药消耗及 UI 显示，暂未加入换弹、武器切换和射速控制。

当前局内成长采用固定的伤害倍率提升：

`1.0 → 2.2 → 3.4`

暂未加入随机 Buff 选择、复杂 Build 组合或永久成长系统。

## Screenshot

![Gameplay](Docs/Images/gameplay.png)

