/*********************
 * Pierre BOISSELIER *
 *********************/

#ifndef HUMTEMP_CPP
#define HUMTEMP_CPP

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdint.h>
#include <thread>

#pragma pack(1)
struct humtemp_data
{
        uint8_t info : 8;
        uint32_t humidity : 20;
        uint32_t temperature : 20;
        uint8_t reserved : 8;
};

#include "BusI2C.hpp"

class HumTemp : public BusI2C<humtemp_data>
{
    public:
        HumTemp (const std::string device_path,
                 uint16_t bus_addr,
                 uint16_t sensor_addr) :
            BusI2C{device_path, bus_addr, sensor_addr}
        {
#ifndef NDEBUG
                std::cout << "Initializing HumTemp" << std::endl;
#endif

                write_data ({0xBE, 0x08, 0x00});
                std::this_thread::sleep_for (std::chrono::milliseconds (10));

#ifndef NDEBUG
                std::cout << "HumTemp initialized!" << std::endl;
#endif
        }
        HumTemp (const std::string device_path) :
            HumTemp{device_path, 0x0703, 0x38}
        {
        }
        /* Delete ? */
        HumTemp (int fd) : BusI2C{fd} {}

        void fetch()
        {
                write_data ({0xAC, 0x33, 0x00});
                std::this_thread::sleep_for (std::chrono::milliseconds (80));
                raw = read_data();
        }

        double humidity() const noexcept
        {
                return (raw.humidity / (1 << 20)) * 100;
        }

        double temperature() const noexcept
        {
                return (raw.temperature / (1 << 20)) * 200 - 50;
        }

        uint8_t info() const noexcept
        {
                return raw.info;
        }

        friend std::ostream& operator<< (std::ostream& stream,
                                         HumTemp const& sensor)
        {
                stream << "Fetched data from humidity sensor at " << sensor.dev
                       << std::endl
                       << "Info: " << sensor.raw.info << std::endl
                       << "Humidity (raw): 0x" << std::hex
                       << sensor.raw.humidity << std::endl
                       << "Temperature (raw): 0x" << std::hex
                       << sensor.raw.temperature << std::endl
                       << std::dec << "Humidity: " << sensor.humidity()
                       << std::endl
                       << "Temperature: " << sensor.temperature();

                return stream;
        }
};

#endif // HUMTEMP_CPP