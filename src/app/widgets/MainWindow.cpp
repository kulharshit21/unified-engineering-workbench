#include "MainWindow.hpp"
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
    resize(1280, 800);
    setMinimumSize(1024, 680);

    m_mainTabs = new QTabWidget(this);
    m_mainTabs->setTabPosition(QTabWidget::North);
    m_mainTabs->setDocumentMode(true);

    // Initial placeholder tabs (will be replaced by full dedicated view widgets)
    auto *geoTab = new QWidget(this);
    auto *thermalTab = new QWidget(this);
    auto *perfTab = new QWidget(this);
    auto *hwTab = new QWidget(this);
    auto *bpTab = new QWidget(this);

    m_mainTabs->addTab(geoTab, "1. Geometry (CAD/ANSYS)");
    m_mainTabs->addTab(thermalTab, "2. Thermal FEA Analysis");
    m_mainTabs->addTab(perfTab, "3. Multicore Performance");
    m_mainTabs->addTab(hwTab, "4. Embedded Integration");
    m_mainTabs->addTab(bpTab, "5. Backpropagation");

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
                    "<p>Features: 1D Steady-State Thermal FEA, ASCII STL & ANSYS CSV Viewer, "
                    "Multicore Benchmarks, Embedded Device Mocks, Handwritten Backpropagation.</p>")
            .arg(QString::fromStdString(Core::getProjectName()))
            .arg(QString::fromStdString(Core::getVersionString())));
    });
}

void MainWindow::createStatusBar() {
    m_statusLabel = new QLabel("System Ready | Offline Standalone Mode", this);
    statusBar()->addWidget(m_statusLabel, 1);
}

} // namespace Workbench::App
