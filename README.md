# UnrealEssenceLite
Simple Lite version (Supported UE 5.7.4 and other)

Включает в себя следующие модули (со временем будет дополнено) ;

1. TypeWriter - печаталка бегущего текста, с поддержкой любого набора символов, рандомно их выбирает, может быть запущена как в прямом так и обратном направлении, с разной скоростью итд.. 





# TypingEffectWidget — UMG-виджет «печати случайных символов»

> Часть проекта **UnrealEssence** · Модуль `UnrealEssenceRuntime` · Папка `UI`
> Совместимость: **Unreal Engine 5.7.4** · C++17 · без внешних зависимостей

---

## 📖 Описание

`UTypingEffectWidget` — лёгкий UMG-виджет, реализующий декоративный эффект
«печати» случайных символов в `UTextBlock`. Каждый тик таймера к текущей строке
добавляется один случайный символ из заданного алфавита; при превышении
максимальной длины самый старый символ удаляется — получается эффект
**«бегущей строки»**.

Типичные сценарии использования:

- «хакерский» терминал / консоль в UI;
- индикатор ожидания / загрузки;
- фоновая «матрица» из HEX / бинарного кода;
- декоративные подписи к объектам в мире.

---

## ✨ Возможности

| Возможность | Описание |
|---|---|
| 🎯 Настраиваемый алфавит | Любая строка: `01`, `ABCD1234!@#$`, HEX, кириллица, эмодзи |
| ⏱ Регулируемая скорость | `TypingInterval` в секундах (по умолчанию `0.05f`) |
| 📏 Ограничение длины | `MaxLineLength` — эффект «бегущей строки» |
| 🔁 Старт / Стоп | BlueprintCallable-функции, безопасно вызывать повторно |
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
   - `Allowed Chars` — алфавит.

### 2. Запуск из Blueprint

```
Event BeginPlay
   → Create Widget (TypingEffectWidgetBP)
   → Add to Viewport
   → Start Typing Effect
```

Для остановки:

```
Event EndPlay / On Button Clicked
   → Stop Typing Effect
```

### 3. Запуск из C++

```cpp
if (UTypingEffectWidget* Widget = CreateWidget<UTypingEffectWidget>(GetWorld(), Subclass))
{
    Widget->TypingInterval  = 0.03f;
    Widget->MaxLineLength   = 60;
    Widget->AllowedChars    = TEXT("0123456789ABCDEF");
    Widget->AddToViewport();
    Widget->StartTypingEffect();
}
```

---

## 🧩 API
### Публичные методы

| Метод | Описание |
|---|---|
| `void StartTypingEffect()` | Запускает таймер. Сбрасывает текущую строку. Повторный вызов перезапускает эффект. |
| `void StopTypingEffect()`  | Останавливает таймер. Текст на экране **сохраняется**. |

### Настраиваемые параметры

| Переменная | Тип | По умолчанию | Описание |
|---|---|---|---|
| `TypingInterval`  | `float` | `0.05f` | Интервал между символами (сек). Минимум `0`. |
| `MaxLineLength`   | `int32` | `40`    | Макс. длина видимой строки. Минимум `1`. |
| `AllowedChars`    | `FString` | `ABCD1234!@#$` | Алфавит случайных символов. Пустая строка отключает генерацию. |
| `DisplayText`     | `UTextBlock*` | — | Привязывается через `BindWidget` в Designer. |

---

## ⚙️ Примеры алфавитов

| Задача | `AllowedChars` |
|---|---|
| Бинарный поток | `TEXT("01")` |
| HEX-дамп | `TEXT("0123456789ABCDEF")` |
| «Хакерский» набор | `TEXT("ABCD1234!@#$")` |
| Только цифры | `TEXT("0123456789")` |
| Кириллица | `TEXT("АБВГДЕЖЗИКЛМНОПРСТ")` |

---

## 🛡 Гарантии и ограничения

- **Потокобезопасность**: все методы вызываются в GameThread.
- **Null-safety**: `DisplayText == nullptr` и пустой `AllowedChars` обрабатываются без крашей.
- **Нет NaN/Inf**: в логике нет делений, `FMath::Max(TypingInterval, 0.f)` защищает от отрицательных значений.
- **Производительность**: `RemoveAt(0, 1)` имеет сложность `O(N)`, но при `MaxLineLength ≤ 200`
  это незаметно даже при 60 FPS.
- **Ограничения**: виджет не поддерживает многострочный режим «из коробки» —
  при необходимости расширьте `UpdateDisplay()` до `TArray<FString>`.

---

## 🧪 Известные сценарии использования

- Экран загрузки в стиле «взлом системы» в проектах жанра cyberpunk.
- Индикатор активности ИИ-ассистента (NeuroCore-модуль UnrealEssence).
- Фоновая анимация терминалов в PCG-локациях.

---
## 📄 Лицензия

Распространяется в рамках лицензии проекта **UnrealEssence**.
Подробности — в файле `LICENSE` в корне репозитория.

---

## 👥 Автор AlisaNya (YukiNoHime)

Команда **UnrealEssence** · Модуль `UnrealEssenceRuntime` · подсистема `UI`.
