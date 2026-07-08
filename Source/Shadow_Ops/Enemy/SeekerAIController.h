#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SeekerAIController.generated.h"

UCLASS()
class SHADOW_OPS_API ASeekerAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASeekerAIController();

	virtual void Tick(float DeltaTime) override;
};
