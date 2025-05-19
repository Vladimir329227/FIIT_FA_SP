#include "../include/fraction.h"

big_int gcd(big_int a, big_int b) {
    while (b != 0) {
        big_int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

void fraction::optimise()
{
    if (_denominator == 0) {
        throw std::logic_error("Denominator is zero after operation");
    }

    big_int gcd_val = gcd(_numerator, _denominator);
    _numerator /= gcd_val;
    _denominator /= gcd_val;

    // Ensure denominator is positive
    if (_denominator < 0) {
        _numerator = -_numerator;
        _denominator = -_denominator;
    }
}

template<std::convertible_to<big_int> f, std::convertible_to<big_int> s>
fraction::fraction(f &&numerator, s &&denominator)
        : _numerator(std::forward<f>(numerator)),
          _denominator(std::forward<s>(denominator))
{
    if (_denominator == 0) {
        throw std::invalid_argument("Denominator cannot be zero");
    }
    optimise();
}

fraction::fraction(pp_allocator<big_int::value_type> alloc)
        : _numerator(0, alloc),
          _denominator(1, alloc)
{}

fraction &fraction::operator+=(fraction const &other) &
{
    _numerator = _numerator * other._denominator + other._numerator * _denominator;
    _denominator *= other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator+(fraction const &other) const
{
    fraction result(*this);
    result += other;
    return result;
}

fraction &fraction::operator-=(fraction const &other) &
{
    _numerator = _numerator * other._denominator - other._numerator * _denominator;
    _denominator *= other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator-(fraction const &other) const
{
    fraction result(*this);
    result -= other;
    return result;
}

fraction &fraction::operator*=(fraction const &other) &
{
    _numerator *= other._numerator;
    _denominator *= other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator*(fraction const &other) const
{
    fraction result(*this);
    result *= other;
    return result;
}

fraction &fraction::operator/=(fraction const &other) &
{
    if (other._numerator == 0) {
        throw std::invalid_argument("Division by zero");
    }
    _numerator *= other._denominator;
    _denominator *= other._numerator;
    optimise();
    return *this;
}

fraction fraction::operator/(fraction const &other) const
{
    fraction result(*this);
    result /= other;
    return result;
}

bool fraction::operator==(fraction const &other) const noexcept
{
    return _numerator == other._numerator && _denominator == other._denominator;
}

std::partial_ordering fraction::operator<=>(const fraction& other) const noexcept
{
    const big_int lhs = _numerator * other._denominator;
    const big_int rhs = other._numerator * _denominator;
    return lhs <=> rhs;
}

std::ostream &operator<<(std::ostream &stream, fraction const &obj)
{
    stream << obj._numerator << "/" << obj._denominator;
    return stream;
}

std::istream &operator>>(std::istream &stream, fraction &obj)
{
    big_int numerator, denominator;
    char slash = 0;

    // Читаем числитель
    if (!(stream >> numerator)) {
        stream.setstate(std::ios::failbit);
        return stream;
    }

    // Проверяем слэш без пропуска пробелов
    stream >> std::noskipws >> slash;
    if (slash != '/') {
        stream.setstate(std::ios::failbit);
        return stream;
    }

    // Возвращаем стандартное поведение потока
    stream >> std::skipws;

    // Читаем знаменатель
    if (!(stream >> denominator) || denominator == 0) {
        stream.setstate(std::ios::failbit);
        return stream;
    }

    // Создаём дробь (оптимизация внутри конструктора)
    obj = fraction(numerator, denominator);
    return stream;
}

std::string fraction::to_string() const
{
    return _numerator.to_string() + "/" + _denominator.to_string();
}

fraction fraction::sin(fraction const &epsilon) const
{
    fraction result(0, 1); // Начальное значение суммы
    fraction term = *this; // Первый член ряда (x)
    big_int factorial = 1_bi;
    big_int sign = 1_bi;
    size_t n = 1;

    do {
        result += term * fraction(sign, 1); // Добавляем член с учетом знака
        sign = -sign; // Меняем знак для следующего члена

        // Вычисляем следующий член: term *= (x^2) / ((2n)*(2n+1))
        term *= (*this) * (*this); // x^2
        term /= fraction((2*n) * (2*n + 1), 1); // Делим на (2n)(2n+1)
        n++;
    } while (term.abs() > epsilon); // Проверка точности

    return result;

}

fraction fraction::cos(fraction const &epsilon) const
{
    fraction result(1, 1);
    fraction term(1, 1);
    big_int sign = 1_bi;
    size_t n = 1;

    do {
        sign = -sign; // Меняем знак
        term *= (*this) * (*this); // x^2
        term /= fraction((2*n - 1) * (2*n), 1); // Делим на (2n-1)(2n)
        result += term * fraction(sign, 1);
        n++;
    } while (term.abs() > epsilon);

    return result;
}

fraction fraction::tg(fraction const &epsilon) const
{
    fraction cos_val = this->cos(epsilon);
    if (cos_val == fraction(0, 1)) {
        throw std::domain_error("Tangent is undefined (cos(x) = 0)");
    }
    return this->sin(epsilon) / cos_val;
}

fraction fraction::ctg(fraction const &epsilon) const
{
    fraction sin_val = this->sin(epsilon);
    if (sin_val == fraction(0, 1)) {
        throw std::domain_error("Cotangent is undefined (sin(x) = 0)");
    }
    return this->cos(epsilon) / sin_val;
}

fraction fraction::sec(fraction const &epsilon) const
{
    fraction cos_val = this->cos(epsilon);
    if (cos_val == fraction(0, 1)) {
        throw std::domain_error("Secant is undefined (cos(x) = 0)");
    }
    return fraction(1, 1) / cos_val;
}

fraction fraction::cosec(fraction const &epsilon) const
{
    fraction sin_val = this->sin(epsilon);
    if (sin_val == fraction(0, 1)) {
        throw std::domain_error("Cosecant is undefined (sin(x) = 0)");
    }
    return fraction(1, 1) / sin_val;
}

fraction fraction::pow(size_t degree) const
{
    if (degree == 0) {
        return fraction(1, 1);
    }

    fraction result(*this);
    fraction base(*this);
    --degree; // Уже учли начальное значение

    // Быстрое возведение в степень
    while (degree > 0) {
        if (degree % 2 == 1) {
            result *= base;
            result.optimise();
        }
        base *= base;
        base.optimise();
        degree /= 2;
    }

    return result;
}

fraction fraction::root(size_t degree, fraction const &epsilon) const
{
    if (degree == 0) {
        throw std::invalid_argument("Zero degree root is undefined");
    }
    if (degree % 2 == 0 && _numerator < 0) {
        throw std::domain_error("Even root of negative number");
    }

    fraction x_prev(1, 1); // Начальное приближение
    fraction x_next;
    fraction x_power;
    bool converged = false;

    // Метод Ньютона: x_{n+1} = ( (k-1)*x_n + a/(x_n^{k-1}) ) / k
    do {
        x_power = x_prev.pow(degree - 1);
        x_next = (x_prev * fraction(degree - 1, 1) + (*this) / x_power) / fraction(degree, 1);

        // Проверка сходимости: |x_next - x_prev| < epsilon
        fraction delta = (x_next - x_prev).abs();
        if (delta <= epsilon) {
            converged = true;
        }
        x_prev = x_next;
    } while (!converged);

    return x_prev;
}

fraction fraction::log2(fraction const &epsilon) const
{
    fraction ln_this = this->ln(epsilon);
    fraction ln2(2, 1);
    ln2.ln(epsilon); // ln(2)
    return ln_this / ln2;
}

fraction fraction::ln(fraction const &epsilon) const
{
    if (_numerator <= 0 || _denominator <= 0) {
        throw std::domain_error("ln(x) is undefined for non-positive x");
    }

    fraction x_original = *this;
    fraction scale_factor(1, 1);
    int scaling = 0;

    // Приведение x к диапазону [0.5, 2] для улучшения сходимости
    while (x_original > fraction(2, 1)) {
        x_original = x_original.root(2, epsilon); // x = sqrt(x)
        scaling++;
    }

    while (x_original < fraction(1, 2)) {
        x_original = x_original.root(2, epsilon); // x = sqrt(x)
        scaling--;
    }

    fraction x = x_original - fraction(1, 1);
    fraction sum(0, 1);
    fraction term = x;
    size_t n = 1;
    const size_t max_iterations = 100000; // Защита от бесконечного цикла

    while (term.abs() > epsilon && n < max_iterations) {
        fraction sign = (n % 2 == 1) ? fraction(1, 1) : fraction(-1, 1);
        sum += term * fraction(1, n) * sign;
        term *= x;
        term.optimise(); // Сокращение дроби для уменьшения размера
        n++;
    }

    // Учет масштабирования: ln(x^(2^k)) = 2^k * ln(x)
    sum = sum * fraction(1 << std::abs(scaling), 1);

    return sum;
}

fraction fraction::lg(fraction const &epsilon) const
{
    fraction ln_this = this->ln(epsilon);
    fraction ln10(10, 1);
    ln10.ln(epsilon); // ln(10)
    return ln_this / ln10;
}

fraction fraction::abs() const {
    if (_numerator < 0)
        return fraction(-_numerator, _denominator);
    return fraction(_numerator, _denominator);
}