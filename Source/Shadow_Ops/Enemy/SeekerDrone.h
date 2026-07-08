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
	
	float ContactDamage{ 15.0f };
	float ContactCooldown{ 1.0f };
	
private:
	float LastContactDamageTime;
	
	void CheckContactDamage();
};