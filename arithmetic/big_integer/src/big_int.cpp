//
// Created by Des Caldnd on 5/27/2024.
//

#include "../include/big_int.h"
#include <ranges>
#include <exception>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>

std::strong_ordering big_int::operator<=>(const big_int &other) const noexcept
{
    if (_sign != other._sign) {
        return _sign ? std::strong_ordering::greater : std::strong_ordering::less;
    }

    const size_t lhs_size = _digits.size();
    const size_t rhs_size = other._digits.size();

    if (lhs_size != rhs_size) {
        const bool size_compare = (lhs_size > rhs_size);
        return (_sign ? size_compare : !size_compare)
               ? std::strong_ordering::greater
               : std::strong_ordering::less;
    }

    for (auto lit = _digits.rbegin(), rit = other._digits.rbegin();
         lit != _digits.rend();
         ++lit, ++rit)
    {
        if (*lit != *rit) {
            const auto cmp = (*lit <=> *rit);
            if (_sign) { // Для положительных чисел
                return cmp;
            } else {     // Для отрицательных инвертируем
                return (cmp == std::strong_ordering::less)
                       ? std::strong_ordering::greater
                       : (cmp == std::strong_ordering::greater)
                         ? std::strong_ordering::less
                         : std::strong_ordering::equal;
            }
        }
    }

    return std::strong_ordering::equal;
}

big_int::operator bool() const noexcept
{
    return !_digits.empty();
}

big_int &big_int::operator++() &
{
    *this += big_int(1);
    return *this;
}


big_int big_int::operator++(int)
{
    big_int temp(*this);
    ++(*this);
    return temp;
}

big_int &big_int::operator--() &
{
    *this -= big_int(1);
    return *this;
}


big_int big_int::operator--(int)
{
    big_int temp(*this);
    --(*this);
    return temp;
}

big_int &big_int::operator+=(const big_int &other) &
{
    if (this == &other) {
        big_int temp(other);
        return this->plus_assign(temp);
    }

    this->plus_assign(other);
    return *this;
}

big_int &big_int::operator-=(const big_int &other) &
{
    if (this == &other) {
        big_int temp(other);
        return this->minus_assign(temp);
    }

    this->minus_assign(other);
    return *this;
}

big_int big_int::operator+(const big_int &other) const
{
    big_int result(*this);
    result.plus_assign(other);
    return result;
}

big_int big_int::operator-(const big_int &other) const
{
    big_int result(*this);
    result.minus_assign(other);
    return result;
}

big_int big_int::operator*(const big_int &other) const
{
    big_int result(*this);
    result.multiply_assign(other);
    return result;
}

big_int big_int::operator/(const big_int &other) const
{
    big_int result(*this);
    result.divide_assign(other);
    return result;
}

big_int big_int::operator%(const big_int &other) const
{
    big_int result(*this);
    result.modulo_assign(other);
    return result;
}

big_int big_int::operator&(const big_int &other) const
{
    if (!_sign || !other._sign) {
        throw std::domain_error("Bitwise AND not supported for negative numbers");
    }

    big_int result;
    size_t max_size = std::max(_digits.size(), other._digits.size());
    result._digits.resize(max_size, 0);

    for (size_t i = 0; i < max_size; ++i) {
        unsigned int a = (i < _digits.size()) ? _digits[i] : 0;
        unsigned int b = (i < other._digits.size()) ? other._digits[i] : 0;
        result._digits[i] = a & b;
    }

    result.optimise();
    return result;
}

big_int big_int::operator|(const big_int &other) const
{
    if (!_sign || !other._sign) {
        throw std::domain_error("Bitwise OR not supported for negative numbers");
    }

    big_int result;
    size_t max_size = std::max(_digits.size(), other._digits.size());
    result._digits.resize(max_size, 0);

    for (size_t i = 0; i < max_size; ++i) {
        unsigned int a = (i < _digits.size()) ? _digits[i] : 0;
        unsigned int b = (i < other._digits.size()) ? other._digits[i] : 0;
        result._digits[i] = a | b;
    }

    result.optimise();
    return result;
}

big_int big_int::operator^(const big_int &other) const
{
    if (!_sign || !other._sign) {
        throw std::domain_error("Bitwise XOR not supported for negative numbers");
    }

    big_int result;
    size_t max_size = std::max(_digits.size(), other._digits.size());
    result._digits.resize(max_size, 0);

    for (size_t i = 0; i < max_size; ++i) {
        unsigned int a = (i < _digits.size()) ? _digits[i] : 0;
        unsigned int b = (i < other._digits.size()) ? other._digits[i] : 0;
        result._digits[i] = a ^ b;
    }

    result.optimise();
    return result;
}

