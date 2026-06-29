// Copyright UnrealEssence. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "TypingEffectWidget.generated.h"

UCLASS()
class UNREALESSENCELITE_API UTypingEffectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypingEffect")
	void StartTypingEffect();

	UFUNCTION(BlueprintCallable, Category = "TypingEffect")
	void StopTypingEffect();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayText; // Привязать в BP

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float TypingInterval = 0.05f; // Скорость появления символов

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 MaxLineLength = 40; // Длина строки до сброса

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString AllowedChars = TEXT("ABCD1234!@#$"); // Можно "01" или "ABCD1234!@#$"

private:
	FString CurrentLine;
	FTimerHandle TypingTimerHandle;

	void AddRandomCharacter();
	void UpdateDisplay();
};