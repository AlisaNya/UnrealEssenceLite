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

void UTypingEffectWidget::StartTypingEffect(bool bRandomize, bool bStartFromClear)
{
	bRandomIsActive = bRandomize;
	if (bStartFromClear)	{ CurrentLine.Empty();	};
	if (bJustifyIsAuto)		{ UpdateJustify();		};
	UpdateDisplay();
	GetWorld()->GetTimerManager().SetTimer(
		TypingTimerHandle,
		this,
		&UTypingEffectWidget::AddCharacter,
		TypingInterval,
		true
	);
}

void UTypingEffectWidget::StopTypingEffect(bool bClear)
{
	GetWorld()->GetTimerManager().ClearTimer(TypingTimerHandle);
	
	if (bClear)
	{
		CurrentLine.Empty();
		UpdateDisplay();
	}
}

void UTypingEffectWidget::ChangeDirection()
{
	if (CurrentDirection == ETypingDirection::LeftToRight)
	{
		CurrentDirection = ETypingDirection::RightToLeft;
	}
	else
	{
		CurrentDirection = ETypingDirection::LeftToRight;
	}
	if (bJustifyIsAuto) { UpdateJustify(); }
	
}

void UTypingEffectWidget::SetDirection(ETypingDirection NewDirection)
{
	CurrentDirection = NewDirection;
	if (bJustifyIsAuto) { UpdateJustify(); }
}

void UTypingEffectWidget::AddCharacter()
{
	if (AllowedChars.IsEmpty()) return;
	
	if (bRandomIsActive)
	{
		CharIndex = FMath::RandRange(0, AllowedChars.Len() - 1);
	}
	else
	{
		CharIndex += 1;
		if (CharIndex > AllowedChars.Len() - 1)
		{
			CharIndex = 0;
			if (OnAllAllowedCharsIsEndTyping.IsBound())
			{
				OnAllAllowedCharsIsEndTyping.Broadcast();
			}
			
			if (bUseAutoStopTyping)
			{
				StopTypingEffect(false);
			}
		};
	}
	
	TCHAR NewChar = AllowedChars[CharIndex];

	switch (CurrentDirection)
	{
	case ETypingDirection::LeftToRight:
		AddCharacterLeftToRight(NewChar);
		break;
	case ETypingDirection::RightToLeft:
		AddCharacterRightToLeft(NewChar);
		break;
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

void UTypingEffectWidget::AddCharacterLeftToRight(TCHAR NewChar)
{
	// Добавляем символ в конец справа
	CurrentLine.AppendChar(NewChar);

	// Если строка слишком длинная - удаляем первый символ (слева)
	if (CurrentLine.Len() > MaxLineLength)
	{
		CurrentLine.RemoveAt(0, 1);
	}
}

void UTypingEffectWidget::AddCharacterRightToLeft(TCHAR NewChar)
{
	// Вставляем символ в начало строки (индекс 0) слева
	CurrentLine.InsertAt(0, NewChar);

	// Если строка слишком длинная - удаляем последний символ (справа)
	if (CurrentLine.Len() > MaxLineLength)
	{
		CurrentLine.RemoveAt(CurrentLine.Len() - 1, 1);
	}
}

void UTypingEffectWidget::UpdateJustify()
{
	if (DisplayText)
	{
		if (CurrentDirection == ETypingDirection::LeftToRight)
		{
			DisplayText->SetJustification(ETextJustify::Type::Left);
		}
		else
		{
			DisplayText->SetJustification(ETextJustify::Type::Right);
		}
	}
};