big_int big_int::operator<<(size_t shift) const
{
    if (!_sign) {
        throw std::domain_error("Bitwise shift not supported for negative numbers");
    }

    big_int result(*this);
    result <<= shift;
    return result;
}

big_int big_int::operator>>(size_t shift) const
{
    if (!_sign) {
        throw std::domain_error("Bitwise shift not supported for negative numbers");
    }

    big_int result(*this);
    result >>= shift;
    return result;
}

big_int &big_int::operator%=(const big_int &other) &
{
    if (this == &other) {
        big_int temp(other);
        return this->modulo_assign(temp);
    }

    this->modulo_assign(other);
    return *this;
}

big_int big_int::operator~() const
{
    if (!_sign) {
        throw std::domain_error("Bitwise NOT not supported for negative numbers");
    }

    big_int result(*this);
    for (auto& digit : result._digits) {
        digit = ~digit;
    }
    result.optimise();
    return result;
}

big_int &big_int::operator&=(const big_int &other) &
{
    if (!_sign || !other._sign) {
        throw std::domain_error("Bitwise AND not supported for negative numbers");
    }

    size_t min_size = std::min(_digits.size(), other._digits.size());
    for (size_t i = 0; i < min_size; ++i) {
        _digits[i] &= other._digits[i];
    }

    if (_digits.size() > min_size) {
        _digits.resize(min_size);
    }

    optimise();
    return *this;
}

big_int &big_int::operator|=(const big_int &other) &
{
    if (!_sign || !other._sign) {
        throw std::domain_error("Bitwise OR not supported for negative numbers");
    }

    size_t min_size = std::min(_digits.size(), other._digits.size());
    for (size_t i = 0; i < min_size; ++i) {
        _digits[i] |= other._digits[i];
    }

    if (_digits.size() > min_size) {
        _digits.resize(min_size);
    }

    optimise();
    return *this;
}

big_int &big_int::operator^=(const big_int &other) &
{
    if (!_sign || !other._sign) {
        throw std::domain_error("Bitwise XOR not supported for negative numbers");
    }

    size_t min_size = std::min(_digits.size(), other._digits.size());
    for (size_t i = 0; i < min_size; ++i) {
        _digits[i] ^= other._digits[i];
    }

    if (_digits.size() > min_size) {
        _digits.resize(min_size);
    }

    optimise();
    return *this;
}

big_int &big_int::operator<<=(size_t shift) &
{
    if (!_sign) {
        throw std::domain_error("Bitwise shift not supported for negative numbers");
    }

    const size_t bits_per_block = sizeof(unsigned int) * 8;
    const size_t block_shift = shift / bits_per_block;
    const size_t bit_shift = shift % bits_per_block;

    // Сдвиг целыми блоками
    if (block_shift > 0) {
        _digits.insert(_digits.begin(), block_shift, 0);
    }

    // Битовый сдвиг
    if (bit_shift > 0) {
        unsigned long long carry = 0;
        for (auto& digit : _digits) {
            carry += static_cast<unsigned long long>(digit) << bit_shift;
            digit = carry & 0xFFFFFFFF;
            carry >>= bits_per_block;
        }
        if (carry) {
            _digits.push_back(carry);
        }
    }

    optimise();
    return *this;
}

big_int &big_int::operator>>=(size_t shift) &
{
    if (!_sign) {
        throw std::domain_error("Bitwise shift not supported for negative numbers");
    }

    const size_t bits_per_block = sizeof(unsigned int) * 8;
    const size_t block_shift = shift / bits_per_block;
    const size_t bit_shift = shift % bits_per_block;

    // Сдвиг целыми блоками
    if (block_shift > 0) {
        if (block_shift >= _digits.size()) {
            _digits.clear();
            return *this;
        }
        _digits.erase(_digits.begin(), _digits.begin() + block_shift);
    }

    // Битовый сдвиг
    if (bit_shift > 0) {
        const size_t complement_shift = bits_per_block - bit_shift;
        unsigned int carry = 0;

        // Обрабатываем от младшего к старшему (прямой порядок)
        for (auto it = _digits.begin(); it != _digits.end(); ++it) {
            unsigned int val = *it;
            unsigned int new_val = (val >> bit_shift) | carry;
            carry = val << complement_shift;
            *it = new_val;
        }
    }

    optimise();
    return *this;
}

