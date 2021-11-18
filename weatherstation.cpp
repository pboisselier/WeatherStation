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

        // Load images in memory
        _images = {{Weather::Rainy, new QGraphicsPixmapItem(QPixmap("assets/rainy.png"))},
                   {Weather::Snowy, new QGraphicsPixmapItem(QPixmap("assets/snowy.png"))}};

        refreshUI();

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

void WeatherStation::updateUI(double humidity, double temp, double pressure, double lux)
{
    _humidity = humidity;
    _temp = temp;
        _pressure = pressure;
        _lux = lux;
    // Change current weather
    if (_lux <= 4)
            _currentWeather = Weather::Night;
    else
    {
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
    QPixmap pixmap = _images[_currentWeather]->pixmap();
    ui->weatherView->setPixmap(pixmap);
    ui->weatherView->show();
}
