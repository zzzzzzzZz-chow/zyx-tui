#pragma once

#include "Name.h"
#include "ID.h"

class Object : public virtual NameType, public virtual IDType {
	using Type = Object;
	using Reference = Type & ;
	using ConstReference = const Type &;
	using RightReference = Type && ;
	using Pointer = Type * ;
	using ConstPointer = const Type *;
public:
	Object() noexcept(false);
	Object(ConstReference that);
	Object(RightReference that) noexcept;
	virtual auto operator=(ConstReference that) ->Reference;
	virtual auto operator=(RightReference that) noexcept ->Reference;
	~Object() noexcept;

	auto name() noexcept ->Name&;
	auto id() noexcept ->ID&;
	auto name() const noexcept ->const Name&;
	auto id() const noexcept ->const ID&;
	auto setName(const Name& name) ->Reference;
	auto setId(const ID& id) noexcept ->Reference;
private:
	static ID s_id;
	Name m_name;
	ID m_id;
};