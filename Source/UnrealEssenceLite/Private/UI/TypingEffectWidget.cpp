// Copyright UnrealEssence. All Rights Reserved.


#include "UI/TypingEffectWidget.h"

void UTypingEffectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TEXT("")));
	}
}

void UTypingEffectWidget::StartTypingEffect()
{
	CurrentLine.Empty();
	UpdateDisplay();
	GetWorld()->GetTimerManager().SetTimer(
		TypingTimerHandle,
		this,
		&UTypingEffectWidget::AddRandomCharacter,
		TypingInterval,
		true
	);
}

void UTypingEffectWidget::StopTypingEffect()
{
	GetWorld()->GetTimerManager().ClearTimer(TypingTimerHandle);
}

void UTypingEffectWidget::AddRandomCharacter()
{
	if (AllowedChars.IsEmpty()) return;

	int32 Index = FMath::RandRange(0, AllowedChars.Len() - 1);
	TCHAR NewChar = AllowedChars[Index];
	CurrentLine.AppendChar(NewChar);

	// Если строка слишком длинная — удаляем первый символ (эффект "бегущей строки")
	if (CurrentLine.Len() > MaxLineLength)
	{
		CurrentLine.RemoveAt(0, 1);
	}

	UpdateDisplay();
}

void UTypingEffectWidget::UpdateDisplay()
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(CurrentLine));
	}
}