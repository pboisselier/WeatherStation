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
        uint32_t pressure_raw : 24;
        uint32_t temp_raw : 24;

        uint8_t coef_padding[11];

        int8_t c0_c1[3];
        int8_t c00_c10[5];

        int16_t c01;
        int16_t c11;
        int16_t c20;
        int16_t c21;
        int16_t c30;

        uint8_t end_padding[5];
};

//struct pressure_data
//{
//        uint8_t data[40];
//};

#include "BusI2C.hpp"

class Pressure : public BusI2C<pressure_data>
{
    public:
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

        inline int32_t complement_2s (const int32_t value, const int32_t nbits)
        {
                int32_t val = value;
                if (value & (1 << (nbits - 1)))
                        val = value - (1 << nbits);
                return val;
        }

        void fetch()
        {
                write_data ({0x00});
                raw = read_data();

                pressure_2s = complement_2s (raw.pressure_raw, 24);
                temp_2s     = complement_2s (raw.temp_raw, 24);

                p = (double) pressure_2s / 1040384;
                t = (double) temp_2s / 1040384;

                c0 = (raw.c0_c1[0] << 4) | (raw.c0_c1[1] & 0xF0);
                c1 = ((raw.c0_c1[1] & 0x0F) << 8) | raw.c0_c1[2];
                //c0 = c0 - (1 << sizeof c0);
                //c1 = c1 - (1 << sizeof c1);
        }

        int pressure() const noexcept
        {
                return c00 + p * (c10 + p * (c20 + p * c30))
                       + t * (c01 + p * (c11 + p * c21));
        }

        int temperature() const noexcept
        {
                return 0.5 * c0 + c1 * t;
                return 0;
        }

        friend std::ostream& operator<< (std::ostream& stream,
                                         Pressure const& sensor)
        {
                stream << "Fetched data from pressure sensor at " << sensor.dev
                       << std::endl
                       << "Pressure (raw): 0x" << std::hex << sensor.pressure_2s
                       << std::endl
                       << "Temperature (raw): 0x" << std::hex << sensor.temp_2s
                       << std::endl
                       << std::dec << "Pressure: " << sensor.pressure()
                       << std::endl
                       << "Temperature: " << sensor.temperature();

                return stream;
        }

    private:
        int32_t pressure_2s;
        int32_t temp_2s;

        double p;
        double t;
        double c0, c1, c00, c10, c20, c30, c01, c11, c21;
};

#endif // PRESSURE_HPP
