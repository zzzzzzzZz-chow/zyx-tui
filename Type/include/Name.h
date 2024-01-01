#pragma once

#include <string>

class NameType {
public:
	NameType() = default;
	NameType(const NameType&) = default;
	NameType(NameType&&) = default;
	virtual NameType& operator=(const NameType&) noexcept = default;
	virtual NameType& operator=(NameType&&) noexcept = default;
	virtual ~NameType() noexcept = default;
	typedef std::string Name;
};