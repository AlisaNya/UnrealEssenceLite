# UnrealEssenceLite
Simple Lite version (Supported UE 5.7.4 and other)

Включает в себя следующие модули (со временем будет дополнено) ;

1. TypeWriter - печаталка бегущего текста, с поддержкой любого набора символов, рандомно их выбирает, может быть запущена как в прямом так и обратном направлении, с разной скоростью итд.. 

# TypingEffectWidget — UMG-виджет «печати символов»

> Часть проекта **UnrealEssence** · Модуль `UnrealEssenceRuntime` · Папка `UI`
> Совместимость: **Unreal Engine 5.7.4** · C++17 · без внешних зависимостей

---

## 📖 Описание

`UTypingEffectWidget` — гибкий UMG-виджет, реализующий декоративный эффект
«печати» символов в `UTextBlock`. Поддерживает **два режима работы**, **два направления печати**,
**автоматическое выравнивание** и **события завершения**.

### Режимы работы

| Режим | Описание |
|---|---|
| **Случайный** (`bRandomIsActive = true`) | Каждый символ выбирается случайно из `AllowedChars`. Подходит для "хакерских" терминалов, фоновой "матрицы". |
| **Последовательный** (`bRandomIsActive = false`) | Символы читаются по порядку из `AllowedChars`. При достижении конца строки вызывается делегат `OnAllAllowedCharsIsEndTyping`. Подходит для вывода конкретного текста. |

### Направления печати

| Направление | Описание |
|---|---|
| **LeftToRight** | Новые символы добавляются **справа**, старые удаляются **слева**. Текст "растёт" вправо. |
| **RightToLeft** | Новые символы добавляются **слева**, старые удаляются **справа**. Текст "растёт" влево. |

Типичные сценарии использования:

- «Хакерский» терминал / консоль в UI.
- Индикатор ожидания / загрузки.
- Фоновая «матрица» из HEX / бинарного кода.
- Вывод конкретного текста с эффектом печати (последовательный режим).
- Декоративные подписи к объектам в мире.

---

## ✨ Возможности

| Возможность | Описание |
|---|---|
| 🎯 Настраиваемый алфавит | Любая строка: `01`, `ABCD1234!@#$`, HEX, кириллица, эмодзи |
| ⏱ Регулируемая скорость | `TypingInterval` в секундах (по умолчанию `0.05f`) |
| 📏 Ограничение длины | `MaxLineLength` — эффект «бегущей строки» |
| 🔄 Два режима | Случайный и последовательный вывод символов |
| ➡️ Два направления | LeftToRight и RightToLeft с автоматическим выравниванием |
| 🔔 События | Делегат `OnAllAllowedCharsIsEndTyping` при завершении последовательного вывода |
| 🛑 Автоостановка | Автоматическая остановка после вывода всей строки (`bUseAutoStopTyping`) |
| 🔁 Старт / Стоп | BlueprintCallable-функции с параметрами, безопасно вызывать повторно |
| 🛡 Null-safe | Отсутствие `DisplayText` не приводит к крашу |
| 🧮 Без NaN | Нет делений и операций, способных породить некорректные float |
| 🧵 GameThread-only | Вся логика идёт через `FTimerManager` — никаких гонок данных |

---

## 📁 Структура в проекте

```
UnrealEssence/
└── Source/
    └── Runtime/
        └── UnrealEssenceRuntime/
            ├── Public/
            │   └── UI/
            │       └── TypingEffectWidget.h
            └── Private/
                └── UI/
                    └── TypingEffectWidget.cpp
```

---

## 🚀 Установка и использование

### 1. Добавьте виджет в Blueprint

1. В редакторе UE создайте **User Widget Blueprint** (ПКМ → *User Interface → Widget Blueprint*).
2. В *Class Settings* установите **Parent Class** → `Typing Effect Widget`.
3. В *Designer* перетащите **Text Block** и назовите его `DisplayText`
   (имя **обязательно** — используется `BindWidget`).
4. Настройте переменные в *Details*:
   - `Typing Interval` — задержка между символами (сек);
   - `Max Line Length` — макс. видимая длина строки;
   - `Allowed Chars` — алфавит или текст для вывода;
   - `Current Direction` — направление печати;
   - `Random Is Active` — режим работы (случайный/последовательный);
   - `Justify Is Auto` — автоматическое выравнивание;
   - `Use Auto Stop Typing` — автоостановка после вывода всей строки.

### 2. Запуск из Blueprint

#### Пример 1: Случайный режим (фоновая "матрица")

```
Event BeginPlay
   → Create Widget (TypingEffectWidgetBP)
   → Add to Viewport
   → Start Typing Effect (bRandomize = true, bStartFromClear = true)
```

#### Пример 2: Последовательный режим (вывод текста)

```
Event BeginPlay
   → Set Allowed Chars = "HELLO WORLD"
   → Set Random Is Active = false
   → Set Use Auto Stop Typing = true
   → Create Widget (TypingEffectWidgetBP)
   → Add to Viewport
   → Start Typing Effect (bRandomize = false, bStartFromClear = true)
   → Bind Event to On All Allowed Chars End Typing
      → Print String "Printing complete!"
```

#### Пример 3: Смена направления

```
Event On Button Clicked
   → Change Direction
```

### 3. Запуск из C++

