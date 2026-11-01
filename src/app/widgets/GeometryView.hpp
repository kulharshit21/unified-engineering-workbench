#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QTableWidget>
#include "../visualization/OpenGLViewport.hpp"

namespace Workbench::App {

class GeometryView : public QWidget {
    Q_OBJECT

public:
    explicit GeometryView(QWidget *parent = nullptr);
    ~GeometryView() override = default;

private slots:
    void onLoadStlClicked();
    void onLoadCsvClicked();
    void onResetViewClicked();
    void onNodeSelected(int nodeId, double x, double y, double z, double tempC);

private:
    void setupUi();
    void loadDefaultSyntheticData();

    OpenGLViewport *m_viewport{nullptr};

    QPushButton *m_loadStlBtn{nullptr};
    QPushButton *m_loadCsvBtn{nullptr};
    QPushButton *m_resetViewBtn{nullptr};

    QCheckBox *m_chkMesh{nullptr};
    QCheckBox *m_chkWireframe{nullptr};
    QCheckBox *m_chkCloud{nullptr};
    QSlider *m_pointSizeSlider{nullptr};

    QLabel *m_stlStatusLabel{nullptr};
    QLabel *m_csvStatusLabel{nullptr};
    QLabel *m_selectedNodeLabel{nullptr};

    QTableWidget *m_statsTable{nullptr};
};

} // namespace Workbench::App
