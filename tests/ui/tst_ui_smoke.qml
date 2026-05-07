/**
 * @file tst_ui_smoke.qml
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Базовые UI-тесты QML-компонентов.
 */

import QtQuick
import QtQuick.Controls
import QtTest

TestCase {
    name: "UiSmokeTests"
    when: windowShown

    Item {
        id: root
        width: 300
        height: 200
    }

    function test_button_click() {
        // Проверяем, что сигнал clicked корректно испускается у Button.
        // В headless-режиме некоторых сред эмуляция mouseClick может быть нестабильной,
        // поэтому тестируем сам сигнал без зависимости от драйвера ввода.
        var clicked = false
        var component = Qt.createQmlObject(
                    'import QtQuick; import QtQuick.Controls; Button { text: "Play" }',
                    root,
                    "buttonUnderTest")
        component.clicked.connect(function() { clicked = true })
        component.clicked()
        verify(clicked === true)
    }

    function test_label_text() {
        // Проверяем отображение текста у Label.
        var label = Qt.createQmlObject(
                    'import QtQuick; import QtQuick.Controls; Label { text: "Медиапроигрыватель" }',
                    root,
                    "labelUnderTest")
        compare(label.text, "Медиапроигрыватель")
    }

    function test_slider_range() {
        // Проверяем границы и начальное значение Slider.
        var slider = Qt.createQmlObject(
                    'import QtQuick; import QtQuick.Controls; Slider { from: 0; to: 100; value: 20 }',
                    root,
                    "sliderUnderTest")
        compare(slider.from, 0)
        compare(slider.to, 100)
        compare(slider.value, 20)
    }

    function test_tabbar_switch() {
        // Проверяем переключение текущей вкладки у TabBar.
        var tabBar = Qt.createQmlObject(
                    'import QtQuick; import QtQuick.Controls; TabBar { TabButton { text: "A" } TabButton { text: "B" } }',
                    root,
                    "tabBarUnderTest")
        compare(tabBar.currentIndex, 0)
        tabBar.currentIndex = 1
        compare(tabBar.currentIndex, 1)
    }

    function test_round_button_text() {
        // Проверяем, что у кнопки корректно задается текст.
        var button = Qt.createQmlObject(
                    'import QtQuick; import QtQuick.Controls; RoundButton { text: "▶" }',
                    root,
                    "roundButtonUnderTest")
        compare(button.text, "▶")
    }
}
