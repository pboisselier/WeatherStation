#include "weatherstation.h"
#include "ui_weatherstation.h"

WeatherStation::WeatherStation(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::WeatherStation)
{
        // Setup UI
        ui->setupUi(this);

        // Defaults values
        _temp = 0;
        _humidity = 0;
        _lux = 0;
        _pressure = 0;
        _currentWeather = Weather::Sunny;

        // Load images in memory
        /*
         * Should these images be loaded in RAM?
         * This program is for a weather station and thus images will never have to be changed very frequently
         * as the weather is not something that changes every second...
         * But, as these images are very small and the RaspberryPi this thing will run on is plentiful of RAM
         * we can all load them and make our job easier :)
         */
        _images = {{Weather::Rainy, new QPixmap("assets/rainy.png")},
                   {Weather::Snowy, new QPixmap("assets/snowy.png")},
                   {Weather::Sunny, new QPixmap("assets/sunny.png")},
                   {Weather::Night, new QPixmap("assets/night.png")},
                   {Weather::SunnyCloud, new QPixmap("assets/sunnycloud.png")},
                   {Weather::Thunderstorm, new QPixmap("assets/thunderstorm.png")}};

        connect (ui->refresh_btn, &QPushButton::pressed, this, &WeatherStation::refreshBtn);

        update();
}

WeatherStation::~WeatherStation()
{
        // Delete loaded images
        for (const auto &i : _images)
        {
                delete i.second;
        }

        delete ui;
}


void WeatherStation::fetchData()
{
        try {
                hum_sensor.fetch();
                _humidity = hum_sensor.humidity();
                _temp = hum_sensor.temperature();
        }  catch (HumTemp::i2c_exception& e) {
            std::cerr << "Error fetching hmtemp sensor" << std::endl;
            e.print();
        }

        try {
                lum_sensor.fetch();
                _lux = lum_sensor.visible();
        } catch (Lum::i2c_exception& e) {
                std::cerr << "Error fetching lum sensor" << std::endl;
                e.print();
        }

}

void WeatherStation::update()
{
        fetchData();

        // Change current weather
        if (_lux < 4)
                _currentWeather = Weather::Night;

        if (_pressure < 1015)
        {
                if (_humidity > 60)
                {
                        if (_lux > 10 && _lux < 200)
                        {
                                if (_temp > 5)
                                        _currentWeather = Weather::Rainy;
                        }
                        else if (_lux < 200)
                        {
                                if (_temp < 5)
                                        _currentWeather = Weather::Snowy;
                        }
                }
        }
        else if (_pressure <= 1015)
        {
                if (_humidity < 60)
                {
                        if (_lux > 10 && _lux < 200)
                                _currentWeather = Weather::SunnyCloud;
                }
                else
                {
                        if (_lux > 4 && _lux <= 10)
                                _currentWeather = Weather::Thunderstorm;
                }
        }
        else
        {
                if (_humidity < 60)
                        if (_lux >= 200)
                                _currentWeather = Weather::Sunny;
        }

}

void WeatherStation::refreshUI()
{
        // Refresh LCD numbers
        ui->humidity_lcd->display(_humidity);
        ui->temp_lcd->display(_temp);
        ui->pressure_lcd->display(_pressure);
        ui->lux_lcd->display(_lux);

        // Change image
        ui->weatherView->setPixmap(*_images[_currentWeather]);
        ui->weatherView->show();
}

void WeatherStation::refreshBtn()
{
        update ();
        refreshUI();
}
