#include "HardwareView.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <fstream>

namespace Workbench::App {

HardwareView::HardwareView(QWidget *parent)
    : QWidget(parent) {
    m_deviceManager.registerDefaultMockDevices();
    setupUi();

    m_pollTimer = new QTimer(this);
    connect(m_pollTimer, &QTimer::timeout, this, &HardwareView::onRefreshTelemetry);
    m_pollTimer->start(500); // 500 ms refresh

    onConnectAllClicked();
}

void HardwareView::setupUi() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    // Top Controls Bar
    auto *topBar = new QWidget(this);
    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);

    m_connectBtn = new QPushButton("Connect All Devices", topBar);
    m_connectBtn->setStyleSheet("padding: 6px 12px; font-weight: bold;");
    connect(m_connectBtn, &QPushButton::clicked, this, &HardwareView::onConnectAllClicked);
    topLayout->addWidget(m_connectBtn);

    m_disconnectBtn = new QPushButton("Disconnect All", topBar);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &HardwareView::onDisconnectAllClicked);
    topLayout->addWidget(m_disconnectBtn);

    m_faultToggle = new QCheckBox("Inject FPGA Timeout Fault", topBar);
    m_faultToggle->setStyleSheet("color: #ffaa44; font-weight: bold; margin-left: 10px;");
    connect(m_faultToggle, &QCheckBox::toggled, this, &HardwareView::onInjectFaultToggled);
    topLayout->addWidget(m_faultToggle);

    m_recoverBtn = new QPushButton("Attempt Recovery", topBar);
    connect(m_recoverBtn, &QPushButton::clicked, this, &HardwareView::onRecoverClicked);
    topLayout->addWidget(m_recoverBtn);

    m_exportReportBtn = new QPushButton("Export Diagnostic Report...", topBar);
    connect(m_exportReportBtn, &QPushButton::clicked, this, &HardwareView::onExportReportClicked);
    topLayout->addWidget(m_exportReportBtn);

    topLayout->addStretch(1);

    auto *badge = new QLabel("MOCK ADAPTER SIMULATION (Zero-Fabrication Disclosed)", topBar);
    badge->setStyleSheet("background: #2a3545; color: #70b0ff; border: 1px solid #3d5575; padding: 4px 8px; border-radius: 4px; font-weight: bold; font-size: 11px;");
    topLayout->addWidget(badge);

    mainLayout->addWidget(topBar);

    // Middle 3 Device Cards
    auto *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(12);

    auto setupCard = [this](const QString &title, DeviceCard &card) -> QGroupBox* {
        auto *box = new QGroupBox(title, this);
        box->setStyleSheet("QGroupBox { font-weight: bold; font-size: 13px; }");
        auto *v = new QVBoxLayout(box);

        auto *header = new QHBoxLayout();
        card.nameLabel = new QLabel(title, box);
        card.nameLabel->setStyleSheet("font-size: 12px; color: #d0d5e5;");
        header->addWidget(card.nameLabel);

        card.statusBadge = new QLabel("DISCONNECTED", box);
        card.statusBadge->setStyleSheet("background: #444; color: white; padding: 2px 6px; border-radius: 3px; font-size: 10px; font-weight: bold;");
        header->addWidget(card.statusBadge);
        v->addLayout(header);

        card.interfaceLabel = new QLabel("Interface: Unknown", box);
        card.interfaceLabel->setStyleSheet("color: #8890a0; font-size: 11px;");
        v->addWidget(card.interfaceLabel);

        v->addSpacing(6);
        card.tempLabel = new QLabel("Internal Temp: --.- °C", box);
        card.tempLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #ff8844;");
        v->addWidget(card.tempLabel);

        card.voltLabel = new QLabel("Voltage: --.- V (Core: --.- V)", box);
        card.voltLabel->setStyleSheet("font-size: 11px; color: #a0a8be;");
        v->addWidget(card.voltLabel);

        card.packetsLabel = new QLabel("Packets: 0", box);
        card.packetsLabel->setStyleSheet("font-size: 11px; color: #70c080;");
        v->addWidget(card.packetsLabel);

        return box;
    };

    cardsLayout->addWidget(setupCard("Raspberry Pi Pico (RP2040)", m_mcuCard));
    cardsLayout->addWidget(setupCard("Digilent Arty A7-35T (Artix-7)", m_fpgaCard));
    cardsLayout->addWidget(setupCard("Digilent JTAG-HS2 Adapter", m_jtagCard));

    mainLayout->addLayout(cardsLayout);

    // Bottom Diagnostic Event Log
    auto *logBox = new QGroupBox("Hardware Telemetry & Event Stream (Task E)", this);
    auto *logLayout = new QVBoxLayout(logBox);

    m_logView = new QTextEdit(logBox);
    m_logView->setReadOnly(true);
    m_logView->setStyleSheet("background: #181a20; color: #b8c0d0; font-family: Consolas, monospace; font-size: 11px;");
    logLayout->addWidget(m_logView);

    mainLayout->addWidget(logBox, 1);
}

void HardwareView::appendLog(const QString &msg) {
    QString timeStr = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    m_logView->append(QString("[%1] %2").arg(timeStr, msg));
}

