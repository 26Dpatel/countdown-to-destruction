#include "ShooterCharacter.h"
#include "Weapons/ShooterWeapon.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Components/HealthComponent.h"
#include "Components/HeartRateComponent.h"

AShooterCharacter::AShooterCharacter()
{
    PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Pawn Noise Emitter"));
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    HeartRateComponent = CreateDefaultSubobject<UHeartRateComponent>(TEXT("HeartRateComponent"));
}

void AShooterCharacter::BeginPlay()
{
    Super::BeginPlay();
}


void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AShooterCharacter::DoStartFiring);
        EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AShooterCharacter::DoStopFiring);
        EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Triggered, this, &AShooterCharacter::DoSwitchWeapon);
        EnhancedInputComponent->BindAction(DebugTakeDamageAction, ETriggerEvent::Started, this, &AShooterCharacter::DoDebugTakeDamage);
        EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AShooterCharacter::DoReload);
    }
}


void AShooterCharacter::ReceiveDamage(float DamageAmount, AActor* DamagingActor)
{
    if (!HealthComponent)
    {
        return;
    }

    // Apply damage through your HealthComponent
    HealthComponent->TakeDamage(DamageAmount);

    // Optional: heart rate spike
    if (HeartRateComponent)
    {
        HeartRateComponent->OnDamageTaken(DamageAmount);
    }

    // If dead, handle death
    if (HealthComponent->IsDead())
    {
        // Stop firing weapon
        if (CurrentWeapon)
        {
            CurrentWeapon->StopFiring();
        }

        if (AController* PlayerController = GetController())
        {
            PlayerController->DisableInput(nullptr);
        }

        // Enable ragdoll physics
        GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
        GetMesh()->SetSimulatePhysics(true);
        
        HealthComponent->OnDeath.Broadcast();

        // Destroy after delay
        SetLifeSpan(5.0f);
    }
}


void AShooterCharacter::DoDebugTakeDamage()
{
    ReceiveDamage(10.0f, this);
}

void AShooterCharacter::DoStartFiring()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->StartFiring();
    }
}

void AShooterCharacter::DoStopFiring()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->StopFiring();
    }
}

void AShooterCharacter::DoSwitchWeapon()
{
    if (OwnedWeapons.Num() > 1)
    {
        CurrentWeapon->DeactivateWeapon();

        int32 WeaponIndex = OwnedWeapons.Find(CurrentWeapon);

        if (WeaponIndex == OwnedWeapons.Num() - 1)
        {
            WeaponIndex = 0;
        }
        else
        {
            ++WeaponIndex;
        }

        CurrentWeapon = OwnedWeapons[WeaponIndex];
        CurrentWeapon->ActivateWeapon();
    }
}

void AShooterCharacter::DoReload()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->Reload();
    }
}

void AShooterCharacter::AttachWeaponMeshes(AShooterWeapon* Weapon)
{
    const FAttachmentTransformRules AttachmentRule(EAttachmentRule::SnapToTarget, false);

    Weapon->AttachToActor(this, AttachmentRule);
    Weapon->GetFirstPersonMesh()->AttachToComponent(GetFirstPersonMesh(), AttachmentRule, FirstPersonWeaponSocket);
    Weapon->GetThirdPersonMesh()->AttachToComponent(GetMesh(), AttachmentRule, ThirdPersonWeaponSocket);
}

void AShooterCharacter::PlayFiringMontage(UAnimMontage* Montage)
{
}

void AShooterCharacter::AddWeaponRecoil(float Recoil)
{
    AddControllerPitchInput(Recoil);
}

FVector AShooterCharacter::GetWeaponTargetLocation()
{
    FHitResult OutHit;

    const FVector Start = GetFirstPersonCameraComponent()->GetComponentLocation();
    const FVector End = Start + (GetFirstPersonCameraComponent()->GetForwardVector() * MaxAimDistance);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, QueryParams);

    return OutHit.bBlockingHit ? OutHit.ImpactPoint : OutHit.TraceEnd;
}

void AShooterCharacter::AddWeaponClass(const TSubclassOf<AShooterWeapon>& WeaponClass)
{
    if (const AShooterWeapon* OwnedWeapon = FindWeaponOfType(WeaponClass); !OwnedWeapon)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        if (AShooterWeapon* AddedWeapon = GetWorld()->SpawnActor<AShooterWeapon>(WeaponClass, GetActorTransform(), SpawnParams))
        {
            OwnedWeapons.Add(AddedWeapon);

            if (CurrentWeapon)
            {
                CurrentWeapon->DeactivateWeapon();
            }

            CurrentWeapon = AddedWeapon;
            CurrentWeapon->ActivateWeapon();
        }
    }
}

void AShooterCharacter::OnWeaponActivated(AShooterWeapon* Weapon)
{
    GetFirstPersonMesh()->SetAnimInstanceClass(Weapon->GetFirstPersonAnimInstanceClass());
    GetMesh()->SetAnimInstanceClass(Weapon->GetThirdPersonAnimInstanceClass());

    OnAmmoChanged.Broadcast(Weapon->GetBulletCount(), Weapon->GetMagazineSize());
}

void AShooterCharacter::OnWeaponDeactivated(AShooterWeapon* Weapon)
{
}

void AShooterCharacter::OnSemiWeaponRefire()
{
}

AShooterWeapon* AShooterCharacter::FindWeaponOfType(TSubclassOf<AShooterWeapon> WeaponClass) const
{
    for (AShooterWeapon* Weapon : OwnedWeapons)
    {
        if (Weapon->IsA(WeaponClass))
        {
            return Weapon;
        }
    }

    return nullptr;
}

void AShooterCharacter::HandleEnemyKilled(AActor* Enemy)
{
    KillCount++;
    OnKillCountChanged.Broadcast(KillCount);
    UE_LOG(LogTemp, Warning, TEXT("Kill Count Updated: %d"), KillCount);
}
