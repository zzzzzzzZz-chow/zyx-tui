#pragma once

#include "Point.h"

class PositionType : public virtual PointType {
	class _Position2D : public Point2D {
		using Type = _Position2D;
		using Reference = Type & ;
		using ConstReference = const Type &;
		using RightReference = Type && ;
		using Pointer = Type * ;
		using ConstPointer = const Type *;
	public:
		_Position2D() noexcept;
		_Position2D(const Number& x, const Number& y) noexcept;
		_Position2D(ConstReference that) noexcept;
		_Position2D(RightReference that) noexcept;
		virtual auto operator=(ConstReference that) noexcept ->Reference;
		virtual auto operator=(RightReference that) noexcept ->Reference;
		~_Position2D() noexcept;

		auto moveX(const Number& x) noexcept ->Reference;
		auto moveY(const Number& y) noexcept ->Reference;
		auto move(const Point2D& distance) noexcept ->Reference;
		auto distance(ConstReference that) const ->Number;
	};
public:
	PositionType() = default;
	PositionType(const PositionType&) = default;
	PositionType(PositionType&&) = default;
	virtual PositionType& operator=(const PositionType&) noexcept = default;
	virtual PositionType& operator=(PositionType&&) noexcept = default;
	~PositionType() noexcept = default;
	typedef _Position2D Position2D;
};