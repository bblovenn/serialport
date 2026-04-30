#ifndef TST_MAINWINDOWLAYOUT_H
#define TST_MAINWINDOWLAYOUT_H

#include <QObject>

class MainWindowLayoutTest : public QObject
{
    Q_OBJECT

private slots:
    void layout_has_header_settings_and_splitter();
    void initial_texts_match_the_current_design();
    void spacing_and_widths_match_the_compact_layout();
    void runtime_state_uses_explicit_property();
    void plot_auto_scale_uses_visible_window_only();
};

#endif // TST_MAINWINDOWLAYOUT_H
