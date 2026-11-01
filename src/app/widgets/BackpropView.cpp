#include "BackpropView.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>

namespace Workbench::App {

BackpropView::BackpropView(QWidget *parent)
    : QWidget(parent) {
    setupUi();
    onVerifyGradientsClicked();
    onRunTrainingClicked();
}

void BackpropView::setupUi() {
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    // Left control panel
    auto *leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(460);
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);

    // 1. Parameter Config Box
    auto *paramBox = new QGroupBox("Neural Network Setup (Task F)", leftPanel);
    auto *form = new QFormLayout(paramBox);

    m_spinX = new QDoubleSpinBox(paramBox);
    m_spinX->setRange(-100.0, 100.0);
    m_spinX->setValue(1.0);
    form->addRow("Input Sample (x):", m_spinX);

    m_spinY = new QDoubleSpinBox(paramBox);
    m_spinY->setRange(-100.0, 100.0);
    m_spinY->setValue(2.0);
    form->addRow("Target Value (y):", m_spinY);

    m_spinW = new QDoubleSpinBox(paramBox);
    m_spinW->setRange(-10.0, 10.0);
    m_spinW->setValue(0.5);
    form->addRow("Initial Weight (w):", m_spinW);

    m_spinB = new QDoubleSpinBox(paramBox);
    m_spinB->setRange(-10.0, 10.0);
    m_spinB->setValue(0.0);
    form->addRow("Initial Bias (b):", m_spinB);

    m_spinV = new QDoubleSpinBox(paramBox);
    m_spinV->setRange(-10.0, 10.0);
    m_spinV->setValue(1.0);
    form->addRow("Output Weight (v):", m_spinV);

    m_spinC = new QDoubleSpinBox(paramBox);
    m_spinC->setRange(-10.0, 10.0);
    m_spinC->setValue(0.0);
    form->addRow("Output Bias (c):", m_spinC);

    m_spinEta = new QDoubleSpinBox(paramBox);
    m_spinEta->setDecimals(4);
    m_spinEta->setRange(0.0001, 2.0);
    m_spinEta->setValue(0.1);
    form->addRow("Learning Rate (η):", m_spinEta);

    m_spinSteps = new QSpinBox(paramBox);
    m_spinSteps->setRange(1, 10000);
    m_spinSteps->setValue(100);
    form->addRow("Training Steps:", m_spinSteps);

    leftLayout->addWidget(paramBox);

    // 2. Action Buttons
    auto *btnRow = new QHBoxLayout();
    m_trainBtn = new QPushButton("Run Training", leftPanel);
    m_trainBtn->setStyleSheet("padding: 6px; font-weight: bold;");
    connect(m_trainBtn, &QPushButton::clicked, this, &BackpropView::onRunTrainingClicked);
    btnRow->addWidget(m_trainBtn);

    m_verifyBtn = new QPushButton("Verify Gradients (ε=1e-6)", leftPanel);
    m_verifyBtn->setStyleSheet("padding: 6px; font-weight: bold;");
    connect(m_verifyBtn, &QPushButton::clicked, this, &BackpropView::onVerifyGradientsClicked);
    btnRow->addWidget(m_verifyBtn);

    leftLayout->addLayout(btnRow);

    m_exportBtn = new QPushButton("Export Loss & Gradient CSVs...", leftPanel);
    connect(m_exportBtn, &QPushButton::clicked, this, &BackpropView::onExportCsvClicked);
    leftLayout->addWidget(m_exportBtn);

    // 3. Summary label
    m_summaryLabel = new QLabel("Ready.", leftPanel);
    m_summaryLabel->setStyleSheet("background: #20242e; color: #70d090; padding: 6px; border-radius: 4px; font-size: 11px;");
    leftLayout->addWidget(m_summaryLabel);

    // 4. Gradient Verification Table
    auto *gradBox = new QGroupBox("Finite-Difference Verification Table", leftPanel);
    auto *gradLayout = new QVBoxLayout(gradBox);
    m_gradientTable = new QTableWidget(4, 5, gradBox);
    m_gradientTable->setHorizontalHeaderLabels({"Param", "Analytical", "Numerical", "Absolute Error", "Status"});
    m_gradientTable->horizontalHeader()->setStretchLastSection(true);
    m_gradientTable->verticalHeader()->setVisible(false);
    gradLayout->addWidget(m_gradientTable);
    leftLayout->addWidget(gradBox);

    mainLayout->addWidget(leftPanel);

    // Right side: Plot and Training History
    auto *rightPanel = new QWidget(this);
    auto *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(10);

    m_plotWidget = new PlotWidget(rightPanel);
    m_plotWidget->setTitle("Backpropagation Loss Convergence: J = 0.5*(y_hat - y)²");
    m_plotWidget->setXAxisLabel("Training Step");
    m_plotWidget->setYAxisLabel("Loss J");
    rightLayout->addWidget(m_plotWidget, 1);

    auto *histBox = new QGroupBox("Step-by-Step Training History", rightPanel);
    auto *histLayout = new QVBoxLayout(histBox);
    histBox->setFixedHeight(180);

    m_historyTable = new QTableWidget(0, 7, histBox);
    m_historyTable->setHorizontalHeaderLabels({"Step", "Loss J", "w", "b", "v", "c", "y_hat"});
    m_historyTable->horizontalHeader()->setStretchLastSection(true);
    m_historyTable->verticalHeader()->setVisible(false);
    histLayout->addWidget(m_historyTable);

