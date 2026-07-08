#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/Damageable.h"
#include "EnemyBase.generated.h"

class UCapsuleComponent;
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

	virtual void ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator) override;
	virtual bool CanBeDamaged_Implementation() const override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyDeath OnEnemyDeath;

	virtual void InitFromData(float InHealth, float InMoveSpeed, float InDropChance, int32 InScoreValue);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFloatingPawnMovement* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth{ 20.0f };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed{ 300.0f };

	bool bIsDead{ false };
	bool bIsStaggered{ false };

	virtual void Die();

private:
	float PhaseInDuration{ 0.3f };
	float SpawnTime;
	bool bIsSpawned{ false };

	FTimerHandle FlashTimerHandle;
	FTimerHandle StaggerTimerHandle;
};
