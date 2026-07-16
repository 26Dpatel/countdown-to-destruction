#include "ShooterProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Interfaces/Damageable.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "Engine/OverlapResult.h"

AShooterProjectile::AShooterProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
    CollisionComponent->SetSphereRadius(16.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
    CollisionComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
    ProjectileMovement->InitialSpeed = 90000.0f;
    ProjectileMovement->MaxSpeed = 90000.0f;
    ProjectileMovement->bShouldBounce = true;

    HitDamageType = UDamageType::StaticClass();
}

void AShooterProjectile::BeginPlay()
{
    Super::BeginPlay();
    CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
}

void AShooterProjectile::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorld()->GetTimerManager().ClearTimer(DestructionTimer);
}

void AShooterProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
                                   bool bSelfMoved, FVector HitLocation, FVector HitNormal,
                                   FVector NormalImpulse, const FHitResult& Hit)
{
    if (bHit)
        return;

    bHit = true;
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    MakeNoise(NoiseLoudness, GetInstigator(), GetActorLocation(), NoiseRange, NoiseTag);

    if (bExplodeOnHit)
    {
        ExplosionCheck(GetActorLocation());
    }
    else
    {
        ProcessHit(Other, OtherComp, Hit.ImpactPoint, -Hit.ImpactNormal);
    }

    BP_OnProjectileHit(Hit);

    if (DeferredDestructionTime > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(DestructionTimer, this,
            &AShooterProjectile::OnDeferredDestruction, DeferredDestructionTime, false);
    }
    else
    {
        Destroy();
    }
}

void AShooterProjectile::ExplosionCheck(const FVector& ExplosionCenter)
{
    TArray<FOverlapResult> Overlaps;

    FCollisionShape OverlapShape;
    OverlapShape.SetSphere(ExplosionRadius);

    FCollisionObjectQueryParams ObjectParams;
    ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
    ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
    ObjectParams.AddObjectTypesToQuery(ECC_PhysicsBody);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    if (!bDamageOwner)
        QueryParams.AddIgnoredActor(GetInstigator());

    GetWorld()->OverlapMultiByObjectType(Overlaps, ExplosionCenter, FQuat::Identity,
                                         ObjectParams, OverlapShape, QueryParams);

    TArray<AActor*> DamagedActors;

    for (const FOverlapResult& CurrentOverlap : Overlaps)
    {
        AActor* Actor = CurrentOverlap.GetActor();
        if (!Actor || DamagedActors.Contains(Actor))
            continue;

        DamagedActors.Add(Actor);

        FVector ExplosionDir = Actor->GetActorLocation() - GetActorLocation();
        ProcessHit(Actor, CurrentOverlap.GetComponent(), GetActorLocation(), ExplosionDir.GetSafeNormal());
    }
}

void AShooterProjectile::ProcessHit(AActor* HitActor, UPrimitiveComponent* HitComp,
                                    const FVector& HitLocation, const FVector& HitDirection)
{
    if (!HitActor || (HitActor == GetOwner() && !bDamageOwner))
        return;

    if (IDamageable* Damageable = Cast<IDamageable>(HitActor))
    {
        Damageable->ReceiveDamage(HitDamage, GetInstigator());
    }
    
    if (ACharacter* HitCharacter = Cast<ACharacter>(HitActor))
    {
        UGameplayStatics::ApplyDamage(HitCharacter, HitDamage,
            GetInstigator()->GetController(), this, HitDamageType);
    }

    // Physics impulse
    if (HitComp && HitComp->IsSimulatingPhysics())
    {
        HitComp->AddImpulseAtLocation(HitDirection * PhysicsForce, HitLocation);
    }
}

void AShooterProjectile::OnDeferredDestruction()
{
    Destroy();
}
