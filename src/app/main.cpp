#include <QApplication>
#include <QStyleFactory>
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
    window.show();

    return app.exec();
}
