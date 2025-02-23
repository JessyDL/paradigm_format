#include "common.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

namespace utils {
std::filesystem::path _get_executable_filepath() {
#ifdef _WIN32
	char path[MAX_PATH];
	GetModuleFileNameA(nullptr, path, MAX_PATH);
	return std::filesystem::canonical(path);
#else
	char path[1024];
	ssize_t count = readlink("/proc/self/exe", path, sizeof(path) - 1);
	if(count != -1) {
		path[count] = '\0';
		return std::filesystem::canonical(path);
	}
	return {};
#endif
}

std::string load_data(std::filesystem::path path) {
	if(!std::filesystem::exists(path)) {
		auto exe_path = _get_executable_filepath();
		path		  = exe_path.parent_path() / path;
		if(!std::filesystem::exists(path)) {
			throw std::runtime_error(std::format("Could not find file {}", path.string()));
		}
	}
	std::ifstream file(path);
	if(file.fail()) {
		throw std::runtime_error("Could not open file");
	}
	std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	if(data.starts_with("/// COMMENT")) {
		auto loc = data.find("/// ENDCOMMENT");
		if(loc != std::string::npos) {
			loc = data.find('\n', loc);
			if(loc != std::string::npos) {
				data = data.substr(loc + 1);
				return data;
			}
		}

		throw std::runtime_error(
		  std::format("Could not find end of '/// ENDCOMMENT' directive, please double check {}", path.string()));
	}
	return data;
}
}	 // namespace utils
