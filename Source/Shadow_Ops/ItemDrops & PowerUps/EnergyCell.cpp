#include "EnergyCell.h"
#include "Character/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AEnergyCell::AEnergyCell()
{
    PrimaryActorTick.bCanEverTick = true;
    
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
    RootComponent = MeshComponent;
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
    CollectionSphere->SetupAttachment(MeshComponent);
    CollectionSphere->SetSphereRadius(CollectionRadius);
    CollectionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    CollectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnergyCell::OnOverlapBegin);
}

void AEnergyCell::BeginPlay()
{
    Super::BeginPlay();
    
    StartLocation = GetActorLocation();
    SpawnTime = GetWorld()->GetTimeSeconds();
    
    SetLifeSpan(LifeTime);
}

void AEnergyCell::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // bobbing motion
    float Time = static_cast<float>(GetWorld()->GetTimeSeconds()) - SpawnTime;
    FVector NewLocation = StartLocation;
    NewLocation.Z += FMath::Sin(Time * BobSpeed) * BobHeight;
    SetActorLocation(NewLocation);

    // despawn warning: start flashing when nearing end of lifetime
    if (const float TimeRemaining = LifeTime - Time; TimeRemaining <= DespawnWarningTime && !bIsFlashing)
    {
        bIsFlashing = true;

        // Start flashing timer
        GetWorldTimerManager().SetTimer(
            FlashTimerHandle,
            this,
            &AEnergyCell::ToggleVisibility,
            1.0f / DespawnFlashSpeed,   
            true                        
        );
    }
}

void AEnergyCell::ToggleVisibility() const
{
    MeshComponent->SetVisibility(!MeshComponent->IsVisible());
}

void AEnergyCell::OnOverlapBegin(UPrimitiveComponent* OverLappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Collect(OtherActor);
}

void AEnergyCell::Collect(AActor* Collector)
{
    if (AShooterCharacter* Player = Cast<AShooterCharacter>(Collector))
    {
        if (CollectionSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, CollectionSound, GetActorLocation());
        }

        // Stop flashing timer before destruction
        GetWorldTimerManager().ClearTimer(FlashTimerHandle);

        Destroy();
    }
}

void AEnergyCell::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Ensure timer is cleared even if destroyed externally
    GetWorldTimerManager().ClearTimer(FlashTimerHandle);

    Super::EndPlay(EndPlayReason);
}
