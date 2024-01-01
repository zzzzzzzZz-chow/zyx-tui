#include <Type/include/Position.h>
#include <utility>

PositionType::_Position2D::_Position2D() noexcept : Point2D() {
}

PositionType::_Position2D::_Position2D(const Number &x, const Number &y) noexcept : Point2D(x, y) {
}

PositionType::_Position2D::_Position2D(ConstReference that) noexcept : Point2D(that) {
}

PositionType::_Position2D::_Position2D(RightReference that) noexcept : Point2D(std::move(that)) {
}

auto PositionType::_Position2D::operator=(ConstReference that) noexcept -> Reference {
    setX(that.x()).setY(that.y());
    return *this;
}

auto PositionType::_Position2D::operator=(RightReference that) noexcept -> Reference {
    setX(std::move(that.x())).setY(std::move(that.y()));
    return *this;
}

PositionType::_Position2D::~_Position2D() noexcept {
}

auto PositionType::_Position2D::moveX(const Number &x) noexcept -> Reference {
    setX(this->x() + x);
    return *this;
}

auto PositionType::_Position2D::moveY(const Number &y) noexcept -> Reference {
    setY(this->y() + y);
    return *this;
}

auto PositionType::_Position2D::move(const Point2D &distance) noexcept -> Reference {
    return moveX(distance.x()).moveY(distance.y());
}

auto PositionType::_Position2D::distance(ConstReference that) const -> Number {
    return (*this - that).length();
}
