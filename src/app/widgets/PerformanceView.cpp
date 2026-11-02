#include "PerformanceView.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <thread>
#include <fstream>

Q_DECLARE_METATYPE(Workbench::Core::Parallel::BenchmarkSuiteResult)

namespace Workbench::App {

PerformanceView::PerformanceView(QWidget *parent)
    : QWidget(parent) {
    qRegisterMetaType<Workbench::Core::Parallel::BenchmarkSuiteResult>("Workbench::Core::Parallel::BenchmarkSuiteResult");
    setupUi();
    // Start initial benchmark asynchronously in background thread so UI starts instantly
    onRunBenchmarkClicked();
}

PerformanceView::~PerformanceView() {
    cleanupWorker();
}

void PerformanceView::cleanupWorker() {
    if (m_worker && m_worker->isRunning()) {
        // Signal the benchmark engine to stop between repetitions
        m_cancelRequested.store(true, std::memory_order_relaxed);
        // Disconnect signals so no queued slots fire on a partially destroyed widget
        m_worker->disconnect(this);
        // Wait indefinitely for the worker to observe cancellation and exit cleanly.
        // The cancellation check runs between every repetition (~5 checks per worker
        // configuration), so the wait is at most one repetition duration.
        m_worker->wait();
    }
    delete m_worker;
    m_worker = nullptr;
    m_cancelRequested.store(false, std::memory_order_relaxed);
}

void PerformanceView::setupUi() {
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    // Left controls and table panel
    auto *leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(460);
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);

    // 1. Benchmark Configuration Box
    auto *configBox = new QGroupBox("Multicore Benchmark Setup (Task D)", leftPanel);
    auto *form = new QFormLayout(configBox);

    m_spinCaseCount = new QSpinBox(configBox);
    m_spinCaseCount->setRange(10, 50000);
    m_spinCaseCount->setValue(1000);
    form->addRow("Batch Workload Cases:", m_spinCaseCount);

    m_spinElementCount = new QSpinBox(configBox);
    m_spinElementCount->setRange(10, 500);
    m_spinElementCount->setValue(80);
    form->addRow("FEA Elements / Case (N):", m_spinElementCount);

    m_spinRepetitions = new QSpinBox(configBox);
    m_spinRepetitions->setRange(1, 20);
    m_spinRepetitions->setValue(5);
    form->addRow("Repetitions / Worker:", m_spinRepetitions);

    leftLayout->addWidget(configBox);

    // 2. Hardware System Info Label
    unsigned int hwThreads = std::thread::hardware_concurrency();
    m_sysInfoLabel = new QLabel(QString("Hardware: Host CPU detected with %1 logical execution threads.")
                                    .arg(hwThreads), leftPanel);
    m_sysInfoLabel->setStyleSheet("color: #a0a5b8; font-size: 11px;");
    leftLayout->addWidget(m_sysInfoLabel);

    // 3. Execution Buttons & Progress Bar
    auto *btnRow = new QHBoxLayout();
    m_runBtn = new QPushButton("Execute Parallel Benchmark", leftPanel);
    m_runBtn->setStyleSheet("padding: 7px; font-weight: bold; font-size: 13px;");
    connect(m_runBtn, &QPushButton::clicked, this, &PerformanceView::onRunBenchmarkClicked);
    btnRow->addWidget(m_runBtn, 3);

    m_cancelBtn = new QPushButton("Cancel", leftPanel);
    m_cancelBtn->setEnabled(false);
    m_cancelBtn->setStyleSheet("padding: 7px; font-size: 13px;");
    connect(m_cancelBtn, &QPushButton::clicked, this, &PerformanceView::onCancelBenchmarkClicked);
    btnRow->addWidget(m_cancelBtn, 1);

    leftLayout->addLayout(btnRow);

    m_progressBar = new QProgressBar(leftPanel);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(false);
    m_progressBar->setFixedHeight(6);
    leftLayout->addWidget(m_progressBar);

