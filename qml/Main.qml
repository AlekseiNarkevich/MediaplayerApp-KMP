/**
 * @file Main.qml
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Пользовательский интерфейс кроссплатформенного приложения "Медиапроигрыватель".
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs
import MediaPlayer 1.0

ApplicationWindow {
    id: window
    width: 1000
    height: 720
    visible: true
    title: qsTr("Медиапроигрыватель")

    // [Общее] Флаги платформ для адаптивной верстки (Android/iOS/Linux/WebAssembly).
    readonly property bool isAndroid: Qt.platform.os === "android"
    readonly property bool isIOS: Qt.platform.os === "ios"
    readonly property bool isLinuxDesktop: Qt.platform.os === "linux"
    readonly property bool isWeb: Qt.platform.os === "wasm"
    readonly property bool mobileLike: isAndroid || isIOS || (isWeb && width < 800)
    // [Web] Для web-desktop используем файловый picker, т.к. прямой доступ к папке ограничен браузером.
    readonly property bool useFilePickerFlow: mobileLike || isWeb
    // Текущая вкладка нижней навигации: 0 - библиотека, 1 - плейлист, 2 - настройки.
    property int currentSection: 0

    // [Android/mobile + общее] Конфигурация Material-темы.
    Material.theme: Material.Light
    Material.primary: Material.Indigo
    Material.accent: Material.DeepPurple

    // C++ модель медиатеки: список треков из выбранной папки.
    MediaLibraryModel {
        id: mediaLibrary
        onScanFinished: function(filesCount) {
            console.log(qsTr("Сканирование завершено. Найдено файлов:"), filesCount)
        }
    }

    // C++ контроллер воспроизведения: плейлист, плеер, состояние текущего трека.
    PlayerController {
        id: playerController
        Component.onCompleted: restorePlaylist()
    }

    // [Android/mobile] Локальная модель медиатеки (заполняется выбранными файлами из FileDialog).
    ListModel {
        id: mobileLibraryModel
    }

    // [Desktop/Linux] Диалог выбора папки с аудиофайлами.
    FolderDialog {
        id: folderDialog
        title: qsTr("Выберите папку с музыкой")
        onAccepted: {
            if (selectedFolder.toString().startsWith("file:")) {
                const selectedPath = selectedFolder.toString().replace("file:///", "")
                mediaLibrary.scanFolder(selectedPath)
            }
        }
    }

    // [Android/mobile] Диалог выбора аудиофайлов (вместо выбора папки).
    FileDialog {
        id: mobileAudioDialog
        title: qsTr("Выберите аудиофайлы")
        fileMode: FileDialog.OpenFiles
        // На Android некоторые файловые провайдеры жестко фильтруют расширения.
        // Первый фильтр оставляем максимально широким, чтобы пользователь видел все аудиофайлы.
        nameFilters: [
            qsTr("Все файлы (*.*)"),
            qsTr("Аудиофайлы (*.mp3 *.MP3 *.wav *.WAV *.flac *.FLAC *.ogg *.OGG *.m4a *.M4A *.aac *.AAC *.opus *.OPUS)")
        ]
        onAccepted: {
            mobileLibraryModel.clear()
            for (let i = 0; i < selectedFiles.length; i++) {
                const sourceUrl = selectedFiles[i].toString()
                mobileLibraryModel.append({
                                            "title": playerController.trackName(sourceUrl),
                                            "filePath": sourceUrl,
                                            "coverPath": "qrc:/assets/default_cover.svg"
                                        })
                playerController.addToPlaylist(sourceUrl)
                if (i === 0) {
                    playerController.playTrack(sourceUrl)
                }
            }
        }
    }

    /**
     * @brief Открывает корректный диалог выбора медиа в зависимости от платформы.
     */
    function openMediaPicker() {
        // [Web] Используем C++ мост с getOpenFileContent, чтобы обойти ограничения FileDialog в wasm.
        if (isWeb && webFileController) {
            webFileController.pickAudioFile()
            return
        }
        // [Android/mobile + Web] Выбор отдельных аудиофайлов через системный picker.
        if (useFilePickerFlow) {
            mobileAudioDialog.open()
        } else {
            // [Desktop/Linux] Выбор папки и последующее сканирование файловой системы.
            folderDialog.open()
        }
    }

    // [Web] Обработка результата выбора файла через C++ wasm bridge.
    Connections {
        target: webFileController
        function onFileChosen(fileUrl, title) {
            playerController.setTrackDisplayName(fileUrl, title)
            mobileLibraryModel.append({
                                        "title": title,
                                        "filePath": fileUrl,
                                        "coverPath": "qrc:/assets/default_cover.svg"
                                    })
            playerController.addToPlaylist(fileUrl)
            playerController.playTrack(fileUrl)
        }
        function onErrorOccurred(message) {
            console.log("WebFileController:", message)
        }
    }

    // [Desktop/Linux] Меню в "настольном" стиле.
    menuBar: MenuBar {
        visible: isLinuxDesktop
        Menu {
            title: qsTr("Файл")
            Action {
                text: qsTr("Открыть папку")
                onTriggered: folderDialog.open()
            }
            Action {
                text: qsTr("Выход")
                onTriggered: Qt.quit()
            }
        }
    }

    // [Общее] Верхняя панель приложения с заголовком.
    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 10
            Label {
                text: qsTr("Медиапроигрыватель")
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    // [Desktop/Linux] Верхняя панель управления воспроизведением (Windows/Linux/Web desktop).
    ToolBar {
        id: desktopToolbar
        visible: !mobileLike
        anchors.top: header.bottom
        width: parent.width
        RowLayout {
            anchors.fill: parent
            anchors.margins: 6
            spacing: 8
            ToolButton { text: qsTr("Библиотека"); onClicked: currentSection = 0 }
            ToolButton { text: qsTr("Плейлист"); onClicked: currentSection = 1 }
            ToolButton { text: qsTr("Настройки"); onClicked: currentSection = 2 }
            ToolSeparator {}
            ToolButton { text: qsTr("Открыть"); onClicked: openMediaPicker() }
            ToolSeparator {}
            ToolButton { text: qsTr("Воспроизвести"); onClicked: playerController.play() }
            ToolButton { text: qsTr("Пауза"); onClicked: playerController.pause() }
            ToolButton { text: qsTr("Стоп"); onClicked: playerController.stop() }
            Slider {
                id: desktopSeekSlider
                Layout.preferredWidth: 260
                from: 0
                to: Math.max(playerController.duration, 1)
                enabled: playerController.seekable && playerController.duration > 0
                value: playerController.position
                onPressedChanged: {
                    if (!pressed) {
                        playerController.seek(value)
                    }
                }
            }
            // Пока пользователь тянет ползунок, отключаем обратную привязку к позиции плеера.
            Binding {
                target: desktopSeekSlider
                property: "value"
                value: playerController.position
                when: !desktopSeekSlider.pressed
            }
            Label {
                Layout.fillWidth: true
                text: playerController.currentTrack.length > 0
                      ? qsTr("Сейчас играет: %1").arg(playerController.trackName(playerController.currentTrack))
                      : qsTr("Трек не выбран")
                elide: Text.ElideRight
            }
        }
    }

    // Основной контейнер интерфейса: контент + блок управления плеером + нижняя навигация.
    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: (!mobileLike) ? desktopToolbar.bottom : parent.top
        spacing: 0

        // Область основного контента (списки треков, плейлист, настройки).
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // [Desktop/Linux] Одноколоночное представление библиотеки на всю ширину экрана.
            RowLayout {
                anchors.fill: parent
                visible: !mobileLike && currentSection === 0
                spacing: 0

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#f4f4f4"
                    border.width: 1
                    border.color: "#e0e0e0"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8
                        Label {
                            text: qsTr("Список треков")
                            font.bold: true
                        }
                        ListView {
                            id: tracksListDesktop
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            // [Web] В браузере библиотека формируется из выбранных файлов (mobileLibraryModel).
                            model: isWeb ? mobileLibraryModel : mediaLibrary
                            clip: true
                            delegate: Rectangle {
                                width: tracksListDesktop.width
                                height: 64
                                color: mouseArea.pressed ? "#e8e8e8" : "transparent"
                                border.width: 1
                                border.color: "#efefef"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 6
                                    spacing: 8
                                    Image {
                                        source: coverPath
                                        Layout.preferredWidth: 42
                                        Layout.preferredHeight: 42
                                        fillMode: Image.PreserveAspectFit
                                    }
                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 2
                                        Label {
                                            text: title
                                            color: "black"
                                            font.bold: true
                                            Layout.fillWidth: true
                                            elide: Text.ElideRight
                                        }
                                        Label {
                                            text: filePath
                                            color: "#666666"
                                            font.pixelSize: 10
                                            Layout.fillWidth: true
                                            elide: Text.ElideMiddle
                                        }
                                    }
                                }

                                MouseArea {
                                    id: mouseArea
                                    anchors.fill: parent
                                    onClicked: {
                                        playerController.addToPlaylist(filePath)
                                        playerController.playTrack(filePath)
                                    }
                                }
                            }
                        }
                    }
                }

            }

            // [Android/mobile] Мобильное представление списка треков.
            ColumnLayout {
                anchors.fill: parent
                visible: mobileLike && currentSection === 0
                spacing: 8
                anchors.margins: 10

                Label {
                    text: qsTr("Треки")
                    font.bold: true
                }
                Button {
                    text: qsTr("Выбрать аудиофайлы")
                    onClicked: openMediaPicker()
                }
                ListView {
                    id: tracksListMobile
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: mobileLibraryModel
                    clip: true
                    delegate: Rectangle {
                        width: tracksListMobile.width
                        height: 70
                        color: mobileMouse.pressed ? "#e8e8e8" : "transparent"
                        border.width: 1
                        border.color: "#efefef"

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 8
                            Image {
                                source: coverPath
                                Layout.preferredWidth: 44
                                Layout.preferredHeight: 44
                                fillMode: Image.PreserveAspectFit
                            }
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2
                                Label {
                                    text: title
                                    color: "black"
                                    font.bold: true
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }
                                Label {
                                    text: filePath
                                    color: "#666666"
                                    font.pixelSize: 10
                                    Layout.fillWidth: true
                                    elide: Text.ElideMiddle
                                }
                            }
                        }

                        MouseArea {
                            id: mobileMouse
                            anchors.fill: parent
                            onClicked: {
                                playerController.addToPlaylist(filePath)
                                playerController.playTrack(filePath)
                            }
                        }
                    }
                }
            }

            // [Общее] Экран плейлиста (вкладка 2).
            ColumnLayout {
                anchors.fill: parent
                visible: currentSection === 1
                spacing: 8
                anchors.margins: 12

                // [Общее] Делегаты плейлиста: удаление долгим нажатием (и на mobile, и на desktop).
                Component {
                    id: playlistMobileDelegate
                    Rectangle {
                        required property string modelData
                        width: ListView.view ? ListView.view.width : parent.width
                        height: 56
                        color: mobilePlaylistMouse.pressed ? "#f0f0f0" : "transparent"
                        border.width: 1
                        border.color: "#efefef"

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 8
                            Label {
                                text: playerController.trackName(modelData)
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }
                            // Удаление выполняется долгим нажатием, отдельная иконка не требуется.
                        }

                        MouseArea {
                            id: mobilePlaylistMouse
                            anchors.fill: parent
                            onClicked: playerController.playTrack(modelData)
                            // [Android/mobile] Долгое нажатие удаляет трек (надежнее свайпа на MIUI).
                            onPressAndHold: playerController.removeFromPlaylist(modelData)
                        }
                    }
                }

                Component {
                    id: playlistDesktopDelegate
                    Rectangle {
                        required property string modelData
                        width: ListView.view ? ListView.view.width : parent.width
                        height: 56
                        color: playlistMouse.pressed ? "#f0f0f0" : "transparent"
                        border.width: 1
                        border.color: "#efefef"

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 8

                            Label {
                                text: playerController.trackName(modelData)
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }
                            // Удаление выполняется долгим нажатием, отдельная иконка не требуется.
                        }

                        MouseArea {
                            id: playlistMouse
                            anchors.fill: parent
                            onClicked: playerController.playTrack(modelData)
                            onPressAndHold: playerController.removeFromPlaylist(modelData)
                        }
                    }
                }

                Label {
                    text: qsTr("Плейлист")
                    font.bold: true
                }
                Label {
                    visible: mobileLike
                    text: qsTr("Удерживайте трек для удаления")
                    color: "#666666"
                    font.pixelSize: 12
                }
                Label {
                    visible: !mobileLike
                    text: qsTr("Для удаления удерживайте трек")
                    color: "#666666"
                    font.pixelSize: 12
                }
                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: playerController.playlist
                    // Делегат выбираем по режиму: mobile / desktop (в обоих режимах удаление долгим нажатием).
                    delegate: mobileLike ? playlistMobileDelegate : playlistDesktopDelegate
                }
            }

            // [Общее] Экран настроек (вкладка 3).
            ColumnLayout {
                anchors.fill: parent
                visible: currentSection === 2
                spacing: 10
                anchors.margins: 14
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    Column {
                        width: parent.width
                        spacing: 18

                        Label { text: qsTr("Настройки"); font.bold: true }
                        Label {
                            // Для режимов с file picker (mobile/web) папка не используется.
                            visible: !useFilePickerFlow
                            text: qsTr("Текущая папка: %1").arg(mediaLibrary.currentFolder.length > 0 ? mediaLibrary.currentFolder : qsTr("не выбрана"))
                        }
                        Button {
                            text: mobileLike ? qsTr("Выбрать аудиофайлы") : qsTr("Выбрать папку с музыкой")
                            onClicked: openMediaPicker()
                        }
                        RowLayout {
                            spacing: 8
                            Label { text: qsTr("Язык интерфейса:") }
                            ComboBox {
                                id: languageBox
                                // Используем коды языков как стабильную модель, чтобы индекс не сбрасывался при retranslate().
                                model: ["ru", "en", "be"]
                                /**
                                 * @brief Возвращает индекс ComboBox по коду языка.
                                 */
                                function indexFromLanguageCode(code) {
                                    if (code === "en")
                                        return 1
                                    if (code === "be")
                                        return 2
                                    return 0
                                }

                                // Инициализируем выбранный язык безопасно (с защитой от null).
                                Component.onCompleted: {
                                    currentIndex = indexFromLanguageCode(
                                        languageController ? languageController.currentLanguage : "ru")
                                }

                                // Синхронизируем отображаемый выбранный язык при runtime-переключении.
                                Connections {
                                    target: languageController
                                    function onCurrentLanguageChanged() {
                                        languageBox.currentIndex = languageBox.indexFromLanguageCode(
                                                    languageController ? languageController.currentLanguage : "ru")
                                    }
                                }

                                onActivated: {
                                    if (!languageController) {
                                        return
                                    }
                                    languageController.setLanguage(languageBox.currentText)
                                }
                            }
                        }
                    }
                }
            }

            // [Desktop/Linux] Drag-and-drop добавление файлов в плейлист.
            DropArea {
                anchors.fill: parent
                enabled: !mobileLike
                onEntered: function(drag) {
                    // [Web/Desktop] Явно принимаем drag-событие, чтобы браузер не блокировал drop.
                    drag.accepted = true
                }
                onDropped: function(drop) {
                    for (let i = 0; i < drop.urls.length; i++) {
                        const droppedUrl = drop.urls[i].toString()
                        // [Web] Сохраняем URL как есть (blob/file/content), не обрезаем префиксы.
                        const normalized = isWeb ? droppedUrl : droppedUrl.replace("file:///", "")
                        playerController.addToPlaylist(normalized)
                        // Для веба сразу показываем файл в библиотеке, как после выбора через FileDialog.
                        if (isWeb) {
                            mobileLibraryModel.append({
                                                        "title": playerController.trackName(normalized),
                                                        "filePath": normalized,
                                                        "coverPath": "qrc:/assets/default_cover.svg"
                                                    })
                        }
                    }
                }
            }
        }

        // [Android/mobile + iOS/Web mobile] Нижняя навигация в стиле iOS / Web mobile.
        TabBar {
            visible: isIOS || (isWeb && mobileLike)
            Layout.fillWidth: true
            currentIndex: currentSection
            onCurrentIndexChanged: currentSection = currentIndex
            TabButton { text: qsTr("Библиотека") }
            TabButton { text: qsTr("Плейлист") }
            TabButton { text: qsTr("Настройки") }
        }

        // [Android/mobile] Нижняя навигация в стиле Material (Android / Web mobile).
        TabBar {
            visible: isAndroid || (isWeb && mobileLike && !isIOS)
            Layout.fillWidth: true
            Material.background: Material.primary
            Material.foreground: "white"
            currentIndex: currentSection
            onCurrentIndexChanged: currentSection = currentIndex
            TabButton { text: qsTr("Главная") }
            TabButton { text: qsTr("Плейлист") }
            TabButton { text: qsTr("Настройки") }
        }
    }

    // [Android/mobile] Нижняя панель управления плеером.
    footer: Rectangle {
        visible: mobileLike
        // На Android parent может быть не тем контейнером, поэтому жестко привязываем к ширине окна.
        width: window.width
        height: 112
        color: "#fafafa"
        border.color: "#dddddd"
        border.width: 1
        clip: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 4

            Label {
                text: playerController.currentTrack.length > 0
                      ? qsTr("Сейчас играет: %1").arg(playerController.trackName(playerController.currentTrack))
                      : qsTr("Трек не выбран")
                Layout.fillWidth: true
                Layout.maximumWidth: parent.width
                elide: Text.ElideRight
            }

            Slider {
                id: mobileSeekSlider
                Layout.fillWidth: true
                from: 0
                to: Math.max(playerController.duration, 1)
                enabled: playerController.seekable && playerController.duration > 0
                value: playerController.position
                onPressedChanged: {
                    if (!pressed) {
                        playerController.seek(value)
                    }
                }
            }
            // Аналогично desktop: не перезаписываем значение слайдера во время перетаскивания.
            Binding {
                target: mobileSeekSlider
                property: "value"
                value: playerController.position
                when: !mobileSeekSlider.pressed
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 4
                // [iOS] Элементы управления в стиле Apple Music: круглые кнопки с пиктограммами.
                RowLayout {
                    visible: isIOS
                    Layout.fillWidth: true
                    spacing: 12
                    Item { Layout.fillWidth: true }
                    RoundButton {
                        text: "⏸"
                        font.pixelSize: 20
                        highlighted: true
                        palette.button: "#f2f2f7"
                        onClicked: playerController.pause()
                    }
                    RoundButton {
                        text: "▶"
                        font.pixelSize: 20
                        highlighted: true
                        palette.button: "#ff2d55"
                        palette.buttonText: "white"
                        onClicked: playerController.play()
                    }
                    RoundButton {
                        text: "⏹"
                        font.pixelSize: 20
                        highlighted: true
                        palette.button: "#f2f2f7"
                        onClicked: playerController.stop()
                    }
                    Item { Layout.fillWidth: true }
                }

                // [Android/mobile + Web mobile] Адаптивная сетка Material-кнопок.
                GridLayout {
                    visible: !isIOS
                    Layout.fillWidth: true
                    columnSpacing: 6
                    rowSpacing: 6
                    columns: window.width < 360 ? 2 : 3

                    Button {
                        Layout.fillWidth: true
                        Layout.minimumWidth: 0
                        text: qsTr("Play")
                        highlighted: true
                        font.pixelSize: 14
                        leftPadding: 6
                        rightPadding: 6
                        onClicked: playerController.play()
                    }
                    Button {
                        Layout.fillWidth: true
                        Layout.minimumWidth: 0
                        text: qsTr("Pause")
                        font.pixelSize: 14
                        leftPadding: 6
                        rightPadding: 6
                        onClicked: playerController.pause()
                    }
                    Button {
                        Layout.fillWidth: true
                        Layout.minimumWidth: 0
                        Layout.columnSpan: window.width < 360 ? 2 : 1
                        text: qsTr("Stop")
                        font.pixelSize: 14
                        leftPadding: 6
                        rightPadding: 6
                        onClicked: playerController.stop()
                    }
                }
            }
        }
    }

    // [Desktop/Web] Горячая клавиша веб-десктопа: перемотка вперед на 5 секунд.
    Shortcut {
        enabled: isWeb && !mobileLike
        sequence: StandardKey.MoveToNextChar
        onActivated: playerController.seek(playerController.position + 5000)
    }

    // [Desktop/Web] Горячая клавиша веб-десктопа: play/pause по пробелу.
    Shortcut {
        enabled: isWeb && !mobileLike
        sequence: "Space"
        onActivated: {
            if (playerController.playing) {
                playerController.pause()
            } else {
                playerController.play()
            }
        }
    }

    // Простая плавная анимация прозрачности окна (совместимо с Qt 6.5.3).
    Behavior on opacity {
        NumberAnimation {
            duration: 220
            easing.type: Easing.InOutQuad
        }
    }
}
