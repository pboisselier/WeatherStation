/*********************
 * Pierre BOISSELIER *
 *********************/

#ifndef LUM_HPP
#define LUM_HPP

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdint.h>
#include <thread>

#pragma pack(1)
struct lum_data
{
        uint8_t diode1[2];
        uint8_t diode2[2];
};

#include "BusI2C.hpp"

#include <string>

class Lum : public BusI2C<lum_data>
{
    public:
        Lum (int fd) : BusI2C{fd} {}
        Lum (const std::string device,
             uint16_t bus_addr,
             uint16_t sensor_addr) :
            BusI2C{device, bus_addr, sensor_addr}
        {
                /* Power up sensor */
                write_data ({0x70, 0x03});
                /* Set integration time */
                write_data ({0x81, 0x02});
        }

        void fetch()
        {
                /* Request reading */
                write_data ({0x8C});
                std::this_thread::sleep_for (std::chrono::milliseconds (500));
                raw = this->read_data();
        }

        int IR() const noexcept
        {
                return 256 * ((int) raw.diode2[1]) + ((int) raw.diode2[0]);
        }

        int visible() const noexcept
        {
                int ir = IR();
                int d1 = 256 * raw.diode1[1] + raw.diode1[0];
                return d1 - ir;
        }

        friend std::ostream& operator<< (std::ostream& stream,
                                         Lum const& sensor)
        {
                stream << "Fetched data from luminosity sensor at "
                       << sensor.dev << std::endl
                       << "Diode 1 (raw): 0x" << std::hex
                       << (int) sensor.raw.diode1[0] << std::hex
                       << (int) sensor.raw.diode1[1] << std::endl
                       << "Diode 2 (raw): 0x" << std::hex
                       << (int) sensor.raw.diode2[0] << std::hex
                       << (int) sensor.raw.diode2[1] << std::endl
                       << std::dec << "IR: " << sensor.IR() << std::endl
                       << "Visible: " << sensor.visible();

                return stream;
        }
};

#endif // LUM_HPP