    m_statusLabel = new QLabel("Ready.", leftPanel);
    m_statusLabel->setStyleSheet("color: #70758a; font-size: 11px;");
    leftLayout->addWidget(m_statusLabel);

    m_exportBtn = new QPushButton("Export Benchmark CSVs...", leftPanel);
    m_exportBtn->setEnabled(false);
    connect(m_exportBtn, &QPushButton::clicked, this, &PerformanceView::onExportCsvClicked);
    leftLayout->addWidget(m_exportBtn);

    // 4. Results Summary Table
    auto *resultsBox = new QGroupBox("Benchmark Metrics Table", leftPanel);
    auto *resLayout = new QVBoxLayout(resultsBox);

    m_resultsTable = new QTableWidget(3, 7, resultsBox);
    m_resultsTable->setHorizontalHeaderLabels({"Workers", "Median", "Min", "Max", "Speedup", "Efficiency", "Max Delta"});
    m_resultsTable->horizontalHeader()->setStretchLastSection(true);
    m_resultsTable->verticalHeader()->setVisible(false);
    resLayout->addWidget(m_resultsTable);

    leftLayout->addWidget(resultsBox);
    mainLayout->addWidget(leftPanel);

    // Right Speedup Plot
    m_plotWidget = new PlotWidget(this);
    m_plotWidget->setTitle("Multicore Scaling: Measured Speedup vs Ideal Linear Scaling");
    m_plotWidget->setXAxisLabel("Worker Threads (p)");
    m_plotWidget->setYAxisLabel("Speedup Factor (S_p = T_1 / T_p)");
    mainLayout->addWidget(m_plotWidget, 1);
}

void PerformanceView::onRunBenchmarkClicked() {
    if (m_worker && m_worker->isRunning()) {
        return; // Already running
    }

    m_cancelRequested.store(false, std::memory_order_relaxed);
    m_runBtn->setEnabled(false);
    m_cancelBtn->setEnabled(true);
    m_exportBtn->setEnabled(false);
    m_progressBar->setRange(0, 0); // Indeterminate marquee animation
    m_statusLabel->setText("Running parallel benchmark in background worker...");

    int cases = m_spinCaseCount->value();
    int elements = m_spinElementCount->value();
    int reps = m_spinRepetitions->value();

    if (m_worker) {
        m_worker->deleteLater();
        m_worker = nullptr;
    }

    m_worker = new BenchmarkWorker(cases, elements, {1, 2, 4}, reps, 42, &m_cancelRequested, this);
    connect(m_worker, &BenchmarkWorker::benchmarkFinished, this, &PerformanceView::onBenchmarkCompleted);
    connect(m_worker, &BenchmarkWorker::benchmarkFailed, this, &PerformanceView::onBenchmarkFailed);
    connect(m_worker, &BenchmarkWorker::benchmarkCancelled, this, &PerformanceView::onBenchmarkCancelled);
    m_worker->start();
}

void PerformanceView::onCancelBenchmarkClicked() {
    if (m_worker && m_worker->isRunning()) {
        m_cancelRequested.store(true, std::memory_order_relaxed);
        m_cancelBtn->setEnabled(false);
        m_statusLabel->setText("Cancellation requested — stopping after current repetition...");
    }
}

void PerformanceView::onBenchmarkCompleted(const Core::Parallel::BenchmarkSuiteResult &res) {
    m_lastResult = res;
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(100);
    m_statusLabel->setText(QString("Completed %1 cases across %2 worker configurations.")
                               .arg(res.caseCount)
                               .arg(res.workerResults.size()));
    m_runBtn->setEnabled(true);
    m_cancelBtn->setEnabled(false);
    m_exportBtn->setEnabled(true);

    updateUiWithResults(m_lastResult);
}

