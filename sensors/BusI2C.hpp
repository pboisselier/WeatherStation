/*********************
 * Pierre BOISSELIER *
 *********************/

#ifndef BUSI2C_HPP
#define BUSI2C_HPP

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <list>
#include <memory>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>

class i2c_exception;

template<typename T = uint8_t>
class BusI2C
{
    public:
        class i2c_exception : public std::runtime_error
        {
            public:
                /* Not the best design to use std::cerr in a library... */
                i2c_exception (const char* what,
                               const uint16_t bus_addr,
                               const uint16_t sensor_addr,
                               const std::string& device) :
                    std::runtime_error{what},
                    bus_addr{bus_addr}, sensor_addr{sensor_addr}, device_path{
                                                                      device}
                {
                        std::cerr << "I2C_EXCEPTION: bus_addr=0x" << std::hex
                                  << bus_addr << ", sensor_addr=0x" << std::hex
                                  << sensor_addr << ", dev=" << device
                                  << std::endl
                                  << "\terrno: " << std::strerror (errno)
                                  << std::endl;
                }
                i2c_exception (const char* what) : std::runtime_error{what} {}
                i2c_exception (const char* what, const BusI2C<T>& i2c) :
                    i2c_exception{what, i2c.bus_addr, i2c.sensor_addr, i2c.dev}
                {
                }
                i2c_exception (const BusI2C<T>& i2c) :
                    i2c_exception{"Unexpected runtime error", i2c}
                {
                }

                uint16_t bus()
                {
                        return bus_addr;
                }
                uint16_t sensor()
                {
                        return sensor_addr;
                }
                const std::string& device()
                {
                        return device_path;
                }

            private:
                const uint16_t bus_addr;
                const uint16_t sensor_addr;
                const std::string device_path;
        };

        BusI2C (const std::string dev,
                uint16_t bus_addr,
                uint16_t sensor_addr) :
            dev{dev},
            bus_addr{bus_addr}, sensor_addr{sensor_addr}
        {
#ifndef NDEBUG
                std::cout << "Opening device " << dev << std::endl;
#endif
                int fd = open (dev.c_str(), O_RDWR);
                if (fd == -1)
                        throw i2c_exception{"Unable to open device", *this};

                this->fd = fd;

                if (ioctl (fd, bus_addr, sensor_addr) == -1)
                        throw i2c_exception{
                            "Unable to establish link with the sensor", *this};

#ifndef NDEBUG
                std::cout << "Opened device " << dev << " !" << std::endl;
#endif
        }

        BusI2C (int fd) : fd{fd} {}
        ~BusI2C()
        {
                if (close (fd) == -1)
                        exit (1);
        }

        T read_data()
        {
                size_t sz = sizeof (T);

                T buf;
                int r = read (fd, &buf, sz);
                if (r == -1)
                        throw i2c_exception{"Unable to read from sensor!",
                                            *this};
                if ((size_t) r != sz)
                {
#ifndef NDEBUG
                        std::cerr << "Read mismatch! Read " << r
                                  << " inside of " << sz << "!" << std::endl;
#endif
                }

                return buf;
        }

        void write_data (const std::list<uint8_t>& data)
        {
                size_t buf_sz = (data.size()) * sizeof (data);
                auto buf      = std::unique_ptr<char[]>{new char[buf_sz]};

                std::copy (data.begin(), data.end(), buf.get());

                if (write (fd, buf.get(), buf_sz) == -1)
                        throw i2c_exception{"Unable to write to device", *this};
        }

        void fetch()
        {
                raw = this->read_data();
        }

    protected:
        const std::string dev;
        uint16_t bus_addr;
        uint16_t sensor_addr;
        T raw;

    private:
        friend i2c_exception;
        int fd = 0;
};

#endif // BUSI2C_HPP