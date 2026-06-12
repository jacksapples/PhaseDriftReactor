// PhaseDriftReactorModule.h
// Header for the runtime impl in PhaseDriftReactorModule.cpp.

#pragma once

#include "CoreMinimal.h"
#include "Buildables/FGBuildableGenerator.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PhaseDriftReactorModule.generated.h"

UCLASS()
class UPhaseDriftReactorStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "PhaseDriftReactor")
    static float ComputePowerMW(class AFGBuildablePhaseDriftReactor* Reactor, float DeltaTime);
};

UCLASS(Abstract, Blueprintable)
class AFGBuildablePhaseDriftReactor : public AFGBuildableGenerator
{
    GENERATED_BODY()

public:
    AFGBuildablePhaseDriftReactor();

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhaseDriftReactor")
    int32 mReactorTier;

    UPROPERTY()
    float SoleProducerSeconds;

    UPROPERTY()
    float SmoothedOutputMW;

    UFUNCTION(BlueprintCallable, Category = "PhaseDriftReactor")
    int32 GetReactorTier() const { return mReactorTier; }

    UFUNCTION(BlueprintCallable, Category = "PhaseDriftReactor")
    void SetReactorTier(int32 InTier) { mReactorTier = FMath::Clamp(InTier, 0, 3); }
};
