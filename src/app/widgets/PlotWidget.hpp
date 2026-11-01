#pragma once

#include <QWidget>
#include <vector>
#include <string>
#include <QString>
#include <QColor>
#include <QPointF>

namespace Workbench::App {

struct PlotSeries {
    QString name;
    QColor color{Qt::blue};
    Qt::PenStyle style{Qt::SolidLine};
    int width{2};
    bool showPoints{false};
    std::vector<QPointF> points;
};

class PlotWidget : public QWidget {
    Q_OBJECT

public:
    explicit PlotWidget(QWidget *parent = nullptr);
    ~PlotWidget() override = default;

    void setTitle(const QString &title);
    void setXAxisLabel(const QString &label);
    void setYAxisLabel(const QString &label);

    void clearSeries();
    void addSeries(const PlotSeries &series);
    void setSeries(int index, const PlotSeries &series);

    void setAutoRange(bool autoRange);
    void setManualRange(double xMin, double xMax, double yMin, double yMax);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void calculateBounds(double &xMin, double &xMax, double &yMin, double &yMax) const;

    QString m_title;
    QString m_xLabel{"X"};
    QString m_yLabel{"Y"};

    std::vector<PlotSeries> m_seriesList;

    bool m_autoRange{true};
    double m_manualXMin{0.0};
    double m_manualXMax{1.0};
    double m_manualYMin{0.0};
    double m_manualYMax{1.0};
};

} // namespace Workbench::App