void HardwareView::onConnectAllClicked() {
    bool ok = m_deviceManager.connectAll();
    if (ok) {
        appendLog("DeviceManager: All devices connected successfully.");
    } else {
        appendLog("DeviceManager: Warning - one or more devices encountered connection errors.");
    }
    updateDashboardCards();
}

void HardwareView::onDisconnectAllClicked() {
    m_deviceManager.disconnectAll();
    appendLog("DeviceManager: All devices disconnected.");
    updateDashboardCards();
}

void HardwareView::onInjectFaultToggled(bool checked) {
    auto fpga = m_deviceManager.getDeviceById("FPGA-ARTY-A7-002");
    if (fpga) {
        fpga->injectFault(checked);
        if (checked) {
            appendLog("FAULT INJECTION ACTIVATED: Simulating communication timeout on FPGA-ARTY-A7-002.");
        } else {
            appendLog("FAULT INJECTION CLEARED: FPGA normal operational mode restored.");
        }
    }
    updateDashboardCards();
}

void HardwareView::onRecoverClicked() {
    appendLog("RECOVERY INITIATED: Attempting automatic re-synchronization on FPGA-ARTY-A7-002...");
    m_faultToggle->setChecked(false);
    bool ok = m_deviceManager.attemptRecovery("FPGA-ARTY-A7-002", 3);
    if (ok) {
        appendLog("RECOVERY SUCCESSFUL: FPGA-ARTY-A7-002 re-established telemetry link.");
    } else {
        appendLog("RECOVERY FAILED: FPGA-ARTY-A7-002 failed to respond within retry limit.");
    }
    updateDashboardCards();
}

void HardwareView::onRefreshTelemetry() {
    auto telemetryMap = m_deviceManager.queryAllTelemetry(200);
    updateDashboardCards();
}

void HardwareView::updateDashboardCards() {
    auto updateCard = [](DeviceCard &card, std::shared_ptr<Core::Hardware::IDeviceAdapter> dev) {
        if (!dev) return;
        auto info = dev->getInfo();
        auto status = dev->getStatus();
        auto telem = dev->readTelemetry(100);

        card.nameLabel->setText(QString::fromStdString(info.displayName));
        card.interfaceLabel->setText(QString("Interface: %1").arg(QString::fromStdString(info.physicalInterface)));

        switch (status) {
            case Core::Hardware::ConnectionStatus::Connected:
                card.statusBadge->setText("CONNECTED");
                card.statusBadge->setStyleSheet("background: #208040; color: white; padding: 2px 6px; border-radius: 3px; font-size: 10px; font-weight: bold;");
                break;
            case Core::Hardware::ConnectionStatus::TimedOut:
                card.statusBadge->setText("TIMED OUT");
                card.statusBadge->setStyleSheet("background: #c06010; color: white; padding: 2px 6px; border-radius: 3px; font-size: 10px; font-weight: bold;");
                break;
            case Core::Hardware::ConnectionStatus::Error:
                card.statusBadge->setText("ERROR");
                card.statusBadge->setStyleSheet("background: #b02020; color: white; padding: 2px 6px; border-radius: 3px; font-size: 10px; font-weight: bold;");
                break;
            case Core::Hardware::ConnectionStatus::Disconnected:
            default:
                card.statusBadge->setText("DISCONNECTED");
                card.statusBadge->setStyleSheet("background: #444; color: #aaa; padding: 2px 6px; border-radius: 3px; font-size: 10px; font-weight: bold;");
                break;
        }

        if (telem.valid) {
            card.tempLabel->setText(QString("Internal Temp: %1 °C").arg(telem.internalTemperatureC, 0, 'f', 1));
            card.voltLabel->setText(QString("Supply: %1 V (Core: %2 V)")
                                        .arg(telem.supplyVoltageV, 0, 'f', 2)
                                        .arg(telem.coreVoltageV, 0, 'f', 2));
            card.packetsLabel->setText(QString("Packets Processed: %1").arg(telem.packetsProcessed));
        } else if (status == Core::Hardware::ConnectionStatus::TimedOut) {
            card.tempLabel->setText("Internal Temp: TIMED OUT");
            card.voltLabel->setText("Voltage: No response from sensor");
        }
    };

    updateCard(m_mcuCard, m_deviceManager.getDeviceById("MCU-RP2040-001"));
    updateCard(m_fpgaCard, m_deviceManager.getDeviceById("FPGA-ARTY-A7-002"));
    updateCard(m_jtagCard, m_deviceManager.getDeviceById("JTAG-HS2-003"));
}

void HardwareView::onExportReportClicked() {
    QString path = QFileDialog::getSaveFileName(this, "Save Diagnostic Report", "results/embedded_diagnostic_report.txt", "Text Files (*.txt);;All Files (*.*)");
    if (path.isEmpty()) return;

    std::string report = m_deviceManager.generateDiagnosticReport();
    std::ofstream out(path.toStdString());
    if (out.is_open()) {
        out << report;
        QMessageBox::information(this, "Report Exported", QString("Saved diagnostic report to:\n%1").arg(path));
    }
}

} // namespace Workbench::App
