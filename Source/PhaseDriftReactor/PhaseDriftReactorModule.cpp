// PhaseDriftReactorModule.cpp
// Phase Drift Reactor — runtime implementation.
//
// Implements:
//   - AFGBuildablePhaseDriftReactor (the building class)
//   - UPhaseDriftReactorStatics (the entropy algorithm)
//
// Built against Satisfactory 1.2 / SML 3.12. The C++ source compiles
// into PhaseDriftReactor.dll via UnrealBuildTool (Alpine SDK 1.2 /
// Visual Studio 2022 / clang).

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "Patching/NativeHookManager.h"
#include "Buildables/FGBuildableGenerator.h"
#include "Buildables/FGBuildableFactory.h"
#include "Buildables/FGBuildableConveyorBelt.h"
#include "Buildables/FGBuildablePowerPole.h"
#include "Power/FGPowerCircuit.h"
#include "Power/FGPowerInfo.h"
#include "Resources/FGItemDescriptor.h"
#include "FICSITNetworkModule.h"

class AFGBuildablePhaseDriftReactor;
class UPhaseDriftReactorStatics;

// =============================================================================
// UPhaseDriftReactorStatics — the entropy algorithm in C++.
// =============================================================================

UCLASS()
class UPhaseDriftReactorStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static constexpr float ENTROPY_RADIUS_M              = 9600.0f;
    static constexpr float PHASELOCK_RADIUS_M            = 25600.0f;
    static constexpr float ENTROPY_EVENT_WEIGHT          = 0.5f;
    static constexpr float DARKNESS_MULTIPLIER           = 1.6f;
    static constexpr float PHASELOCK_BONUS[]             = {1.0f, 1.45f, 1.85f, 2.20f, 2.40f, 2.50f};
    static constexpr float SOLE_PRODUCER_GRACE_SECONDS   = 1800.0f;
    static constexpr float TIER_CAPS_MW[]                = {50.0f, 200.0f, 500.0f, 1000.0f};

    UFUNCTION(BlueprintCallable, Category = "PhaseDriftReactor")
    static float ComputePowerMW(
        AFGBuildablePhaseDriftReactor* Reactor,
        float DeltaTime)
    {
        if (!IsValid(Reactor) || !IsValid(Reactor->GetWorld()))
            return 0.0f;

        // --- STEP 1: Entropy event count ---
        int32 Count = 0;
        const FVector Origin = Reactor->GetActorLocation();

        TArray<AActor*> Nearby;
        Reactor->GetWorld()->OverlapMultiByChannel(
            Nearby,
            Origin,
            FQuat::Identity,
            ECC_WorldDynamic,
            FCollisionShape::MakeSphere(ENTROPY_RADIUS_M),
            FCollisionQueryParams::DefaultQueryParam);

        for (AActor* A : Nearby)
        {
            if (auto* B = Cast<AFGBuildableFactory>(A))
            {
                if (B == Reactor) continue;  // anti self-count
                if (B->IsA<AFGBuildablePhaseDriftReactor>()) continue;  // PDRs do not self-count
                if (B->GetIsProductionActive())
                {
                    Count += 1;
                }
                else if (auto* Belt = Cast<AFGBuildableConveyorBelt>(B))
                {
                    // Count the belt if it has moved in the last tick.
                    if (Belt->HasMovedRecently(1.0f))
                        Count += 1;
                }
                else if (B->IsA<AFGBuildablePowerPole>())
                {
                    Count += 1;
                }
            }
        }

        // Tier-3 bonus: Time Crystal in any connected storage nearby.
        if (Reactor->GetReactorTier() == 3 &&
            Reactor->HasTimeCrystalInNearbyStorage(ENTROPY_RADIUS_M))
        {
            Count += 25;
        }

        const float EventsPerSec = Count / FMath::Max(DeltaTime, 0.001f);
        float baselineMW = EventsPerSec * ENTROPY_EVENT_WEIGHT;
        baselineMW = FMath::Max(baselineMW, 2.0f);

        // --- STEP 2: Darkness bonus ---
        if (!Reactor->HasLineOfSightToOpenSky())
        {
            baselineMW *= DARKNESS_MULTIPLIER;
        }

        // --- STEP 3: Phase-lock multiplier ---
        const int32 Peers = Reactor->CountPhaseLockedPeers();
        int32 BonusIdx = FMath::Clamp(Peers, 0, 5);
        baselineMW *= PHASELOCK_BONUS[BonusIdx];

        // --- STEP 4: Sole-producer penalty ---
        UFGPowerCircuit* Circuit = Reactor->GetPowerCircuit();
        if (IsValid(Circuit))
        {
            const TArray<UFGPowerInfo*>& Producers = Circuit->GetProducers();
            const bool bSole =
                Producers.Num() == 1 &&
                Producers[0] && Producers[0]->GetOwner() == Reactor;

            if (bSole)
            {
                Reactor->SoleProducerSeconds += DeltaTime;
                if (Reactor->SoleProducerSeconds > SOLE_PRODUCER_GRACE_SECONDS)
                {
                    const float Over = Reactor->SoleProducerSeconds - SOLE_PRODUCER_GRACE_SECONDS;
                    const float Ramp = FMath::Clamp(1.0f - (Over / 3600.0f), 0.0f, 1.0f);
                    baselineMW *= Ramp;
                }
            }
            else
            {
                Reactor->SoleProducerSeconds = 0.0f;
            }
        }

        // --- STEP 5: Tier cap ---
        const int32 Tier = FMath::Clamp(Reactor->GetReactorTier(), 0, 3);
        baselineMW = FMath::Min(baselineMW, TIER_CAPS_MW[Tier]);
        baselineMW = FMath::Max(baselineMW, 2.0f);

        // --- STEP 6: Smoothed output ---
        Reactor->SmoothedOutputMW = FMath::Lerp(
            Reactor->SmoothedOutputMW, baselineMW, 0.15f);
        return Reactor->SmoothedOutputMW;
    }
};

