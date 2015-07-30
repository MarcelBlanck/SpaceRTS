

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
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerPawnChanged);

public:
	UGameInstanceSpaceRTS(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerOptions)
	bool GameJustStarted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerOptions)
	bool ShowPlayerPawnBody;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerOptions)
	EPlayerBodyPosition PlayerBodyPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerOptions)
	float InterpupillaryDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerOptions)
	FLinearColor GuiColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerOptions)
	FLinearColor GuiHighlightColor;

	UFUNCTION(BlueprintCallable, Category = Updating)
	void NotifyOnPlayerPawnChanged();

	UPROPERTY(BlueprintAssignable)
	FOnPlayerPawnChanged OnPlayerPawnChanged;
};
