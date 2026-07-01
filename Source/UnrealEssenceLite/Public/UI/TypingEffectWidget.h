// Copyright UnrealEssence. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "TypingEffectWidget.generated.h"

/**
 * @enum    ETypingDirection
 * @brief   Направление печати символов в виджете.
 *
 * @details
 * Определяет, с какой стороны строки добавляются новые символы и с какой удаляются старые:
 *   - LeftToRight:  новые символы добавляются в конец строки (справа),
 *                   старые удаляются из начала (слева).
 *   - RightToLeft:  новые символы добавляются в начало строки (слева),
 *                   старые удаляются из конца (справа).
*/
UENUM(BlueprintType)
enum class ETypingDirection : uint8
{
	LeftToRight UMETA(DisplayName = "Слева -> направо"),	/**< Новые символы добавляются справа. */
	RightToLeft UMETA(DisplayName = "Справа <- налево")		/**< Новые символы добавляются слева. */
};

/**
 * @brief   Делегат, вызываемый при завершении печати всей строки AllowedChars
 *          в последовательном режиме (bRandomIsActive = false).
 *
 * @details
 * Срабатывает в момент, когда последовательный просмотр AllowedChars
 * дошёл до конца и индекс CharIndex был сброшен в 0.
 * Полезно для:
 *   - Запуска анимации после завершения печати.
 *   - Переключения на следующий текст.
 *   - Уведомления UI о готовности.
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllAllowedCharsIsEndTypingEvent);

/**
 * @class  UTypingEffectWidget
 * @brief  UMG-виджет для эффекта "печати" с гибкой настройкой режимов и направлений.
 * 
 * @details
 * Виджет предназначен для декоративных UI-элементов:
 *   - "хакерский" терминал,
 *   - индикатор загрузки / ожидания,
 *   - фоновая "матрица" из случайных символов,
 *   - вывод конкретного текста с эффектом печати.
 * @details
 * Генерирует символы из заданного алфавита (AllowedChars) двумя способами:
 *    - Случайный режим (bRandomIsActive = true): каждый символ выбирается случайно.
 *    - Последовательный режим (bRandomIsActive = false): символы читаются по порядку,
 *      при достижении конца строки вызывается делегат OnAllAllowedCharsIsEndTyping,
 *      если bUseAutoStopTyping = true то печать останавливается в противном случае повторяется с начала.
 * @details
 * Поддерживаются два направления печати:
 *    - LeftToRight: новые символы добавляются справа, старые удаляются слева.
 *    - RightToLeft: новые символы добавляются слева, старые удаляются справа.
 * 
 * @note Потокобезопасность: вся логика работает в GameThread через FTimerManager.
 * @warning Если AllowedChars пуст — AddCharacter() просто выходит (защита от деления по модулю 0).
 * 
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
	
	/**
	 * @brief   Событие, вызываемое при завершении печати всей строки AllowedChars
	 *          в последовательном режиме.
	 *
	 * @details
	 * Полезно для:
	 *   - Запуска анимации после завершения печати.
	 *   - Переключения на следующий текст.
	 *   - Уведомления UI о готовности.
	 *
	 * @note    Вызывается только в последовательном режиме (bRandomIsActive = false).
	*/
	UPROPERTY(BlueprintAssignable, Category = "TypingEffect",
		meta = (DisplayName = "On All Allowed Chars End Typing",
				ToolTip = "Broadcast when all characters in AllowedChars have been printed (sequential mode only)."))
	FOnAllAllowedCharsIsEndTypingEvent OnAllAllowedCharsIsEndTyping;
	
	/**
	 * @brief   Запускает эффект печатной машинки.
	 *
	 * @details
	 * Запускает таймер с интервалом TypingInterval. Повторный вызов перезапускает таймер.
	 * Если bJustifyIsAuto = true, автоматически обновляет выравнивание текста.
	 *
	 * @param   bRandomize        Если true — символы выбираются случайно.
	 *                            Если false — символы читаются по порядку из AllowedChars.
	 * @param   bStartFromClear   Если true — очищает текущую строку перед запуском.
	 *
	 * @note    Безопасно вызывать из Blueprint / EventGraph.
	 * @note    Требует валидный WorldContext (т.е. Виджет уже должен быть добавлен на экран).
	*/
	UFUNCTION(BlueprintCallable, Category = "TypingEffect",
		meta = (DisplayName = "Start Typing Effect",
				ToolTip = "Starts the typing effect. bRandomize: random or sequential mode. bStartFromClear: clear current line before start."))
	void StartTypingEffect(bool bRandomize = true, bool bStartFromClear = false);

	/**
	 * @brief   Останавливает эффект печатной машинки.
	 *
	 * @details
	 * Очищает таймер. Если bClear = true, также сбрасывает CurrentLine.
	 * Повторный вызов безопасен (ClearTimer для неактивного handle — no-op).
	 *
	 * @param   bClear   Если true — очищает текущую строку и обновляет дисплей.
	*/
	UFUNCTION(BlueprintCallable, Category = "TypingEffect",
		meta = (DisplayName = "Stop Typing Effect",
				ToolTip = "Stops the typing effect. bClear: also clears the current line."))
	void StopTypingEffect(bool bClear = false);

	/**
	 * @brief   Переключает направление печати на противоположное.
	 *
	 * @details
	 * Если текущее направление LeftToRight — меняет на RightToLeft, и наоборот.
	 * @note Если bJustifyIsAuto = true, автоматически обновляет выравнивание текста.
	*/
	UFUNCTION(BlueprintCallable, Category = "TypingEffect",
		meta = (DisplayName = "Change Direction",
				ToolTip = "Toggles the typing direction between LeftToRight and RightToLeft."))
	void ChangeDirection();

	/**
	 * @brief   Устанавливает указанное направление печати.
	 *
	 * @note
	 * Если bJustifyIsAuto = true, автоматически обновляет выравнивание текста.
	 *
	 * @param   NewDirection   Новое направление (LeftToRight или RightToLeft).
	*/
	UFUNCTION(BlueprintCallable, Category = "TypingEffect",
		meta = (DisplayName = "Set Direction",
				ToolTip = "Sets the typing direction to the specified value."))
	void SetDirection(ETypingDirection NewDirection);
	
