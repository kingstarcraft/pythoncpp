#pragma once
#include <string>
#include "zero/common.h"
namespace zero {
	std::string ZERO_API Lower(const std::string &input);
    std::string ZERO_API Execute(const std::string& command, const bool error = false);
    std::string ZERO_API Convert(const std::wstring& input);
    std::wstring ZERO_API Convert(const std::string& input);
    // template <typename To, typename From>
    // typename std::enable_if<!std::is_same<To, From>::value, To>::type lexical_cast(const From& from);
    // template <typename To, typename From>
    // typename std::enable_if<std::is_same<To, From>::value, To>::type lexical_cast(const From& from);
	// 
	// template ZERO_API double lexical_cast<double>(const double& from);
}