#include "ThermalView.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>

namespace Workbench::App {

ThermalView::ThermalView(QWidget *parent)
    : QWidget(parent) {
    setupUi();
    onRunConvergenceStudyClicked();
}

void ThermalView::setupUi() {
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    // Left parameter & table panel
    auto *leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(460);
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);

    // 1. Parameter Group
    auto *paramBox = new QGroupBox("1D Thermal Rod Parameters (Task C)", leftPanel);
    auto *form = new QFormLayout(paramBox);

    m_spinL = new QDoubleSpinBox(paramBox);
    m_spinL->setRange(0.01, 100.0);
    m_spinL->setValue(1.0);
    m_spinL->setSuffix(" m");
    form->addRow("Rod Length (L):", m_spinL);

    m_spinA = new QDoubleSpinBox(paramBox);
    m_spinA->setDecimals(6);
    m_spinA->setRange(1e-6, 1.0);
    m_spinA->setValue(1e-4);
    m_spinA->setSuffix(" m²");
    form->addRow("Cross-section (A):", m_spinA);

    m_spinK = new QDoubleSpinBox(paramBox);
    m_spinK->setRange(0.1, 10000.0);
    m_spinK->setValue(10.0);
    m_spinK->setSuffix(" W/(m·K)");
    form->addRow("Conductivity (k):", m_spinK);

    m_spinQ = new QDoubleSpinBox(paramBox);
    m_spinQ->setRange(0.0, 1e7);
    m_spinQ->setValue(10000.0);
    m_spinQ->setSuffix(" W/m³");
    form->addRow("Heat Source (q):", m_spinQ);

    m_spinT0 = new QDoubleSpinBox(paramBox);
    m_spinT0->setRange(-273.15, 5000.0);
    m_spinT0->setValue(100.0);
    m_spinT0->setSuffix(" °C");
    form->addRow("Left Temp T(0):", m_spinT0);

    m_spinTL = new QDoubleSpinBox(paramBox);
    m_spinTL->setRange(-273.15, 5000.0);
    m_spinTL->setValue(20.0);
    m_spinTL->setSuffix(" °C");
    form->addRow("Right Temp T(L):", m_spinTL);

    m_comboN = new QComboBox(paramBox);
    m_comboN->addItems({"10", "20", "40", "80"});
    m_comboN->setCurrentIndex(1); // default 20
    form->addRow("Elements (N):", m_comboN);

    leftLayout->addWidget(paramBox);

    // 2. Action Buttons
    auto *btnLayout = new QHBoxLayout();
    m_runFeaBtn = new QPushButton("Solve Single FEA", leftPanel);
    m_runFeaBtn->setStyleSheet("padding: 6px; font-weight: bold;");
    connect(m_runFeaBtn, &QPushButton::clicked, this, &ThermalView::onRunSingleFeaClicked);
    btnLayout->addWidget(m_runFeaBtn);

    m_runConvergenceBtn = new QPushButton("Run Convergence Study", leftPanel);
    m_runConvergenceBtn->setStyleSheet("padding: 6px; font-weight: bold;");
    connect(m_runConvergenceBtn, &QPushButton::clicked, this, &ThermalView::onRunConvergenceStudyClicked);
    btnLayout->addWidget(m_runConvergenceBtn);

    leftLayout->addLayout(btnLayout);

    m_exportCsvBtn = new QPushButton("Export Solution & Convergence CSV...", leftPanel);
    connect(m_exportCsvBtn, &QPushButton::clicked, this, &ThermalView::onExportCsvClicked);
    leftLayout->addWidget(m_exportCsvBtn);

    // 3. Convergence Table Box
    auto *convBox = new QGroupBox("Convergence Table (p ≈ 2.0 Quadratic Verification)", leftPanel);
    auto *convLayout = new QVBoxLayout(convBox);

    m_convergenceTable = new QTableWidget(4, 6, convBox);
    m_convergenceTable->setHorizontalHeaderLabels({"N", "h (m)", "Observed Error", "Expected", "Ratio", "Order p"});
    m_convergenceTable->horizontalHeader()->setStretchLastSection(true);
    m_convergenceTable->verticalHeader()->setVisible(false);
    convLayout->addWidget(m_convergenceTable);

    leftLayout->addWidget(convBox);
    mainLayout->addWidget(leftPanel);

    // Right Plot Area
    m_plotWidget = new PlotWidget(this);
    m_plotWidget->setTitle("1D Steady-State Thermal Distribution: FEA vs Analytical");
    m_plotWidget->setXAxisLabel("Position x (m)");
    m_plotWidget->setYAxisLabel("Temperature T (°C)");
    mainLayout->addWidget(m_plotWidget, 1);
}

