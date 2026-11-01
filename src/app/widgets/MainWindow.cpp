#include "MainWindow.hpp"
#include "GeometryView.hpp"
#include "ThermalView.hpp"
#include "PerformanceView.hpp"
#include "HardwareView.hpp"
#include "BackpropView.hpp"
#include "core/CoreVersion.hpp"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>

namespace Workbench::App {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setupUi();
    createMenus();
    createStatusBar();
}

void MainWindow::setupUi() {
    setWindowTitle(QString::fromStdString(Core::getProjectName()));
    resize(1360, 860);
    setMinimumSize(1080, 720);

    // Apply dark engineering stylesheet for modern UI feel
    setStyleSheet(
        "QMainWindow { background-color: #1a1c23; color: #e0e4ef; }"
        "QTabWidget::pane { border: 1px solid #2e3442; background: #1f222a; }"
        "QTabBar::tab { background: #262a34; color: #a0a6b8; padding: 8px 16px; margin-right: 2px; font-weight: bold; border-top-left-radius: 4px; border-top-right-radius: 4px; }"
        "QTabBar::tab:selected { background: #323846; color: #60b0ff; border-bottom: 2px solid #3090ff; }"
        "QGroupBox { border: 1px solid #353b4c; border-radius: 6px; margin-top: 10px; font-weight: bold; color: #d0d6e6; padding-top: 15px; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 5px; color: #60b0ff; }"
        "QPushButton { background-color: #2b3345; color: #dce2f0; border: 1px solid #3d4760; border-radius: 4px; padding: 5px 10px; }"
        "QPushButton:hover { background-color: #38435c; border-color: #556688; }"
        "QPushButton:pressed { background-color: #1e2432; }"
        "QTableWidget { background-color: #181a20; gridline-color: #2a2e3a; color: #e0e4f0; selection-background-color: #354565; }"
        "QHeaderView::section { background-color: #242832; color: #9aa2b5; padding: 4px; border: 1px solid #2e3442; font-weight: bold; font-size: 11px; }"
        "QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox { background-color: #181a20; color: #e0e4f0; border: 1px solid #353b4c; border-radius: 3px; padding: 3px 6px; }"
    );

    m_mainTabs = new QTabWidget(this);
    m_mainTabs->setTabPosition(QTabWidget::North);
    m_mainTabs->setDocumentMode(true);

    m_geometryView = new GeometryView(this);
    m_thermalView = new ThermalView(this);
    m_performanceView = new PerformanceView(this);
    m_hardwareView = new HardwareView(this);
    m_backpropView = new BackpropView(this);

    m_mainTabs->addTab(m_geometryView, "1. Geometry (CAD / ANSYS 3D)");
    m_mainTabs->addTab(m_thermalView, "2. Thermal FEA Analysis");
    m_mainTabs->addTab(m_performanceView, "3. Multicore Performance");
    m_mainTabs->addTab(m_hardwareView, "4. Embedded Integration");
    m_mainTabs->addTab(m_backpropView, "5. Backpropagation");

    setCentralWidget(m_mainTabs);
}

void MainWindow::createMenus() {
    auto *fileMenu = menuBar()->addMenu("&File");
    auto *exitAction = fileMenu->addAction("E&xit");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    auto *helpMenu = menuBar()->addMenu("&Help");
    auto *aboutAction = helpMenu->addAction("&About Workbench");
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "About PASS Science Works Workbench",
            QString("<h3>%1</h3>"
                    "<p>Version: %2</p>"
                    "<p>R&D Common Internship Technical Assignment (2026–2027)</p>"
                    "<p><b>Zero-Fabrication Empirical Engineering Workbench</b></p>"
                    "<p>Modules:</p>"
                    "<ul>"
                    "<li><b>Task A:</b> Unified Qt 6 Engineering Desktop Interface</li>"
                    "<li><b>Task B:</b> ASCII STL CAD & ANSYS Nodal CSV 3D Point Cloud Inspection</li>"
                    "<li><b>Task C:</b> 1D Steady-State Thermal FEA with Quadratic Order-of-Accuracy (p=2.0)</li>"
                    "<li><b>Task D:</b> Multicore Parallel Scaling & Numerical Consistency Benchmark</li>"
                    "<li><b>Task E:</b> Embedded Target Integration (RP2040, Artix-7, JTAG-HS2) with Fault Recovery</li>"
                    "<li><b>Task F:</b> Simultaneous Backpropagation & Finite-Difference Verification</li>"
                    "</ul>")
            .arg(QString::fromStdString(Core::getProjectName()))
            .arg(QString::fromStdString(Core::getVersionString())));
    });
}

void MainWindow::createStatusBar() {
    m_statusLabel = new QLabel("System Operational | All Modules Loaded | Standalone Offline Mode", this);
    m_statusLabel->setStyleSheet("color: #70d090; font-weight: bold; margin-left: 8px;");
    statusBar()->addWidget(m_statusLabel, 1);
}

} // namespace Workbench::App
