#pragma once

#include "Number.h"

class PointType : public virtual NumberType {
	class _Point2D {
		using Type = _Point2D;
		using Reference = Type & ;
		using ConstReference = const Type &;
		using RightReference = Type && ;
		using Pointer = Type * ;
		using ConstPointer = const Type *;
	public:
		_Point2D() noexcept;
		_Point2D(const Number& x, const Number& y) noexcept;
		_Point2D(ConstReference that) noexcept;
		_Point2D(RightReference that) noexcept;
		virtual auto operator=(ConstReference that) noexcept ->Reference;
		virtual auto operator=(RightReference that) noexcept ->Reference;
		virtual ~_Point2D() noexcept;

		auto area() const noexcept ->Number;
		auto length() const ->Number;
		auto one(const Number& k=1) const ->Type;
		auto operator+(ConstReference that) const noexcept ->Type;
		auto operator-() const noexcept ->Type;
		auto operator-(ConstReference that) const noexcept ->Type;
		auto operator*(ConstReference that) const noexcept ->Number;
		auto operator*(const Number& that) const noexcept ->Type;
		auto operator/(const Number& number) const ->Type;

		auto x() noexcept ->Number&;
		auto y() noexcept ->Number&;
		auto x() const noexcept ->const Number&;
		auto y() const noexcept ->const Number&;
		auto setX(const Number& x) noexcept ->Reference;
		auto setY(const Number& y) noexcept ->Reference;
	private:
		Number m_x;
		Number m_y;
	};
public:
	PointType() = default;
	PointType(const PointType&) = default;
	PointType(PointType&&) = default;
	virtual PointType& operator=(const PointType&) noexcept = default;
	virtual PointType& operator=(PointType&&) noexcept = default;
	~PointType() noexcept = default;
	typedef _Point2D Point2D;
};