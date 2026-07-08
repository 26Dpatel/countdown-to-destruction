#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
    
    CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitCapsuleSize(30.0f, 20.0f);
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

}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (!bIsSpawned)
    {
       // get amount of time passed
       float TimePassed{ static_cast<float>(GetWorld()->GetTimeSeconds() - SpawnTime) }; 
       
       // calculate scale: time passed / PhaseInDuration
       float CurrentScale{ TimePassed / PhaseInDuration };
       if (CurrentScale >= 1.0f)
       {
          bIsSpawned = true;
          CurrentScale = 1.0f;
       }
       SetActorScale3D(FVector(CurrentScale));
    }
}


void AEnemyBase::InitFromData(float InHealth, float InMoveSpeed, float InDropChance, int32 InScoreValue)
{
    MaxHealth = InHealth;
    CurrentHealth = InHealth;
    MoveSpeed = InMoveSpeed;

    
    MovementComponent->MaxSpeed = MoveSpeed;
}

bool AEnemyBase::CanBeDamaged_Implementation() const
{
    return !bIsDead && !bIsStaggered;
}

void AEnemyBase::ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator)
{
    if (!CanBeDamaged_Implementation())
    {
        return;
    }

    CurrentHealth -= DamageAmount;

    if (CurrentHealth <= 0.0f)
    {
        bIsDead = true;

        OnEnemyDeath.Broadcast(this);

        Die();
    }
}

void AEnemyBase::Die()
{
    Destroy();
}

