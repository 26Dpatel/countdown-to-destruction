#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "EnergyCell.generated.h"

UCLASS()
class SHADOW_OPS_API AEnergyCell : public AActor
{
    GENERATED_BODY()

public:
    AEnergyCell();
    
    virtual void Tick(float DeltaTime) override;
    
protected:
    virtual void BeginPlay() override;
    
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverLappedComponent, AActor* OtherActor, 
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                        bool bFromSweep, const FHitResult& SweepResult);
    
    void Collect(AActor* Collector);
    void EndPlay(EEndPlayReason::Type EndPlayReason);

    // Flashing callback
    UFUNCTION()
    void ToggleVisibility();
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<USphereComponent> CollectionSphere;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Energy Cell")
    int32 ScoreValue{ 25 };

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Energy Cell")
    float CollectionRadius{ 75.0f };

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Energy Cell")
    float LifeTime{ 10.0f };

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Energy Cell")
    float BobHeight{ 20.0f };

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Energy Cell")
    float BobSpeed{ 2.0f };

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Energy Cell")
    float DespawnWarningTime{ 2.0f };

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Energy Cell")
    float DespawnFlashSpeed{ 8.0f };

    UPROPERTY(EditDefaultsOnly, Category="Energy Cell|Feedback")
    TObjectPtr<USoundBase> CollectionSound;
    
private:
    FVector StartLocation;
    float SpawnTime;
    bool bIsFlashing{ false };

    // Timer for flashing effect
    FTimerHandle FlashTimerHandle;
};
