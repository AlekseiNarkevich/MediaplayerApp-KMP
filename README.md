# Media Player (Qt Quick / QML, C++17, qmake) — Lab 9

![Build Status](https://github.com/AlekseiNarkevich/MediaplayerApp-KMP/actions/workflows/qt-ci.yml/badge.svg)

Кроссплатформенное приложение **«Медиапроигрыватель»**, разработанное в рамках **Лабораторной работы №9 (Задание 3)** на базе **C++17 + Qt Quick/QML** с использованием **qmake**.  
Цель — изучить работу с **QtMultimedia**, файловой системой, интеграцию **C++ ↔ QML**, локализацию, анимации и хранение данных в **SQLite**.

Поддерживаемые платформы: **Android**, **iOS**, **Linux (Desktop)** и **Web (WebAssembly)**.

---

## 🚀 Функциональные возможности
- **Библиотека аудио:**
  - Desktop/Linux: просмотр аудиофайлов из выбранной папки (сканирование файловой системы).
  - Android/Web: выбор аудиофайлов через системный файловый picker (учёт ограничений SAF/браузера).
- **Управление воспроизведением:** Play / Pause / Stop / Seek (перемотка слайдером).
- **Плейлист:** добавление/удаление треков, воспроизведение из плейлиста.
- **Хранение данных:** плейлист сохраняется в **SQLite** и восстанавливается при перезапуске.
- **Обложка альбома (если доступна):**
  - попытка взять обложку из метаданных,
  - затем поиск изображения рядом с треком (`cover.*`, `folder.*`, `<track>.*` и т.п.),
  - иначе используется заглушка `default_cover.svg`.
- **Локализация интерфейса:** **Русский / English / Беларуская**, переключение языка во время работы.
- **Обработка ошибок:** вывод диагностических сообщений в консоль.
- **Анимации UI:** плавная анимация элементов интерфейса (выбранный вариант — анимация прозрачности окна).

---

## 🛠 Технологии
- **UI:** Qt Quick / QML (`ApplicationWindow`, `ListView`, `TabBar`, `ToolBar`, `Dialogs`)
- **Логика:** C++17 (контроллеры + модели для QML)
- **Мультимедиа:** `QtMultimedia` (`QMediaPlayer`, `QAudioOutput`)
- **Хранение:** `QtSql` + **SQLite**
- **Сборка:** `qmake`
- **Тестирование:** `QtTest` (unit/integration) + `Qt Quick Test` (UI)

---

## 📱 Адаптация UI под платформы (по требованиям методички)
- **Android:**
  - Material-стилизация
  - нижняя навигация + большие сенсорные зоны
  - выбор файлов через системный picker (устойчиво для SAF/MIUI)
- **iOS:**
  - нижний `TabBar`
  - плавные переходы/анимации
  - элементы управления воспроизведением в стиле Apple Music (круглые кнопки)
- **Linux (Desktop):**
  - верхнее меню + панель инструментов
  - поддержка **drag-and-drop** файлов в плейлист
- **Web (WebAssembly):**
  - адаптивность: на узком экране — как mobile, на широком — как desktop
  - горячие клавиши (desktop-web)
  - выбор файлов через браузерный picker (с учётом ограничений WebAssembly)

---

## 🧪 Тестирование (3–7 тестов каждого типа)
Проект содержит:
- **Unit tests** (`tests/unit`) — 3 теста
- **Integration tests** (`tests/integration`) — 5 тестов
- **UI tests** (`tests/ui`) — 5 тестов

### Запуск тестов (Windows / MinGW)
Unit:
```powershell
cd tests\unit
qmake unit_tests.pro
mingw32-make -j4
.\unit_tests.exe
```

Integration:
```powershell
cd tests\integration
qmake integration_tests.pro
mingw32-make -j4
.\integration_tests.exe
```

UI:
```powershell
cd tests\ui
qmake ui_tests.pro
mingw32-make -j4
.\ui_tests.exe -platform offscreen
```

---

## 🏗 CI / GitHub Actions
CI настроен в `.github/workflows/qt-ci.yml` и собирает:
- **Linux** + прогон **unit/integration/UI** тестов
- **Android** (arm64-v8a) + сборка APK
- **iOS** (simulator) + сборка проекта
- **WebAssembly** + сборка wasm-бандла и публикация артефактов

---

## 🌍 Локализация (ru/en/be)
Переводы находятся в `i18n/*.ts`.  
После правок переводов выполните:
```powershell
lupdate MediaPlayerApp.pro
lrelease MediaPlayerApp.pro
```

---

## ▶️ Сборка и запуск (кратко)

### Windows (MinGW)
```powershell
qmake MediaPlayerApp.pro
mingw32-make -j4
.\MediaPlayerApp.exe
```

### WebAssembly (Windows, PowerShell)
Пример сценария:
```powershell
cd C:\Users\Alexfresh\Desktop\Android_projects\Lab9\Lab9-project3_Narkevich\build-wasm
C:\emsdk\emsdk_env.bat
set PATH=C:\Qt\Tools\mingw1120_64\bin;%PATH%
C:\Qt\6.5.3\wasm_singlethread\bin\qmake ..\MediaPlayerApp.pro -spec wasm-emscripten CONFIG+=release
mingw32-make -j4
python -m http.server 8080
```

Открыть: `http://localhost:8080/MediaPlayerApp.html`

---

## 📌 Примечания
- Сборочные директории (`build-*`, `build-wasm`, `build-android`) **не коммитятся** — они генерируются локально или в CI.
- Проект оформлен русскими Doxygen/Javadoc-стилем комментариями, версия файлов: **1.0**.

---

**Автор:** Наркевич Алексей  
**Версия:** 1.0
