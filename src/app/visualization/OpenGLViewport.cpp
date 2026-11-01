#include "OpenGLViewport.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QFontMetrics>
#include <QLinearGradient>
#include <cmath>
#include <algorithm>

namespace Workbench::App {

OpenGLViewport::OpenGLViewport(QWidget *parent)
    : QOpenGLWidget(parent) {
    setMinimumSize(400, 400);
    setMouseTracking(true);
}

void OpenGLViewport::setStlModel(const Core::Geometry::StlModel &model) {
    m_stlModel = model;
    m_hasStl = (m_stlModel.triangleCount() > 0);
    fitToView();
    update();
}

void OpenGLViewport::setTemperatureDataset(const Core::Geometry::TemperatureDataset &dataset) {
    m_tempDataset = dataset;
    m_hasCloud = !m_tempDataset.nodes.empty();
    fitToView();
    update();
}

void OpenGLViewport::setRenderStlMesh(bool enable) {
    m_renderStlMesh = enable;
    update();
}

void OpenGLViewport::setRenderWireframe(bool enable) {
    m_renderWireframe = enable;
    update();
}

void OpenGLViewport::setRenderPointCloud(bool enable) {
    m_renderPointCloud = enable;
    update();
}

void OpenGLViewport::setPointSize(int size) {
    m_pointSize = std::clamp(size, 3, 20);
    update();
}

void OpenGLViewport::fitToView() {
    float minX = 1e30f, maxX = -1e30f;
    float minY = 1e30f, maxY = -1e30f;
    float minZ = 1e30f, maxZ = -1e30f;
    bool hasBounds = false;

    if (m_hasStl && m_stlModel.bounds.isValid()) {
        hasBounds = true;
        minX = std::min(minX, m_stlModel.bounds.min.x);
        maxX = std::max(maxX, m_stlModel.bounds.max.x);
        minY = std::min(minY, m_stlModel.bounds.min.y);
        maxY = std::max(maxY, m_stlModel.bounds.max.y);
        minZ = std::min(minZ, m_stlModel.bounds.min.z);
        maxZ = std::max(maxZ, m_stlModel.bounds.max.z);
    }

    if (m_hasCloud && m_tempDataset.bounds.isValid()) {
        hasBounds = true;
        minX = std::min(minX, m_tempDataset.bounds.min.x);
        maxX = std::max(maxX, m_tempDataset.bounds.max.x);
        minY = std::min(minY, m_tempDataset.bounds.min.y);
        maxY = std::max(maxY, m_tempDataset.bounds.max.y);
        minZ = std::min(minZ, m_tempDataset.bounds.min.z);
        maxZ = std::max(maxZ, m_tempDataset.bounds.max.z);
    }

    if (hasBounds) {
        m_targetCenter = QVector3D((minX + maxX) * 0.5f, (minY + maxY) * 0.5f, (minZ + maxZ) * 0.5f);
        float dx = maxX - minX;
        float dy = maxY - minY;
        float dz = maxZ - minZ;
        m_modelRadius = std::max({dx, dy, dz, 0.001f});
        m_distance = m_modelRadius * 2.2f;
    } else {
        m_targetCenter = QVector3D(0.0f, 0.0f, 0.0f);
        m_modelRadius = 1.0f;
        m_distance = 2.5f;
    }
}

void OpenGLViewport::resetCamera() {
    m_yaw = -45.0f;
    m_pitch = 30.0f;
    fitToView();
    update();
}

QColor OpenGLViewport::temperatureToColor(double tempC, double minT, double maxT) const {
    double range = maxT - minT;
    double u = (range > 1e-6) ? std::clamp((tempC - minT) / range, 0.0, 1.0) : 0.5;

    // Smooth Turbo/Jet 4-stop gradient: Blue -> Cyan -> Green -> Yellow -> Red
    int r = 0, g = 0, b = 0;
    if (u < 0.25) {
        double f = u / 0.25;
        r = 0;
        g = static_cast<int>(255.0 * f);
        b = 255;
    } else if (u < 0.50) {
        double f = (u - 0.25) / 0.25;
        r = 0;
        g = 255;
        b = static_cast<int>(255.0 * (1.0 - f));
    } else if (u < 0.75) {
        double f = (u - 0.50) / 0.25;
        r = static_cast<int>(255.0 * f);
        g = 255;
        b = 0;
    } else {
        double f = (u - 0.75) / 0.25;
        r = 255;
        g = static_cast<int>(255.0 * (1.0 - f));
        b = 0;
    }

    return QColor(r, g, b);
}

void OpenGLViewport::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Deep modern dark viewport background
    p.fillRect(rect(), QColor(20, 22, 27));

    int w = width();
    int h = height();
    if (w <= 10 || h <= 10) return;

