#include "File.h"
#include <filesystem>

std::string Util::rootFilePath()
{
	return std::filesystem::current_path().string();
}
