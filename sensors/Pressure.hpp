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

struct pressure_data
{
        uint8_t pressure_raw[3]; 
        uint8_t temp_raw[3];     

        uint8_t coef_padding[11];

        uint8_t c0c1[3];   
        uint8_t c00c10[5]; 

        uint8_t c01[2]; 
        uint8_t c11[2]; 
        uint8_t c20[2]; 
        uint8_t c21[2]; 
        uint8_t c30[2]; 
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
                if (value & ((uint32_t) 1 << (nbits - 1)))
                        val = (value) - ((uint32_t) 1 << nbits);
                return val;
        }

        void fetch()
        {
                write_data ({0x00});
                raw = read_data();

                c0 = ((uint16_t) raw.c0c1[0] << 4)
                     | (((uint16_t) raw.c0c1[1] >> 4) & 0x0F);

                c1 = (((uint16_t) raw.c0c1[1] & 0x0F) << 8) | raw.c0c1[2];

                c00 = ((uint32_t) raw.c00c10[0] << 12)
                      | ((uint32_t) raw.c00c10[2] << 4)
                      | (uint32_t)(( raw.c00c10[3] & 0xF0) >> 4);

                c10 = (((uint32_t) raw.c00c10[3] & 0x0F) << 16)
                      | ((uint32_t) raw.c00c10[4] << 8)
                      | (uint32_t) raw.c00c10[5];

                c01 = ((uint16_t) raw.c01[0] << 8) | (uint16_t) raw.c01[1];
                c11 = ((uint16_t) raw.c11[0] << 8) | (uint16_t) raw.c11[1];
                c20 = ((uint16_t) raw.c20[0] << 8) | (uint16_t) raw.c20[1];
                c21 = ((uint16_t) raw.c21[0] << 8) | (uint16_t) raw.c21[1];
                c30 = ((uint16_t) raw.c30[0] << 8) | (uint16_t) raw.c30[1];

                c0  = complement_2s (c0, 12);
                c1  = complement_2s (c1, 12);
                c00 = complement_2s (c00, 20);
                c10 = complement_2s (c10, 20);
                c01 = complement_2s (c01, 16);
                c11 = complement_2s (c11, 16);
                c20 = complement_2s (c20, 16);
                c21 = complement_2s (c21, 16);
                c30 = complement_2s (c30, 16);

                int32_t raw_temperature = raw.temp_raw[0] << 16
                                          | raw.temp_raw[1] << 8
                                          | raw.temp_raw[2];
                int32_t raw_pressure = raw.pressure_raw[0] << 16
                                       | raw.pressure_raw[1] << 8
                                       | raw.pressure_raw[2];

                raw_temperature = complement_2s (raw_temperature, 24);
                raw_pressure    = complement_2s (raw_pressure, 24);

                double temp_scaled = (double) raw_temperature / 1040384.0;
                temp               = c0 * 0.5 + temp_scaled * c1;

                double pressure_scaled = (double) raw_pressure / 1040384.0;
                press                  = (int32_t) c00
                        + pressure_scaled
                              * ((int32_t) c10
                                 + pressure_scaled
                                       * ((int32_t) c20
                                          + pressure_scaled * (int32_t) c30))
                        + temp_scaled
                              * ((int32_t) c01
                                 + pressure_scaled
                                       * ((int32_t) c11
                                          + pressure_scaled * (int32_t) c21));
        }

        double pressure() const noexcept
        {
                return press;
        }

        double temperature() const noexcept
        {
                return temp;
        }

        friend std::ostream& operator<< (std::ostream& stream,
                                         Pressure const& sensor)
        {
                stream << "Fetched data from pressure sensor at " << sensor.dev
                       << std::endl
                       << std::dec << "Pressure: " << sensor.pressure()
                       << std::endl
                       << "Temperature: " << sensor.temperature();

                return stream;
        }

    private:
        int32_t c00, c10;
        int16_t c0, c1, c01, c11, c20, c21, c30;
        double temp;
        double press;
};

#endif // PRESSURE_HPP
