#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include "../include/client_logger.h"
#include <not_implemented.h>

std::unordered_map<std::string, std::pair<size_t, std::ofstream>> client_logger::refcounted_stream::_global_streams;


logger& client_logger::log(
    const std::string &text,
    logger::severity severity) &
{
    throw not_implemented("const logger& client_logger::log(const std::string &,logger::severity) const &", "your code should be here...");
}

std::string client_logger::make_format(const std::string &message, severity sev) const
{
    throw not_implemented("std::string client_logger::make_format(const std::string &, severity) const", "your code should be here...");
}

client_logger::client_logger(
        const std::unordered_map<logger::severity, std::pair<std::forward_list<refcounted_stream>, bool>> &streams,
        std::string format)
{
    throw not_implemented("client_logger::client_logger(const std::unordered_map<logger::severity, std::pair<std::forward_list<refcounted_stream>, bool>> &, std::string)", "your code should be here...");
}

client_logger::flag client_logger::char_to_flag(char c) noexcept
{
    throw not_implemented("client_logger::flag client_logger::char_to_flag(char) noexcept", "your code should be here...");
}

client_logger::client_logger(const client_logger &other)
{
    throw not_implemented("client_logger::client_logger(const client_logger &other)", "your code should be here...");
}

client_logger &client_logger::operator=(const client_logger &other)
{
    throw not_implemented("client_logger::flag client_logger::char_to_flag(char) noexcept", "your code should be here...");
}

client_logger::client_logger(client_logger &&other) noexcept
{
    throw not_implemented("client_logger &client_logger::operator=(const client_logger &other)", "your code should be here...");
}

client_logger &client_logger::operator=(client_logger &&other) noexcept
{
    throw not_implemented("client_logger &client_logger::operator=(client_logger &&other) noexcept", "your code should be here...");
}

client_logger::~client_logger() noexcept
{
    throw not_implemented("client_logger::~client_logger() noexcept", "your code should be here...");
}

client_logger::refcounted_stream::refcounted_stream(const std::string &path)
{
    throw not_implemented("client_logger::refcounted_stream::refcounted_stream(const std::string &path)", "your code should be here...");
}

client_logger::refcounted_stream::refcounted_stream(const client_logger::refcounted_stream &oth)
{
    throw not_implemented("client_logger::refcounted_stream::refcounted_stream(const client_logger::refcounted_stream &)", "your code should be here...");
}

client_logger::refcounted_stream &
client_logger::refcounted_stream::operator=(const client_logger::refcounted_stream &oth)
{
    throw not_implemented("client_logger::refcounted_stream & client_logger::refcounted_stream::operator=(const client_logger::refcounted_stream &)", "your code should be here...");
}

client_logger::refcounted_stream::refcounted_stream(client_logger::refcounted_stream &&oth) noexcept
{
    throw not_implemented("client_logger::refcounted_stream::refcounted_stream(client_logger::refcounted_stream &&) noexcept", "your code should be here...");
}

client_logger::refcounted_stream &client_logger::refcounted_stream::operator=(client_logger::refcounted_stream &&oth) noexcept
{
    throw not_implemented("client_logger::refcounted_stream &client_logger::refcounted_stream::operator=(client_logger::refcounted_stream &&) noexcept", "your code should be here...");
}

client_logger::refcounted_stream::~refcounted_stream()
{
    throw not_implemented("client_logger::refcounted_stream::~refcounted_stream()", "your code should be here...");
}
