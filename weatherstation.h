#ifndef WEATHERSTATION_H
#define WEATHERSTATION_H

#include <QMainWindow>
#include <map>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui { class WeatherStation; }
QT_END_NAMESPACE

class WeatherStation : public QMainWindow
{
        Q_OBJECT

public:
        WeatherStation(QWidget *parent = nullptr);
        ~WeatherStation();

        void updateUI(double humidity, double temp, double pressure, double lux);
private:
        enum class Weather { Rainy, Snowy, SunnyCloud, Thunderstorm, Sunny, Night };

        Ui::WeatherStation *ui;
        void refreshUI();

        double _humidity;
        double  _temp;
        double  _pressure;
        double  _lux;
        Weather _currentWeather;


        /* Not a lot of images, map is faster than unordered_map on small sets*/
        std::map<Weather, QPixmap*> _images;
};
#endif // WEATHERSTATION_H
