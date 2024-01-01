#include <Type/include/Point.h>
#include <cmath>
#include <type_traits>
#include <exception>
#include <stdexcept>

PointType::_Point2D::_Point2D() noexcept : m_x(Number{}), m_y(Number{}) {
}

PointType::_Point2D::_Point2D(const Number &x, const Number &y) noexcept : m_x(x), m_y(y) {
}

PointType::_Point2D::_Point2D(ConstReference that) noexcept : m_x(that.m_x), m_y(that.m_y) {
}

PointType::_Point2D::_Point2D(RightReference that) noexcept : m_x(std::move(that.m_x)), m_y(std::move(that.m_y)) {
}

auto PointType::_Point2D::operator=(ConstReference that) noexcept -> Reference {
    m_x = that.m_x;
    m_y = that.m_y;
    return *this;
}

auto PointType::_Point2D::operator=(RightReference that) noexcept -> Reference {
    m_x = std::move(that.m_x);
    m_y = std::move(that.m_y);
    return *this;
}

PointType::_Point2D::~_Point2D() noexcept {
}

auto PointType::_Point2D::area() const noexcept -> Number {
    return x() * y();
}

auto PointType::_Point2D::length() const -> Number {
    return static_cast<Number>(std::sqrt(x() * x() + y() * y()));
}

auto PointType::_Point2D::one(const Number &k) const -> Type {
    const Number len = length();
    if (len == 0)
        return Type{};
    return (*this) * k / len;
}

auto PointType::_Point2D::operator+(ConstReference that) const noexcept -> Type {
    return Type{m_x + that.m_x, m_y + that.m_y};
}

auto PointType::_Point2D::operator-() const noexcept -> Type {
    return Type{-m_x, -m_y};
}

auto PointType::_Point2D::operator-(ConstReference that) const noexcept -> Type {
    return Type{m_x - that.m_x, m_y - that.m_y};
}

auto PointType::_Point2D::operator*(ConstReference that) const noexcept -> Number {
    return m_x * that.m_x + m_y * that.m_y;
}

auto PointType::_Point2D::operator*(const Number &that) const noexcept -> Type {
    return Type{m_x * that, m_y * that};
}

auto PointType::_Point2D::operator/(const Number &number) const -> Type {
    if (number == Number{0})
        throw std::runtime_error("divided by zero\n");
    return Type{m_x / number, m_y / number};
}

auto PointType::_Point2D::x() noexcept -> Number & {
    return m_x;
}

auto PointType::_Point2D::y() noexcept -> Number & {
    return m_y;
}

auto PointType::_Point2D::x() const noexcept -> const Number & {
    return m_x;
}

auto PointType::_Point2D::y() const noexcept -> const Number & {
    return m_y;
}

auto PointType::_Point2D::setX(const Number &x) noexcept -> Reference {
    m_x = x;
    return *this;
}

auto PointType::_Point2D::setY(const Number &y) noexcept -> Reference {
    m_y = y;
    return *this;
}
