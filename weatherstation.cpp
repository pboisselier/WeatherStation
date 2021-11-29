#include "weatherstation.h"

#include "ui_weatherstation.h"

WeatherStation::WeatherStation (QWidget* parent) :
    QMainWindow (parent), ui (new Ui::WeatherStation)
{
        // Setup UI
        ui->setupUi (this);

        // Defaults values
        temp           = 0;
        humidity       = 0;
        lum            = 0;
        pressure       = 0;
        currentWeather = Weather::Sunny;

        // Load images in memory
        /*
         * Should these images be loaded in RAM?
         * This program is for a weather station and thus images will never have to be changed very frequently
         * as the weather is not something that changes every second...
         * But, as these images are very small and the RaspberryPi this thing will run on is plentiful of RAM
         * we can all load them and make our job easier :)
         */
        _images = {
            {Weather::Rainy, new QPixmap ("assets/rainy.png")},
            {Weather::Snowy, new QPixmap ("assets/snowy.png")},
            {Weather::Sunny, new QPixmap ("assets/sunny.png")},
            {Weather::Night, new QPixmap ("assets/night.png")},
            {Weather::SunnyCloud, new QPixmap ("assets/sunnycloud.png")},
            {Weather::Thunderstorm, new QPixmap ("assets/thunderstorm.png")}};

        connect (ui->refresh_btn,
                 &QPushButton::pressed,
                 this,
                 &WeatherStation::refreshBtn);

        timer = new QTimer (this);
        connect (timer,
                 &QTimer::timeout,
                 this,
                 QOverload<>::of (&WeatherStation::refreshTimer));
        timer->start (5000);

        update();
}

WeatherStation::~WeatherStation()
{
        // Delete loaded images
        for (const auto& i : _images)
        {
                delete i.second;
        }

        delete timer;
        delete ui;
}

void
WeatherStation::fetchData()
{
        try
        {
                hum_sensor.fetch();
                humidity = hum_sensor.humidity();
                temp     = hum_sensor.temperature();
        }
        catch (HumTemp::i2c_exception& e)
        {
                std::cerr << "Error fetching hmtemp sensor" << std::endl;
                e.print();
        }

        try
        {
                lum_sensor.fetch();
                lum = lum_sensor.visible();
        }
        catch (Lum::i2c_exception& e)
        {
                std::cerr << "Error fetching lum sensor" << std::endl;
                e.print();
        }
}

void
WeatherStation::update()
{
        fetchData();

        // Change current weather
        if (lum < 4)
                currentWeather = Weather::Night;

        if (pressure < 1015)
        {
                if (humidity > 60)
                {
                        if (lum > 10 && lum < 200)
                        {
                                if (temp > 5)
                                        currentWeather = Weather::Rainy;
                        }
                        else if (lum < 200)
                        {
                                if (temp < 5)
                                        currentWeather = Weather::Snowy;
                        }
                }
        }
        else if (pressure <= 1015)
        {
                if (humidity < 60)
                {
                        if (lum > 10 && lum < 200)
                                currentWeather = Weather::SunnyCloud;
                }
                else
                {
                        if (lum > 4 && lum <= 10)
                                currentWeather = Weather::Thunderstorm;
                }
        }
        else
        {
                if (humidity < 60)
                        if (lum >= 200)
                                currentWeather = Weather::Sunny;
        }
}

void
WeatherStation::refreshUI()
{
        // Refresh LCD numbers
        ui->humidity_lcd->display (humidity);
        ui->temp_lcd->display (temp);
        ui->pressure_lcd->display (pressure);
        ui->lux_lcd->display (lum);

        // Change image
        ui->weatherView->setPixmap (*_images[currentWeather]);
        ui->weatherView->show();
}

void
WeatherStation::refreshBtn()
{
        update();
        refreshUI();
}

void
WeatherStation::refreshTimer()
{
        update();
        refreshUI();
}
