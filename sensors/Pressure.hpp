/*********************
 * Pierre BOISSELIER *
 *********************/

#ifndef PRESSURE_HPP
#define PRESSURE_HPP

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdint.h>
#include <thread>

#pragma pack(1)
struct pressure_data
{
        uint32_t pressure : 24;
        uint32_t temp : 24;
};

#include "BusI2C.hpp"

class Pressure : public BusI2C<pressure_data>
{
        Pressure (int fd) : BusI2C{fd} {}
        Pressure (const std::string device,
                  uint16_t bus_addr,
                  uint16_t sensor_addr) :
            BusI2C{device, bus_addr, sensor_addr}
        {
                write_data ({0x06, 0x26});
                write_data ({0x07, 0xA6});
                write_data ({0x08, 0x07});
                write_data ({0x09, 0x0C});
        }

        void fetch()
        {
                write_data ({0x00});
                raw = read_data();
        }

        int pressure() const noexcept
        {
                return 0;
        }

        int temperature() const noexcept
        {
                return 0;
        }

        friend std::ostream& operator<< (std::ostream& stream,
                                         Pressure const& sensor)
        {
                stream << "Fetched data from pressure sensor at " << sensor.dev
                       << std::endl
                       << "Pressure (raw): 0x" << std::hex
                       << sensor.raw.pressure << std::endl
                       << "Temperature (raw): 0x" << std::hex << sensor.raw.temp
                       << std::endl
                       << std::dec << "Pressure: " << sensor.pressure()
                       << std::endl
                       << "Temperature: " << sensor.temperature();

                return stream;
        }
};

#endif // PRESSURE_HPP
