#include "server.h"
#include <QApplication>

int main(int argc, char** argv)
{
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("imageformats");
    paths.append("platforms");
    paths.append("sqldrivers");
    QCoreApplication::setLibraryPaths(paths);

    QApplication app(argc, argv);
    Server     server(2323);

    server.show();

    return app.exec();
}
