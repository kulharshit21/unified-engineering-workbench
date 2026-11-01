#include "PlotWidget.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <QFontMetrics>
#include <cmath>
#include <algorithm>

namespace Workbench::App {

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget(parent) {
    setMinimumSize(320, 240);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void PlotWidget::setTitle(const QString &title) {
    m_title = title;
    update();
}

void PlotWidget::setXAxisLabel(const QString &label) {
    m_xLabel = label;
    update();
}

void PlotWidget::setYAxisLabel(const QString &label) {
    m_yLabel = label;
    update();
}

void PlotWidget::clearSeries() {
    m_seriesList.clear();
    update();
}

void PlotWidget::addSeries(const PlotSeries &series) {
    m_seriesList.push_back(series);
    update();
}

void PlotWidget::setSeries(int index, const PlotSeries &series) {
    if (index >= 0 && index < static_cast<int>(m_seriesList.size())) {
        m_seriesList[index] = series;
        update();
    }
}

void PlotWidget::setAutoRange(bool autoRange) {
    m_autoRange = autoRange;
    update();
}

void PlotWidget::setManualRange(double xMin, double xMax, double yMin, double yMax) {
    m_autoRange = false;
    m_manualXMin = xMin;
    m_manualXMax = xMax;
    m_manualYMin = yMin;
    m_manualYMax = yMax;
    update();
}

void PlotWidget::calculateBounds(double &xMin, double &xMax, double &yMin, double &yMax) const {
    if (!m_autoRange) {
        xMin = m_manualXMin;
        xMax = m_manualXMax;
        yMin = m_manualYMin;
        yMax = m_manualYMax;
        return;
    }

    xMin = 1e30;
    xMax = -1e30;
    yMin = 1e30;
    yMax = -1e30;

    bool hasPoints = false;
    for (const auto &s : m_seriesList) {
        for (const auto &pt : s.points) {
            hasPoints = true;
            if (pt.x() < xMin) xMin = pt.x();
            if (pt.x() > xMax) xMax = pt.x();
            if (pt.y() < yMin) yMin = pt.y();
            if (pt.y() > yMax) yMax = pt.y();
        }
    }

    if (!hasPoints) {
        xMin = 0.0; xMax = 1.0;
        yMin = 0.0; yMax = 1.0;
        return;
    }

    if (std::abs(xMax - xMin) < 1e-9) {
        xMin -= 0.5; xMax += 0.5;
    }
    if (std::abs(yMax - yMin) < 1e-9) {
        yMin -= 0.5; yMax += 0.5;
    }

    // Add 5% padding
    double xPad = (xMax - xMin) * 0.05;
    double yPad = (yMax - yMin) * 0.05;
    xMin -= xPad;
    xMax += xPad;
    yMin -= yPad;
    yMax += yPad;
}

void PlotWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Dark sleek background
    p.fillRect(rect(), QColor(24, 26, 31));

    int leftMargin = 65;
    int rightMargin = 25;
    int topMargin = m_title.isEmpty() ? 30 : 45;
    int bottomMargin = 45;

    QRect plotRect(leftMargin, topMargin, 
                   width() - leftMargin - rightMargin, 
                   height() - topMargin - bottomMargin);

    if (plotRect.width() <= 10 || plotRect.height() <= 10) return;

    // Draw plot area background
    p.fillRect(plotRect, QColor(32, 35, 42));
    p.setPen(QPen(QColor(60, 65, 78), 1));
    p.drawRect(plotRect);

    // Title
    if (!m_title.isEmpty()) {
        p.setPen(QColor(220, 225, 235));
        QFont titleFont = font();
        titleFont.setBold(true);
        titleFont.setPointSize(titleFont.pointSize() + 2);
        p.setFont(titleFont);
        p.drawText(QRect(0, 8, width(), 30), Qt::AlignCenter, m_title);
    }

    double xMin, xMax, yMin, yMax;
    calculateBounds(xMin, xMax, yMin, yMax);

    auto toScreenX = [&](double x) {
        return plotRect.left() + (x - xMin) / (xMax - xMin) * plotRect.width();
    };
    auto toScreenY = [&](double y) {
        return plotRect.bottom() - (y - yMin) / (yMax - yMin) * plotRect.height();
    };

    // Draw subtle grid lines (5 vertical, 5 horizontal)
    p.setPen(QPen(QColor(48, 52, 63), 1, Qt::DashLine));
    QFont labelFont = font();
    labelFont.setPointSize(8);
    p.setFont(labelFont);

    for (int i = 0; i <= 5; ++i) {
        // Horizontal grid
        double fracY = i / 5.0;
        double valY = yMin + fracY * (yMax - yMin);
        int sy = toScreenY(valY);
        p.drawLine(plotRect.left(), sy, plotRect.right(), sy);

        // Y-axis tick label
        p.setPen(QColor(160, 165, 178));
        QString yStr = QString::number(valY, 'g', 4);
        p.drawText(QRect(5, sy - 8, leftMargin - 10, 16), Qt::AlignRight | Qt::AlignVCenter, yStr);
        p.setPen(QPen(QColor(48, 52, 63), 1, Qt::DashLine));

        // Vertical grid
        double fracX = i / 5.0;
        double valX = xMin + fracX * (xMax - xMin);
        int sx = toScreenX(valX);
        p.drawLine(sx, plotRect.top(), sx, plotRect.bottom());

        // X-axis tick label
        p.setPen(QColor(160, 165, 178));
        QString xStr = QString::number(valX, 'g', 4);
        p.drawText(QRect(sx - 35, plotRect.bottom() + 5, 70, 16), Qt::AlignCenter, xStr);
        p.setPen(QPen(QColor(48, 52, 63), 1, Qt::DashLine));
    }

    // Axis labels
    p.setPen(QColor(200, 205, 215));
    QFont axisFont = font();
    axisFont.setBold(true);
    axisFont.setPointSize(9);
    p.setFont(axisFont);

    // X-axis label
    p.drawText(QRect(plotRect.left(), height() - 25, plotRect.width(), 20), Qt::AlignCenter, m_xLabel);

    // Y-axis label (vertical)
    p.save();
    p.translate(15, plotRect.center().y());
    p.rotate(-90);
    p.drawText(QRect(-plotRect.height() / 2, -15, plotRect.height(), 30), Qt::AlignCenter, m_yLabel);
    p.restore();

    // Plot curves with clipping
    p.setClipRect(plotRect);

    for (const auto &series : m_seriesList) {
        if (series.points.empty()) continue;

        p.setPen(QPen(series.color, series.width, series.style));

        QPolygonF poly;
        poly.reserve(static_cast<int>(series.points.size()));
        for (const auto &pt : series.points) {
            poly.append(QPointF(toScreenX(pt.x()), toScreenY(pt.y())));
        }

        p.drawPolyline(poly);

        if (series.showPoints) {
            p.setBrush(series.color);
            for (const auto &screenPt : poly) {
                p.drawEllipse(screenPt, 3.5, 3.5);
            }
        }
    }

    p.setClipping(false);

    // Draw Legend in top-right corner if series have names
    int legendItemCount = 0;
    for (const auto &s : m_seriesList) {
        if (!s.name.isEmpty()) legendItemCount++;
    }

    if (legendItemCount > 0) {
        int legWidth = 140;
        int legHeight = legendItemCount * 20 + 10;
        int legX = plotRect.right() - legWidth - 10;
        int legY = plotRect.top() + 10;

        p.fillRect(QRect(legX, legY, legWidth, legHeight), QColor(24, 26, 31, 210));
        p.setPen(QPen(QColor(60, 65, 78), 1));
        p.drawRect(QRect(legX, legY, legWidth, legHeight));

        int curY = legY + 8;
        QFont legFont = font();
        legFont.setPointSize(8);
        p.setFont(legFont);

        for (const auto &s : m_seriesList) {
            if (s.name.isEmpty()) continue;

            p.setPen(QPen(s.color, s.width, s.style));
            p.drawLine(legX + 8, curY + 6, legX + 28, curY + 6);
            if (s.showPoints) {
                p.setBrush(s.color);
                p.drawEllipse(QPointF(legX + 18, curY + 6), 3, 3);
            }

            p.setPen(QColor(220, 225, 235));
            p.drawText(QRect(legX + 34, curY, legWidth - 38, 16), Qt::AlignLeft | Qt::AlignVCenter, s.name);
            curY += 20;
        }
    }
}

} // namespace Workbench::App
