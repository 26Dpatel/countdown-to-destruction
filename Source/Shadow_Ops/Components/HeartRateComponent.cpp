#include "HeartRateComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"

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

void UHeartRateComponent::PlayHeartRateSound(float StaminaPercent, float HealthPercent) 
{
    if (!HeartRateSound || !GetOwner())
        return;

    // Lower stamina + lower health = louder & higher pitch
    float StaminaFactor = 1.0f - StaminaPercent;
    float HealthFactor  = 1.0f - HealthPercent;

    // Volume: 0.3 (calm) → 1.0 (panic)
    float Volume = 0.3f + (StaminaFactor * 0.4f) + (HealthFactor * 0.3f);
    Volume = FMath::Clamp(Volume, 0.3f, 1.0f);

    // Pitch: 1.0 (normal) → 1.6 (panic)
    float Pitch = 1.0f + (StaminaFactor * 0.4f) + (HealthFactor * 0.2f);
    Pitch = FMath::Clamp(Pitch, 1.0f, 1.6f);

    UGameplayStatics::PlaySoundAtLocation(
        this,
        HeartRateSound,
        GetOwner()->GetActorLocation(),
        Volume,
        Pitch
    );
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

    PlayHeartRateSound(StaminaPercent, HealthPercent);
}

void UHeartRateComponent::OnDamageTaken(float DamageAmount)
{
    CurrentHeartRate = FMath::Clamp(
        CurrentHeartRate + DamageAmount * 0.5f,
        RestingHeartRate,
        MaxHeartRate
    );

    OnHeartRateChanged.Broadcast(FMath::RoundToInt(CurrentHeartRate));

    // Damage spikes = louder + higher pitch
    PlayHeartRateSound(0.0f, 0.0f);
}
