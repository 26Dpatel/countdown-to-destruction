#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HeartRateComponent.generated.h"

class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHeartRateChanged, float, NewHeartRate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UHeartRateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHeartRateComponent();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Heart Rate")
	void UpdateHeartRate(float DeltaSeconds, float StaminaPercent, float HealthPercent);

	UFUNCTION(BlueprintCallable, Category="Heart Rate")
	void OnDamageTaken(float DamageAmount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
	TObjectPtr<USoundBase> HeartRateSound;

	UFUNCTION(BlueprintPure, Category="Heart Rate")
	float GetHeartRate() const { return CurrentHeartRate; }

	UPROPERTY(BlueprintAssignable, Category="HeartRate|Events")
	FOnHeartRateChanged OnHeartRateChanged;

protected:

	void PlayHeartRateSound(float StaminaPercent, float HealthPercent);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HeartRate")
	float RestingHeartRate = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HeartRate")
	float MaxHeartRate = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HeartRate")
	float HeartRateChangeSpeed = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HeartRate")
	float CurrentHeartRate = 70.0f;
};
