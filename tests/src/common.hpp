#include <filesystem>
#include <fstream>
#include <format>
#include <stdexcept>

namespace utils {
std::string load_data(std::filesystem::path path) {
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

		throw std::runtime_error(std::format("Could not find end of '/// ENDCOMMENT' directive, please double check {}", path.string()));
	}
	return data;
}
}	 // namespace utils
