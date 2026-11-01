#pragma once

#include <QWidget>
#include <QMatrix4x4>
#include <QVector3D>
#include <QPoint>
#include "geometry/StlParser.hpp"
#include "geometry/TemperatureCsvParser.hpp"

namespace Workbench::App {

class OpenGLViewport : public QWidget {
    Q_OBJECT

public:
    explicit OpenGLViewport(QWidget *parent = nullptr);
    ~OpenGLViewport() override = default;

    void setStlModel(const Core::Geometry::StlModel &model);
    void setTemperatureDataset(const Core::Geometry::TemperatureDataset &dataset);

    void setRenderStlMesh(bool enable);
    void setRenderWireframe(bool enable);
    void setRenderPointCloud(bool enable);
    void setPointSize(int size);

    void fitToView();
    void resetCamera();

signals:
    void nodeSelected(int nodeId, double x, double y, double z, double tempC);
    void nodeHovered(int nodeId, double x, double y, double z, double tempC);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QColor temperatureToColor(double tempC, double minT, double maxT) const;
    void drawAxes(QPainter &painter, const QMatrix4x4 &viewMatrix);
    void drawLegend(QPainter &painter);
    void drawSelectedNodeBanner(QPainter &painter);

    Core::Geometry::StlModel m_stlModel;
    Core::Geometry::TemperatureDataset m_tempDataset;

    bool m_hasStl{false};
    bool m_hasCloud{false};

    bool m_renderStlMesh{true};
    bool m_renderWireframe{true};
    bool m_renderPointCloud{true};
    int m_pointSize{7};

    // Camera orbit parameters
    float m_yaw{-45.0f};
    float m_pitch{30.0f};
    float m_distance{2.5f};
    QVector3D m_targetCenter{0.0f, 0.0f, 0.0f};
    float m_modelRadius{1.0f};

    // Mouse tracking
    QPoint m_lastMousePos;
    bool m_isRotating{false};
    bool m_isPanning{false};

    // Inspection selection
    int m_selectedNodeId{-1};
    Core::Geometry::TemperatureNode m_selectedNode;
    bool m_hasSelection{false};
};

} // namespace Workbench::App
