#include "EnemyBase.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/ShooterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "ItemDrops & PowerUps/EnergyCell.h"
#include "Kismet/GameplayStatics.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitCapsuleSize(30.f, 20.f);
    CollisionComponent->SetCollisionProfileName(TEXT("Pawn"));
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    CollisionComponent->SetGenerateOverlapEvents(true);
    RootComponent = CollisionComponent;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(CollisionComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    MovementComponent->MaxSpeed = MoveSpeed;
    SpawnTime = GetWorld()->GetTimeSeconds();
        
    if (FlyingSound)
    {
        UGameplayStatics::SpawnSoundAtLocation(this, FlyingSound, GetActorLocation());
    }

    if (AShooterCharacter* Shooter = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
    {
        OnEnemyDeath.AddDynamic(Shooter, &AShooterCharacter::HandleEnemyKilled);
    }
}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsSpawned)
    {
        float TimePassed = GetWorld()->GetTimeSeconds() - SpawnTime;
        float CurrentScale = TimePassed / PhaseInDuration;

        if (CurrentScale >= 1.f)
        {
            bIsSpawned = true;
            CurrentScale = 1.f;
        }

        SetActorScale3D(FVector(CurrentScale));
    }
}

void AEnemyBase::ReceiveDamage(float DamageAmount, AActor* DamageInstigator)
{
    if (bIsDead || bIsStaggered)
        return;

    CurrentHealth -= DamageAmount;

    // Trigger stagger on hit
    StartStagger();

    if (CurrentHealth <= 0.f)
    {
        bIsDead = true;
        OnEnemyDeath.Broadcast(this);
        Die();
    }
}

void AEnemyBase::InitFromData(float InHealth, float InMoveSpeed, float InDropChance, int32 InScoreValue, float InHealthForPlayer)
{
    MaxHealth = InHealth;
    CurrentHealth = InHealth;
    HealthForPlayer = InHealthForPlayer;
    MoveSpeed = InMoveSpeed;
    DropChance = InDropChance;

    if (MovementComponent)
    {
        MovementComponent->MaxSpeed = MoveSpeed;
    }
}

void AEnemyBase::StartStagger()
{
    if (bIsStaggered)
        return;

    bIsStaggered = true;

    // Stop movement
    if (MovementComponent)
    {
        MovementComponent->StopMovementImmediately();
    }

    // Start flashing effect
    GetWorld()->GetTimerManager().SetTimer(
        FlashTimerHandle,
        this,
        &AEnemyBase::ToggleStaggerFlash,
        1.0f / StaggerFlashSpeed,
        true
    );

    // End stagger after duration
    GetWorld()->GetTimerManager().SetTimer(
        StaggerTimerHandle,
        this,
        &AEnemyBase::EndStagger,
        StaggerDuration,
        false
    );
}

void AEnemyBase::EndStagger()
{
    bIsStaggered = false;

    // Stop flashing
    GetWorld()->GetTimerManager().ClearTimer(FlashTimerHandle);

    // Ensure mesh becomes visible again
    MeshComponent->SetVisibility(true);
}

void AEnemyBase::ToggleStaggerFlash() const
{
    MeshComponent->SetVisibility(!MeshComponent->IsVisible());
}

void AEnemyBase::Die()
{
    
    if (DeathSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
    }
    if (DeathEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DeathEffect, GetActorLocation());
    }
    
    if (FMath::FRand() < DropChance && EnergyCellClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        GetWorld()->SpawnActor<AEnergyCell>(EnergyCellClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
    }
    
    AShooterCharacter* Player = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (Player && Player->HealthComponent)
    {
        Player->HealthComponent->Heal(HealthForPlayer);
    }

    
    Destroy();
}
