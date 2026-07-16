#include "SeekerDrone.h"
#include "Interfaces/Damageable.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBase.h"

ASeekerDrone::ASeekerDrone()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASeekerDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move toward the player
	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (Player)
	{
		FVector Direction = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		AddMovementInput(Direction);
	}

	CheckContactDamage();
}

void ASeekerDrone::CheckContactDamage()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastContactDamageTime < ContactCooldown)
		return;

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Cast<AEnemyBase>(Actor))
			continue;
		
		if (IDamageable* Damageable = Cast<IDamageable>(Actor))
		{
			Damageable->ReceiveDamage(ContactDamage, this);
			LastContactDamageTime = CurrentTime;
			break;
		}
	}
}
