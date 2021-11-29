#ifndef WEATHERSTATION_H
#define WEATHERSTATION_H

#include "sensors/HumTemp.hpp"
#include "sensors/Lum.hpp"

#include <QMainWindow>
#include <QPixmap>
#include <QTimer>
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui
{
class WeatherStation;
}
QT_END_NAMESPACE

class WeatherStation : public QMainWindow
{
        Q_OBJECT

    public:
        WeatherStation (QWidget* parent = nullptr);
        ~WeatherStation();
        void refreshUI();

    private:
        enum class Weather
        {
                Rainy,
                Snowy,
                SunnyCloud,
                Thunderstorm,
                Sunny,
                Night
        };

        Ui::WeatherStation* ui;
        QTimer* timer;

        double humidity;
        double temp;
        double pressure;
        double lum;
        Weather currentWeather;

        HumTemp hum_sensor{"/dev/i2c-1", 0x703, 0x38};
        Lum lum_sensor{"/dev/i2c-1", 0x703, 0x29};

        void update();
        void fetchData();

        /* Not a lot of images, map is faster than unordered_map on small sets*/
        std::map<Weather, QPixmap*> _images;

    private slots:
        void refreshBtn();
        void refreshTimer();
};
#endif // WEATHERSTATION_H
