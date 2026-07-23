#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/Damageable.h"
#include "EnemyBase.generated.h"

class AEnergyCell;
class UCapsuleComponent;
class UNiagaraSystem;
class USoundBase;
class UFloatingPawnMovement;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, AActor*, Enemy);

UCLASS()
class SHADOW_OPS_API AEnemyBase : public APawn, public IDamageable
{
    GENERATED_BODY()

public:
    AEnemyBase();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    virtual void ReceiveDamage(float DamageAmount, AActor* DamagingActor) override;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEnemyDeath OnEnemyDeath;

    virtual void InitFromData(float InHealth, float InMoveSpeed, float InDropChance, int32 InScoreValue, float InHealthForPlayer);

    // Feedback
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Feedback")
    TObjectPtr<USoundBase> DeathSound;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Feedback")
    TObjectPtr<USoundBase> ExplosionSound;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Feedback")
    TObjectPtr<USoundBase> FlyingSound;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Feedback")
    TObjectPtr<UNiagaraSystem> DeathEffect;
    
    // Drops
    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Drops")
    TSubclassOf<AEnergyCell> EnergyCellClass;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCapsuleComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UFloatingPawnMovement* MovementComponent;

    // Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
    float MaxHealth{ 20.0f }; 
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
    float HealthForPlayer{ 5.0f };

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
    float MoveSpeed{ 300.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
    float DropChance{ 0.5f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
    float DropPositionOffset{ 450.0f };

    // Stagger
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stagger")
    float StaggerDuration{ 0.3f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stagger")
    float StaggerFlashSpeed{ 20.0f };

    bool bIsDead{ false };
    bool bIsStaggered{ false };

    virtual void Die();

private:
    float PhaseInDuration{ 0.3f };
    float SpawnTime;
    bool bIsSpawned{ false };

    FTimerHandle FlashTimerHandle;
    FTimerHandle StaggerTimerHandle;

    void StartStagger();
    void EndStagger();
    void ToggleStaggerFlash() const;
};