    // View matrix
    QMatrix4x4 view;
    view.translate(0.0f, 0.0f, -m_distance);
    view.rotate(m_pitch, 1.0f, 0.0f, 0.0f);
    view.rotate(m_yaw, 0.0f, 1.0f, 0.0f);
    view.translate(-m_targetCenter);

    // Projection matrix
    QMatrix4x4 proj;
    float aspect = static_cast<float>(w) / static_cast<float>(h);
    proj.perspective(45.0f, aspect, 0.001f * m_modelRadius, 100.0f * m_modelRadius);

    QMatrix4x4 mvp = proj * view;

    auto projectPoint = [&](const QVector3D &pt, QPointF &screenPt, float &depth) -> bool {
        QVector4D clip = mvp * QVector4D(pt, 1.0f);
        if (clip.w() <= 1e-4f) return false;
        float invW = 1.0f / clip.w();
        float ndcX = clip.x() * invW;
        float ndcY = clip.y() * invW;
        float ndcZ = clip.z() * invW;

        if (ndcZ < -1.0f || ndcZ > 1.0f) return false;

        screenPt.setX((ndcX + 1.0f) * 0.5f * w);
        screenPt.setY((1.0f - ndcY) * 0.5f * h);
        depth = clip.w(); // depth in camera space
        return true;
    };

    // 1. Render STL Triangles (Depth sorted)
    if (m_hasStl && (m_renderStlMesh || m_renderWireframe)) {
        struct RenderTriangle {
            QPointF p0, p1, p2;
            float depth;
            float diffuse;
        };

        std::vector<RenderTriangle> renderList;
        renderList.reserve(m_stlModel.triangles.size());

        QVector3D lightDir(0.4f, 0.7f, 0.6f);
        lightDir.normalize();

        for (const auto &tri : m_stlModel.triangles) {
            QVector3D v1(tri.v1.x, tri.v1.y, tri.v1.z);
            QVector3D v2(tri.v2.x, tri.v2.y, tri.v2.z);
            QVector3D v3(tri.v3.x, tri.v3.y, tri.v3.z);

            QPointF sp1, sp2, sp3;
            float d1 = 0.0f, d2 = 0.0f, d3 = 0.0f;
            if (projectPoint(v1, sp1, d1) && projectPoint(v2, sp2, d2) && projectPoint(v3, sp3, d3)) {
                // Compute surface normal
                QVector3D edge1 = v2 - v1;
                QVector3D edge2 = v3 - v1;
                QVector3D norm = QVector3D::crossProduct(edge1, edge2).normalized();

                float diff = std::max(0.25f, std::abs(QVector3D::dotProduct(norm, lightDir)));

                RenderTriangle rt;
                rt.p0 = sp1; rt.p1 = sp2; rt.p2 = sp3;
                rt.depth = (d1 + d2 + d3) / 3.0f;
                rt.diffuse = diff;
                renderList.push_back(rt);
            }
        }

        // Sort triangles back-to-front
        std::sort(renderList.begin(), renderList.end(), [](const RenderTriangle &a, const RenderTriangle &b) {
            return a.depth > b.depth;
        });

        for (const auto &rt : renderList) {
            QPolygonF poly;
            poly << rt.p0 << rt.p1 << rt.p2;

            if (m_renderStlMesh) {
                int baseShade = static_cast<int>(200.0f * rt.diffuse);
                QColor faceColor(baseShade * 85 / 100, baseShade * 95 / 100, baseShade);
                p.setBrush(faceColor);
            } else {
                p.setBrush(Qt::NoBrush);
            }

            if (m_renderWireframe) {
                p.setPen(QPen(QColor(40, 50, 70, 160), 1));
            } else {
                p.setPen(Qt::NoPen);
            }

            p.drawPolygon(poly);
        }
    }

    // 2. Render ANSYS Temperature Point Cloud
    if (m_hasCloud && m_renderPointCloud) {
        struct ProjectedNode {
            int nodeId;
            QPointF screenPt;
            float depth;
            double tempC;
            double x, y, z;
        };

        std::vector<ProjectedNode> pNodes;
        pNodes.reserve(m_tempDataset.nodes.size());

        for (const auto &node : m_tempDataset.nodes) {
            QVector3D pt(static_cast<float>(node.x), static_cast<float>(node.y), static_cast<float>(node.z));
            QPointF sp;
            float d = 0.0f;
            if (projectPoint(pt, sp, d)) {
                pNodes.push_back({node.nodeId, sp, d, node.temperatureC, node.x, node.y, node.z});
            }
        }

        // Sort points back-to-front
        std::sort(pNodes.begin(), pNodes.end(), [](const ProjectedNode &a, const ProjectedNode &b) {
            return a.depth > b.depth;
        });

        for (const auto &pn : pNodes) {
            QColor col = temperatureToColor(pn.tempC, m_tempDataset.minTemperature, m_tempDataset.maxTemperature);
            p.setBrush(col);
            p.setPen(QPen(QColor(10, 10, 15, 200), 1.0));

            p.drawEllipse(pn.screenPt, m_pointSize, m_pointSize);

            // Highlight selected node
            if (m_hasSelection && m_selectedNodeId == pn.nodeId) {
                p.setBrush(Qt::NoBrush);
                p.setPen(QPen(QColor(255, 230, 0), 2.5));
                p.drawEllipse(pn.screenPt, m_pointSize + 4, m_pointSize + 4);
                p.drawEllipse(pn.screenPt, m_pointSize + 8, m_pointSize + 8);
            }
        }
    }