big_int big_int::operator-() && {
    big_int result(std::move(*this));
    if (!result._digits.empty()) {
        result._sign = !result._sign;
    }
    return result;
}

big_int big_int::operator-() const & {
    if (_digits.empty())
        return *this;

    big_int result(*this);
    result._sign = !_sign;
    return result;
}

big_int &big_int::plus_assign(const big_int &other, size_t shift) &
{
    if (_sign == other._sign) {
        // Случай 1: Одинаковые знаки — складываем абсолютные значения
        size_t new_size = std::max(_digits.size(), other._digits.size() + shift) + 1;
        _digits.resize(new_size, 0);

        unsigned int carry = 0;
        for (size_t i = 0; i < other._digits.size() || carry; ++i) {
            unsigned int other_digit = (i < other._digits.size()) ? other._digits[i] : 0;
            unsigned int& target = _digits[i + shift];

            unsigned long long sum = static_cast<unsigned long long>(target)
                                     + other_digit
                                     + carry;
            target = static_cast<unsigned int>(sum);
            carry = static_cast<unsigned int>(sum >> (sizeof(unsigned int) * 8));
        }
    } else {
        // Случай 2: Разные знаки — вычитаем абсолютные значения
        const big_int abs_this = abs(*this);
        const big_int abs_other = abs(other);

        if (abs_this >= abs_other) {
            // |this| >= |other|: this = abs_this - abs_other
            unsigned int borrow = 0;
            for (size_t i = 0; i < other._digits.size() || borrow; ++i) {
                unsigned int other_digit = (i < other._digits.size()) ? other._digits[i] : 0;
                unsigned int& target = _digits[i + shift];

                long long diff = static_cast<long long>(target)
                                 - other_digit
                                 - borrow;

                if (diff < 0) {
                    diff += (1ull << (sizeof(unsigned int) * 8));
                    borrow = 1;
                } else {
                    borrow = 0;
                }

                target = static_cast<unsigned int>(diff);
            }
        } else {
            // |this| < |other|: this = -(abs_other - abs_this)
            big_int result = abs_other;
            result.minus_assign(abs_this, shift);
            *this = result;
            _sign = !_sign;
        }
    }

    optimise();
    return *this;
}

big_int &big_int::minus_assign(const big_int &other, size_t shift) &
{
    big_int negated(other);
    negated._sign = !negated._sign;
    return plus_assign(negated, shift);
}

big_int &big_int::operator*=(const big_int &other) &
{
    if (this == &other) {
        big_int temp(other);
        return this->multiply_assign(temp);
    }

    this->multiply_assign(other);
    return *this;
}

big_int &big_int::operator/=(const big_int &other) &
{
    if (this == &other) {
        big_int temp(other);
        return this->divide_assign(temp);
    }

    this->divide_assign(other);
    return *this;
}

std::string big_int::to_string() const
{
    if (is_zero()) return "0";

    big_int num = *this;
    num._sign = true; // Работаем с модулем
    std::string result;
    constexpr unsigned int chunk_radix = 1'000'000'000; // 10^9 для оптимизации
    constexpr int chunk_digits = 9;

    while (!num.is_zero()) {
        unsigned int remainder = num.divide_by_small(chunk_radix);
        std::string part = std::to_string(remainder);

        if (!num.is_zero()) {
            part = std::string(chunk_digits - part.size(), '0') + part;
        }

        result = part + result;
    }

    if (!_sign) result = "-" + result;
    return result;
}

std::ostream &operator<<(std::ostream &stream, const big_int &value)
{
    return stream << value.to_string();
}

std::istream &operator>>(std::istream &stream, big_int &value)
{
    std::string input;
    char ch;

    // Пропускаем ведущие пробелы
    stream >> std::ws;

    // Читаем знак
    if (stream.peek() == '-' || stream.peek() == '+') {
        stream.get(ch);
        input += ch;
    }

    // Читаем цифры
    bool has_digits = false;
    while (stream.get(ch) && std::isdigit(static_cast<unsigned char>(ch))) {
        input += ch;
        has_digits = true;
    }

    // Если считали хотя бы одну цифру - возвращаем последний символ
    if (has_digits && !stream.eof()) {
        stream.unget();
    }

    // Проверка валидности ввода
    if (!has_digits || input.empty()) {
        stream.setstate(std::ios::failbit);
        return stream;
    }

    // Пробуем создать big_int
    try {
        value = big_int(input, 10); // Основание 10
    } catch (const std::invalid_argument&) {
        stream.setstate(std::ios::failbit);
    } catch (const std::exception&) {
        stream.setstate(std::ios::failbit | std::ios::badbit);
    }

    return stream;
}

