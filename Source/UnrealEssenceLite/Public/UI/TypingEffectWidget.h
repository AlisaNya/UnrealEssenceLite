// Copyright UnrealEssence. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "TypingEffectWidget.generated.h"

UENUM(BlueprintType)
enum class ETypingDirection : uint8
{
	LeftToRight UMETA(DisplayName = "Слева -> направо"),
	RightToLeft UMETA(DisplayName = "Справа <- налево")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllAllowedCharsIsEndTypingEvent); // в упорядоченном режиме уведомляет о завершении печати переданного набора символов / строки

/**
 * @example
 * - Создайте виджет-блюпринт на основе UTypingEffectWidget.
 * - Добавьте в него TextBlock и назовите DisplayText (строго как в коде).
 * - В Event Construct или по кнопке вызовите Start Typing Effect.
 */
UCLASS()
class UNREALESSENCELITE_API UTypingEffectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	// События
	UPROPERTY(BlueprintAssignable, Category = "TypingEffect")
	FOnAllAllowedCharsIsEndTypingEvent OnAllAllowedCharsIsEndTyping;
	
	UFUNCTION(BlueprintCallable, Category = "TypingEffect")
	void StartTypingEffect(bool bRandomize = true, bool bStartFromClear = false);

	UFUNCTION(BlueprintCallable, Category = "TypingEffect")
	void StopTypingEffect(bool bClear = false);

	UFUNCTION(BlueprintCallable, Category = "TypingEffect")
	void ChangeDirection();

	UFUNCTION(BlueprintCallable, Category = "TypingEffect")
	void SetDirection(ETypingDirection NewDirection);
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayText; // Привязать в UI

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float TypingInterval = 0.05f; // Скорость появления символов

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 MaxLineLength = 40; // Длина строки до сброса

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString AllowedChars = TEXT("ABCD1234!@#$"); // Набор символов для печати "ABCD1234!@#$" (читается по порядку либо рандомно)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	ETypingDirection CurrentDirection = ETypingDirection::LeftToRight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bRandomIsActive = false; // рандомизация
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bJustifyIsAuto = false; // автоматизация управления выравниванием текста
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bUseAutoStopTyping = false; // автоматическая остановка печати по окончании вывода набора символов
	
private:
	FString CurrentLine;
	FTimerHandle TypingTimerHandle;
	int32 CharIndex = 0;
	
	void AddCharacter();
	void UpdateDisplay();
	
	void AddCharacterLeftToRight(TCHAR NewChar);
	void AddCharacterRightToLeft(TCHAR NewChar);
	void UpdateJustify();
};