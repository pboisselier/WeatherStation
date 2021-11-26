#include "sensors/HumTemp.hpp"
#include "sensors/Lum.hpp"
#include "sensors/Pressure.hpp"
#include "weatherstation.h"

#include <QApplication>
#include <chrono>
#include <thread>

const int refresh_time       = 5;
const uint16_t bus_addr      = 0x703;
const uint16_t humtemp_addr  = 0x38;
const uint16_t lum_addr      = 0x29;
const uint16_t pressure_addr = 0x77;

int
main (int argc, char* argv[])
{
        QApplication a (argc, argv);
        WeatherStation w;
        w.show();
	w.updateUI (0, 0, 0, 0);

        HumTemp humtemp_sensor{"/dev/i2c-1", bus_addr, humtemp_addr};
        Lum lum_sensor{"/dev/i2c-1", bus_addr, lum_addr};

        double humidity    = 0;
        double lux         = 0;
        double pressure    = 0;
        double temperature = 0;

        for (;;)
        {
                humtemp_sensor.fetch();
                lum_sensor.fetch();

                humidity    = humtemp_sensor.humidity();
                temperature = humtemp_sensor.temperature();
                lux         = lum_sensor.visible();

                w.updateUI (humidity, temperature, pressure, lux);
                std::this_thread::sleep_for (
                    std::chrono::seconds (refresh_time));
        }

        try
        {
                HumTemp hum{"/dev/i2c-1", 0x703, 0x39};
        }
        catch (const HumTemp::i2c_exception& e)
        {
                e.print();
        }

        return a.exec();
}
