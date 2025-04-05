#include "../include/fraction.h"

void fraction::optimise()
{
    throw not_implemented("void fraction::optimise()", "your code should be here...");
}

template<std::convertible_to<big_int> f, std::convertible_to<big_int> s>
fraction::fraction(f &&numerator, s &&denominator)
{
    throw not_implemented("template<std::convertible_to<big_int> f, std::convertible_to<big_int> s> fraction::fraction(f &&, s &&)", "your code should be here...");
}

fraction::fraction(pp_allocator<big_int::value_type>)
{
    throw not_implemented("fraction::fraction(pp_allocator<big_int::value_type>)", "your code should be here...");
}

fraction &fraction::operator+=(fraction const &other) &
{
    throw not_implemented("fraction &fraction::operator+=(fraction const &) &", "your code should be here...");
}

fraction fraction::operator+(fraction const &other) const
{
    throw not_implemented("fraction fraction::operator+(fraction const &) const", "your code should be here...");
}

fraction &fraction::operator-=(fraction const &other) &
{
    throw not_implemented("fraction &fraction::operator-=(fraction const &) &", "your code should be here...");
}

fraction fraction::operator-(fraction const &other) const
{
    throw not_implemented("fraction fraction::operator-(fraction const &) const", "your code should be here...");
}

fraction &fraction::operator*=(fraction const &other) &
{
    throw not_implemented("fraction &fraction::operator*=(fraction const &) &", "your code should be here...");
}

fraction fraction::operator*(fraction const &other) const
{
    throw not_implemented("fraction fraction::operator*(fraction const &) const", "your code should be here...");
}

fraction &fraction::operator/=(fraction const &other) &
{
    throw not_implemented("fraction &fraction::operator/=(fraction const &) &", "your code should be here...");
}

fraction fraction::operator/(fraction const &other) const
{
    throw not_implemented("fraction fraction::operator/(fraction const &) const", "your code should be here...");
}

bool fraction::operator==(fraction const &other) const noexcept
{
    throw not_implemented("bool fraction::operator==(fraction const &) const noexcept", "your code should be here...");
}

std::partial_ordering fraction::operator<=>(const fraction& other) const noexcept
{
    throw not_implemented("std::partial_ordering fraction::operator<=>(const fraction& ) const noexcept", "your code should be here...");
}

std::ostream &operator<<(std::ostream &stream, fraction const &obj)
{
    throw not_implemented("std::ostream &operator<<(std::ostream &, fraction const &)", "your code should be here...");
}

std::istream &operator>>(std::istream &stream, fraction &obj)
{
    throw not_implemented("std::istream &operator>>(std::istream &, fraction &)", "your code should be here...");
}

std::string fraction::to_string() const
{
    throw not_implemented("std::string fraction::to_string() const", "your code should be here...");
}

fraction fraction::sin(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::sin(fraction const &) const", "your code should be here...");
}

fraction fraction::cos(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::cos(fraction const &) const", "your code should be here...");
}

fraction fraction::tg(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::tg(fraction const &) const", "your code should be here...");
}

fraction fraction::ctg(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::ctg(fraction const &) const", "your code should be here...");
}

fraction fraction::sec(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::sec(fraction const &) const", "your code should be here...");
}

fraction fraction::cosec(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::cosec(fraction const &) const", "your code should be here...");
}

fraction fraction::pow(size_t degree) const
{
    throw not_implemented("fraction fraction::pow(size_t) const", "your code should be here...");
}

fraction fraction::root(size_t degree, fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::root(size_t , fraction const &) const", "your code should be here...");
}

fraction fraction::log2(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::log2(fraction const &) const", "your code should be here...");
}

fraction fraction::ln(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::ln(fraction const &) const", "your code should be here...");
}

fraction fraction::lg(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::lg(fraction const &) const", "your code should be here...");
}