void ThermalView::updatePlotWithSolution(const Core::Thermal::ThermalSolution &sol, const Core::Thermal::ThermalParameters &p) {
    m_plotWidget->clearSeries();

    Core::Thermal::ThermalFeaSolver solver(p);

    // 1. Fine analytical curve (100 samples)
    PlotSeries anaSeries;
    anaSeries.name = "Analytical T(x)";
    anaSeries.color = QColor(60, 160, 255);
    anaSeries.width = 2;
    anaSeries.style = Qt::SolidLine;

    int sampleCount = 100;
    for (int i = 0; i <= sampleCount; ++i) {
        double x = p.length * (static_cast<double>(i) / sampleCount);
        double T = solver.evaluateExact(x);
        anaSeries.points.push_back(QPointF(x, T));
    }
    m_plotWidget->addSeries(anaSeries);

    // 2. FEA Nodal Solution points
    PlotSeries feaSeries;
    feaSeries.name = QString("FEA (N=%1)").arg(sol.numElements);
    feaSeries.color = QColor(255, 140, 40);
    feaSeries.width = 2;
    feaSeries.style = Qt::DashLine;
    feaSeries.showPoints = true;

    for (size_t i = 0; i < sol.nodeCoordinates.size(); ++i) {
        feaSeries.points.push_back(QPointF(sol.nodeCoordinates[i], sol.nodalTemperatures[i]));
    }
    m_plotWidget->addSeries(feaSeries);
}

void ThermalView::onRunSingleFeaClicked() {
    Core::Thermal::ThermalParameters p;
    p.length = m_spinL->value();
    p.area = m_spinA->value();
    p.conductivity = m_spinK->value();
    p.heatGeneration = m_spinQ->value();
    p.tempLeft = m_spinT0->value();
    p.tempRight = m_spinTL->value();

    int N = m_comboN->currentText().toInt();

    Core::Thermal::ThermalFeaSolver solver(p);
    m_lastSolution = solver.solve(N);
    updatePlotWithSolution(m_lastSolution, p);
}

void ThermalView::onRunConvergenceStudyClicked() {
    Core::Thermal::ThermalParameters p;
    p.length = m_spinL->value();
    p.area = m_spinA->value();
    p.conductivity = m_spinK->value();
    p.heatGeneration = m_spinQ->value();
    p.tempLeft = m_spinT0->value();
    p.tempRight = m_spinTL->value();

    Core::Thermal::ThermalFeaSolver solver(p);
    m_lastConvergenceRecords = Core::Thermal::ThermalConvergence::evaluateConvergence(solver, {10, 20, 40, 80});

    m_convergenceTable->setRowCount(static_cast<int>(m_lastConvergenceRecords.size()));
    for (int i = 0; i < static_cast<int>(m_lastConvergenceRecords.size()); ++i) {
        const auto &r = m_lastConvergenceRecords[i];
        m_convergenceTable->setItem(i, 0, new QTableWidgetItem(QString::number(r.numElements)));
        m_convergenceTable->setItem(i, 1, new QTableWidgetItem(QString::number(r.elementLength, 'f', 4)));
        m_convergenceTable->setItem(i, 2, new QTableWidgetItem(QString::number(r.maxMidpointError, 'f', 6)));
        m_convergenceTable->setItem(i, 3, new QTableWidgetItem(QString::number(r.theoreticalError, 'f', 6)));
        m_convergenceTable->setItem(i, 4, new QTableWidgetItem(QString::number(r.errorRatio, 'f', 2)));
        m_convergenceTable->setItem(i, 5, new QTableWidgetItem(QString::number(r.convergenceOrder, 'f', 4)));
    }

    int N = m_comboN->currentText().toInt();
    m_lastSolution = solver.solve(N);
    updatePlotWithSolution(m_lastSolution, p);
}

void ThermalView::onExportCsvClicked() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory to Export Thermal Results", "results/thermal");
    if (dir.isEmpty()) return;

    Core::Thermal::ThermalParameters p;
    p.length = m_spinL->value();
    p.area = m_spinA->value();
    p.conductivity = m_spinK->value();
    p.heatGeneration = m_spinQ->value();
    p.tempLeft = m_spinT0->value();
    p.tempRight = m_spinTL->value();
    Core::Thermal::ThermalFeaSolver solver(p);

    std::string convCsv = Core::Thermal::ThermalConvergence::exportConvergenceCsv(m_lastConvergenceRecords);
    std::string profCsv = Core::Thermal::ThermalConvergence::exportProfilesCsv(solver);

    std::string convPath = dir.toStdString() + "/convergence.csv";
    std::string profPath = dir.toStdString() + "/profiles.csv";

    std::ofstream outConv(convPath);
    if (outConv.is_open()) outConv << convCsv;

    std::ofstream outProf(profPath);
    if (outProf.is_open()) outProf << profCsv;

    QMessageBox::information(this, "Export Succeeded", 
        QString("Exported thermal results successfully:\n%1\n%2")
            .arg(QString::fromStdString(convPath))
            .arg(QString::fromStdString(profPath)));
}

} // namespace Workbench::App
