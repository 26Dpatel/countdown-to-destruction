#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HeartRateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHeartRateChanged, float, NewHeartRate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UHeartRateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHeartRateComponent();
	virtual void BeginPlay() override;

	// ---- Public Interface ----
	UFUNCTION(BlueprintCallable, Category="Heart Rate")
	void UpdateHeartRate(float DeltaSeconds, float StaminaPercent, float HealthPercent);

	// Spike heart rate when taking damage
	UFUNCTION(BlueprintCallable, Category="Heart Rate")
	void OnDamageTaken(float DamageAmount);

	UFUNCTION(BlueprintPure, Category="Heart Rate")
	float GetHeartRate() const { return CurrentHeartRate; }

	// ---- Events ----
	UPROPERTY(BlueprintAssignable, Category="HeartRate|Events")
	FOnHeartRateChanged OnHeartRateChanged;

protected:

	// Base resting heart rate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HeartRate")
	float RestingHeartRate = 70.0f;

	// Max possible heart rate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HeartRate")
	float MaxHeartRate = 180.0f;

	// How fast heart rate changes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HeartRate")
	float HeartRateChangeSpeed = 50.0f;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HeartRate")
	float CurrentHeartRate = 70.0f;
};
