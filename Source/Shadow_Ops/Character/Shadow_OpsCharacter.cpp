#include "Shadow_OpsCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Shadow_Ops.h"

AShadow_OpsCharacter::AShadow_OpsCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
    
    // Create the first person mesh
    FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
    FirstPersonMesh->SetupAttachment(GetMesh());
    FirstPersonMesh->SetOnlyOwnerSee(true);
    FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
    FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

    // Create the Camera Component  
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
    FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
    FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
    FirstPersonCameraComponent->bUsePawnControlRotation = true;
    FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
    FirstPersonCameraComponent->bEnableFirstPersonScale = true;
    FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
    FirstPersonCameraComponent->FirstPersonScale = 0.6f;

    // configure the character comps
    GetMesh()->SetOwnerNoSee(true);
    GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

    GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

    // Configure character movement
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
    GetCharacterMovement()->AirControl = 0.5f;

    // Set default walk speed
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AShadow_OpsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{   
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AShadow_OpsCharacter::DoJumpStart);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AShadow_OpsCharacter::DoJumpEnd);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShadow_OpsCharacter::MoveInput);

        // Looking/Aiming
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShadow_OpsCharacter::LookInput);
        EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AShadow_OpsCharacter::LookInput);

        // Sprinting
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AShadow_OpsCharacter::DoSprintStart);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AShadow_OpsCharacter::DoSprintEnd);
    }
    else
    {
        UE_LOG(LogShadow_Ops, Error, TEXT("'%s' Failed to find an Enhanced Input Component!"), *GetNameSafe(this));
    }
}

void AShadow_OpsCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsSprinting)
    {
        Stamina -= StaminaDrainRate * DeltaSeconds;
        Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);

        if (Stamina <= 0.0f)
        {
            DoSprintEnd();
        }
    }
    else
    {
        if (Stamina < MaxStamina)
        {
            Stamina += StaminaRegenRate * DeltaSeconds;
            Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
        }
    }

    // Broadcast stamina changed (matching health delegate)
    OnStaminaChanged.Broadcast(Stamina, MaxStamina);
}


float AShadow_OpsCharacter::GetStaminaPercent() const
{
    return (MaxStamina > 0.f) ? (Stamina / MaxStamina) : 0.f;
}

void AShadow_OpsCharacter::MoveInput(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    DoMove(MovementVector.X, MovementVector.Y);
}

void AShadow_OpsCharacter::LookInput(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    DoAim(LookAxisVector.X, LookAxisVector.Y);
}

void AShadow_OpsCharacter::DoAim(float Yaw, float Pitch)
{
    if (GetController())
    {
        AddControllerYawInput(Yaw);
        AddControllerPitchInput(Pitch);
    }
}

void AShadow_OpsCharacter::DoMove(float Right, float Forward)
{
    if (GetController())
    {
        AddMovementInput(GetActorRightVector(), Right);
        AddMovementInput(GetActorForwardVector(), Forward);
    }
}

void AShadow_OpsCharacter::DoJumpStart()
{
    Jump();
}

void AShadow_OpsCharacter::DoJumpEnd()
{
    StopJumping();
}

void AShadow_OpsCharacter::DoSprintStart()
{
    if (Stamina > 0.0f)
    {
        bIsSprinting = true;
        GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    }
}

void AShadow_OpsCharacter::DoSprintEnd()
{
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
