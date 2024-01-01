#include <Type/include/Object.h>

Object::ID Object::s_id = ID{ 0 };

Object::Object() noexcept(false) : m_name(Name{ "unnamed" }), m_id(s_id++) {}

Object::Object(ConstReference that) : m_name(that.m_name), m_id(s_id++) {}

Object::Object(RightReference that) noexcept : m_name(std::move(that.m_name)), m_id(std::move(that.m_id)) {}

auto Object::operator=(ConstReference that) ->Reference {
	m_name = that.m_name;
	return *this;
}

auto Object::operator=(RightReference that) noexcept ->Reference {
	m_name = std::move(that.m_name);
	m_id = std::move(that.m_id);
	return *this;
}

Object::~Object() noexcept {}

auto Object::name() noexcept ->Name& { 
	return m_name; 
}

auto Object::id() noexcept ->ID& { 
	return m_id; 
}

auto Object::name() const noexcept ->const Name& { 
	return m_name; 
}

auto Object::id() const noexcept ->const ID& { 
	return m_id; 
}

auto Object::setName(const Name& name) ->Reference { 
	m_name = name; 
	return *this; 
}

auto Object::setId(const ID& id) noexcept ->Reference { 
	m_id = id; 
	return *this; 
}