    rightLayout->addWidget(histBox);
    mainLayout->addWidget(rightPanel, 1);
}

void BackpropView::onVerifyGradientsClicked() {
    Core::Neural::NetworkParameters p;
    p.w = m_spinW->value();
    p.b = m_spinB->value();
    p.v = m_spinV->value();
    p.c = m_spinC->value();

    double x = m_spinX->value();
    double y = m_spinY->value();

    m_lastChecks = Core::Neural::BackpropagationSolver::verifyGradients(x, y, p, 1e-6, 1e-5);
    updateGradientTable(m_lastChecks);
}

void BackpropView::updateGradientTable(const std::vector<Core::Neural::GradientCheckResult> &checks) {
    m_gradientTable->setRowCount(static_cast<int>(checks.size()));
    for (int i = 0; i < static_cast<int>(checks.size()); ++i) {
        const auto &c = checks[i];
        m_gradientTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(c.paramName)));
        m_gradientTable->setItem(i, 1, new QTableWidgetItem(QString::number(c.analyticalGrad, 'f', 6)));
        m_gradientTable->setItem(i, 2, new QTableWidgetItem(QString::number(c.numericalGrad, 'f', 6)));
        m_gradientTable->setItem(i, 3, new QTableWidgetItem(QString::number(c.absoluteError, 'e', 2)));

        auto *statusItem = new QTableWidgetItem(c.passed ? "PASS" : "FAIL");
        statusItem->setForeground(c.passed ? QColor(80, 220, 100) : QColor(255, 60, 60));
        m_gradientTable->setItem(i, 4, statusItem);
    }
}

void BackpropView::onRunTrainingClicked() {
    Core::Neural::NetworkParameters initParams;
    initParams.w = m_spinW->value();
    initParams.b = m_spinB->value();
    initParams.v = m_spinV->value();
    initParams.c = m_spinC->value();

    double x = m_spinX->value();
    double y = m_spinY->value();
    double eta = m_spinEta->value();
    int steps = m_spinSteps->value();

    m_lastHistory = Core::Neural::BackpropagationSolver::train(x, y, initParams, eta, steps);
    updateUiWithTraining(m_lastHistory);
}

void BackpropView::updateUiWithTraining(const std::vector<Core::Neural::TrainingStepRecord> &history) {
    if (history.empty()) return;

    // Update Summary Label
    const auto &first = history.front();
    const auto &last = history.back();
    m_summaryLabel->setText(
        QString("<b>Training Converged</b>: Initial Loss: %1 -> Final Loss: %2<br>"
                "Target y: %3 | Final Prediction ŷ: %4")
            .arg(first.loss, 0, 'f', 6)
            .arg(last.loss, 0, 'e', 4)
            .arg(m_spinY->value(), 0, 'f', 2)
            .arg(last.y_hat, 0, 'f', 6));

    // Update Plot
    m_plotWidget->clearSeries();
    PlotSeries lossSeries;
    lossSeries.name = "Loss J";
    lossSeries.color = QColor(255, 100, 70);
    lossSeries.width = 2;
    for (const auto &r : history) {
        lossSeries.points.push_back(QPointF(r.step, r.loss));
    }
    m_plotWidget->addSeries(lossSeries);

    // Update History Table (every 5 steps to keep snappy)
    m_historyTable->setRowCount(0);
    int row = 0;
    for (size_t i = 0; i < history.size(); ++i) {
        if (i % 5 == 0 || i == history.size() - 1) {
            const auto &r = history[i];
            m_historyTable->insertRow(row);
            m_historyTable->setItem(row, 0, new QTableWidgetItem(QString::number(r.step)));
            m_historyTable->setItem(row, 1, new QTableWidgetItem(QString::number(r.loss, 'e', 4)));
            m_historyTable->setItem(row, 2, new QTableWidgetItem(QString::number(r.params.w, 'f', 4)));
            m_historyTable->setItem(row, 3, new QTableWidgetItem(QString::number(r.params.b, 'f', 4)));
            m_historyTable->setItem(row, 4, new QTableWidgetItem(QString::number(r.params.v, 'f', 4)));
            m_historyTable->setItem(row, 5, new QTableWidgetItem(QString::number(r.params.c, 'f', 4)));
            m_historyTable->setItem(row, 6, new QTableWidgetItem(QString::number(r.y_hat, 'f', 6)));
            row++;
        }
    }
}

void BackpropView::onExportCsvClicked() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory to Export Backprop CSVs", "results/backprop");
    if (dir.isEmpty()) return;

    std::string histPath = dir.toStdString() + "/loss_history.csv";
    std::string gradPath = dir.toStdString() + "/gradient_verification.csv";

    Core::Neural::BackpropagationSolver::exportLossHistoryCsv(histPath, m_lastHistory);
    Core::Neural::BackpropagationSolver::exportGradientCheckCsv(gradPath, m_lastChecks);

    QMessageBox::information(this, "Export Succeeded", 
        QString("Exported backprop CSVs successfully:\n%1\n%2")
            .arg(QString::fromStdString(histPath))
            .arg(QString::fromStdString(gradPath)));
}

} // namespace Workbench::App
