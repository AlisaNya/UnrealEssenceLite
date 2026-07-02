// Copyright UnrealEssence. All Rights Reserved.

#include "UI/TypingEffectWidget.h"

// ----------------------------------------------------------------------------
// Lifecycle
// ----------------------------------------------------------------------------

void UTypingEffectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Очищаем текст в первый кадр, чтобы не показывать дефолтное содержимое TextBlock.
	// Проверка на nullptr обязательна: BindWidget может быть не настроен в BP.
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TEXT("")));
	}
}

void UTypingEffectWidget::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearTimer(TypingTimerHandle);
	TypingTimerHandle.Invalidate();
}




// ----------------------------------------------------------------------------
// Public API
// ----------------------------------------------------------------------------

void UTypingEffectWidget::StartTypingEffect(bool bRandomize, bool bStartFromClear)
{
	bRandomIsActive = bRandomize;							// Устанавливаем режим (случайный/последовательный).
	if (bStartFromClear)	{ CurrentLine.Empty();	};		// Если нужно начать с чистой строки — очищаем буфер.
	if (bJustifyIsAuto)		{ UpdateJustify();		};		// Если включено автоматическое выравнивание — обновляем его.
	UpdateDisplay();										// Обновляем отрисовку перед запуском таймера.
	
	if (! GetWorld()->GetTimerManager().TimerExists(TypingTimerHandle))
	{
		// Запускаем таймер. FMath::Max защищает от отрицательных значений из BP.
			GetWorld()->GetTimerManager().SetTimer(
			TypingTimerHandle,
			this,
			&UTypingEffectWidget::AddCharacter,
			FMath::Max(TypingInterval, 0.001f),
			true
		);
	}
}

void UTypingEffectWidget::StopTypingEffect(bool bClear)
{
	GetWorld()->GetTimerManager().ClearTimer(TypingTimerHandle);
	
	// Если нужно очистить строку — сбрасываем буфер и обновляем отрисовку
	if (bClear)
	{
		CurrentLine.Empty();
		UpdateDisplay();
	}
}

void UTypingEffectWidget::ChangeDirection()
{
	// Переключаем направление на противоположное.
	if (CurrentDirection == ETypingDirection::LeftToRight)
	{
		CurrentDirection = ETypingDirection::RightToLeft;
	}
	else
	{
		CurrentDirection = ETypingDirection::LeftToRight;
	}
	
	// Если включено автоматическое выравнивание — обновляем его.
	if (bJustifyIsAuto) { UpdateJustify(); }
	
}

void UTypingEffectWidget::SetDirection(ETypingDirection NewDirection)
{
	// Устанавливаем указанное направление.
	CurrentDirection = NewDirection;
	
	// Если включено автоматическое выравнивание — обновляем его.
	if (bJustifyIsAuto) { UpdateJustify(); }
}




// ----------------------------------------------------------------------------
// Private: логика эффекта
// ----------------------------------------------------------------------------

void UTypingEffectWidget::AddCharacter()
{
	// Защита от пустого алфавита: без этой проверки FMath::RandRange(0, -1)
	// мог бы привести к некорректному поведению.
	if (AllowedChars.IsEmpty()) { return; };
	
	// Выбираем индекс символа в зависимости от режима.
	if (bRandomIsActive)
	{
		// Случайный режим: выбираем случайный индекс.
		// FMath::RandRange inclusive по обоим концам
		CharIndex = FMath::RandRange(0, AllowedChars.Len() - 1);
	}
	else
	{
		// Последовательный режим: увеличиваем индекс.
		CharIndex += 1;
		
		// Если достигли конца строки — сбрасываем индекс и вызываем делегат.
		if (CharIndex > AllowedChars.Len() - 1)
		{
			CharIndex = 0;
			
			// Вызываем делегат, если есть подписчики.
			if (OnAllAllowedCharsIsEndTyping.IsBound())
			{
				OnAllAllowedCharsIsEndTyping.Broadcast();
			}
			
			// Если включена автоматическая остановка — останавливаем печать.
			if (bUseAutoStopTyping)
			{
				StopTypingEffect(false);
			}
		};
	}
	
	// Получаем символ по индексу.
	TCHAR NewChar = AllowedChars[CharIndex];

	// Вставляем символ в зависимости от направления.
	switch (CurrentDirection)
	{
	case ETypingDirection::LeftToRight:
		AddCharacterLeftToRight(NewChar);
		break;
	case ETypingDirection::RightToLeft:
		AddCharacterRightToLeft(NewChar);
		break;
	}

	// Обновляем отрисовку
	UpdateDisplay();
}

void UTypingEffectWidget::UpdateDisplay()
{
	// nullptr-safe: если дизайнер забыл привязать DisplayText,
	// виджет всё равно работает (просто без визуала), а не крашит.
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(CurrentLine));
	}
	else
	{
		UE_LOG(LogTemp,Error, TEXT("in TypeWriter -> DisplayText is nullptr !!! Please Create TextBlock with name - DisplayText in your child widget WBP"));
	}
}

void UTypingEffectWidget::AddCharacterLeftToRight(TCHAR NewChar)
{
	// Добавляем символ в конец строки (справа).
	CurrentLine.AppendChar(NewChar);

	// Эффект "бегущей строки": как только буфер переполняется,
	// удаляем самый старый символ (индекс 0).
	if (CurrentLine.Len() > MaxLineLength)
	{
		CurrentLine.RemoveAt(0, 1);
	}
}

void UTypingEffectWidget::AddCharacterRightToLeft(TCHAR NewChar)
{
	// Вставляем символ в начало строки (индекс 0) слева
	CurrentLine.InsertAt(0, NewChar);

	// Эффект "бегущей строки": как только буфер переполняется,
	// удаляем самый старый символ (последний индекс справа).
	if (CurrentLine.Len() > MaxLineLength)
	{
		CurrentLine.RemoveAt(CurrentLine.Len() - 1, 1);
	}
}

void UTypingEffectWidget::UpdateJustify()
{
	// Обновляем выравнивание текста в зависимости от направления.
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
	else
	{
		UE_LOG(LogTemp,Error, TEXT("in TypeWriter -> DisplayText is nullptr !!! Please Create TextBlock with name - DisplayText in your child widget WBP"));
	}
};
