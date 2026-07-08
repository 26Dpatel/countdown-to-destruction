#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/Damageable.h"
#include "EnemyBase.generated.h"

class UCapsuleComponent;
class UFloatingPawnMovement;

// Removed ScoreValue parameter
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, AActor*, Enemy);

UCLASS()
class SHADOW_OPS_API AEnemyBase : public APawn, public IDamageable
{
    GENERATED_BODY()

public:
    AEnemyBase();
    
    virtual void Tick(float DeltaTime) override;

    bool bIsDead{ false };
    
    virtual void InitFromData(float InHealth, float InMoveSpeed, float InDropChance, int32 InScoreValue);
    
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEnemyDeath OnEnemyDeath;
    
    virtual void ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator) override;
    virtual bool CanBeDamaged_Implementation() const override;
    
    virtual void BeginPlay() override;

protected:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCapsuleComponent> CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFloatingPawnMovement> MovementComponent;
    
    // Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
    float MaxHealth{ 20.0f };

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
    float MoveSpeed{ 300.0f };
    
    bool bIsStaggered{ false };

    virtual void Die();
    
private:
    // Spawn / scaling
    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Spawn")
    float PhaseInDuration{ 0.3f };
    
    float SpawnTime;
    bool bIsSpawned{ false };

    // Timer for hit flash
    FTimerHandle FlashTimerHandle;

    FTimerHandle StaggerTimerHandle;
};
