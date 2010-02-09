#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include "include/myqmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QString locale = QLocale::system().name();
    
    /// Traduccion de objetos de Qt
    QTranslator qtTranslator;
    qtTranslator.load(QString(":/translations/qt_" + locale));
    app.installTranslator(&qtTranslator);
    
    /// Traduccion de objetos de QTesseract
    QTranslator translator;
    translator.load(QString(":/translations/qtesseract_" + locale));
    app.installTranslator(&translator);
    
    app.setWindowIcon(QIcon(":/images/Logo 32x32.png"));
    
    MyQMainWindow mainWindow;
    mainWindow.resize( 700, 800 );
    mainWindow.show();
    return app.exec();
}
