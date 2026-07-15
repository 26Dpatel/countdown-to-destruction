#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveCompleted, int32, WaveNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOnAllWavesCompleted);

struct FEnemyTypeData;
struct FSLWaveDefinition;

UCLASS()
class SHADOW_OPS_API AWaveManager : public AActor
{
    GENERATED_BODY()

public:
    AWaveManager();

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnWaveStarted OnWaveStarted;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnWaveCompleted OnWaveCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnOnAllWavesCompleted OnAllWavesCompleted;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave System|State")
    int32 EnemiesAlive{ 0 };

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave System")
    TObjectPtr<UDataTable> EnemyTypeTable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave System")
    TObjectPtr<UDataTable> WaveDefinitionsTable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave System")
    float WaveRespiteTime{ 3.0f };

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave System")
    int32 MaxSimultaneousEnemies{ 20 };

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave System")
    TArray<AActor*> SpawnPoints;

    // State
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave System|State")
    int32 CurrentWaveIndex{ 0 };
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave System|State")
    int32 EnemiesToSpawn{ 0 };

private:
    void StartNextWave();
    void SpawnEnemy();

    UFUNCTION()
    void OnEnemyDied(AActor* Enemy);

    void CompleteWave();

    FTimerHandle RespiteTimerHandle;
    FTimerHandle SpawnTimerHandle;

    TArray<TPair<FEnemyTypeData, int32>> CurrentSpawnQueue;
    float CurrentSpawnInterval;

    TArray<FName> WaveRowNames;
};
