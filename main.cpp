#include "weatherstation.h"

#include <QApplication>
#include <QTimer>
#include <chrono>
#include <thread>

int
main (int argc, char* argv[])
{
        QApplication a (argc, argv);
        WeatherStation w;
        w.show();
        w.refreshUI();

        //        QTimer *timer = new QTimer(this);
        //        connect(timer, &QTimer::timeout, this, &Foo::processOneThing);
        //        timer->start();
        //
        //        HumTemp humtemp_sensor{"/dev/i2c-1", bus_addr, humtemp_addr};
        //        Lum lum_sensor{"/dev/i2c-1", bus_addr, lum_addr};
        //
        //        double humidity    = 0;
        //        double lux         = 0;
        //        double pressure    = 0;
        //        double temperature = 0;
        //
        //        for (;;)
        //        {
        //                humtemp_sensor.fetch();
        //                lum_sensor.fetch();
        //
        //                humidity    = humtemp_sensor.humidity();
        //                temperature = humtemp_sensor.temperature();
        //                lux         = lum_sensor.visible();
        //
        //                w.updateUI (humidity, temperature, pressure, lux);
        //                std::this_thread::sleep_for (
        //                    std::chrono::seconds (refresh_time));
        //        }

        return a.exec();
}
