#pragma once

#include <QWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QProgressBar>
#include <QTableWidget>
#include <QLabel>
#include <QThread>
#include "PlotWidget.hpp"
#include "parallel/BenchmarkEngine.hpp"

namespace Workbench::App {

class BenchmarkWorker : public QThread {
    Q_OBJECT
public:
    BenchmarkWorker(int cases, int elements, std::vector<size_t> workers, int reps, uint32_t seed, QObject *parent = nullptr)
        : QThread(parent), m_cases(cases), m_elements(elements), m_workers(std::move(workers)), m_reps(reps), m_seed(seed) {}

    void run() override {
        try {
            auto res = Core::Parallel::BenchmarkEngine::runSuite(m_cases, m_elements, m_workers, m_reps, m_seed);
            emit benchmarkFinished(res);
        } catch (const std::exception &e) {
            emit benchmarkFailed(QString::fromStdString(e.what()));
        } catch (...) {
            emit benchmarkFailed("Unknown error occurred during parallel benchmark.");
        }
    }

signals:
    void benchmarkFinished(const Workbench::Core::Parallel::BenchmarkSuiteResult &result);
    void benchmarkFailed(const QString &errorMsg);

private:
    int m_cases;
    int m_elements;
    std::vector<size_t> m_workers;
    int m_reps;
    uint32_t m_seed;
};

class PerformanceView : public QWidget {
    Q_OBJECT

public:
    explicit PerformanceView(QWidget *parent = nullptr);
    ~PerformanceView() override;

private slots:
    void onRunBenchmarkClicked();
    void onExportCsvClicked();
    void onBenchmarkCompleted(const Core::Parallel::BenchmarkSuiteResult &res);
    void onBenchmarkFailed(const QString &errorMsg);

private:
    void setupUi();
    void updateUiWithResults(const Core::Parallel::BenchmarkSuiteResult &res);

    QSpinBox *m_spinCaseCount{nullptr};
    QSpinBox *m_spinElementCount{nullptr};
    QSpinBox *m_spinRepetitions{nullptr};

    QPushButton *m_runBtn{nullptr};
    QPushButton *m_exportBtn{nullptr};
    QProgressBar *m_progressBar{nullptr};

    QLabel *m_sysInfoLabel{nullptr};
    QLabel *m_statusLabel{nullptr};
    QTableWidget *m_resultsTable{nullptr};
    PlotWidget *m_plotWidget{nullptr};

    BenchmarkWorker *m_worker{nullptr};
    Core::Parallel::BenchmarkSuiteResult m_lastResult;
};

} // namespace Workbench::App
