#pragma once
#include <string>
#include "zero/common.h"
namespace zero {
	std::string ZERO_API Lower(const std::string &input);
    std::string ZERO_API Execute(const std::string& command, const bool error = false);
    std::string ZERO_API Convert(const std::wstring& input);
    std::wstring ZERO_API Convert(const std::string& input);
}