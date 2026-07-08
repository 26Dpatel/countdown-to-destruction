#include "SeekerAIController.h"
#include "Kismet/GameplayStatics.h"

ASeekerAIController::ASeekerAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASeekerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	APawn* ControlledPawn{ GetPawn() };
	APawn* Player{ UGameplayStatics::GetPlayerPawn(this, 0) };
	if (!ControlledPawn || !Player)
	{
		return;
	}
}