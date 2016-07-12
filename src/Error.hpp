#pragma once

#include <string>

class Error
{
public:
	static void report(const std::string &text);
};