

#pragma once

#include "Engine/LevelScriptActor.h"
#include "SpaceRTSLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class SPACERTS_API ASpaceRTSLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Level Data")
	FName LevelName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelData")
	int32 FrameIndex;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;	
};