bool big_int::operator==(const big_int &other) const noexcept
{
    if (_sign != other._sign || _digits.size() != other._digits.size()) {
        return false;
    }

    return std::equal(_digits.rbegin(), _digits.rend(),
                      other._digits.rbegin(), other._digits.rend());
}

big_int::big_int(const std::vector<unsigned int, pp_allocator<unsigned int>> &digits, bool sign)
        : big_int(std::vector<unsigned int, pp_allocator<unsigned int>>(digits), sign)
{}

big_int::big_int(std::vector<unsigned int, pp_allocator<unsigned int>> &&digits, bool sign) noexcept
        : _sign(sign),
          _digits(std::move(digits))
{
    optimise();
}

big_int::big_int(const std::string &num, unsigned int radix, pp_allocator<unsigned int> alloc)
        : big_int(alloc)
{
    if (radix < 2 || radix > 36)
        throw std::invalid_argument("Radix must be 2-36");

    bool new_sign = true;
    size_t start = 0;

    if (!num.empty()) {
        if (num[0] == '-') {
            new_sign = false;
            start = 1;
        } else if (num[0] == '+') {
            start = 1;
        }
    }

    if (start >= num.size())
        throw std::invalid_argument("Invalid number format");

    for (size_t i = start; i < num.size(); ++i) {
        char c = num[i];
        unsigned int digit;

        if (isdigit(static_cast<unsigned char>(c))) {
            digit = c - '0';
        } else if (isalpha(static_cast<unsigned char>(c))) {
            digit = toupper(static_cast<unsigned char>(c)) - 'A' + 10;
        } else {
            throw std::invalid_argument("Invalid character");
        }

        if (digit >= radix)
            throw std::invalid_argument("Digit exceeds radix");

        *this *= radix;
        *this += big_int(digit);
    }

    if (is_zero()) {
        _sign = true;
    } else {
        _sign = new_sign;
    }
}

big_int::big_int(pp_allocator<unsigned int> alloc)
        : big_int(std::vector<unsigned int, pp_allocator<unsigned int>>(alloc), true)
{}

big_int &big_int::multiply_assign(const big_int &other, big_int::multiplication_rule rule) &
{
    if (this == &other) {
        big_int temp(other);
        return multiply_assign(temp, rule);
    }

    switch (rule) {
        case multiplication_rule::trivial: {
            big_int result;
            result._digits.resize(_digits.size() + other._digits.size(), 0);
            result._sign = !(_sign ^ other._sign);

            for (size_t i = 0; i < _digits.size(); ++i) {
                uint64_t carry = 0; // Используем 64-битный аккумулятор
                for (size_t j = 0; j < other._digits.size() || carry; ++j) {
                    // Рассчитываем индекс в результате
                    const size_t idx = i + j;

                    // Получаем текущие цифры
                    const uint64_t a = _digits[i];
                    const uint64_t b = (j < other._digits.size()) ? other._digits[j] : 0;

                    // Вычисляем произведение и добавляем перенос
                    uint64_t product = a * b + carry;

                    // Добавляем существующее значение в результате
                    if (idx < result._digits.size()) {
                        product += result._digits[idx];
                    }

                    // Сохраняем младшие 32 бита
                    if (idx < result._digits.size()) {
                        result._digits[idx] = static_cast<unsigned int>(product & 0xFFFFFFFFu);
                    }

                    // Переносим старшие 32 бита
                    carry = product >> 32;
                }
            }

            result.optimise();
            *this = std::move(result);
            break;
        }
        case multiplication_rule::Karatsuba: {
            // Упрощенная реализация Карацубы
            if (_digits.empty() || other._digits.empty()) {
                *this = big_int();
                return *this;
            }

            size_t m = std::max(_digits.size(), other._digits.size()) / 2;

            big_int low1 = split(0, m);
            big_int high1 = split(m, _digits.size());
            big_int low2 = other.split(0, m);
            big_int high2 = other.split(m, other._digits.size());

            big_int z0 = low1 * low2;
            big_int z1 = (low1 + high1) * (low2 + high2);
            big_int z2 = high1 * high2;

            *this = (z2 << (2 * m * 32)) + ((z1 - z2 - z0) << (m * 32)) + z0;
            _sign = !(_sign ^ other._sign);
            break;
        }

        case multiplication_rule::SchonhageStrassen:
            throw std::invalid_argument("Unknown multiplication rule");

        default:
            throw std::invalid_argument("Unknown multiplication rule");
    }

    return *this;
}

