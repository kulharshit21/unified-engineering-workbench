#pragma once

#include <QWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include "PlotWidget.hpp"
#include "neural/BackpropagationSolver.hpp"

namespace Workbench::App {

class BackpropView : public QWidget {
    Q_OBJECT

public:
    explicit BackpropView(QWidget *parent = nullptr);
    ~BackpropView() override = default;

private slots:
    void onRunTrainingClicked();
    void onVerifyGradientsClicked();
    void onExportCsvClicked();

private:
    void setupUi();
    void updateUiWithTraining(const std::vector<Core::Neural::TrainingStepRecord> &history);
    void updateGradientTable(const std::vector<Core::Neural::GradientCheckResult> &checks);

    QDoubleSpinBox *m_spinX{nullptr};
    QDoubleSpinBox *m_spinY{nullptr};
    QDoubleSpinBox *m_spinW{nullptr};
    QDoubleSpinBox *m_spinB{nullptr};
    QDoubleSpinBox *m_spinV{nullptr};
    QDoubleSpinBox *m_spinC{nullptr};
    QDoubleSpinBox *m_spinEta{nullptr};
    QSpinBox *m_spinSteps{nullptr};

    QPushButton *m_trainBtn{nullptr};
    QPushButton *m_verifyBtn{nullptr};
    QPushButton *m_exportBtn{nullptr};

    QLabel *m_summaryLabel{nullptr};
    QTableWidget *m_gradientTable{nullptr};
    QTableWidget *m_historyTable{nullptr};
    PlotWidget *m_plotWidget{nullptr};

    std::vector<Core::Neural::TrainingStepRecord> m_lastHistory;
    std::vector<Core::Neural::GradientCheckResult> m_lastChecks;
};

} // namespace Workbench::App
