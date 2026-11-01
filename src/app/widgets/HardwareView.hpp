#pragma once

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QTextEdit>
#include <QTimer>
#include "hardware/DeviceManager.hpp"

namespace Workbench::App {

class HardwareView : public QWidget {
    Q_OBJECT

public:
    explicit HardwareView(QWidget *parent = nullptr);
    ~HardwareView() override = default;

private slots:
    void onConnectAllClicked();
    void onDisconnectAllClicked();
    void onInjectFaultToggled(bool checked);
    void onRecoverClicked();
    void onRefreshTelemetry();
    void onExportReportClicked();

private:
    void setupUi();
    void updateDashboardCards();
    void appendLog(const QString &msg);

    Core::Hardware::DeviceManager m_deviceManager;
    QTimer *m_pollTimer{nullptr};

    QPushButton *m_connectBtn{nullptr};
    QPushButton *m_disconnectBtn{nullptr};
    QCheckBox *m_faultToggle{nullptr};
    QPushButton *m_recoverBtn{nullptr};
    QPushButton *m_exportReportBtn{nullptr};

    // Card widgets
    struct DeviceCard {
        QLabel *nameLabel{nullptr};
        QLabel *statusBadge{nullptr};
        QLabel *interfaceLabel{nullptr};
        QLabel *tempLabel{nullptr};
        QLabel *voltLabel{nullptr};
        QLabel *packetsLabel{nullptr};
    };

    DeviceCard m_mcuCard;
    DeviceCard m_fpgaCard;
    DeviceCard m_jtagCard;

    QTextEdit *m_logView{nullptr};
};

} // namespace Workbench::App