big_int &big_int::divide_assign(const big_int &other, big_int::division_rule rule) &
{
    if (other == 0) {
        throw std::invalid_argument("Division by zero");
    }

    switch (rule) {
        case division_rule::trivial: {
            bool result_sign = !(_sign ^ other._sign);
            big_int dividend = abs(*this);
            const big_int divisor = abs(other);
            _digits.clear();
            big_int current;

            for (auto it = dividend._digits.rbegin(); it != dividend._digits.rend(); ++it) {
                current <<= sizeof(unsigned int) * 8;
                current += *it;

                uint64_t quotient = 0;
                uint64_t left = 0;
                uint64_t right = UINT32_MAX;

                while (left <= right) {
                    uint64_t mid = left + (right - left) / 2; // Избегаем переполнение
                    big_int temp = divisor;
                    temp *= static_cast<unsigned int>(mid); // Явное умножение

                    if (temp <= current) {
                        quotient = mid;
                        left = mid + 1;
                    } else {
                        right = mid - 1;
                    }
                }

                _digits.insert(_digits.begin(), static_cast<unsigned int>(quotient));
                current -= divisor * static_cast<unsigned int>(quotient);
            }

            _sign = result_sign;
            optimise();
            break;
        }
        case division_rule::Newton:
            throw std::invalid_argument("Unknown division rule");
        case division_rule::BurnikelZiegler:
            throw std::invalid_argument("Unknown division rule");
        default:
            throw std::invalid_argument("Unknown division rule");
    }

    return *this;
}

big_int &big_int::modulo_assign(const big_int &other, big_int::division_rule rule) &
{
    if (other == 0) {
        throw std::invalid_argument("modulo_assign: division by zero");
    }

    // Сохраняем исходные данные
    bool original_sign = _sign;
    big_int original = *this;

    // Вычисляем частное и произведение (quotient * other)
    big_int quotient = original;
    quotient.divide_assign(other, rule);
    quotient.multiply_assign(other, multiplication_rule::trivial);

    // Остаток = original - quotient * other
    *this = original;
    this->minus_assign(quotient, 0);

    // Корректируем остаток в диапазон [0, |other|)
    const big_int abs_other = other._sign ? other : -other;
    if (*this < 0) {
        plus_assign(abs_other, 0);
    } else if (*this >= abs_other) {
        minus_assign(abs_other, 0);
    }

    // Сохраняем знак исходного делимого
    _sign = original_sign;

    optimise();
    return *this;
}

big_int operator""_bi(unsigned long long n)
{
    return big_int(n);
}

void big_int::optimise() {
    // Удаляем ведущие нули (которые находятся в конце вектора)
    while (!_digits.empty() && _digits.back() == 0) {
        _digits.pop_back();
    }

    // Если число нулевое, устанавливаем корректное состояние
    if (_digits.empty()) {
        _sign = true;
        _digits.push_back(0);
    }
}

// Разделение числа на части для Карацубы
big_int big_int::split(size_t start, size_t end) const {
    big_int part;
    part._digits.assign(
            _digits.begin() + start,
            _digits.begin() + std::min(end, _digits.size())
    );
    part.optimise();
    return part;
}

bool big_int::is_zero() const noexcept {
    return _digits.empty() || (std::all_of(_digits.begin(), _digits.end(), [](auto d) { return d == 0; }));
}

unsigned int big_int::divide_by_small(unsigned int divisor) {
    unsigned long long remainder = 0;
    for (auto it = _digits.rbegin(); it != _digits.rend(); ++it) {
        remainder = (remainder << 32) | *it;
        *it = static_cast<unsigned int>(remainder / divisor);
        remainder %= divisor;
    }

    // Удаление ведущих нулей
    while (!_digits.empty() && _digits.back() == 0) {
        _digits.pop_back();
    }

    return static_cast<unsigned int>(remainder);
}

big_int big_int::abs(const big_int& num) {
    big_int result(num);
    result._sign = true;
    return result;
}