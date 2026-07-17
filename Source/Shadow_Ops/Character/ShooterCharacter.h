#pragma once

#include "CoreMinimal.h"
#include "Shadow_OpsCharacter.h"
#include "Interfaces/Damageable.h"
#include "Weapons/ShooterWeapon.h"
#include "Weapons/ShooterWeaponHolder.h"
#include "ShooterCharacter.generated.h"

class AShooterWeapon;
class UInputAction;
class UInputComponent;
class UHealthComponent;
class UHeartRateComponent;
class UPawnNoiseEmitterComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillCountChanged, int32, NewKillCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, CurrentBullets, int32, MagazineSize);

UCLASS(abstract)
class SHADOW_OPS_API AShooterCharacter : public AShadow_OpsCharacter, public IShooterWeaponHolder, public IDamageable
{
    GENERATED_BODY()
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    UPawnNoiseEmitterComponent* PawnNoiseEmitter;

public:
    
    AShooterCharacter();
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
    
    UPROPERTY(BlueprintAssignable, Category="Weapons")
    FOnAmmoChanged OnAmmoChanged;
    
    UPROPERTY(BlueprintAssignable, Category="Stats")
    FOnKillCountChanged OnKillCountChanged;
    
    virtual void ReceiveDamage(float DamageAmount, AActor* DamagedActor) override;
    
    UFUNCTION(BlueprintCallable, Category="Debug")
    void DoDebugTakeDamage();
    
    UFUNCTION(BlueprintCallable, Category="Input")
    void DoStartFiring();

    UFUNCTION(BlueprintCallable, Category="Input")
    void DoStopFiring();

    UFUNCTION(BlueprintCallable, Category="Input")
    void DoSwitchWeapon();
    
    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* DebugTakeDamageAction;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* ReloadAction;
    
    UFUNCTION(BlueprintCallable, Category="Input")
    void DoReload();
    
    virtual void AttachWeaponMeshes(AShooterWeapon* Weapon) override;
    virtual void PlayFiringMontage(UAnimMontage* Montage) override;
    virtual void AddWeaponRecoil(float Recoil) override;
    virtual FVector GetWeaponTargetLocation() override;
    virtual void AddWeaponClass(const TSubclassOf<AShooterWeapon>& WeaponClass) override;
    virtual void OnWeaponActivated(AShooterWeapon* Weapon) override;
    virtual void OnWeaponDeactivated(AShooterWeapon* Weapon) override;
    virtual void OnSemiWeaponRefire() override;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
    UHealthComponent* HealthComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
    UHeartRateComponent* HeartRateComponent;
 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats")
    int32 KillCount = 0;

    UFUNCTION()
    void HandleEnemyKilled(AActor* Enemy);
    
    
protected:

    UPROPERTY(EditAnywhere, Category ="Input")
    UInputAction* FireAction;

    UPROPERTY(EditAnywhere, Category ="Input")
    UInputAction* SwitchWeaponAction;

    UPROPERTY(EditAnywhere, Category ="Weapons")
    FName FirstPersonWeaponSocket = FName("HandGrip_R");

    UPROPERTY(EditAnywhere, Category ="Weapons")
    FName ThirdPersonWeaponSocket = FName("HandGrip_R");

    UPROPERTY(EditAnywhere, Category ="Aim", meta = (ClampMin = 0, ClampMax = 100000, Units = "cm"))
    float MaxAimDistance = 10000.0f;
    
    UPROPERTY(EditAnywhere, Category ="Weapons")
    TArray<AShooterWeapon*> OwnedWeapons;

    UPROPERTY(EditAnywhere, Category ="Weapons")
    TObjectPtr<AShooterWeapon> CurrentWeapon;

    UPROPERTY(EditAnywhere, Category ="Destruction", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
    float RespawnTime = 5.0f;

    FTimerHandle RespawnTimer;
    
    AShooterWeapon* FindWeaponOfType(TSubclassOf<AShooterWeapon> WeaponClass) const;
    
};
