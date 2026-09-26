#include "File.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

std::string Util::rootFilePath()
{
	return std::filesystem::current_path().string();
}

std::string Util::parseFileToString(const std::string& fileName)
{
	// Setting the result
	std::string result;

	// Setting the main file path object from filepath parameter
	std::filesystem::path filePath = fileName;

	// Checking if the file actually exists or not
	if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath))
	{
		// Opens the file for reading
		std::ifstream inputFile(filePath);

		// Checks if the file opened properly
		if (inputFile.is_open())
		{
			// Reads the entire content of the file and then puts that into a string
			std::ostringstream contentStream;
			contentStream << inputFile.rdbuf();
			result = contentStream.str();

			inputFile.close();
		}
		else {
			std::cerr << "Unable to open file \"" + fileName + "\"" << '\n';
			result = "ERROR";
		}
	}
	else {
		std::cerr << "File at \"" + fileName + "\" was unable to be reached" << '\n';
		result = "ERROR";
	}

	return result;
}
