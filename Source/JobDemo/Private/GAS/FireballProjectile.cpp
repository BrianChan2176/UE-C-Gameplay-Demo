#include "GAS/FireballProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

AFireballProjectile::AFireballProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true);
    
    // 碰撞球：真正负责命中检测
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));

    SetRootComponent(CollisionSphere);

    CollisionSphere->InitSphereRadius(12.f);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Block);
    CollisionSphere->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    CollisionSphere->SetGenerateOverlapEvents(false);

    // 外观：跟随碰撞球移动
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));

    ProjectileMesh->SetupAttachment(CollisionSphere);
    ProjectileMesh->SetCollisionEnabled( ECollisionEnabled::NoCollision);
    ProjectileMesh->SetRelativeScale3D(FVector(0.24f));

    // 用ProjectileMovement组件飞行：沿 Actor 的本地 X 正方向发射
    ProjectileMovement =CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

    ProjectileMovement->SetUpdatedComponent(CollisionSphere);
    ProjectileMovement->InitialSpeed = 2000.f;
    ProjectileMovement->MaxSpeed = 2000.f;
    ProjectileMovement->Velocity = FVector(1.f, 0.f, 0.f);
    ProjectileMovement->bInitialVelocityInLocalSpace = true;
    ProjectileMovement->ProjectileGravityScale = 0.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
}

void AFireballProjectile::BeginPlay()
{
    Super::BeginPlay();

    // 本轮由服务器模拟飞行，客户端接收位置更新
    if (!HasAuthority())
    {
        ProjectileMovement->Deactivate();
        CollisionSphere->SetCollisionEnabled(
            ECollisionEnabled::NoCollision);
        return;
    }

    // 火球从角色身体范围内生成，移动时忽略施法者
    if (AActor* CasterActor = GetOwner())
    {
        CollisionSphere->IgnoreActorWhenMoving(CasterActor, true);
    }

    CollisionSphere->OnComponentHit.AddDynamic(
        this, &AFireballProjectile::HandleHit);

    // 没撞到东西，也会在 3 秒后清理
    SetLifeSpan(3.f);
}

void AFireballProjectile::HandleHit(
    UPrimitiveComponent* HitComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
    if (!HasAuthority() || bHasHit) { return; }
    if (OtherActor == GetOwner()) { return; }

    // 一颗火球只结算一次命中
    bHasHit = true;

    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ProjectileMovement->StopMovementImmediately();


    if (IsValid(OtherActor))
    {
        UGameplayStatics::ApplyDamage(
            OtherActor,
            DamageAmount,
            GetInstigatorController(),
            this,
            UDamageType::StaticClass());

        UE_LOG(LogTemp, Display,
            TEXT("Fireball hit: %s, Damage=%.1f"),
            *OtherActor->GetName(),
            DamageAmount);
    }

    Destroy();
}