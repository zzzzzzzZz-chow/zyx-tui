#pragma once

class NumberType {
public:
	NumberType() = default;
	NumberType(const NumberType&) = default;
	NumberType(NumberType&&) = default;
	virtual NumberType& operator=(const NumberType&) = default;
	virtual NumberType& operator=(NumberType&&) = default;
	virtual ~NumberType() = default;
	typedef int Number;
};