#pragma once

#include <QWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QProgressBar>
#include <QTableWidget>
#include <QLabel>
#include "PlotWidget.hpp"
#include "parallel/BenchmarkEngine.hpp"

namespace Workbench::App {

class PerformanceView : public QWidget {
    Q_OBJECT

public:
    explicit PerformanceView(QWidget *parent = nullptr);
    ~PerformanceView() override = default;

private slots:
    void onRunBenchmarkClicked();
    void onExportCsvClicked();

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
    QTableWidget *m_resultsTable{nullptr};
    PlotWidget *m_plotWidget{nullptr};

    Core::Parallel::BenchmarkSuiteResult m_lastResult;
};

} // namespace Workbench::App