```cpp
if (UTypingEffectWidget* Widget = CreateWidget<UTypingEffectWidget>(GetWorld(), Subclass))
{
    // Настройка параметров
    Widget->TypingInterval     = 0.03f;
    Widget->MaxLineLength      = 60;
    Widget->AllowedChars       = TEXT("0123456789ABCDEF");
    Widget->CurrentDirection   = ETypingDirection::LeftToRight;
    Widget->bRandomIsActive    = false;  // Последовательный режим
    Widget->bJustifyIsAuto     = true;   // Автовыравнивание
    Widget->bUseAutoStopTyping = true;   // Автоостановка

    // Подписка на событие
    Widget->OnAllAllowedCharsIsEndTyping.AddDynamic(this, &UMyClass::OnTypingComplete);

    Widget->AddToViewport();
    Widget->StartTypingEffect(false, true);  // Последовательный режим, начать с чистой строки
}
```

---

## 🧩 API

### Публичные методы

| Метод | Параметры | Описание |
|---|---|---|
| `void StartTypingEffect(bool bRandomize = true, bool bStartFromClear = false)` | `bRandomize`: режим работы (случайный/последовательный)<br>`bStartFromClear`: очистить строку перед запуском | Запускает таймер. Если `bJustifyIsAuto = true`, обновляет выравнивание. Повторный вызов перезапускает эффект. |
| `void StopTypingEffect(bool bClear = false)` | `bClear`: очистить строку после остановки | Останавливает таймер. Если `bClear = true`, также сбрасывает `CurrentLine`. |
| `void ChangeDirection()` | — | Переключает направление на противоположное. Если `bJustifyIsAuto = true`, обновляет выравнивание. |
| `void SetDirection(ETypingDirection NewDirection)` | `NewDirection`: новое направление | Устанавливает указанное направление. Если `bJustifyIsAuto = true`, обновляет выравнивание. |

### События

| Событие | Описание |
|---|---|
| `FOnAllAllowedCharsIsEndTypingEvent OnAllAllowedCharsIsEndTyping` | Вызывается при завершении печати всей строки `AllowedChars` в последовательном режиме. |

### Настраиваемые параметры

| Переменная | Тип | По умолчанию | Описание |
|---|---|---|---|
| `TypingInterval` | `float` | `0.05f` | Интервал между символами (сек). Минимум `0`. |
| `MaxLineLength` | `int32` | `40` | Макс. длина видимой строки. Минимум `1`. |
| `AllowedChars` | `FString` | `ABCD1234!@#$` | Алфавит или текст для вывода. Пустая строка отключает генерацию. |
| `CurrentDirection` | `ETypingDirection` | `LeftToRight` | Направление печати. |
| `bRandomIsActive` | `bool` | `false` | Режим работы: `true` — случайный, `false` — последовательный. |
| `bJustifyIsAuto` | `bool` | `false` | Автоматическое управление выравниванием текста. |
| `bUseAutoStopTyping` | `bool` | `false` | Автоматическая остановка печати по окончании вывода набора символов (только в последовательном режиме). |
| `DisplayText` | `UTextBlock*` | — | Привязывается через `BindWidget` в Designer. |

---

## ⚙️ Примеры конфигураций

### Пример 1: "Хакерский" терминал (случайный режим)

```cpp
Widget->AllowedChars    = TEXT("0123456789ABCDEF");
Widget->bRandomIsActive = true;
Widget->TypingInterval  = 0.02f;
Widget->MaxLineLength   = 80;
Widget->StartTypingEffect(true, true);
```

### Пример 2: Вывод конкретного текста (последовательный режим)

```cpp
Widget->AllowedChars       = TEXT("SYSTEM INITIALIZED...");
Widget->bRandomIsActive    = false;
Widget->bUseAutoStopTyping = true;
Widget->TypingInterval     = 0.05f;
Widget->StartTypingEffect(false, true);
```

### Пример 3: Двунаправленная "бегущая строка"

```cpp
// Запуск слева направо
Widget->CurrentDirection = ETypingDirection::LeftToRight;
Widget->StartTypingEffect(true, true);

// Через 5 секунд — переключение на справа налево
GetWorldTimerManager().SetTimerForNextTick([Widget]()
{
    Widget->ChangeDirection();
});
```

---

## 🛡 Гарантии и ограничения

- **Потокобезопасность**: все методы вызываются в GameThread.
- **Null-safety**: `DisplayText == nullptr` и пустой `AllowedChars` обрабатываются без крашей.
- **Нет NaN/Inf**: в логике нет делений, `FMath::Max(TypingInterval, 0.f)` защищает от отрицательных значений.
- **Производительность**: `RemoveAt(0, 1)` и `InsertAt(0, ...)` имеют сложность `O(N)`, но при `MaxLineLength ≤ 200` это незаметно даже при 60 FPS.
- **Ограничения**: виджет не поддерживает многострочный режим «из коробки» — при необходимости расширьте `UpdateDisplay()` до `TArray<FString>`.

---

## 🧪 Известные сценарии использования

- Экран загрузки в стиле «взлом системы» в проектах жанра cyberpunk.
- Индикатор активности ИИ-ассистента (NeuroCore-модуль UnrealEssence).
- Фоновая анимация терминалов в PCG-локациях.
- Вывод диалогов NPC с эффектом печати.
- Декоративные элементы интерфейса в sci-fi играх.

---

## 📄 Лицензия

Распространяется в рамках лицензии проекта **UnrealEssence**.
Подробности — в файле `LICENSE` в корне репозитория.

---

## 👥 Автор AlisaNya (YukiNoHime)

Команда **UnrealEssence** · Модуль `UnrealEssenceRuntime` · подсистема `UI`.
