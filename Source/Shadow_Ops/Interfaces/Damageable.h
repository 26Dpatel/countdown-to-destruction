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
	virtual void ReceiveDamage(float DamageAmount, AActor* DamagingActor) = 0; 
};
