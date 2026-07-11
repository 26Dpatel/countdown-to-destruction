#include "WaveManager.h"
#include "DataTypes.h"
#include "EnemyBase.h"
#include "Character/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"

AWaveManager::AWaveManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWaveManager::BeginPlay()
{
    Super::BeginPlay();

    if (!WaveDefinitionsTable || !EnemyTypeTable)
    {
        UE_LOG(LogTemp, Error, TEXT("WaveManager: Data tables not assigned!"));
        return;
    }

    WaveRowNames = WaveDefinitionsTable->GetRowNames();

    GetWorldTimerManager().SetTimer(
        RespiteTimerHandle,
        this,
        &AWaveManager::StartNextWave,
        WaveRespiteTime,
        false
    );
}

void AWaveManager::StartNextWave()
{
    checkf(WaveDefinitionsTable, TEXT("Wave Definitions Table not assigned!"));
    checkf(EnemyTypeTable, TEXT("Enemy Type Table not assigned!"));

    if (CurrentWaveIndex >= WaveRowNames.Num())
    {
        return;
    }

    const FName RowName = WaveRowNames[CurrentWaveIndex];

    FWaveDefinition* WaveDef =
        WaveDefinitionsTable->FindRow<FWaveDefinition>(
            RowName,
            TEXT("StartNextWave")
        );

    if (!WaveDef)
    {
        UE_LOG(LogTemp, Error, TEXT("WaveManager: Wave definition missing for row %s"), *RowName.ToString());
        return;
    }

    CurrentSpawnQueue.Empty();
    EnemiesToSpawn = 0;

    for (const FWaveEnemyEntry& Entry : WaveDef->Enemies)
    {
        FEnemyTypeData* EnemyData =
            EnemyTypeTable->FindRow<FEnemyTypeData>(
                Entry.EnemyTypeRowName,
                TEXT("StartNextWave")
            );

        if (!EnemyData)
        {
            UE_LOG(LogTemp, Warning, TEXT("WaveManager: EnemyType '%s' not found, skipping."),
                *Entry.EnemyTypeRowName.ToString());
            continue;
        }

        FEnemyTypeData EnemyType = *EnemyData;

        EnemyType.Health *= WaveDef->HealthMultiplier;
        EnemyType.MoveSpeed *= WaveDef->SpeedMultiplier;

        CurrentSpawnQueue.Add(TPair<FEnemyTypeData, int32>(EnemyType, Entry.Count));
        EnemiesToSpawn += Entry.Count;
    }

    if (EnemiesToSpawn == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("WaveManager: Wave %d has no valid enemies, auto-completing."),
            CurrentWaveIndex + 1);
        CompleteWave();
        return;
    }

    CurrentSpawnInterval =
        WaveDef->SpawnDuration / FMath::Max(EnemiesToSpawn, 1);

    const int32 WaveNumber = CurrentWaveIndex + 1;

    OnWaveStarted.Broadcast(WaveNumber);
    UE_LOG(LogTemp, Log, TEXT("Wave %d Started!"), WaveNumber);

    GetWorldTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &AWaveManager::SpawnEnemy,
        CurrentSpawnInterval,
        true,
        0.0f
    );
}

void AWaveManager::SpawnEnemy()
{
    if (CurrentSpawnQueue.Num() == 0)
    {
        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

        if (EnemiesToSpawn <= 0 && EnemiesAlive <= 0)
        {
            CompleteWave();
        }
        return;
    }

    if (EnemiesAlive >= MaxSimultaneousEnemies)
    {
        return;
    }

    checkf(SpawnPoints.Num() > 0, TEXT("WaveManager: No Spawn Points assigned!"));

    AActor* SpawnPoint =
        SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];

    TPair<FEnemyTypeData, int32>& QueueEntry = CurrentSpawnQueue[0];
    FEnemyTypeData& EnemyData = QueueEntry.Key;

    if (!EnemyData.EnemyClass)
    {
        UE_LOG(LogTemp, Error, TEXT("WaveManager: EnemyClass missing, skipping entry."));
        CurrentSpawnQueue.RemoveAt(0);
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AEnemyBase* Enemy =
        GetWorld()->SpawnActor<AEnemyBase>(
            EnemyData.EnemyClass,
            SpawnPoint->GetActorLocation(),
            SpawnPoint->GetActorRotation(),
            SpawnParams
        );

    if (Enemy)
    {
        Enemy->InitFromData(
            EnemyData.Health,
            EnemyData.MoveSpeed,
            0.f,
            0
        );

        Enemy->OnEnemyDeath.AddDynamic(this, &AWaveManager::OnEnemyDied);
        EnemiesAlive++;
    }

    QueueEntry.Value--;

    if (QueueEntry.Value <= 0)
    {
        CurrentSpawnQueue.RemoveAt(0);
    }

    EnemiesToSpawn--;

    if (EnemiesToSpawn <= 0 && EnemiesAlive <= 0)
    {
        CompleteWave();
    }
}

void AWaveManager::OnEnemyDied(AActor* Enemy)
{
    EnemiesAlive = FMath::Max(EnemiesAlive - 1, 0);

    if (AShooterCharacter* Player =
        Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
    {
        Player->KillCount++;
    }

    if (EnemiesToSpawn <= 0 && EnemiesAlive <= 0)
    {
        CompleteWave();
    }
}

void AWaveManager::CompleteWave()
{
    const int32 WaveNumber = CurrentWaveIndex + 1;

    OnWaveCompleted.Broadcast(WaveNumber);
    UE_LOG(LogTemp, Log, TEXT("Wave %d Completed!"), WaveNumber);

    CurrentWaveIndex++;

    if (CurrentWaveIndex < WaveRowNames.Num())
    {
        GetWorldTimerManager().SetTimer(
            RespiteTimerHandle,
            this,
            &AWaveManager::StartNextWave,
            WaveRespiteTime,
            false
        );
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("All waves complete"));
        OnAllWavesCompleted.Broadcast();
    }
}
