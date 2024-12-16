#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVBoxLayout>

QT_CHARTS_USE_NAMESPACE

    class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onUpdateTemperatureClicked();
    void onStatisticsRequest();
    void onStatisticsReply(QNetworkReply *reply);

private:
    void setupUi(); // Настройка интерфейса

    QtCharts::QChartView *chartView;  // Для отображения графика
    QLabel *temperatureLabel;  // Метка для отображения температуры
    QLineEdit *startTimeInput;  // Ввод стартового времени
    QLineEdit *endTimeInput;  // Ввод конечного времени
    QPushButton *getStatisticsButton;  // Кнопка для запроса статистики
    QNetworkAccessManager *networkManager;  // Для сетевых запросов
};

#endif // MAINWINDOW_H
