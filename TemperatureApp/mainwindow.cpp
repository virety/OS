#include "mainwindow.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVBoxLayout>
#include <QWidget>
#include <QUrl>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), networkManager(new QNetworkAccessManager(this))
{
    setupUi();
    QUrl url("http://localhost:8080/current-temperature");
    QNetworkRequest request(url);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onStatisticsReply);
    networkManager->get(request);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout();

    // Метка для температуры
    temperatureLabel = new QLabel("Текущая температура:: Загрузка...", this);
    layout->addWidget(temperatureLabel);

    // Ввод стартового и конечного времени
    startTimeInput = new QLineEdit(this);
    startTimeInput->setPlaceholderText("Запустить таймер(Unix timestamp)");
    layout->addWidget(startTimeInput);

    endTimeInput = new QLineEdit(this);
    endTimeInput->setPlaceholderText("Выключить таймер (Unix timestamp): ");
    layout->addWidget(endTimeInput);

    // Кнопка для запроса статистики
    getStatisticsButton = new QPushButton("Cоздать график", this);
    layout->addWidget(getStatisticsButton);
    connect(getStatisticsButton, &QPushButton::clicked, this, &MainWindow::onStatisticsRequest);
    // Создание графика
    QtCharts::QChart *chart = new QtCharts::QChart();
    QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chartView = new QtCharts::QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(chartView);

    centralWidget->setLayout(layout);
}

void MainWindow::onUpdateTemperatureClicked() {
    QUrl url("http://localhost:8080/current-temperature");
    QNetworkRequest request(url);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onStatisticsReply);
    networkManager->get(request);
}

void MainWindow::onStatisticsRequest() {
    // Получаем start и end значения из input полей
    QString start = startTimeInput->text();
    QString end = endTimeInput->text();
    QUrl url(QString("http://localhost:8080/statistics?start=%1&end=%2").arg(start, end));
    QNetworkRequest request(url);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onStatisticsReply);
    networkManager->get(request);
}

void MainWindow::onStatisticsReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonResponse = doc.object();

        if (jsonResponse.contains("current_temperature")) {
            // Обновляем текущую температуру на метке
            double currentTemp = jsonResponse["current_temperature"].toDouble();
            temperatureLabel->setText(QString("Текущая температура: %1 °C").arg(currentTemp));
        }

        if (jsonResponse.contains("temperatures")) {
            QJsonArray temperaturesArray = jsonResponse["temperatures"].toArray();

            // Сбор данных для графика
            QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
            QStringList labels;

            for (const QJsonValue &value : temperaturesArray) {
                QJsonObject tempData = value.toObject();
                double temp = tempData["temperature"].toDouble();
                long timestamp = tempData["timestamp"].toVariant().toLongLong();
                QDateTime dateTime = QDateTime::fromSecsSinceEpoch(timestamp);
                labels.append(dateTime.toString("HH:mm:ss"));
                series->append(timestamp, temp);
            }

            // Обновляем график
            QtCharts::QChart *chart = chartView->chart();
            chart->removeAllSeries();
            chart->addSeries(series);
            chart->createDefaultAxes();
        }
    } else {
        qDebug() << "Error fetching data:" << reply->errorString();
    }
    reply->deleteLater();
}
