#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

class IDamageable
{
	GENERATED_BODY()

public:
	// Apply damage to this entity. Each implementing class
	// decides what "taking damage" means for its type.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	void ApplyDamage(float DamageAmount, AActor* DamageInstigator);

	// Can this entity currently receive damage?
	// Supports invincibility frames, shields, death state, etc.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	bool CanBeDamaged() const;
};