    // 3. Render 3D Coordinate Tripod in bottom-left
    drawAxes(p, view);

    // 4. Render Colorbar Legend in bottom-right
    if (m_hasCloud) {
        drawLegend(p);
    }

    // 5. Render Selected Node Info Banner
    if (m_hasSelection) {
        drawSelectedNodeBanner(p);
    }
}

void OpenGLViewport::drawAxes(QPainter &p, const QMatrix4x4 &viewMatrix) {
    int originX = 50;
    int originY = height() - 50;
    float axisLen = 32.0f;

    // Extract view orientation (without translation)
    QMatrix4x4 rot = viewMatrix;
    rot.setColumn(3, QVector4D(0, 0, 0, 1));

    auto drawAxisLine = [&](const QVector3D &axisDir, const QColor &col, const QString &label) {
        QVector3D transformed = rot.map(axisDir);
        float sx = originX + transformed.x() * axisLen;
        float sy = originY - transformed.y() * axisLen; // invert Y for screen

        p.setPen(QPen(col, 2.5));
        p.drawLine(QPointF(originX, originY), QPointF(sx, sy));

        p.setPen(col);
        QFont f = font();
        f.setBold(true);
        f.setPointSize(8);
        p.setFont(f);
        p.drawText(QPointF(sx + 3, sy + 3), label);
    };

    drawAxisLine(QVector3D(1, 0, 0), QColor(240, 70, 70), "X");
    drawAxisLine(QVector3D(0, 1, 0), QColor(60, 210, 80), "Y");
    drawAxisLine(QVector3D(0, 0, 1), QColor(60, 150, 255), "Z");
}

void OpenGLViewport::drawLegend(QPainter &p) {
    int legWidth = 36;
    int legHeight = 160;
    int x = width() - legWidth - 65;
    int y = height() - legHeight - 20;

    // Legend panel box
    p.fillRect(QRect(x - 8, y - 24, legWidth + 60, legHeight + 36), QColor(24, 27, 33, 210));
    p.setPen(QPen(QColor(60, 65, 75), 1));
    p.drawRect(QRect(x - 8, y - 24, legWidth + 60, legHeight + 36));

    // Legend title
    p.setPen(QColor(220, 225, 235));
    QFont titleFont = font();
    titleFont.setBold(true);
    titleFont.setPointSize(8);
    p.setFont(titleFont);
    p.drawText(QRect(x - 5, y - 20, 70, 16), Qt::AlignLeft, "Temp (°C)");

    // Gradient bar
    QLinearGradient grad(QPointF(x, y), QPointF(x, y + legHeight));
    grad.setColorAt(0.00, temperatureToColor(1.0, 0.0, 1.0)); // Red (Hot)
    grad.setColorAt(0.25, temperatureToColor(0.75, 0.0, 1.0));
    grad.setColorAt(0.50, temperatureToColor(0.50, 0.0, 1.0));
    grad.setColorAt(0.75, temperatureToColor(0.25, 0.0, 1.0));
    grad.setColorAt(1.00, temperatureToColor(0.00, 0.0, 1.0)); // Blue (Cold)

    p.fillRect(QRect(x, y, 16, legHeight), grad);
    p.setPen(QPen(QColor(80, 85, 95), 1));
    p.drawRect(QRect(x, y, 16, legHeight));

    // Tick labels
    QFont labelFont = font();
    labelFont.setPointSize(7);
    p.setFont(labelFont);
    p.setPen(QColor(200, 205, 215));

    double maxT = m_tempDataset.maxTemperature;
    double minT = m_tempDataset.minTemperature;
    double midT = (maxT + minT) * 0.5;

    p.drawText(QPointF(x + 22, y + 10), QString::number(maxT, 'f', 1));
    p.drawText(QPointF(x + 22, y + legHeight / 2 + 4), QString::number(midT, 'f', 1));
    p.drawText(QPointF(x + 22, y + legHeight), QString::number(minT, 'f', 1));
}