protected:
	/**
	 * @brief   Вызывается один раз при создании виджета (после Native initialization).
	 *
	 * @details
	 * Инициализирует DisplayText пустой строкой, чтобы избежать показа
	 * мусорного текста в первом кадре до первого тика таймера.
	*/
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;

	/**
	 * @brief   Целевой текстовый блок, в который выводится текущая строка.
	 *
	 * @note    Должен быть привязан в Designer (BindWidget).
	 *          Если привязки нет — ядро работает, но UpdateDisplay() становится no-op.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayText;

	/**
	 * @brief   Интервал (в секундах) между добавлением двух соседних символов.
	 *
	 * @details
	 *   - 0.05f  → ~20 символов/сек (быстрая печать)
	 *   - 0.1f   → 10 символов/сек (обычная печать)
	 *   - 1.0f   → 1 символ/сек (медленная анимация)
	 *
	 * @note    Должен быть >= 0. При 0 таймер работает "каждый кадр".
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings",
		meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "2.0",
				ToolTip = "Delay in seconds between two consecutive characters."))
	float TypingInterval = 0.05f;

	/**
	 * @brief   Максимальная длина отображаемой строки.
	 *
	 * @details
	 * Как только CurrentLine становится длиннее MaxLineLength,
	 * самый старый символ удаляется (с учётом направления печати).
	 *
	 * @note    Должен быть >= 1. При 0 строка всегда будет пустой.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings",
		meta = (ClampMin = "1", UIMin = "1", UIMax = "200",
				ToolTip = "Maximum visible line length. Older characters are dropped."))
	int32 MaxLineLength = 40;

	/**
	 * @brief   Алфавит, из которого берутся символы для печати.
	 *
	 * @details
	 * В случайном режиме символы выбираются случайно.
	 * В последовательном режиме символы читаются по порядку.
	 *
	 * Примеры:
	 *   - TEXT("01")                 → бинарный поток
	 *   - TEXT("ABCD1234!@#$")       → "хакерский" набор
	 *   - TEXT("0123456789ABCDEF")   → HEX-дамп
	 *   - TEXT("HELLO WORLD")        → вывод конкретного текста
	 *
	 * @warning Если строка пустая — AddCharacter() просто выходит (защита от FMath::RandRange(0,-1)).
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings",
		meta = (ToolTip = "Alphabet for typing. Random mode: random sampling. Sequential mode: read in order."))
	FString AllowedChars = TEXT("ABCD1234!@#$");

	/**
	 * @brief   Текущее направление печати.
	 *
	 * @details
	 *   - LeftToRight:  новые символы добавляются справа.
	 *   - RightToLeft:  новые символы добавляются слева.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings",
		meta = (ToolTip = "Direction of character insertion."))
	ETypingDirection CurrentDirection = ETypingDirection::LeftToRight;
	
	/**
	 * @brief   Флаг случайного режима.
	 *
	 * @details
	 *   - true:  символы выбираются случайно из AllowedChars.
	 *   - false: символы читаются по порядку из AllowedChars.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings",
		meta = (ToolTip = "If true, characters are randomized. If false, read sequentially."))
	bool bRandomIsActive = false;
	
	/**
	 * @brief   Флаг автоматического управления выравниванием текста.
	 *
	 * @details
	 * Если true, при смене направления автоматически обновляется выравнивание:
	 *   - LeftToRight  → ETextJustify::Left
	 *   - RightToLeft  → ETextJustify::Right
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings",
		meta = (ToolTip = "If true, automatically adjusts text justification based on direction."))
	bool bJustifyIsAuto = false;
	
	/**
	 * @brief   Флаг автоматической остановки печати.
	 *
	 * @details
	 * Если true, в последовательном режиме печать автоматически останавливается
	 * после вывода всех символов из AllowedChars.
	 *
	 * @note    Работает только в последовательном режиме (bRandomIsActive = false).
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings",
		meta = (ToolTip = "If true, stops typing automatically after printing all characters (sequential mode only)."))
	bool bUseAutoStopTyping = false;
	
private:
	
	/** @brief  Текущая накапливаемая строка (логический буфер). Длина ограничена MaxLineLength. */
	FString CurrentLine;
	/** @brief  Handle активного таймера. Нужен для корректной остановки. */
	FTimerHandle TypingTimerHandle;
	/** @brief  Индекс текущего символа в последовательном режиме. В случайном режиме — последний выбранный индекс. */
	int32 CharIndex = 0;
	
	/**
	 * @brief   Один тик таймера: выбирает символ и добавляет его в CurrentLine.
	 *
	 * @details
	 * В зависимости от bRandomIsActive:
	 *   - случайный режим: выбирает случайный индекс через FMath::RandRange.
	 *   - последовательный режим: увеличивает CharIndex, при достижении конца
	 *     сбрасывает в 0 и вызывает делегат OnAllAllowedCharsIsEndTyping.
	*/
	void AddCharacter();
	
	/**
	 * @brief   Синхронизирует содержимое CurrentLine с DisplayText.
	 *
	 * @note    Если DisplayText == nullptr (не привязан в BP) — функция no-op, без крашей.
	*/
	void UpdateDisplay();
	
	/**
	 * @brief   Добавляет символ справа (LeftToRight).
	 *
	 * @details
	 * Добавляет NewChar в конец строки. Если длина превышает MaxLineLength —
	 * удаляет первый символ (слева).
	 *
	 * @param   NewChar   Символ для добавления.
	*/
	void AddCharacterLeftToRight(TCHAR NewChar);
	
	/**
	 * @brief   Добавляет символ слева (RightToLeft).
	 *
	 * @details
	 * Вставляет NewChar в начало строки (индекс 0). Если длина превышает MaxLineLength —
	 * удаляет последний символ (справа).
	 *
	 * @param   NewChar   Символ для добавления.
	*/
	void AddCharacterRightToLeft(TCHAR NewChar);
	
	/**
	 * @brief   Обновляет выравнивание текста в зависимости от направления.
	 *
	 * @details
	 * Если CurrentDirection == LeftToRight → ETextJustify::Left.
	 * Если CurrentDirection == RightToLeft → ETextJustify::Right.
	 *
	 * @note    Вызывается если bJustifyIsAuto = true.
	*/
	void UpdateJustify();
};