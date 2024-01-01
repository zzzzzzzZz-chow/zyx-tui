#pragma once

class IDType {
public:
	IDType() = default;
	IDType(const IDType&) = default;
	IDType(IDType&&) = default;
	virtual IDType& operator=(const IDType&) noexcept = default;
	virtual IDType& operator=(IDType&&) noexcept = default;
	virtual ~IDType() noexcept = default;
	typedef unsigned long long ID;
};