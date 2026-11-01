#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QStatusBar>

namespace Workbench::App {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private:
    void setupUi();
    void createMenus();
    void createStatusBar();

    QTabWidget *m_mainTabs{nullptr};
    QLabel *m_statusLabel{nullptr};
};

} // namespace Workbench::App
