#ifndef WEATHERSTATION_H
#define WEATHERSTATION_H

#include <QMainWindow>
#include <map>
#include <QPixmap>

#include "sensors/HumTemp.hpp"
#include "sensors/Lum.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class WeatherStation; }
QT_END_NAMESPACE

class WeatherStation : public QMainWindow
{
        Q_OBJECT

public:
        WeatherStation(QWidget *parent = nullptr);
        ~WeatherStation();
        void refreshUI();

private:
        enum class Weather { Rainy, Snowy, SunnyCloud, Thunderstorm, Sunny, Night };

        Ui::WeatherStation *ui;

        double _humidity;
        double  _temp;
        double  _pressure;
        double  _lux;
        Weather _currentWeather;

        HumTemp hum_sensor {"/dev/i2c-1", 0x703, 0x38};
        Lum lum_sensor {"/dev/i2c-1", 0x703, 0x29};

        void update();
        void fetchData();

        /* Not a lot of images, map is faster than unordered_map on small sets*/
        std::map<Weather, QPixmap*> _images;
private slots:
        void refreshBtn();
};
#endif // WEATHERSTATION_H
