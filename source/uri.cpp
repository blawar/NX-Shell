#include "uri.h"
#include <filesystem>

Uri::Uri()
{
}

Uri::Uri(const char* uri) : m_buffer(uri)
{
}

Uri::Uri(const std::string& uri) : m_buffer(uri)
{
}

Uri::~Uri()
{
}

bool Uri::operator==(const Uri& uri) const
{
	return uri.m_buffer == m_buffer;
}

bool Uri::operator!=(const Uri& uri) const
{
	return !operator==(uri.m_buffer);
}

int Uri::compare(const Uri& uri) const
{
	return *this == uri ? 0 : -1;
}

bool Uri::empty()
{
	return m_buffer.length() == 0;
}

void Uri::reset()
{
	m_buffer = "";
}

void Uri::clear()
{
	reset();
}

Uri Uri::join(const Uri& uri) const
{
	return (std::filesystem::path(m_buffer) / uri.c_str()).c_str();
}

Uri Uri::parent() const
{
	return std::filesystem::path(m_buffer).parent_path().c_str();
}

std::string Uri::basename() const
{
	return "";
}

const char* Uri::c_str() const
{
	return m_buffer.c_str();
}

std::string Uri::scheme() const
{
	return "sdmc";
}
