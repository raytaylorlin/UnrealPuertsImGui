#pragma once

#include "CoreMinimal.h"

#include "PuertsXImGuiBridge.generated.h"

UCLASS()
class UPuertsXImGuiBridge : public UObject
{
	GENERATED_BODY()
	
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNotifyTSOnGUI);
	
	UPROPERTY()
	FNotifyTSOnGUI OnGUI;
};
