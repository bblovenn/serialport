#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QtTest>

#include "mainwindow.h"

class MainWindowLayoutTest : public QObject
{
    Q_OBJECT

private slots:
    void layout_has_header_and_settings_rows();
    void initial_texts_match_the_approved_design();
    void spacing_and_widths_match_the_compact_layout();
};

void MainWindowLayoutTest::layout_has_header_and_settings_rows()
{
    MainWindow window;

    QWidget* centralWidget = window.centralWidget();
    QVERIFY2(centralWidget != nullptr, "centralwidget should exist");

    auto* mainLayout = qobject_cast<QVBoxLayout*>(centralWidget->layout());
    QVERIFY2(mainLayout != nullptr, "verticalLayout_main should be attached to centralwidget");
    QCOMPARE(mainLayout->objectName(), QString("verticalLayout_main"));
    QCOMPARE(mainLayout->count(), 3);

    auto* title = window.findChild<QLabel*>("label_title");
    QVERIFY2(title != nullptr, "label_title should exist");
    QCOMPARE(title->text(), QString("Serial Plot Rebuild"));

    auto* headerItem = mainLayout->itemAt(0);
    QVERIFY2(headerItem != nullptr && headerItem->layout() != nullptr,
             "first item should be the header layout");
    auto* headerLayout = qobject_cast<QHBoxLayout*>(headerItem->layout());
    QVERIFY2(headerLayout != nullptr, "horizontalLayout_header should exist");
    QCOMPARE(headerLayout->objectName(), QString("horizontalLayout_header"));
    QCOMPARE(headerLayout->count(), 6);
    QCOMPARE(headerLayout->itemAt(0)->widget()->objectName(), QString("label_title"));
    QCOMPARE(headerLayout->itemAt(2)->widget()->objectName(), QString("pushButton_openClose"));
    QCOMPARE(headerLayout->itemAt(3)->widget()->objectName(), QString("pushButton_pause"));
    QCOMPARE(headerLayout->itemAt(4)->widget()->objectName(), QString("pushButton_clear"));
    QCOMPARE(headerLayout->itemAt(5)->widget()->objectName(), QString("pushButton_refreshPorts"));

    auto* settingsItem = mainLayout->itemAt(1);
    QVERIFY2(settingsItem != nullptr && settingsItem->layout() != nullptr,
             "second item should be the settings layout");
    auto* settingsLayout = qobject_cast<QHBoxLayout*>(settingsItem->layout());
    QVERIFY2(settingsLayout != nullptr, "horizontalLayout_settings should exist");
    QCOMPARE(settingsLayout->objectName(), QString("horizontalLayout_settings"));
    QCOMPARE(settingsLayout->count(), 9);
    QCOMPARE(settingsLayout->itemAt(0)->widget()->objectName(), QString("label_port"));
    QCOMPARE(settingsLayout->itemAt(1)->widget()->objectName(), QString("comboBox_port"));
    QCOMPARE(settingsLayout->itemAt(2)->widget()->objectName(), QString("label_baudRate"));
    QCOMPARE(settingsLayout->itemAt(3)->widget()->objectName(), QString("comboBox_baud"));
    QCOMPARE(settingsLayout->itemAt(4)->widget()->objectName(), QString("checkBox_autoScaleY"));
    QCOMPARE(settingsLayout->itemAt(5)->widget()->objectName(), QString("label_sampleWindow"));
    QCOMPARE(settingsLayout->itemAt(6)->widget()->objectName(), QString("spinBox_sampleWindow"));
    QCOMPARE(settingsLayout->itemAt(8)->widget()->objectName(), QString("label_runtimeState"));

    auto* plotItem = mainLayout->itemAt(2);
    QVERIFY2(plotItem != nullptr, "third item should exist");
    auto* plotArea = plotItem->widget();
    QVERIFY2(plotArea != nullptr, "third item should contain widget_plotArea");
    QCOMPARE(plotArea->objectName(), QString("widget_plotArea"));
    const QSizePolicy plotAreaSizePolicy = plotArea->sizePolicy();
    QCOMPARE(plotAreaSizePolicy.horizontalPolicy(), QSizePolicy::Expanding);
    QCOMPARE(plotAreaSizePolicy.verticalPolicy(), QSizePolicy::Expanding);
}

void MainWindowLayoutTest::initial_texts_match_the_approved_design()
{
    MainWindow window;

    auto* openCloseButton = window.findChild<QPushButton*>("pushButton_openClose");
    QVERIFY2(openCloseButton != nullptr, "pushButton_openClose should exist");
    QCOMPARE(openCloseButton->text(), QString("Open"));

    auto* pauseButton = window.findChild<QPushButton*>("pushButton_pause");
    QVERIFY2(pauseButton != nullptr, "pushButton_pause should exist");
    QCOMPARE(pauseButton->text(), QString("Pause"));

    auto* clearButton = window.findChild<QPushButton*>("pushButton_clear");
    QVERIFY2(clearButton != nullptr, "pushButton_clear should exist");
    QCOMPARE(clearButton->text(), QString("Clear"));

    auto* refreshPortsButton = window.findChild<QPushButton*>("pushButton_refreshPorts");
    QVERIFY2(refreshPortsButton != nullptr, "pushButton_refreshPorts should exist");
    QCOMPARE(refreshPortsButton->text(), QString("Refresh"));

    auto* runtimeStateLabel = window.findChild<QLabel*>("label_runtimeState");
    QVERIFY2(runtimeStateLabel != nullptr, "label_runtimeState should exist");
    QCOMPARE(runtimeStateLabel->text(), QString("Demo Mode"));
}

void MainWindowLayoutTest::spacing_and_widths_match_the_compact_layout()
{
    MainWindow window;

    QWidget* centralWidget = window.centralWidget();
    QVERIFY2(centralWidget != nullptr, "centralwidget should exist");

    auto* verticalLayoutMain = qobject_cast<QVBoxLayout*>(centralWidget->layout());
    QVERIFY2(verticalLayoutMain != nullptr, "verticalLayout_main should be attached to centralwidget");
    QCOMPARE(verticalLayoutMain->spacing(), 8);

    const QMargins margins = verticalLayoutMain->contentsMargins();
    QCOMPARE(margins.left(), 12);
    QCOMPARE(margins.top(), 12);
    QCOMPARE(margins.right(), 12);
    QCOMPARE(margins.bottom(), 12);

    auto* comboBoxPort = window.findChild<QComboBox*>("comboBox_port");
    QVERIFY2(comboBoxPort != nullptr, "comboBox_port should exist");
    QCOMPARE(comboBoxPort->maximumWidth(), 150);

    auto* comboBoxBaud = window.findChild<QComboBox*>("comboBox_baud");
    QVERIFY2(comboBoxBaud != nullptr, "comboBox_baud should exist");
    QCOMPARE(comboBoxBaud->maximumWidth(), 120);

    auto* spinBoxSampleWindow = window.findChild<QSpinBox*>("spinBox_sampleWindow");
    QVERIFY2(spinBoxSampleWindow != nullptr, "spinBox_sampleWindow should exist");
    QCOMPARE(spinBoxSampleWindow->maximumWidth(), 100);
}

QTEST_MAIN(MainWindowLayoutTest)
#include "tst_mainwindowlayout.moc"
