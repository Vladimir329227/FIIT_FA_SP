#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include <chrono>
#include <ctime>
#include "../include/client_logger.h"
#include <not_implemented.h>
#include <mutex>

std::unordered_map<std::string, std::pair<size_t, std::ofstream>> client_logger::refcounted_stream::_global_streams;
std::mutex client_logger::refcounted_stream::_global_streams_mutex;

logger& client_logger::log(
        const std::string &text,
        logger::severity severity) &
{
    // Поиск настроек для указанного уровня серьёзности
    auto it = _output_streams.find(severity);
    if (it == _output_streams.end())
    {
        // Уровень не найден, логирование не требуется
        return *this;
    }

    // Получение списка потоков и флага консоли
    auto& [streams, need_console] = it->second;

    // Форматирование сообщения
    std::string formatted_message = make_format(text, severity);

    // Запись во все файловые потоки
    for (const auto& stream : streams)
    {
        std::lock_guard<std::mutex> lock(client_logger::refcounted_stream::_global_streams_mutex);
        if (stream._stream.second != nullptr && stream._stream.second->is_open())
            *stream._stream.second << formatted_message << std::endl;
        else
            continue;
        //throw std::runtime_error("Log stream is not open for path: " + stream._stream.first);
    }


    // Запись в консоль, если требуется
    if (need_console)
        std::cout << formatted_message << std::endl;

    return *this;
}


std::string client_logger::make_format(const std::string& message, severity sev) const
{
    std::ostringstream formatted_message;
    const size_t format_len = _format.size();

    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm = std::gmtime(&time); // GMT+0

    for (size_t i = 0; i < format_len; ++i)
    {
        if (_format[i] == '%' && i + 1 < format_len)
        {
            switch (_format[++i])
            {
                case 'd': // Date (YYYY-MM-DD)
                {
                    char buf[20];
                    std::strftime(buf, sizeof(buf), "%F", tm);
                    formatted_message << buf;
                }
                    break;

                case 't': // Time (HH:MM:SS)
                {
                    char buf[20];
                    std::strftime(buf, sizeof(buf), "%T", tm);
                    formatted_message << buf;
                }
                    break;

                case 's': // Severity string
                    formatted_message << severity_to_string(sev);
                case 'm': // Message
                    formatted_message << message;
                    break;
                default: // Unknown specifier - copy as-is
                    formatted_message << '%' << _format[i];
            }
        }
        else
        {
            formatted_message << _format[i];
        }
    }

    return formatted_message.str();
}

client_logger::client_logger(
        const std::unordered_map<logger::severity, std::pair<std::forward_list<refcounted_stream>, bool>>& streams,
        std::string format)
        : _output_streams(),
          _format(std::move(format))
{
    // Обрабатываем каждый уровень логирования
    for (const auto& [severity, streams_info] : streams)
    {
        const auto& [src_streams, need_console] = streams_info;

        // Создаем новый список потоков
        std::forward_list<refcounted_stream> new_streams;

        // Копируем файловые потоки
        auto prev = new_streams.before_begin();
        for (const auto& stream : src_streams){
            prev = new_streams.insert_after(prev, stream);
            prev->open();
        }

        // Сохраняем в выходные потоки
        _output_streams[severity] = {std::move(new_streams), need_console};
    }
}

client_logger::flag client_logger::char_to_flag(char c) noexcept
{
    throw not_implemented("const logger& client_logger::log(const std::string &,logger::severity) const &", "your code should be here...");
}

client_logger::client_logger(const client_logger &other)
        : client_logger(other._output_streams, other._format)
{}

client_logger &client_logger::operator=(const client_logger &other)
{
    if (this == &other) return *this;

    this->~client_logger();

    new (this) client_logger(other);

    return *this;
}

client_logger::client_logger(client_logger &&other) noexcept
        : _output_streams(std::move(other._output_streams)),
          _format(std::move(other._format))
{
    other._output_streams.clear();
    other._format.clear();
}

client_logger &client_logger::operator=(client_logger &&other) noexcept
{
    if (this == &other) return *this;

    this->~client_logger();

    _output_streams = std::move(other._output_streams);
    _format = std::move(other._format);

    other._output_streams.clear();
    other._format.clear();

    return *this;
}

client_logger::~client_logger() noexcept
{
    _output_streams.clear();
}

client_logger::refcounted_stream::refcounted_stream(const std::string &path)
{
    _stream.first = path;
    _stream.second = nullptr;
}

client_logger::refcounted_stream::refcounted_stream(const client_logger::refcounted_stream &oth)
{
    _stream = std::move(oth._stream);
}

client_logger::refcounted_stream &
client_logger::refcounted_stream::operator=(const client_logger::refcounted_stream &oth)
{
    _stream = std::move(oth._stream);
}

client_logger::refcounted_stream::refcounted_stream(client_logger::refcounted_stream &&oth) noexcept
{
    _stream = oth._stream;
}

client_logger::refcounted_stream &client_logger::refcounted_stream::operator=(client_logger::refcounted_stream &&oth) noexcept
{
    _stream = oth._stream;
    return *this;
}

void client_logger::refcounted_stream::open()
{
    std::lock_guard<std::mutex> lock(_global_streams_mutex);
    if (_stream.second == nullptr){
        if (_global_streams.count(_stream.first))
            _global_streams[_stream.first].first++;
        else{
            _global_streams[_stream.first] = std::make_pair((size_t)1, std::ofstream());
            _global_streams[_stream.first].second.open(_stream.first, std::ofstream::out | std::ofstream::app);
            if (!_global_streams[_stream.first].second.is_open())
                _stream.second = nullptr;
                //throw std::runtime_error("Cannot open log file: " + _stream.first);
            else
                _stream.second = &_global_streams[_stream.first].second;
        }
    }
    else
        throw not_implemented("void client_logger::refcounted_stream::open()", "Error _stream");
}

client_logger::refcounted_stream::~refcounted_stream()
{
    std::lock_guard<std::mutex> lock(_global_streams_mutex);
    if (_stream.second != nullptr && _global_streams.count(_stream.first))
    {
        _global_streams[_stream.first].first--;
        if (_global_streams[_stream.first].first == 0)
            _global_streams[_stream.first].second.close();
    }
}
