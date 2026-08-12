# UE5 C++ Gameplay Demo

## Demo Video

[观看演示视频](https://www.bilibili.com/video/BV1mQuC6fEhP/)

一个使用 Unreal Engine 5.7 和 C++ 开发的第三人称 Gameplay Demo。

核心玩法逻辑由 C++ 实现，Blueprint 主要用于资源配置、角色外观和 UMG 界面布局。

项目已完成基础 Gameplay 闭环，并已将射击、伤害、冲刺、钥匙与开门等核心流程改造为 Server Authoritative 网络架构。

## Download

[下载Windows版本](https://pan.baidu.com/s/18vaVbsl8Fsgbifo7sfd7Ng?pwd=1234)

## Gameplay Loop

玩家移动与探索  
→ 拾取钥匙  
→ 开门  
→ 遭遇巡逻敌人  
→ 敌人感知、追击和攻击  
→ 玩家射击并造成伤害  
→ 玩家死亡失败，或清空敌人胜利  
→ 重新开始关卡

## Features

* Enhanced Input角色移动、视角、跳跃和冲刺
* 基于射线检测的射击与交互
* `IInteractable`交互接口
* 拾取钥匙和开门流程
* AI定点巡逻
* AI Perception视觉感知
* 发现玩家、追击、攻击、攻击冷却
* 丢失玩家后恢复巡逻
* 可复用的`UHealthComponent`
* Damage、生命值、死亡广播
* 玩家和敌人Ragdoll死亡表现
* 玩家血条和准星
* 玩家死亡、重新开始
* 清空敌人、胜利、重新开始
* Windows Development打包验证

### Networking

* 玩家与敌人Actor支持网络复制和Movement Replication
* Server Authoritative射击与伤害判定
* 使用`Server RPC`发送客户端射击请求
* 服务器校验客户端提交的射击起点
* 服务器对客户端方向向量进行标准化处理
* 生命值`CurrentHealth`由服务器权威修改并复制到客户端
* 死亡状态`bIsDead`由服务器权威修改并复制到客户端
* 使用`ReplicatedUsing / OnRep`响应客户端复制数据变化
* 服务器与客户端分别广播生命值和死亡事件
* 使用`IsLocallyControlled()`区分本地玩家UI、输入和摄像机逻辑
* 使用`Client RPC`向每个客户端发送胜利通知
* GameMode通过PlayerController迭代器遍历在线玩家并触发客户端胜利UI
* NetMulticast同步所有客户端射击效果
* 冲刺采用本地预测，并通过`ServerSprint`同步服务器移动速度
* 使用`ServerInteract`将客户端交互请求发送到服务器权威执行
* 钥匙Actor由服务器销毁，并将销毁结果同步到所有客户端
* `AMyGameStateBase`复制队伍共享的`bTeamHasDoorKey`状态
* 任意玩家拾取钥匙后，全队都获得开门权限
* 服务器验证队伍钥匙状态并切换门的开关
* 门Actor、`bIsOpen`与`PivotPoint`支持网络复制

## Core Architecture

|类|职责|
|-|-|
|`AMyCharacter`|玩家移动、冲刺本地预测、射击与交互输入、Server RPC及服务器权威执行|
|`AMyPlayerController`|Enhanced Input、本地UI管理与Client RPC胜利通知|
|`AEnemyAIController`|巡逻、感知、追击、攻击与状态切换|
|`AEnemyCharacter`|敌人攻击、死亡表现、网络移动复制与服务器胜利条件通知|
|`UHealthComponent`|服务器权威生命值、伤害处理、属性复制、OnRep与死亡广播|
|`AMyGameModeBase`|服务器敌人数量统计、胜利条件和玩家遍历|
|`AMyGameStateBase`|复制全队共享的钥匙权限，使所有客户端获得一致状态|
|`ADoorKey`|服务器权威处理拾取、更新GameState并销毁网络Actor|
|`ADoor`|服务器检查队伍钥匙权限并复制开关状态与门轴旋转|
|`IInteractable`|可交互对象统一接口|
|`URestartWidget`|重新加载当前关卡|

## Network Architecture

当前网络实现遵循 Server Authoritative 模型：

```text
Client Local Input
        ↓
   ShootDamage()
        ↓
ServerShoot() RPC
        ↓
Server validates request
        ↓
 PerformShoot()
        ↓
   ApplyDamage()
        ↓
UHealthComponent
(Server Authority)
        ↓
CurrentHealth / bIsDead
        ↓
   Replication
        ↓
Client OnRep
        ↓
Delegate Broadcast
        ↓
UI / Death Presentation
```

### Server Authoritative Shooting

客户端只负责产生射击输入和提供射击视角信息：

```text
Client
ShootDamage()
    ↓
ServerShoot(ClientTraceStart, ClientDirection)
```

服务器收到RPC后进行基础合法性检查：

* 玩家是否已经死亡
* 方向向量是否有效
* 客户端提交的射线起点是否位于服务器角色附近
* 对方向向量执行`GetSafeNormal()`

只有通过验证后服务器才会调用`PerformShoot()`执行真正的射线检测和`ApplyDamage()`。

因此最终的命中和伤害结果由服务器决定，而不是由客户端直接修改游戏状态。

射击表现与伤害状态分开同步：服务器使用`NetMulticast`向所有端播放射线和命中特效，而实际伤害仍只在服务器计算。

### Sprint Local Prediction

冲刺输入先在本地立即修改`MaxWalkSpeed`，避免等待网络往返造成操作延迟；客户端随后调用`ServerSprint`，让服务器同步执行相同的速度切换。

```text
Local Sprint Input
        ↓
ChangeMovementSpeed()
  (Local Prediction)
        ↓
ServerSprint() RPC
        ↓
Server ChangeMovementSpeed()
        ↓
Character Movement Replication
```

### Server Authoritative Interaction

客户端本地射线负责选择交互目标和显示提示，按下交互键后通过`ServerInteract`把请求发送到服务器。服务器确认角色仍然存活、目标有效且实现`IInteractable`后，才调用接口执行真正的Gameplay结果。

```text
Client Interact Input
        ↓
Local Interaction Trace
        ↓
ServerInteract(Target) RPC
        ↓
PerformInteract()
  (Server Authority)
        ↓
IInteractable::Execute\_Interact()
```

### Shared Key and Door Replication

钥匙权限不再保存在某个玩家角色上，而是保存在所有客户端都拥有副本的`AMyGameStateBase`中：

```text
Any Player Picks Up Key
        ↓
ServerInteract(DoorKey)
        ↓
GameState.bTeamHasDoorKey = true
        ↓
DOREPLIFETIME
        ↓
All Clients Receive Team Key State
        ↓
Any Player Can Open Door
```

服务器拾取钥匙后销毁`ADoorKey`网络Actor；服务器开门前读取GameState中的队伍钥匙状态，通过后切换`bIsOpen`并旋转`PivotPoint`。门Actor、开关状态和旋转组件都开启复制，使所有客户端看到一致的钥匙与门状态。

### Health Replication

`UHealthComponent`开启组件复制：

```text
Server ApplyDamage
        ↓
Server modifies CurrentHealth
        ↓
DOREPLIFETIME
        ↓
Client receives CurrentHealth
        ↓
OnRep\_CurrentHealth()
        ↓
OnHealthChanged.Broadcast()
        ↓
Local Health UI
```

`CurrentHealth`和`bIsDead`使用`ReplicatedUsing`进行同步。

服务器负责修改最终权威状态；客户端收到复制后的数据后，通过`OnRep`触发对应表现逻辑。

### Death Replication

死亡状态由服务器决定：

```text
Server
CurrentHealth <= 0
        ↓
bIsDead = true
        ↓
Replication
        ↓
Client OnRep\_Dead()
        ↓
OnDeath.Broadcast()
        ↓
HandleDeath()
```

`HandleDeath()`可以在服务器和客户端角色副本上执行，而只有本地玩家需要执行的内容通过：

```cpp
IsLocallyControlled()
```

进行区分，例如：

* 禁用本地移动和镜头输入
* 隐藏准星
* 显示死亡UI
* 显示鼠标

### Client RPC Victory

敌人死亡数量由服务器GameMode管理。

当服务器检测到所有敌人被清空后：

```text
Server GameMode
        ↓
GetPlayerControllerIterator()
        ↓
遍历所有 PlayerController
        ↓
ClientVictory() Client RPC
        ↓
Each Local Client
        ↓
Victory UI
```

这样服务器负责决定“游戏是否胜利”，客户端只负责显示属于自己的胜利UI。

## Technical Highlights

* 使用Actor Component复用生命系统
* 使用动态多播委托传递血量变化和死亡事件
* 使用Timer控制AI巡逻等待、追击刷新和攻击冷却
* 使用AI Perception处理玩家感知状态
* 使用GameMode管理服务器胜利条件
* 使用Actor Replication同步网络Actor
* 使用Movement Replication同步角色移动
* 使用`Server RPC`建立客户端到服务器的Gameplay请求入口
* 使用服务器权威模型处理射击、交互、钥匙、门、伤害和死亡状态
* 使用本地预测保证客户端冲刺输入响应，并由服务器校正权威移动速度
* 使用`DOREPLIFETIME`复制Gameplay属性
* 使用`ReplicatedUsing / OnRep`响应客户端状态更新
* 使用GameState复制全队共享的Gameplay状态
* 使用`NetMulticast`同步所有客户端的射击表现
* 使用`Client RPC`从服务器通知指定客户端执行本地表现
* 使用`HasAuthority()`区分服务器权威逻辑
* 使用`IsLocallyControlled()` / `IsLocalController()`隔离本地输入和UI
* 使用PlayerController Iterator遍历服务器中的所有玩家控制器
* 核心Gameplay逻辑使用C++实现

## Controls

|输入|功能|
|-|-|
|WASD|移动|
|鼠标|控制视角|
|Space|跳跃|
|Shift|冲刺|
|F|交互|
|鼠标左键|射击|

## Performance

Windows Development打包版本测试：

* CPU：Intel i5-12600KF
* GPU：NVIDIA RTX 2080 Ti
* Frame：约4.71 ms
* Game Thread：约1.44 ms
* Draw Thread：约4.70 ms
* GPU：约4.13 ms
* Draw Calls：约291

测试场景使用约1400×788内部渲染分辨率。该数据仅表示当前Demo场景的性能基线。

## Development Environment

* Unreal Engine 5.7
* C++
* Visual Studio
* Windows

## Status

基础Gameplay闭环已经完成，并完成Windows Development打包验证。

当前已完成基础网络Gameplay改造，包括：

* Server Authoritative射击
* 射击、冲刺与交互Server RPC
* Actor / Movement Replication
* 冲刺本地预测与服务器权威速度同步
* NetMulticast射击表现同步
* 生命值属性复制
* 死亡状态复制
* OnRep客户端状态响应
* 本地玩家UI与网络副本逻辑区分
* Client RPC胜利通知
* GameState队伍共享钥匙权限复制
* 服务器权威拾取钥匙与网络Actor销毁
* 服务器权威开门及门状态复制

后续继续完善多人AI目标选择、交互请求的服务器端距离与视线复核，以及完整的多人回合/重开流程。

![Gameplay](Docs/Images/gameplay.png)

