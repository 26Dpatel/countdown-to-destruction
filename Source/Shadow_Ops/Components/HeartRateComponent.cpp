#include "HeartRateComponent.h"
#include "Math/UnrealMathUtility.h"

UHeartRateComponent::UHeartRateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentHeartRate = RestingHeartRate;
}

void UHeartRateComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHeartRate = RestingHeartRate;
}

void UHeartRateComponent::UpdateHeartRate(float DeltaSeconds, float StaminaPercent, float HealthPercent)
{
	float StaminaFactor = 1.0f - StaminaPercent;
	float HealthFactor  = 1.0f - HealthPercent;

	float TargetHeartRate =
		RestingHeartRate +
		(StaminaFactor * 60.0f) +
		(HealthFactor  * 50.0f);

	TargetHeartRate = FMath::Clamp(TargetHeartRate, RestingHeartRate, MaxHeartRate);

	CurrentHeartRate = FMath::FInterpTo(CurrentHeartRate, TargetHeartRate, DeltaSeconds, HeartRateChangeSpeed);

	int32 RoundedHeartRate = FMath::RoundToInt(CurrentHeartRate);

	OnHeartRateChanged.Broadcast(RoundedHeartRate);
}

void UHeartRateComponent::OnDamageTaken(float DamageAmount)
{
	CurrentHeartRate = FMath::Clamp(
		CurrentHeartRate + DamageAmount * 0.5f,
		RestingHeartRate,
		MaxHeartRate
	);

	OnHeartRateChanged.Broadcast(FMath::RoundToInt(CurrentHeartRate));
}

