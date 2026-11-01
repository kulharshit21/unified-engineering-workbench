#include <QApplication>
#include <QStyleFactory>
#include <QPixmap>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <iostream>
#include "widgets/MainWindow.hpp"

int main(int argc, char *argv[]) {
    // Enable High DPI scaling
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);
    app.setApplicationName("UnifiedEngineeringWorkbench");
    app.setOrganizationName("PassScienceWorks");
    app.setApplicationVersion("1.0.0");

    // Modern clean fusion style
    if (QStyleFactory::keys().contains("Fusion")) {
        QApplication::setStyle(QStyleFactory::create("Fusion"));
    }

    Workbench::App::MainWindow window;
    window.resize(1280, 800);
    window.show();

    QStringList args = QCoreApplication::arguments();
    {
        QFile logFile("D:/PASS Science Works/docs/screenshots/log.txt");
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream ts(&logFile);
            ts << "Arguments count: " << args.size() << "\n";
            for (const auto &a : args) {
                ts << "Arg: " << a << "\n";
            }
        }
    }
    if (args.contains("--screenshot-views")) {
        int idx = args.indexOf("--screenshot-views");
        QString outDir = (idx + 1 < args.size()) ? args.at(idx + 1) : "D:/PASS Science Works/docs/screenshots";
        QDir().mkpath(outDir);

        for (int i = 0; i < window.tabCount(); ++i) {
            window.setCurrentTab(i);
            for (int k = 0; k < 10; ++k) {
                app.processEvents();
            }
            QPixmap pixmap = window.grab();
            QString safeName = window.tabText(i);
            safeName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
            QString path = QString("%1/view_%2_%3.png")
                .arg(outDir)
                .arg(i + 1)
                .arg(safeName);
            pixmap.save(path);
        }
        return 0;
    }

    return app.exec();
}