// =============================================================================
// AFGBuildablePhaseDriftReactor — the building itself.
// =============================================================================

UCLASS(Abstract, Blueprintable)
class AFGBuildablePhaseDriftReactor : public AFGBuildableGenerator
{
    GENERATED_BODY()

public:
    AFGBuildablePhaseDriftReactor()
    {
        // We do our own power production in Tick. Set the base
        // capacity to 0 — overridden every tick.
        mPowerProductionCapacity = 0.0f;
        mPowerProductionEfficiency = 1.0f;
    }

    virtual void Tick(float DeltaTime) override
    {
        Super::Tick(DeltaTime);

        if (!HasAuthority()) return;

        const float NewMW = UPhaseDriftReactorStatics::ComputePowerMW(this, DeltaTime);
        SetTargetPowerProduction(NewMW);
    }

    int32 GetReactorTier() const { return mReactorTier; }
    void  SetReactorTier(int32 InTier) { mReactorTier = FMath::Clamp(InTier, 0, 3); }

    float SoleProducerSeconds = 0.0f;
    float SmoothedOutputMW   = 0.0f;

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhaseDriftReactor")
    int32 mReactorTier = 0;

    UPROPERTY()
    bool bLOSCheckedThisFrame = false;

    UPROPERTY()
    bool bHasLOSOpenSky = true;

    bool HasLineOfSightToOpenSky() const
    {
        // Cast a ray straight up; if it hits world-static geometry
        // within 500m, we're "dark" (underground / enclosed).
        FHitResult Hit;
        FCollisionQueryParams Params(SCENE_QUERY_STAT(PDR_LOS), false, this);
        const bool bBlocked = GetWorld()->LineTraceSingleByChannel(
            Hit,
            GetActorLocation(),
            GetActorLocation() + FVector(0, 0, 50000.0f),
            ECC_WorldStatic,
            Params);
        return !bBlocked;
    }

    int32 CountPhaseLockedPeers() const
    {
        TArray<AActor*> Nearby;
        GetWorld()->OverlapMultiByChannel(
            Nearby,
            GetActorLocation(),
            FQuat::Identity,
            ECC_WorldDynamic,
            FCollisionShape::MakeSphere(UPhaseDriftReactorStatics::PHASELOCK_RADIUS_M),
            FCollisionQueryParams::DefaultQueryParam);
        int32 Count = 0;
        for (AActor* A : Nearby)
        {
            if (auto* Other = Cast<AFGBuildablePhaseDriftReactor>(A))
            {
                if (Other != this) Count += 1;
            }
        }
        return Count;
    }

    bool HasTimeCrystalInNearbyStorage(float RadiusCM) const
    {
        // Scan for any UFGInventoryComponent within RadiusCM and
        // check if it contains Desc_TimeCrystal. Implementation
        // detail: iterate via GetWorld()->GetCurrentLevel()->Actors
        // for AFGBuildableStorage or AFGBuildableManufacturer.
        // (Skipped in stub — see Resources/POWER_ALGORITHM.txt for
        //  the full procedural spec.)
        return false;
    }
};

// =============================================================================
// Module entry point.
// =============================================================================

class FPhaseDriftReactorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("[PhaseDriftReactor] loaded"));
    }
    virtual void ShutdownModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("[PhaseDriftReactor] unloaded"));
    }
};

IMPLEMENT_MODULE(FPhaseDriftReactorModule, PhaseDriftReactor)
