#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QStatusBar>

namespace Workbench::App {

class GeometryView;
class ThermalView;
class PerformanceView;
class HardwareView;
class BackpropView;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

    int tabCount() const { return m_mainTabs ? m_mainTabs->count() : 0; }
    void setCurrentTab(int index) { if (m_mainTabs) m_mainTabs->setCurrentIndex(index); }
    QString tabText(int index) const { return m_mainTabs ? m_mainTabs->tabText(index) : QString(); }

private:
    void setupUi();
    void createMenus();
    void createStatusBar();

    QTabWidget *m_mainTabs{nullptr};
    QLabel *m_statusLabel{nullptr};

    GeometryView *m_geometryView{nullptr};
    ThermalView *m_thermalView{nullptr};
    PerformanceView *m_performanceView{nullptr};
    HardwareView *m_hardwareView{nullptr};
    BackpropView *m_backpropView{nullptr};
};

} // namespace Workbench::App
