#include "ShooterWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "ShooterProjectile.h"
#include "ShooterWeaponHolder.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Character/ShooterCharacter.h"

AShooterWeapon::AShooterWeapon()
{
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
    FirstPersonMesh->SetupAttachment(RootComponent);
    FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
    FirstPersonMesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::FirstPerson);
    FirstPersonMesh->bOnlyOwnerSee = true;

    ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Third Person Mesh"));
    ThirdPersonMesh->SetupAttachment(RootComponent);
    ThirdPersonMesh->SetCollisionProfileName(FName("NoCollision"));
    ThirdPersonMesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::WorldSpaceRepresentation);
    ThirdPersonMesh->bOwnerNoSee = true;
}

void AShooterWeapon::BeginPlay()
{
    Super::BeginPlay();

    GetOwner()->OnDestroyed.AddDynamic(this, &AShooterWeapon::OnOwnerDestroyed);

    WeaponOwner = Cast<IShooterWeaponHolder>(GetOwner());
    PawnOwner = Cast<APawn>(GetOwner());

    CurrentBullets = MagazineSize;

    WeaponOwner->AttachWeaponMeshes(this);
}

void AShooterWeapon::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorld()->GetTimerManager().ClearTimer(RefireTimer);
}

void AShooterWeapon::OnOwnerDestroyed(AActor* DestroyedActor)
{
    Destroy();
}

void AShooterWeapon::ActivateWeapon()
{
    SetActorHiddenInGame(false);
    WeaponOwner->OnWeaponActivated(this);
}

void AShooterWeapon::DeactivateWeapon()
{
    StopFiring();
    SetActorHiddenInGame(true);
    WeaponOwner->OnWeaponDeactivated(this);
}

void AShooterWeapon::StartFiring()
{
    if (bIsReloading)
        return;

    if (CurrentBullets <= 0)
    {
        if (DryFireSound && PawnOwner)
        {
            UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, PawnOwner->GetActorLocation());
        }
        return;
    }

    bIsFiring = true;

    const float TimeSinceLastShot = GetWorld()->GetTimeSeconds() - TimeOfLastShot;

    if (TimeSinceLastShot > RefireRate)
    {
        Fire();
    }
    else if (bFullAuto)
    {
        GetWorld()->GetTimerManager().SetTimer(RefireTimer, this, &AShooterWeapon::Fire, TimeSinceLastShot, false);
    }
}

void AShooterWeapon::StopFiring()
{
    bIsFiring = false;
    GetWorld()->GetTimerManager().ClearTimer(RefireTimer);
}

void AShooterWeapon::Fire()
{
    if (bIsReloading)
        return;

    if (CurrentBullets <= 0)
    {
        if (DryFireSound && PawnOwner)
        {
            UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, PawnOwner->GetActorLocation());
        }

        StopFiring();
        return;
    }

    if (!bIsFiring)
        return;

    FireProjectile(WeaponOwner->GetWeaponTargetLocation());

    TimeOfLastShot = GetWorld()->GetTimeSeconds();

    if (bFullAuto)
    {
        GetWorld()->GetTimerManager().SetTimer(RefireTimer, this, &AShooterWeapon::Fire, RefireRate, false);
    }
    else
    {
        GetWorld()->GetTimerManager().SetTimer(RefireTimer, this, &AShooterWeapon::FireCooldownExpired, RefireRate, false);
    }
}

void AShooterWeapon::FireCooldownExpired() const
{
    WeaponOwner->OnSemiWeaponRefire();
}

