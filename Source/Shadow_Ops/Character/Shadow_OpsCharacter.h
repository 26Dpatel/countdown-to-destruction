#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Shadow_OpsCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

// -------------------------
// Stamina Delegates
// -------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, CurrentStamina, float, MaxStamina);

UCLASS(abstract)
class AShadow_OpsCharacter : public ACharacter
{
    GENERATED_BODY()

    /** Pawn mesh: first person view (arms; seen only by self) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* FirstPersonMesh;

    /** First person camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCameraComponent;

public:
    AShadow_OpsCharacter();

    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
    virtual void Tick(float DeltaSeconds) override;

    /** Returns the first person mesh **/
    USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

    /** Returns first person camera component **/
    UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

    /** Returns stamina as a per cent (0–1) */
    UFUNCTION(BlueprintCallable, Category="Stamina")
    float GetStaminaPercent() const;

protected:

    // -------------------------
    // UI
    // -------------------------
    UPROPERTY(BlueprintReadOnly, Category = "UI")
    TObjectPtr<UUserWidget> PauseWidget;
    
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> PauseMenuClass;

    // -------------------------
    // Input Actions
    // -------------------------
    UPROPERTY(EditAnywhere, Category ="Input")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, Category ="Input")
    UInputAction* PauseGameAction;

    UPROPERTY(EditAnywhere, Category ="Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, Category ="Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, Category ="Input")
    UInputAction* MouseLookAction;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* SprintAction;

    // -------------------------
    // Movement Speeds
    // -------------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
    float WalkSpeed = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
    float SprintSpeed = 900.0f;

    // -------------------------
    // Stamina System
    // -------------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
    float Stamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
    float MaxStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
    float StaminaDrainRate = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
    float StaminaRegenRate = 10.0f;

    bool bIsSprinting = false;

    /** Stamina changed event */
    UPROPERTY(BlueprintAssignable, Category="Stamina|Events")
    FOnStaminaChanged OnStaminaChanged;
    
    // -------------------------
    // Input Functions
    // -------------------------
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
};