void OpenGLViewport::drawSelectedNodeBanner(QPainter &p) {
    int bannerWidth = 360;
    int bannerHeight = 36;
    int x = (width() - bannerWidth) / 2;
    int y = 15;

    p.fillRect(QRect(x, y, bannerWidth, bannerHeight), QColor(28, 32, 42, 230));
    p.setPen(QPen(QColor(255, 200, 50), 1.5));
    p.drawRoundedRect(QRect(x, y, bannerWidth, bannerHeight), 4, 4);

    p.setPen(QColor(240, 245, 255));
    QFont f = font();
    f.setPointSize(8);
    f.setBold(true);
    p.setFont(f);

    QString text = QString("Node #%1 | (%2, %3, %4) m | Temp: %5 °C")
                       .arg(m_selectedNode.nodeId)
                       .arg(m_selectedNode.x, 0, 'f', 3)
                       .arg(m_selectedNode.y, 0, 'f', 3)
                       .arg(m_selectedNode.z, 0, 'f', 3)
                       .arg(m_selectedNode.temperatureC, 0, 'f', 2);

    p.drawText(QRect(x, y, bannerWidth, bannerHeight), Qt::AlignCenter, text);
}

void OpenGLViewport::mousePressEvent(QMouseEvent *event) {
    m_lastMousePos = event->pos();
    if (event->button() == Qt::LeftButton) {
        // First check if user clicked on a temperature node to inspect
        if (m_hasCloud) {
            int w = width();
            int h = height();
            QMatrix4x4 view;
            view.translate(0.0f, 0.0f, -m_distance);
            view.rotate(m_pitch, 1.0f, 0.0f, 0.0f);
            view.rotate(m_yaw, 0.0f, 1.0f, 0.0f);
            view.translate(-m_targetCenter);

            QMatrix4x4 proj;
            float aspect = static_cast<float>(w) / static_cast<float>(h);
            proj.perspective(45.0f, aspect, 0.001f * m_modelRadius, 100.0f * m_modelRadius);
            QMatrix4x4 mvp = proj * view;

            float bestDistSq = 18.0f * 18.0f; // 18px pick radius
            int foundId = -1;
            Core::Geometry::TemperatureNode foundNode;

            for (const auto &node : m_tempDataset.nodes) {
                QVector4D clip = mvp * QVector4D(static_cast<float>(node.x), 
                                                 static_cast<float>(node.y), 
                                                 static_cast<float>(node.z), 1.0f);
                if (clip.w() > 1e-4f) {
                    float invW = 1.0f / clip.w();
                    float sx = (clip.x() * invW + 1.0f) * 0.5f * w;
                    float sy = (1.0f - clip.y() * invW) * 0.5f * h;
                    float dx = sx - event->pos().x();
                    float dy = sy - event->pos().y();
                    float distSq = dx * dx + dy * dy;
                    if (distSq < bestDistSq) {
                        bestDistSq = distSq;
                        foundId = node.nodeId;
                        foundNode = node;
                    }
                }
            }

            if (foundId != -1) {
                m_hasSelection = true;
                m_selectedNodeId = foundId;
                m_selectedNode = foundNode;
                emit nodeSelected(foundNode.nodeId, foundNode.x, foundNode.y, foundNode.z, foundNode.temperatureC);
                update();
                return;
            }
        }
        m_isRotating = true;
    } else if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
        m_isPanning = true;
    }
}

void OpenGLViewport::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->pos().x() - m_lastMousePos.x();
    int dy = event->pos().y() - m_lastMousePos.y();
    m_lastMousePos = event->pos();

    if (m_isRotating) {
        m_yaw += dx * 0.5f;
        m_pitch += dy * 0.5f;
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
        update();
    } else if (m_isPanning) {
        float panSpeed = 0.002f * m_modelRadius * (m_distance / m_modelRadius);
        QMatrix4x4 rot;
        rot.rotate(m_pitch, 1, 0, 0);
        rot.rotate(m_yaw, 0, 1, 0);
        QVector3D panRight = rot.inverted().map(QVector3D(1, 0, 0));
        QVector3D panUp = rot.inverted().map(QVector3D(0, 1, 0));

        m_targetCenter -= panRight * (dx * panSpeed);
        m_targetCenter += panUp * (dy * panSpeed);
        update();
    }
}

void OpenGLViewport::mouseReleaseEvent(QMouseEvent *) {
    m_isRotating = false;
    m_isPanning = false;
}

void OpenGLViewport::wheelEvent(QWheelEvent *event) {
    float numDegrees = event->angleDelta().y() / 8.0f;
    float numSteps = numDegrees / 15.0f;
    float zoomFactor = std::pow(0.90f, numSteps);

    m_distance = std::clamp(m_distance * zoomFactor, 0.05f * m_modelRadius, 20.0f * m_modelRadius);
    update();
}

} // namespace Workbench::App