void AShooterWeapon::FireProjectile(const FVector& TargetLocation)
{
    FTransform ProjectileTransform = CalculateProjectileSpawnTransform(TargetLocation);

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;
    SpawnParams.Owner = GetOwner();
    SpawnParams.Instigator = PawnOwner;

    if (bIsShotgun)
    {
        const FVector MuzzleLoc = FirstPersonMesh->GetSocketLocation(MuzzleSocketName);

        for (int32 i = 0; i < PelletsPerShot; ++i)
        {
            const FVector Dir = (TargetLocation - MuzzleLoc).GetSafeNormal();
            const FRotator BaseRot = Dir.Rotation();

            const float YawOffset = FMath::RandRange(-ShotgunSpread, ShotgunSpread);
            const float PitchOffset = FMath::RandRange(-ShotgunSpread, ShotgunSpread);

            const FRotator PelletRot = BaseRot + FRotator(PitchOffset, YawOffset, 0.0f);
            const FVector PelletDir = PelletRot.Vector();
            const FVector PelletTarget = MuzzleLoc + PelletDir * 10000.0f;

            FTransform PelletTransform = CalculateProjectileSpawnTransform(PelletTarget);
            GetWorld()->SpawnActor<AShooterProjectile>(ProjectileClass, PelletTransform, SpawnParams);
        }
    }
    else
    {
        GetWorld()->SpawnActor<AShooterProjectile>(ProjectileClass, ProjectileTransform, SpawnParams);
    }

    WeaponOwner->PlayFiringMontage(FiringMontage);
    WeaponOwner->AddWeaponRecoil(FiringRecoil);

    if (FireSound && PawnOwner)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, PawnOwner->GetActorLocation());
    }

    if (MuzzleFlash && FirstPersonMesh)
    {
        UGameplayStatics::SpawnEmitterAttached(
            MuzzleFlash,
            FirstPersonMesh,
            MuzzleSocketName,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTarget,
            true
        );
    }

    --CurrentBullets;

    if (AShooterCharacter* Char = Cast<AShooterCharacter>(PawnOwner))
    {
        Char->OnAmmoChanged.Broadcast(CurrentBullets, MagazineSize);
    }
}

void AShooterWeapon::Reload()
{
    if (bIsReloading)
        return;

    if (CurrentBullets >= MagazineSize)
        return;

    if (ReserveAmmo <= 0)
        return;

    bIsReloading = true;
    StopFiring();

    if (ReloadSound && PawnOwner)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, PawnOwner->GetActorLocation());
    }

    GetWorld()->GetTimerManager().SetTimer(
        ReloadTimer,
        this,
        &AShooterWeapon::FinishReload,
        ReloadTime,
        false
    );
}

void AShooterWeapon::FinishReload()
{
    bIsReloading = false;

    const int32 NeededAmmo = MagazineSize - CurrentBullets;
    const int32 AmmoToLoad = FMath::Min(NeededAmmo, ReserveAmmo);

    CurrentBullets += AmmoToLoad;
    ReserveAmmo -= AmmoToLoad;

    if (AShooterCharacter* Player = Cast<AShooterCharacter>(PawnOwner))
    {
        Player->OnAmmoChanged.Broadcast(CurrentBullets, MagazineSize);
    }
}

FTransform AShooterWeapon::CalculateProjectileSpawnTransform(const FVector& TargetLocation) const
{
    const FVector MuzzleLoc = FirstPersonMesh->GetSocketLocation(MuzzleSocketName);
    const FVector SpawnLoc = MuzzleLoc + ((TargetLocation - MuzzleLoc).GetSafeNormal() * MuzzleOffset);

    const FRotator AimRot = UKismetMathLibrary::FindLookAtRotation(
        SpawnLoc,
        TargetLocation + (UKismetMathLibrary::RandomUnitVector() * AimVariance)
    );

    return FTransform(AimRot, SpawnLoc, FVector::OneVector);
}

const TSubclassOf<UAnimInstance>& AShooterWeapon::GetFirstPersonAnimInstanceClass() const
{
    return FirstPersonAnimInstanceClass;
}

const TSubclassOf<UAnimInstance>& AShooterWeapon::GetThirdPersonAnimInstanceClass() const
{
    return ThirdPersonAnimInstanceClass;
}
