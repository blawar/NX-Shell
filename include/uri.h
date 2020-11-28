#pragma once
#include <string>

class Uri
{
public:
	Uri();
	Uri(const char* uri);
	Uri(const std::string& uri);
	virtual ~Uri();

	bool operator==(const Uri& uri) const;
	bool operator!=(const Uri& uri) const;
	int compare(const Uri& uri) const;

	Uri join(const Uri& uri) const;
	Uri parent() const;
	std::string basename() const;
	const char* c_str() const;
	std::string scheme() const;
	bool empty();
	void reset();
	void clear();
protected:
	std::string m_buffer;
};