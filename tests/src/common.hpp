#pragma once

#include <filesystem>
#include <string>


namespace utils {
std::filesystem::path _get_executable_filepath();
std::string load_data(std::filesystem::path path);
}	 // namespace utils
