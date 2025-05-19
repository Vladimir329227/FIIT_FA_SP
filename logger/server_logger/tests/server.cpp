//
// Created by Des Caldnd on 3/27/2024.
//

#include "server.h"
#include <logger_builder.h>
#include <fstream>
#include <iostream>
#include <asio/thread_pool.hpp>
#include <asio/post.hpp>

using json = nlohmann::json;

server::server(uint16_t port)
{
    // Настройка маршрутов
    CROW_ROUTE(app, "/log")
        .methods("POST"_method)
        ([this](const crow::request& req) {
            std::lock_guard<std::mutex> lock(_mut);
        
            try {
                auto data = json::parse(req.body);
                
                std::cout << "=== Received full JSON ===" << std::endl;
                std::cout << data.dump(2) << std::endl;
                std::cout << "=== End JSON ===" << std::endl;
                
                // Извлечение данных
                int pid = data["pid"];
                std::string severity_str = data["severity"];
                std::string message = data["message"];
                auto streams = data["streams"];
                
                // Конвертация уровня логирования
                logger::severity sev;
                if (severity_str == "TRACE") sev = logger::severity::trace;
                else if (severity_str == "DEBUG") sev = logger::severity::debug;
                else if (severity_str == "INFO") sev = logger::severity::information;
                else if (severity_str == "WARNING") sev = logger::severity::warning;
                else if (severity_str == "ERROR") sev = logger::severity::error;
                else if (severity_str == "CRITICAL") sev = logger::severity::critical;
                else return crow::response(400, "Invalid severity");
                
                // Обработка потоков вывода
                for (const auto& stream : streams) 
                {
                    std::string type = stream["type"];
                    if (type == "file") 
                    {
                        std::string path = stream["path"];
                        std::ofstream file(path, std::ios::app);
                        if (file) file << message << "\n";
                    } 
                    else if (type == "console") 
                    {
                        std::cout << message << std::endl;
                    }
                }
                
                return crow::response(200);
            } 
            catch (const json::exception& e) 
            {
                return crow::response(400, "JSON parse error: " + std::string(e.what()));
            }
            catch (const std::exception& e) 
            {
                return crow::response(500, "Server error: " + std::string(e.what()));
            }
        });

    // Запуск сервера в конструкторе
    app.port(port).run();
}

/*
server::server(uint16_t port)
{
    // Настройка маршрутов
    CROW_ROUTE(app, "/log")
            .methods("POST"_method)
                    ([this](const crow::request& req) {
                        std::lock_guard<std::mutex> lock(_mut);

                        try {
                            // Парсинг JSON
                            auto data = json::parse(req.body);

                            // Вывод полной информации о запросе
                            std::cout << "\n=== NEW LOG MESSAGE ===" << std::endl;
                            std::cout << "From IP: " << req.remote_ip_address << std::endl;
                            std::cout << "User-Agent: " << req.get_header_value("User-Agent") << std::endl;
                            std::cout << "Content-Length: " << req.body.size() << std::endl;

                            // Извлечение и отображение основных данных
                            std::cout << "\n[HEADER]" << std::endl;
                            std::cout << "PID:      " << data["pid"].get<int>() << std::endl;
                            std::cout << "Severity: " << data["severity"].get<std::string>() << std::endl;

                            // Отображение сообщения
                            std::cout << "\n[MESSAGE]" << std::endl;
                            std::cout << data["message"].get<std::string>() << std::endl;

                            // Отображение информации о потоках
                            std::cout << "\n[STREAMS]" << std::endl;
                            for (const auto& stream : data["streams"]) {
                                std::string type = stream["type"];
                                if (type == "file") {
                                    std::cout << "File: " << stream["path"].get<std::string>() << std::endl;
                                } else if (type == "console") {
                                    std::cout << "Console output" << std::endl;
                                }
                            }

                            std::cout << "=== END OF MESSAGE ===\n" << std::endl;
                            return crow::response(200, "Message received");
                        }
                        catch (const json::exception& e) {
                            std::cerr << "JSON Error: " << e.what() << std::endl;
                            return crow::response(400, "JSON error: " + std::string(e.what()));
                        }
                        catch (const std::exception& e) {
                            std::cerr << "Server Error: " << e.what() << std::endl;
                            return crow::response(500, "Server error: " + std::string(e.what()));
                        }
                    });

    // Запуск сервера с дополнительными параметрами
    app.port(port)
            .bindaddr("0.0.0.0")
            .multithreaded()
            .run();
}
*/