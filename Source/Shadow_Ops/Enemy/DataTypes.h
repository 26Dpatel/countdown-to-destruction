#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Engine/DataTable.h"
#include "DataTypes.generated.h"

class ASLEnemyBase;

/**
 * One raw in DT_EnemyTypes.
 * Each raw defines a type of enemy (Seeker, Fast Seeker, Heavy Seeker, etc).
 */
USTRUCT(BlueprintType)
struct FEnemyTypeData : public FTableRowBase
{
	GENERATED_BODY()

	/** The Blueprint class to spawn for this enemy type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	TSubclassOf<AEnemyBase> EnemyClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float Health{ 20.0f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float MoveSpeed{ 300.0f };
};

/** 
 * Define which enemies appear in a wave and how many  
 */
USTRUCT(BlueprintType)
struct FWaveEnemyEntry
{
	GENERATED_BODY()
	
	/** Row name in DT_EnemyType (e.g, "Seeker", "FastSeeker" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	FName EnemyTypeRowName;
	/** How many of this type to spawn in the wave */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 Count{ 5 };
};

/**
 * One raw in DT_WaveDefinitions.
 * Each row defines one wave of the game 
 */
USTRUCT(BlueprintType)
struct FWaveDefinition : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TArray<FWaveEnemyEntry> Enemies;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float SpawnDuration{ 5.0f };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float HealthMultiplier{ 1.0f };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float SpeedMultiplier{ 1.0f };
};

