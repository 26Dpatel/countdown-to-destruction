#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Shadow_OpsCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class USoundBase;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, CurrentStamina, float, MaxStamina);

UCLASS(abstract)
class AShadow_OpsCharacter : public ACharacter
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* FirstPersonMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCameraComponent;
    
    UPROPERTY(BlueprintAssignable, Category="Stamina|Events")
    FOnStaminaChanged OnStaminaChanged;

public:
    AShadow_OpsCharacter();

    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
    virtual void Tick(float DeltaSeconds) override;

    USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
    UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

    UFUNCTION(BlueprintCallable, Category="Stamina")
    float GetStaminaPercent() const;

protected:

    // UI
    UPROPERTY(BlueprintReadOnly, Category = "UI")
    TObjectPtr<UUserWidget> PauseWidget;
    
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> PauseMenuClass;

    // Input Actions
    UPROPERTY(EditAnywhere, Category ="Input")
    UInputAction* JumpAction;
    
    UPROPERTY(EditAnywhere, Category ="Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, Category ="Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, Category ="Input")
    UInputAction* MouseLookAction;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* SprintAction;

    // Movement Speeds
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
    float WalkSpeed = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
    float SprintSpeed = 1000.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
    USoundBase* FootstepsSounds;

    // Footstep timing
    FTimerHandle FootstepTimerHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Footsteps")
    float WalkFootstepInterval = 0.45f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Footsteps")
    float SprintFootstepInterval = 0.30f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Footsteps")
    float SprintFootstepVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Footsteps")
    float WalkFootstepVolume = 0.4f;


    // Stamina System
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
    float Stamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
    float MaxStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
    float StaminaDrainRate = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
    float StaminaRegenRate = 25.0f;

    bool bIsSprinting = false;

    // Input Functions
    void MoveInput(const FInputActionValue& Value);
    void LookInput(const FInputActionValue& Value);

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoAim(float Yaw, float Pitch);

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoMove(float Right, float Forward);

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoJumpStart();

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoJumpEnd();

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoSprintStart();

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoSprintEnd();

    void PlayFootstepSound() const;
};
