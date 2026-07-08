#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterWeaponHolder.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundBase.h"
#include "ShooterWeapon.generated.h"

class IShooterWeaponHolder;
class AShooterProjectile;
class USkeletalMeshComponent;
class UAnimMontage;
class UAnimInstance;

UCLASS(abstract)
class SHADOW_OPS_API AShooterWeapon : public AActor
{
    GENERATED_BODY()
    
    /** First-person perspective mesh */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* FirstPersonMesh;

    /** Third-person perspective mesh */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* ThirdPersonMesh;

protected:

    /** Cast pointer to the weapon owner */
    IShooterWeaponHolder* WeaponOwner;

    /** Type of projectiles this weapon will shoot */
    UPROPERTY(EditAnywhere, Category="Ammo")
    TSubclassOf<AShooterProjectile> ProjectileClass;

    /** Number of bullets in a magazine */
    UPROPERTY(EditAnywhere, Category="Ammo", meta = (ClampMin = 0, ClampMax = 100))
    int32 MagazineSize = 10;

    /** Number of bullets in the current magazine */
    int32 CurrentBullets = 0;

    /** Total reserve ammo */
    UPROPERTY(EditAnywhere, Category="Ammo", meta = (ClampMin = 0, ClampMax = 500))
    int32 ReserveAmmo = 90;

    /** Animation montage to play when firing this weapon */
    UPROPERTY(EditAnywhere, Category="Animation")
    UAnimMontage* FiringMontage;

    /** AnimInstance class to set for the first person character mesh when this weapon is active */
    UPROPERTY(EditAnywhere, Category="Animation")
    TSubclassOf<UAnimInstance> FirstPersonAnimInstanceClass;

    /** AnimInstance class to set for the third person character mesh when this weapon is active */
    UPROPERTY(EditAnywhere, Category="Animation")
    TSubclassOf<UAnimInstance> ThirdPersonAnimInstanceClass;

    /** Cone half-angle for variance while aiming */
    UPROPERTY(EditAnywhere, Category="Aim", meta = (ClampMin = 0, ClampMax = 90, Units = "Degrees"))
    float AimVariance = 0.0f;

    /** Amount of firing recoil to apply to the owner */
    UPROPERTY(EditAnywhere, Category="Aim", meta = (ClampMin = 0, ClampMax = 100))
    float FiringRecoil = 0.0f;

    /** Name of the first person muzzle socket where projectiles will spawn */
    UPROPERTY(EditAnywhere, Category="Aim")
    FName MuzzleSocketName;

    /** Distance ahead of the muzzle that bullets will spawn at */
    UPROPERTY(EditAnywhere, Category="Aim", meta = (ClampMin = 0, ClampMax = 1000, Units = "cm"))
    float MuzzleOffset = 10.0f;

    /** If true, this weapon will automatically fire at the refire rate */
    UPROPERTY(EditAnywhere, Category="Refire")
    bool bFullAuto = false;

    /** Fire the weapon */
    virtual void Fire();

    /** Called when the refire rate time has passed while shooting semi auto weapons */
    void FireCooldownExpired() const;

    /** Fire a projectile towards the target location */
    virtual void FireProjectile(const FVector& TargetLocation);

    /** Calculates the spawn transform for projectiles shot by this weapon */
    FTransform CalculateProjectileSpawnTransform(const FVector& TargetLocation) const;
    
    /** Gameplay initialization */
    virtual void BeginPlay() override;

    /** Gameplay Clean-up */
    virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
    
    /** Called when the weapon's owner is destroyed */
    UFUNCTION()
    void OnOwnerDestroyed(AActor* DestroyedActor);

    /** If true, this weapon behaves like a shotgun (multiple pellets per shot) */
    UPROPERTY(EditAnywhere, Category="Shotgun")
    bool bIsShotgun = false;

    /** Number of pellets fired per shotgun shot */
    UPROPERTY(EditAnywhere, Category="Shotgun", meta = (ClampMin = 1, ClampMax = 50))
    int32 PelletsPerShot = 8;

    /** Additional spread for shotgun pellets (degrees) */
    UPROPERTY(EditAnywhere, Category="Shotgun", meta = (ClampMin = 0, ClampMax = 30, Units = "Degrees"))
    float ShotgunSpread = 5.0f;

    /** Sound to play when firing this weapon */
    UPROPERTY(EditAnywhere, Category="Effects")
    USoundBase* FireSound;

    /** Sound to play when attempting to fire with an empty magazine */
    UPROPERTY(EditAnywhere, Category="Effects")
    USoundBase* DryFireSound;

    /** Sound to play when reloading */
    UPROPERTY(EditAnywhere, Category="Effects")
    USoundBase* ReloadSound;

    /** Muzzle flash particle system */
    UPROPERTY(EditAnywhere, Category="Effects")
    UParticleSystem* MuzzleFlash;

    /** Time between shots for this weapon */
    UPROPERTY(EditAnywhere, Category="Refire", meta = (ClampMin = 0, ClampMax = 5, Units = "s"))
    float RefireRate = 0.5f;

    /** Game time of last shot fired */
    float TimeOfLastShot = 0.0f;

    /** If true, the weapon is currently firing */
    bool bIsFiring = false;

    /** If true, the weapon is currently reloading */
    bool bIsReloading = false;

    /** Timer to handle full auto refiring */
    FTimerHandle RefireTimer;

    /** Timer to handle reload */
    FTimerHandle ReloadTimer;

    /** Reload duration */
    UPROPERTY(EditAnywhere, Category="Reload", meta = (ClampMin = 0, ClampMax = 10))
    float ReloadTime = 1.5f;

    /** Cast pawn pointer to the owner */
    UPROPERTY()
    TObjectPtr<APawn> PawnOwner;

public: 

    /** Constructor */
    AShooterWeapon();
    
    /** Activates this weapon and gets it ready to fire */
    void ActivateWeapon();

    /** Deactivates this weapon */
    void DeactivateWeapon();

    /** Start firing this weapon */
    void StartFiring();

    /** Stop firing this weapon */
    void StopFiring();

    /** Start reloading */
    void Reload();

    /** Finish reloading */
    void FinishReload();
    
    /** Returns the first person mesh */
    UFUNCTION(BlueprintPure, Category="Weapon")
    USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; };

    /** Returns the third person mesh */
    UFUNCTION(BlueprintPure, Category="Weapon")
    USkeletalMeshComponent* GetThirdPersonMesh() const { return ThirdPersonMesh; };

    /** Returns the first person anim instance class */
    const TSubclassOf<UAnimInstance>& GetFirstPersonAnimInstanceClass() const;

    /** Returns the third person anim instance class */
    const TSubclassOf<UAnimInstance>& GetThirdPersonAnimInstanceClass() const;

    /** Returns the magazine size */
    int32 GetMagazineSize() const { return MagazineSize; };

    /** Returns the current bullet count */
    int32 GetBulletCount() const { return CurrentBullets; }
};
