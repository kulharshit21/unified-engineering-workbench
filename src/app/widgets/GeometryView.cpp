#include "GeometryView.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <filesystem>

namespace Workbench::App {

GeometryView::GeometryView(QWidget *parent)
    : QWidget(parent) {
    setupUi();
    loadDefaultSyntheticData();
}

void GeometryView::setupUi() {
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    // Left control panel
    auto *controlPanel = new QWidget(this);
    controlPanel->setFixedWidth(380);
    auto *panelLayout = new QVBoxLayout(controlPanel);
    panelLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->setSpacing(10);

    // 1. File Import Box
    auto *fileBox = new QGroupBox("Data Import (Task B)", controlPanel);
    auto *fileLayout = new QVBoxLayout(fileBox);

    m_loadStlBtn = new QPushButton("Load ASCII STL Mesh...", fileBox);
    m_loadStlBtn->setStyleSheet("padding: 6px; font-weight: bold;");
    connect(m_loadStlBtn, &QPushButton::clicked, this, &GeometryView::onLoadStlClicked);
    fileLayout->addWidget(m_loadStlBtn);

    m_stlStatusLabel = new QLabel("STL: None loaded", fileBox);
    m_stlStatusLabel->setStyleSheet("color: #a0a5b0; font-size: 11px;");
    fileLayout->addWidget(m_stlStatusLabel);

    m_loadCsvBtn = new QPushButton("Load ANSYS Nodal CSV...", fileBox);
    m_loadCsvBtn->setStyleSheet("padding: 6px; font-weight: bold;");
    connect(m_loadCsvBtn, &QPushButton::clicked, this, &GeometryView::onLoadCsvClicked);
    fileLayout->addWidget(m_loadCsvBtn);

    m_csvStatusLabel = new QLabel("CSV: None loaded", fileBox);
    m_csvStatusLabel->setStyleSheet("color: #a0a5b0; font-size: 11px;");
    fileLayout->addWidget(m_csvStatusLabel);

    panelLayout->addWidget(fileBox);

    // 2. Display Controls Box
    auto *displayBox = new QGroupBox("Viewport Display Options", controlPanel);
    auto *dispLayout = new QVBoxLayout(displayBox);

    m_chkMesh = new QCheckBox("Show STL Solid Mesh", displayBox);
    m_chkMesh->setChecked(true);
    connect(m_chkMesh, &QCheckBox::toggled, this, [this](bool c) { m_viewport->setRenderStlMesh(c); });
    dispLayout->addWidget(m_chkMesh);

    m_chkWireframe = new QCheckBox("Show Mesh Wireframe", displayBox);
    m_chkWireframe->setChecked(true);
    connect(m_chkWireframe, &QCheckBox::toggled, this, [this](bool c) { m_viewport->setRenderWireframe(c); });
    dispLayout->addWidget(m_chkWireframe);

    m_chkCloud = new QCheckBox("Show Thermal Point Cloud", displayBox);
    m_chkCloud->setChecked(true);
    connect(m_chkCloud, &QCheckBox::toggled, this, [this](bool c) { m_viewport->setRenderPointCloud(c); });
    dispLayout->addWidget(m_chkCloud);

    auto *sizeRow = new QHBoxLayout();
    sizeRow->addWidget(new QLabel("Node Point Size:", displayBox));
    m_pointSizeSlider = new QSlider(Qt::Horizontal, displayBox);
    m_pointSizeSlider->setRange(3, 16);
    m_pointSizeSlider->setValue(7);
    connect(m_pointSizeSlider, &QSlider::valueChanged, this, [this](int v) { m_viewport->setPointSize(v); });
    sizeRow->addWidget(m_pointSizeSlider);
    dispLayout->addLayout(sizeRow);

    m_resetViewBtn = new QPushButton("Reset / Fit Camera View", displayBox);
    connect(m_resetViewBtn, &QPushButton::clicked, this, &GeometryView::onResetViewClicked);
    dispLayout->addWidget(m_resetViewBtn);

    panelLayout->addWidget(displayBox);

    // 3. Node Inspector Box
    auto *inspectBox = new QGroupBox("Selected Node Inspection", controlPanel);
    auto *inspectLayout = new QVBoxLayout(inspectBox);
    m_selectedNodeLabel = new QLabel("Click any thermal node in the 3D viewport to inspect its ID, coordinates, and temperature.", inspectBox);
    m_selectedNodeLabel->setWordWrap(true);
    m_selectedNodeLabel->setStyleSheet("color: #e0e5f0; background: #20242e; padding: 8px; border-radius: 4px;");
    inspectLayout->addWidget(m_selectedNodeLabel);
    panelLayout->addWidget(inspectBox);

    // 4. Statistics Table
    auto *statsBox = new QGroupBox("Mesh & Field Statistics", controlPanel);
    auto *statsLayout = new QVBoxLayout(statsBox);
    m_statsTable = new QTableWidget(7, 2, statsBox);
    m_statsTable->setHorizontalHeaderLabels({"Metric", "Value"});
    m_statsTable->horizontalHeader()->setStretchLastSection(true);
    m_statsTable->verticalHeader()->setVisible(false);
    m_statsTable->setItem(0, 0, new QTableWidgetItem("STL Triangles"));
    m_statsTable->setItem(1, 0, new QTableWidgetItem("STL Bounding Extent"));
    m_statsTable->setItem(2, 0, new QTableWidgetItem("Thermal Nodes"));
    m_statsTable->setItem(3, 0, new QTableWidgetItem("Min Temperature"));
    m_statsTable->setItem(4, 0, new QTableWidgetItem("Max Temperature"));
    m_statsTable->setItem(5, 0, new QTableWidgetItem("Mean Temperature"));
    m_statsTable->setItem(6, 0, new QTableWidgetItem("Duplicate Nodes"));
    statsLayout->addWidget(m_statsTable);
    panelLayout->addWidget(statsBox);

    mainLayout->addWidget(controlPanel);

    // Right Viewport
    m_viewport = new OpenGLViewport(this);
    connect(m_viewport, &OpenGLViewport::nodeSelected, this, &GeometryView::onNodeSelected);
    mainLayout->addWidget(m_viewport, 1);
}

void GeometryView::loadDefaultSyntheticData() {
    // Attempt loading packaged synthetic test assets
    const std::string defaultStl = "data/geometry/sample_bracket.stl";
    const std::string defaultCsv = "data/thermal/synthetic_ansys_temperatures.csv";

    if (std::filesystem::exists(defaultStl)) {
        try {
            auto model = Core::Geometry::StlParser::parseFile(defaultStl);
            m_viewport->setStlModel(model);
            m_stlStatusLabel->setText(QString("STL: %1 (%2 triangles)")
                                          .arg(QString::fromStdString(model.solidName))
                                          .arg(model.triangleCount()));
            m_statsTable->setItem(0, 1, new QTableWidgetItem(QString::number(model.triangleCount())));
            float extent = std::max({model.bounds.max.x - model.bounds.min.x,
                                     model.bounds.max.y - model.bounds.min.y,
                                     model.bounds.max.z - model.bounds.min.z});
            m_statsTable->setItem(1, 1, new QTableWidgetItem(QString("%1 m").arg(extent, 0, 'f', 3)));
        } catch (...) {}
    }

    if (std::filesystem::exists(defaultCsv)) {
        try {
            auto cloud = Core::Geometry::TemperatureCsvParser::parseFile(defaultCsv);
            m_viewport->setTemperatureDataset(cloud);
            m_csvStatusLabel->setText(QString("CSV: %1 nodes [%2°C - %3°C]")
                                          .arg(cloud.nodeCount())
                                          .arg(cloud.minTemperature, 0, 'f', 1)
                                          .arg(cloud.maxTemperature, 0, 'f', 1));
            m_statsTable->setItem(2, 1, new QTableWidgetItem(QString::number(cloud.nodeCount())));
            m_statsTable->setItem(3, 1, new QTableWidgetItem(QString("%1 °C").arg(cloud.minTemperature, 0, 'f', 2)));
            m_statsTable->setItem(4, 1, new QTableWidgetItem(QString("%1 °C").arg(cloud.maxTemperature, 0, 'f', 2)));
            m_statsTable->setItem(5, 1, new QTableWidgetItem(QString("%1 °C").arg(cloud.meanTemperature, 0, 'f', 2)));
            m_statsTable->setItem(6, 1, new QTableWidgetItem(QString::number(cloud.duplicateCount)));
        } catch (...) {}
    }
}

void GeometryView::onLoadStlClicked() {
    QString path = QFileDialog::getOpenFileName(this, "Select ASCII STL File", "data/geometry", "STL Files (*.stl);;All Files (*.*)");
    if (path.isEmpty()) return;

    try {
        auto model = Core::Geometry::StlParser::parseFile(path.toStdString());
        m_viewport->setStlModel(model);
        m_stlStatusLabel->setText(QString("STL: %1 (%2 triangles)")
                                      .arg(QString::fromStdString(model.solidName))
                                      .arg(model.triangleCount()));
        m_statsTable->setItem(0, 1, new QTableWidgetItem(QString::number(model.triangleCount())));
        float extent = std::max({model.bounds.max.x - model.bounds.min.x,
                                 model.bounds.max.y - model.bounds.min.y,
                                 model.bounds.max.z - model.bounds.min.z});
        m_statsTable->setItem(1, 1, new QTableWidgetItem(QString("%1 m").arg(extent, 0, 'f', 3)));
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error Parsing STL", QString("Failed to parse STL:\n%1").arg(e.what()));
    }
}

void GeometryView::onLoadCsvClicked() {
    QString path = QFileDialog::getOpenFileName(this, "Select ANSYS Nodal CSV", "data/thermal", "CSV Files (*.csv);;All Files (*.*)");
    if (path.isEmpty()) return;

    try {
        auto cloud = Core::Geometry::TemperatureCsvParser::parseFile(path.toStdString());
        m_viewport->setTemperatureDataset(cloud);
        m_csvStatusLabel->setText(QString("CSV: %1 nodes [%2°C - %3°C]")
                                      .arg(cloud.nodeCount())
                                      .arg(cloud.minTemperature, 0, 'f', 1)
                                      .arg(cloud.maxTemperature, 0, 'f', 1));
        m_statsTable->setItem(2, 1, new QTableWidgetItem(QString::number(cloud.nodeCount())));
        m_statsTable->setItem(3, 1, new QTableWidgetItem(QString("%1 °C").arg(cloud.minTemperature, 0, 'f', 2)));
        m_statsTable->setItem(4, 1, new QTableWidgetItem(QString("%1 °C").arg(cloud.maxTemperature, 0, 'f', 2)));
        m_statsTable->setItem(5, 1, new QTableWidgetItem(QString("%1 °C").arg(cloud.meanTemperature, 0, 'f', 2)));
        m_statsTable->setItem(6, 1, new QTableWidgetItem(QString::number(cloud.duplicateCount)));
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error Parsing CSV", QString("Failed to parse CSV:\n%1").arg(e.what()));
    }
}

void GeometryView::onResetViewClicked() {
    m_viewport->resetCamera();
}

void GeometryView::onNodeSelected(int nodeId, double x, double y, double z, double tempC) {
    m_selectedNodeLabel->setText(
        QString("<b>Selected Node #%1</b><br>"
                "Position: (%2, %3, %4) m<br>"
                "Temperature: <b>%5 °C</b>")
            .arg(nodeId)
            .arg(x, 0, 'f', 3)
            .arg(y, 0, 'f', 3)
            .arg(z, 0, 'f', 3)
            .arg(tempC, 0, 'f', 2));
}

} // namespace Workbench::App
