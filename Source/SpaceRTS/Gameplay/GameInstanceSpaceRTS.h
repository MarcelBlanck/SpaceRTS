

#pragma once

#include "Engine/GameInstance.h"
#include "GameInstanceSpaceRTS.generated.h"

UENUM(BlueprintType)
enum class EPlayerBodyPosition : uint8
{
	Standing			UMETA(DisplayName = "Standing"),
	NormalChair			UMETA(DisplayName = "Normal Chair"),
	SwivelChair			UMETA(DisplayName = "Swivel Chair"),
	EmporerChair		UMETA(DisplayName = "Emporer Chair"),
};

UCLASS()
class SPACERTS_API UGameInstanceSpaceRTS : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UGameInstanceSpaceRTS(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SolusHUDCrosshair)
	bool ShowPlayerPawnBody;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SolusHUDCrosshair)
	EPlayerBodyPosition PlayerBodyPosition;
};
