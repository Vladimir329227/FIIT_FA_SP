#include <filesystem>
#include <utility>
#include <not_implemented.h>
#include "../include/client_logger_builder.h"
#include <not_implemented.h>

using namespace nlohmann;

logger_builder& client_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity) &
{
    _output_streams[severity].first.emplace_front(stream_file_path);
    return *this;
}

logger_builder& client_logger_builder::add_console_stream(
    logger::severity severity) &
{
    _output_streams[severity].second = true;
    return *this;
}

logger_builder& client_logger_builder::transform_with_configuration(
        std::string const &configuration_file_path,
        std::string const &configuration_path) &
{
    // Чтение JSON-файла
    std::ifstream config_file(configuration_file_path);
    if (!config_file.is_open())
        return *this;

    nlohmann::json config;
    try
    {
        config_file >> config;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        throw std::runtime_error("JSON parse error: " + std::string(e.what()));
    }

    // Навигация по пути в JSON
    std::istringstream path_stream(configuration_path);
    std::string path_part;
    nlohmann::json* current = &config;
    while (std::getline(path_stream, path_part, '.'))
    {
        if (current->contains(path_part))
            current = &(*current)[path_part];
        else
            throw std::invalid_argument("Configuration path not found: " + configuration_path);
    }

    // Обработка каждого уровня в текущем JSON-узле
    for (auto& [severity_str, severity_config] : current->items())
        parse_severity(logger_builder::string_to_severity(severity_str), severity_config);

    return *this;
}

logger_builder& client_logger_builder::clear() &
{
    _output_streams.clear();
    _format = "%m";
    _destination = "http://127.0.0.1:9200";
    return *this;
}

logger *client_logger_builder::build() const
{
    return new client_logger(_output_streams, _format);
}

logger_builder& client_logger_builder::set_format(const std::string &format) &
{
    _format = std::move(format);
    return *this;
}

void client_logger_builder::parse_severity(logger::severity sev, nlohmann::json& j)
{
    // Обработка флага консоли
    if (j.contains("console"))
    {
        bool console_flag = j["console"].get<bool>();
        _output_streams[sev].second = console_flag;
    }

    // Обработка списка файлов
    if (j.contains("files"))
    {
        for (const auto& file : j["files"])
        {
            std::string path = file.get<std::string>();
            _output_streams[sev].first.emplace_front(path);
        }
    }
}

logger_builder& client_logger_builder::set_destination(const std::string &format) &
{
    _destination = std::move(format);
    return *this;
}
