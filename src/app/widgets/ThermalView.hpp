#pragma once

#include <QWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include "PlotWidget.hpp"
#include "thermal/ThermalParameters.hpp"
#include "thermal/ThermalFeaSolver.hpp"
#include "thermal/ThermalConvergence.hpp"

namespace Workbench::App {

class ThermalView : public QWidget {
    Q_OBJECT

public:
    explicit ThermalView(QWidget *parent = nullptr);
    ~ThermalView() override = default;

private slots:
    void onRunSingleFeaClicked();
    void onRunConvergenceStudyClicked();
    void onExportCsvClicked();

private:
    void setupUi();
    void updatePlotWithSolution(const Core::Thermal::ThermalSolution &sol, const Core::Thermal::ThermalParameters &p);

    QDoubleSpinBox *m_spinL{nullptr};
    QDoubleSpinBox *m_spinA{nullptr};
    QDoubleSpinBox *m_spinK{nullptr};
    QDoubleSpinBox *m_spinQ{nullptr};
    QDoubleSpinBox *m_spinT0{nullptr};
    QDoubleSpinBox *m_spinTL{nullptr};
    QComboBox *m_comboN{nullptr};

    QPushButton *m_runFeaBtn{nullptr};
    QPushButton *m_runConvergenceBtn{nullptr};
    QPushButton *m_exportCsvBtn{nullptr};

    QTableWidget *m_convergenceTable{nullptr};
    PlotWidget *m_plotWidget{nullptr};

    Core::Thermal::ThermalSolution m_lastSolution;
    std::vector<Core::Thermal::ConvergenceEntry> m_lastConvergenceRecords;
};

} // namespace Workbench::App
