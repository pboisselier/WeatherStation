#include "weatherstation.h"

#include <QApplication>

int main(int argc, char *argv[])
{
        QApplication a(argc, argv);
        WeatherStation w;
        w.show();

            w.updateUI(25.5, 32, 1999.15, 50);
        return a.exec();
}
