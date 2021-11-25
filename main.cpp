#include "weatherstation.h"
#include "sensors/HumTemp.hpp"

#include <QApplication>

int
main (int argc, char* argv[])
{
        QApplication a (argc, argv);
        WeatherStation w;
        w.show();

        w.updateUI (25.5, 32, 1999.15, 50);

        try {
       HumTemp hum {"/dev/i2c-1", 0x703, 0x39};
        } catch (const HumTemp::i2c_exception& e) {
            e.print ();
        }

        return a.exec();
}
