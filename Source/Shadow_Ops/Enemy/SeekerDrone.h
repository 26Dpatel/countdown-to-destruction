#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "SeekerDrone.generated.h"

UCLASS()
class SHADOW_OPS_API ASeekerDrone : public AEnemyBase
{
	GENERATED_BODY()

public:
	ASeekerDrone();

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float ContactDamage{ 15.f };	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float ContactCooldown{ 1.f };

private:
	float LastContactDamageTime{ 0.f };

	void CheckContactDamage();
};