void PerformanceView::onBenchmarkFailed(const QString &errorMsg) {
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_statusLabel->setText("Benchmark failed.");
    m_runBtn->setEnabled(true);
    m_cancelBtn->setEnabled(false);

    QMessageBox::warning(this, "Benchmark Error", QString("Benchmark execution failed:\n%1").arg(errorMsg));
}

void PerformanceView::onBenchmarkCancelled() {
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_statusLabel->setText("Benchmark cancelled by user.");
    m_runBtn->setEnabled(true);
    m_cancelBtn->setEnabled(false);
}

void PerformanceView::updateUiWithResults(const Core::Parallel::BenchmarkSuiteResult &res) {
    m_resultsTable->setRowCount(static_cast<int>(res.workerResults.size()));

    PlotSeries measuredSeries;
    measuredSeries.name = "Measured Speedup (Sp)";
    measuredSeries.color = QColor(60, 200, 120);
    measuredSeries.width = 3;
    measuredSeries.showPoints = true;

    PlotSeries idealSeries;
    idealSeries.name = "Ideal Linear (Sp = p)";
    idealSeries.color = QColor(140, 150, 170);
    idealSeries.width = 2;
    idealSeries.style = Qt::DashLine;

    size_t maxWorkers = 1;

    for (int i = 0; i < static_cast<int>(res.workerResults.size()); ++i) {
        const auto &r = res.workerResults[i];
        if (r.workerCount > maxWorkers) maxWorkers = r.workerCount;

        m_resultsTable->setItem(i, 0, new QTableWidgetItem(QString::number(r.workerCount)));
        m_resultsTable->setItem(i, 1, new QTableWidgetItem(QString("%1 ms").arg(r.medianTimeMs, 0, 'f', 2)));
        m_resultsTable->setItem(i, 2, new QTableWidgetItem(QString("%1 ms").arg(r.minTimeMs, 0, 'f', 2)));
        m_resultsTable->setItem(i, 3, new QTableWidgetItem(QString("%1 ms").arg(r.maxTimeMs, 0, 'f', 2)));
        m_resultsTable->setItem(i, 4, new QTableWidgetItem(QString("%1x").arg(r.speedup, 0, 'f', 2)));
        m_resultsTable->setItem(i, 5, new QTableWidgetItem(QString("%1%").arg(r.efficiency * 100.0, 0, 'f', 1)));
        m_resultsTable->setItem(i, 6, new QTableWidgetItem(QString::number(r.maxDeltaVsSerial, 'e', 2)));

        measuredSeries.points.push_back(QPointF(static_cast<double>(r.workerCount), r.speedup));
    }

    idealSeries.points.push_back(QPointF(1.0, 1.0));
    idealSeries.points.push_back(QPointF(2.0, 2.0));
    idealSeries.points.push_back(QPointF(static_cast<double>(maxWorkers), static_cast<double>(maxWorkers)));

    m_plotWidget->clearSeries();
    m_plotWidget->addSeries(idealSeries);
    m_plotWidget->addSeries(measuredSeries);
}

void PerformanceView::onExportCsvClicked() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory to Export Benchmark CSVs", "results/performance");
    if (dir.isEmpty()) return;

    std::string sumPath = dir.toStdString() + "/summary.csv";
    std::string rawPath = dir.toStdString() + "/raw_timings.csv";

    std::string sumCsv = Core::Parallel::BenchmarkEngine::exportSummaryCsv(m_lastResult);
    std::string rawCsv = Core::Parallel::BenchmarkEngine::exportRawTimingsCsv(m_lastResult);

    {
        std::ofstream out(sumPath);
        if (out.is_open()) out << sumCsv;
    }
    {
        std::ofstream out(rawPath);
        if (out.is_open()) out << rawCsv;
    }

    QMessageBox::information(this, "Export Succeeded", 
        QString("Exported benchmark CSVs successfully:\n%1\n%2")
            .arg(QString::fromStdString(sumPath))
            .arg(QString::fromStdString(rawPath)));
}

} // namespace Workbench::App
