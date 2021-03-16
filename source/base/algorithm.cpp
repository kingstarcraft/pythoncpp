#pragma once
#include <algorithm>
#include <codecvt>
#include <array>
#include "zero/base/algorithm.h"

#ifdef _WIN32
#define pclose _pclose
#define popen _popen
#endif


namespace zero {
	static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> g_converter;

	std::string Lower(const std::string &input) {
		std::string output;
		output.resize(input.size());
		std::transform(input.begin(), input.end(), output.begin(), ::tolower);
		return output;
	}

	template <typename To, typename From>
	struct Converter {
		static typename std::enable_if<std::is_same<From, To>::value, bool>::type Convert(const From &from) {
			return from;
		}
	};

	template <typename From>
	struct Converter<std::string, From> {
		static std::string Convert(From &from) {
			return std::to_string(from);
		}
	};

	template <typename From>
	struct Converter<double, From> {
		static std::string Convert(From &from) {
			return std::to_string(from);
		}
	};

	template <typename To, typename From>
	typename To lexical_cast(const From& from) {
		return Converter<To, From>::Convert(from);
	}


	std::string Execute(const std::string &command, const bool error) {
		std::array<char, 1024> buffer;
		std::string result;
		std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.data(), "r"), pclose);
		if (pipe) {  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) { result += buffer.data(); } }
		else if (error) { throw std::runtime_error(std::string("popen ") + command + " failed!"); }
		return result;
	}

	std::string Convert(const std::wstring& input) {
		return g_converter.to_bytes(input);
	}

	std::wstring Convert(const std::string& input) {
		return g_converter.from_bytes(input);
	}
}