#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <bits/this_thread_sleep.h>
#include <nlohmann/json.hpp>  // Для работы с JSON
#include <sqlite3.h>          // Для работы с SQLite
#include <string>
#include <deque>
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <fstream>
#include <thread>
#include <mutex>
#include <fcntl.h>
#include <unistd.h>           // Для работы с файловыми дескрипторами

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

// SQLite
sqlite3* db;
std::mutex log_mutex;
std::deque<std::pair<std::time_t, double>> measurements;

// Инициализация базы данных
void init_database() {
    int rc = sqlite3_open("temperature_logs.db", &db);
    if (rc) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    const char* create_table_sql = "CREATE TABLE IF NOT EXISTS temperatures (timestamp INTEGER, temperature REAL);";
    char* err_msg = nullptr;
    rc = sqlite3_exec(db, create_table_sql, nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }
}

// Запись данных в базу
void write_to_db(std::time_t timestamp, double temperature) {
    std::lock_guard<std::mutex> lock(log_mutex);

    std::string query = "INSERT INTO temperatures (timestamp, temperature) VALUES (" +
                        std::to_string(timestamp) + ", " + std::to_string(temperature) + ");";
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }
}

// Логирование необработанных запросов
void log_raw_request(tcp::socket& socket) {
    try {
        char buffer[1024] = {0};
        boost::system::error_code ec;
        size_t bytes_read = socket.read_some(boost::asio::buffer(buffer), ec);

        if (!ec) {
            std::cout << "Raw request:\n" << std::string(buffer, bytes_read) << std::endl;
        } else {
            std::cerr << "Error reading raw request: " << ec.message() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception logging raw request: " << e.what() << std::endl;
    }
}

// Обработка HTTP-запросов
void handle_request(tcp::socket& socket) {
    try {
        boost::beast::flat_buffer buffer;
        http::request<http::string_body> req;

        // Чтение и парсинг запроса
        try {
            http::read(socket, buffer, req);
        } catch (const boost::beast::system_error& err) {
            std::cerr << "HTTP parsing error: " << err.code().message()<<std::endl;
            return;
        }
        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::content_type, "application/json");
        res.set("Access-Control-Allow-Origin", "*");

        // Обработка запроса к главной странице
        if (req.target() == "/") {
            std::ifstream file("index.html");
            if (file) {
                std::ostringstream ss;
                ss << file.rdbuf();
                res.set(http::field::content_type, "text/html");
                res.body() = ss.str();
            } else {
                res.result(http::status::not_found);
                res.body() = "index.html not found";
            }
            http::write(socket, res);
            return;
        }

        // Запрос текущей температуры
        if (req.target() == "/current-temperature") {
            nlohmann::json json_response;
            std::lock_guard<std::mutex> lock(log_mutex);if (!measurements.empty()) {
                double current_temp = measurements.back().second;
                json_response["current_temperature"] = current_temp;
            } else {
                json_response["error"] = "No data available";
            }
            res.body() = json_response.dump();
            http::write(socket, res);
            return;
        }

        // Обработка статистики
        if (req.target().starts_with("/statistics?")) {
            std::string query = std::string(req.target());
            size_t start_pos = query.find("start=");
            size_t end_pos = query.find("end=");

            if (start_pos != std::string::npos && end_pos != std::string::npos) {
                start_pos += 6;
                size_t start_end = query.find("&", start_pos);
                std::string start_str = query.substr(start_pos, start_end - start_pos);
                std::string end_str = query.substr(end_pos + 4);

                try {
                    std::time_t start_time = std::stoll(start_str);
                    std::time_t end_time = std::stoll(end_str);

                    nlohmann::json json_response;
                    {
                        std::lock_guard<std::mutex> lock(log_mutex);
                        for (const auto& [timestamp, temp] : measurements) {
                            if (timestamp >= start_time && timestamp <= end_time) {
                                json_response["temperatures"].push_back(
                                    {{"timestamp", timestamp}, {"temperature", temp}});
                            }
                        }
                    }

                    if (json_response.contains("temperatures") && !json_response["temperatures"].empty()) {
                        res.body() = json_response.dump();
                    } else {
                        json_response["error"] = "No data available for the specified period";
                        res.body() = json_response.dump();
                    }
                } catch (const std::exception& e) {
                    nlohmann::json json_response;
                    json_response["error"] = std::string("Invalid parameters: ") + e.what();
                    res.body() = json_response.dump();
                }
            } else {
                nlohmann::json json_response;
                json_response["error"] = "Invalid query parameters";
                res.body() = json_response.dump();
            }
            http::write(socket, res);
            return;
        }

        // Обработка неизвестного запроса
        res.result(http::status::not_found);
        res.body() = "Not Found";
        http::write(socket, res);

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main() {
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, {boost::asio::ip::make_address("0.0.0.0"), 8080});

    init_database();

    // Поток для чтения температуры
    std::thread temperature_thread([]() {
        const std::string device = "/tmp/virtual_serial";

        int fifo = open(device.c_str(), O_RDONLY);
        if (fifo < 0) {
            std::cerr << "Error opening FIFO" << std::endl;
            return;
        }

        char buffer[256];
        while (true) {
            int n = read(fifo, buffer, sizeof(buffer) - 1);
            if (n > 0) {
                buffer[n] = '\0';
                double temperature = std::stod(buffer);
                auto now = std::chrono::system_clock::now();
                auto now_time = std::chrono::system_clock::to_time_t(now);

                {
                    std::lock_guard<std::mutex> lock(log_mutex);
                    measurements.push_back({now_time, temperature});
                }
                write_to_db(now_time, temperature);
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        close(fifo);
    });
    // HTTP-сервер
    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);
        handle_request(socket);
    }

    temperature_thread.join();
    return 0;
}